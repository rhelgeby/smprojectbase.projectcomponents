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

// ---------------
//     Public
// ---------------

/**
 * All available methods of sending text to clients.
 */
enum MsgTypes
{
    MsgType_Server,     /** Text printed to the server's console */
    MsgType_Console,    /** Text printed in client's console. */
    MsgType_Chat,       /** Text printed at the bottom of the screen. */
    MsgType_Reply,      /** Replies with the same msg type as the client used to trigger a command */
    MsgType_Center,     /** Small, white text printed in the center of the screen. */
    MsgType_Hint        /** Translucent grey box containing white text at the bottom center of the screen. */
}

/**
 * An array to know which msg types be colored.
 */
new const g_bMsgTypeColor[MsgTypes] = {false, false, true, false, false};

/**
 * The types of automatic message formatting.
 * Look in project.inc for editable defines regarding these options.
 */
enum MsgFormatting
{
    MsgFormat_None = -1,    /** Don't add anything to the text. */
    MsgFormat_Plugin,       /** Prepend the plugin name to the text. */
    MsgFormat_Module,       /** Prepend the module name to the text. */
    MsgFormat_Both          /** Prepend the both the plugin name and module name to the text. */
}

/**
 * Different options for translating bools into phrases.
 */
enum BoolPhrases
{
    BoolPhrase_YesNo,   /** Translates boolean values 1/0 into "yes/no" phrases. */
    BoolPhrase_OnOff    /** Translates boolean values 1/0 into "on/off" phrases. */
}

// ---------------
//     Private
// ---------------

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

/**
 * Print text to a client with special formatting and translated text.
 * 
 * @param client    The client index.  Use SERVER_INDEX when printing to the server.
 * @param msgformat How to format the text before sending.  See enum MsgFormatting.
 * @param msgtype   The type of message to print to the client.  See enum MsgTypes.
 * @param moduleID  The module ID of the module this is coming from.  Needed for MsgFormat_Module or MsgFormat_Both.
 * @param literal   True to print the last variable as a string, instead of translating it.
 * @param any       The translations phrase or literal string followed by formatting parameters.
 */
stock TransMgr_PrintText(client, MsgFormatting:msgformat, MsgTypes:msgtype, moduleID = -1, bool:literal = false, any:...)
{
    // Tells SourceMod's translations system to translate to this client's (or server's) language.
    SetGlobalTransTarget(client);
    
    decl String:translated[512]; // Give plenty of room for long messages, better too much than not enough.
    
    // Method for getting text from the ... param depends on if literal is true or false.
    if (literal)
        VFormat(translated, sizeof(translated), "%s", 6);
    else
        VFormat(translated, sizeof(translated), "%t", 6);
    
    // Format the message.
    
    // Get the name of the module if a module ID was given.
    new String:fullname[32] = "Invalid Module ID";
    if (ModuleMgr_IsModuleValid(moduleID))
    {
        ModuleMgr_ReadString(moduleID, ModuleData_FullName, fullname, sizeof(fullname));
    }
    
    // If this type of message supports color, then use the color formatting defines.
    if (g_bMsgTypeColor[msgtype])
    {
        switch(msgformat)
        {
            case MsgFormat_Plugin:  Format(translated, sizeof(translated), TM_MSGFORMAT_PLUGIN_COLOR, PROJECT_FULLNAME, translated);
            case MsgFormat_Module:  Format(translated, sizeof(translated), TM_MSGFORMAT_MODULE_COLOR, fullname, translated);
            case MsgFormat_Both:    Format(translated, sizeof(translated), TM_MSGFORMAT_BOTH_COLOR, PROJECT_FULLNAME, fullname, translated);
        }
    }
    // This type of message doesn't support color.
    else
    {
        switch(msgformat)
        {
            case MsgFormat_Plugin:  Format(translated, sizeof(translated), TM_MSGFORMAT_PLUGIN_NOCOLOR, PROJECT_FULLNAME, translated);
            case MsgFormat_Module:  Format(translated, sizeof(translated), TM_MSGFORMAT_MODULE_NOCOLOR, fullname, translated);
            case MsgFormat_Both:    Format(translated, sizeof(translated), TM_MSGFORMAT_BOTH_NOCOLOR, PROJECT_FULLNAME, fullname, translated);
        }
    }
    
    // Now print the text to the client.
    switch(msgtype)
    {
        case MsgType_Server:    PrintToServer(translated);
        case MsgType_Console:   PrintToConsole(client, translated);
        case MsgType_Chat:      PrintToChat(client, translated);
        case MsgType_Reply:     ReplyToCommand(client, translated);
        case MsgType_Center:    PrintCenterText(client, translated);
        case MsgType_Hint:      PrintHintText(client, translated);
    }
}

