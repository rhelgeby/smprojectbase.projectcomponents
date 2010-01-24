/*
 * ============================================================================
 *
 *  Project
 *
 *  File:          translationsmanager.inc
 *  Type:          Base
 *  Description:   Manages translations throughout the project.
 *
 *  Copyright (C) 2009-2010  Greyscale
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * ============================================================================
 */

/**
 * Provides the plugin a way to know if the translations manager is included in the project.
 */
#define TRANSLATIONS_MANAGER

/**
 * The max number of cells needed for the translations manager's allocated index.
 */
#define TM_DATA_MAX_CELLS 1
// Translations manager only needs 1 cell for a boolean value.

/**
 * Defines the block of data in the module data arrays that contains whether a module uses translations or not.
 */
#define TRANSLATIONS_DATA_ENABLED g_iTMAllocatedIndexes[0]

/**
 * Array to store the index of the allocated space in the module data arrays for the translations manager.
 */
new g_iTMAllocatedIndexes[1];

// **********************************************
//                 Forwards
// **********************************************

/**
 * Plugin has loaded.
 */
TransMgr_OnPluginStart()
{
    // Allocate 1 index for the data we want to store for each module.
    ModuleMgr_Allocate(1, g_iTMAllocatedIndexes);
    
    // Create translations commands.
    RegConsoleCmd("project_reload_translations", Command_ReloadTranslations, "Reload all project translations files.");
}

/**
 * A module was just registered.  This is being called before the module has been loaded.
 * 
 * @param adtModule The adt array of the module being registered.
 */
TransMgr_OnModuleRegister(Handle:adtModule)
{
    // Push the 'false' into our allocated space to signify if this module is using translations or not.
    PushArrayCell(adtModule, false);
}

/**
 * All of the project's modules have been registered.
 */
TransMgr_OnModulesRegistered()
{
    // Load all the translations for the project.
    TransMgr_LoadTranslations();
}

// **********************************************
//                Public API
// **********************************************

/**
 * Register your module to use translations.
 * Your module's translations file path is defined as: project/<moduleshortname>.phrases.txt
 * 
 * @param moduleID  The module ID to register for translations.
 */
stock TransMgr_Register(moduleID)
{
    TransMgr_SetEnabled(moduleID, true);
}

// **********************************************
//   Private API (For base project files only)
// **********************************************

/**
 * Check if the specified module has translations enabled.
 * This is meant for other base project files to use, modules shouldn't need this.
 * 
 * @param moduleID      The module ID of the module to check.
 * 
 * @return              True if enabled, false otherwise. 
 */
stock bool:TransMgr_IsEnabled(moduleID)
{
    return bool:GetArrayCell(ModuleMgr_GetModuleArray(moduleID), TRANSLATIONS_DATA_ENABLED);
}

/**
 * Change if translations is enabled for a module.
 * This is meant for other base project files to use, modules shouldn't need this.
 * 
 * @param moduleID      The module ID of the module to change.
 * @param enable        True to enable translations in this module, false to disable.
 */
stock TransMgr_SetEnabled(moduleID, bool:enable)
{
    SetArrayCell(ModuleMgr_GetModuleArray(moduleID), TRANSLATIONS_DATA_ENABLED, _:enable);
}

/**
 * (Re)loads all project translations files.
 */
TransMgr_LoadTranslations()
{
    // Load the project base translations.
    LoadTranslations("project/base.phrases");
    
    decl String:shortname[16];
    decl String:translationfile[PLATFORM_MAX_PATH];
    
    // Loop through all the modules.
    for (new moduleID = 0; moduleID < MODULE_COUNT; moduleID++)
    {
        // If the module doesn't have translations enabled, then skip to next module ID.
        if (!TransMgr_IsEnabled(moduleID))
            continue;
        
        // Get the module's shortname to use as the translations name.
        ModuleMgr_ReadString(moduleID, ModuleData_ShortName, shortname, sizeof(shortname));
        
        Format(translationfile, sizeof(translationfile), "project/%s.phrases", shortname);
        LoadTranslations(translationfile);
    }
}

/**
 * Command callback: project_reload_translations
 * Reloads all translations files used in the project.
 * 
 * @param client    The client index.
 * @param argc      The number of arguments that the client sent with the command.
 */
public Action:Command_ReloadTranslations(client, arc)
{
    /**
     * TODO: Check if the client has access.
     */
    
    TransMgr_LoadTranslations();
}
    