/**
 * Print text to all clients with special formatting and translated text.
 * 
 * @param server    True to print this phrase to server, along with all clients in the game.
 * @param admin     True to only print to 
 * @param msgformat How to format the text before sending.  See enum MsgFormatting.
 * @param msgtype   The type of message to print to the clients.  See enum MsgTypes.
 * @param literal   True to print the last variable as a string, instead of translating it.
 * @param any       The translations phrase follow by formatting parameters.
 * 
 * TODO: CLIENT ACCESS
 */
stock TransMgr_PrintTextAll(bool:server, bool:admin, MsgFormatting:msgformat, MsgTypes:msgtype, moduleID = -1, bool:literal = false, any:...)
{
    // There's only 1 server.
    if (msgtype == MsgType_Server)
    {
        ThrowError("TransMgr_PrintTextAll doesn't support MsgType_Server.");
    }
    
    // Can't reply to all clients simultaneously.
    if (msgtype == MsgType_Reply)
    {
        ThrowError("TransMgr_PrintTextAll doesn't support MsgType_Reply.");
    }
    
    decl String:translated[512]; // Give plenty of room for long messages, better too much than not enough.
    
    // Print the phrase in the server's language.
    if (server)
    {
        SetGlobalTransTarget(LANG_SERVER);
        
        // Method for getting text from the ... param depends on if literal is true or false.
        if (literal)
            VFormat(translated, sizeof(translated), "%s", 7);
        else
            VFormat(translated, sizeof(translated), "%t", 7);
        
        TransMgr_PrintText(SERVER_INDEX, msgformat, MsgType_Server, moduleID, true, translated);
    }
    
    for (new client = 1; client <= MaxClients; client++)
    {
        if (!IsClientInGame(client))
            continue;
        
        /**
         * TODO: Check if client has permissions
         */
        
        if (admin)
        {
        }
        
        // Print the phrase in the client's language.
        SetGlobalTransTarget(client);
        
        // Method for getting text from the ... param depends on if literal is true or false.
        if (literal)
            VFormat(translated, sizeof(translated), "%s", 7);
        else
            VFormat(translated, sizeof(translated), "%t", 7);
        
        TransMgr_PrintText(client, msgformat, msgtype, moduleID, true, translated);
    }
}

/**
 * Translates a boolean value into a text phrase.
 * 
 * @param target        The client to translate phrase to.  LANG_SERVER to use server's language.
 * @param boolean       The boolean value to translate.
 * @param boolphrase    The boolean phrase pair to translate to.
 * @param phrase        The output string with the translated prase.
 * @param maxlen        The max length of the output string.
 */
stock TransMgr_TranslateBool(target, bool:boolean, BoolPhrases:boolphrase, String:phrase[], maxlen)
{
    SetGlobalTransTarget(target);
    
    switch (boolphrase)
    {
        case BoolPhrase_YesNo:
            boolean ? Format(phrase, maxlen, "%t", "Yes") : Format(phrase, maxlen, "%t", "No");
        
        case BoolPhrase_OnOff:
            boolean ? Format(phrase, maxlen, "%t", "On") : Format(phrase, maxlen, "%t", "Off");
    }
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
stock TransMgr_LoadTranslations()
{
    // Loads extra translations from the main project.inc file.
    TransMgr_LoadExtraTranslations();
    
    decl String:translationfile[PLATFORM_MAX_PATH];
    
    // Load base file translations.
    Format(translationfile, sizeof(translationfile), "%s/%s.phrases", PROJECT_SHORTNAME, PROJECT_SHORTNAME);
    LoadTranslations(translationfile);
    
    decl String:shortname[16];
    
    // Loop through all the modules.
    for (new moduleID = 0; moduleID < MODULE_COUNT; moduleID++)
    {
        // If the module doesn't have translations enabled, then skip to next module ID.
        if (!TransMgr_IsEnabled(moduleID))
            continue;
        
        // Get the module's shortname to use as the translations name.
        ModuleMgr_ReadString(moduleID, ModuleData_ShortName, shortname, sizeof(shortname));
        
        Format(translationfile, sizeof(translationfile), "%s/%s.phrases", PROJECT_SHORTNAME, shortname);
        LoadTranslations(translationfile);
    }
}

/**
 * Command callback: project_reload_translations
 * Reloads all translations files used in the project.
 * 
 * @param client    The client index.  Or SERVER_INDEX if coming from the server.
 * @param argc      The number of arguments that the client sent with the command.
 */
public Action:Command_ReloadTranslations(client, arc)
{
    /**
     * TODO: Check if the client has access.
     */
    
    TransMgr_LoadTranslations();
    
    TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Reply, _, false, "TransMgr cmd reloadtranslations");
}
