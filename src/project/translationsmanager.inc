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
new g_bMsgTypeColor[MsgTypes] = {false, false, true, false, false, false};

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
    BoolPhrase_YesNo,           /** Translates boolean values 1/0 into "yes/no" phrases. */
    BoolPhrase_OnOff,           /** Translates boolean values 1/0 into "on/off" phrases. */
    BoolPhrase_EnabledDisabled  /** Translates boolean values 1/0 into "enabled/disabled" phrases. */
}

/**
 * Return results for translating phrases into a boolean.
 * Note these can be read as bools. Ex:
 * new PhraseToBool:ptb = TranslatePhraseToBool(...);
 * if (ptb != PhraseToBool_Neither) new bool:b = bool:ptb;
 */
enum PhraseToBool
{
    PhraseToBool_False = 0, /** The phrase is equivalent to boolean 'false'. */
    PhraseToBool_True = 1,  /** The phrase is equivalent to boolean 'true'. */
    PhraseToBool_Neither    /** The phrase is equivalent to neither boolean value. */
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

/**
 * List of stock translations files that come with the project.
 */
enum ProjectTransFiles
{
    TransFile_ProjectBase,    /** projectbase.phrases.txt, contains phrases used by the base project files. */
    TransFile_Project,        /** <projectshortname>.phrases.txt, contains phrases general translations used by the plugin. */
    TransFile_Module,         /** User-created translation file for a module. */
}

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
    Project_RegServerCmd("reload_translations", Command_ReloadTranslations, "Reload all project translation files.");
}

/**
 * A module was just registered.  This is being called before the module has been assigned a module identifier.
 * 
 * @param adtModule The adt array of the module being registered.
 */
stock TransMgr_OnModuleRegister(Handle:adtModule)
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

/**
 * Command project_module_info is being called.
 * Print the module data allocated by the translation manager.
 * Note: |stock| tag will stop this function from being compiled if the project_module_info command
 * isn't working due to required base components not being included.
 * 
 * @param client    The client index the text is being printed to.
 * @param module    The module to print info for.
 * @param msgtype   The type of message to tell Translations Manager to print.
 */
stock TransMgr_OnPrintModuleInfo(client, Module:module, MsgTypes:msgtype)
{
    decl String:translationfile[PLATFORM_MAX_PATH];
    
    if (TransMgr_IsEnabled(module))
        TransMgr_BuildTranslationPath(TransFile_Module, module, translationfile, sizeof(translationfile), true);
    else
        Format(translationfile, sizeof(translationfile), "%t", "_None");
    
    // Print the module event info.
    TransMgr_PrintText(client, MsgFormat_None, msgtype, _, false, "TransMgr cmd base lvl2 modules info print", translationfile);
}

// **********************************************
//                Public API
// **********************************************

/**
 * Register your module to use translations.
 * Your module's translation file path is defined as: project/<moduleshortname>.phrases.txt
 * 
 * @param module    The module identifier to register for translations.
 */
stock TransMgr_Register(Module:module)
{
    TransMgr_SetEnabled(module, true);
}

/**
 * Print text to a client with special formatting and translated text.
 * 
 * @param client    The client index.  Use SERVER_INDEX when printing to the server.
 * @param msgformat How to format the text before sending.  See enum MsgFormatting.
 * @param msgtype   The type of message to print to the client.  See enum MsgTypes.
 * @param module    The module this is coming from.  Needed for MsgFormat_Module or MsgFormat_Both.
 * @param literal   True to print the last variable as a string, instead of translating it.
 * @param any       The translations phrase or literal string followed by formatting parameters.
 */
stock TransMgr_PrintText(client, MsgFormatting:msgformat, MsgTypes:msgtype, Module:module = INVALID_MODULE, bool:literal = false, any:...)
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
    
    // Update the part of the array that contains the info for coloring text with the current reply source.
    g_bMsgTypeColor[MsgType_Reply] = bool:(GetCmdReplySource() == SM_REPLY_TO_CHAT);
    
    // Get the name of the module if a module identifier was given.
    new String:modulefullname[32] = "Invalid Module";
    if (ModuleMgr_IsModuleValid(module))
    {
        ModuleMgr_ReadString(module, ModuleData_FullName, modulefullname, sizeof(modulefullname));
    }
    
    // If this type of message supports color, then use the color formatting defines.
    if (g_bMsgTypeColor[msgtype])
    {
        switch(msgformat)
        {
            case MsgFormat_Plugin:  Format(translated, sizeof(translated), TM_MSGFORMAT_PLUGIN_COLOR);
            case MsgFormat_Module:  Format(translated, sizeof(translated), TM_MSGFORMAT_MODULE_COLOR);
            case MsgFormat_Both:    Format(translated, sizeof(translated), TM_MSGFORMAT_BOTH_COLOR);
        }
    }
    // This type of message doesn't support color.
    else
    {
        switch(msgformat)
        {
            case MsgFormat_Plugin:  Format(translated, sizeof(translated), TM_MSGFORMAT_PLUGIN_NOCOLOR);
            case MsgFormat_Module:  Format(translated, sizeof(translated), TM_MSGFORMAT_MODULE_NOCOLOR);
            case MsgFormat_Both:    Format(translated, sizeof(translated), TM_MSGFORMAT_BOTH_NOCOLOR);
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
stock TransMgr_PrintTextAll(bool:server, bool:admin, MsgFormatting:msgformat, MsgTypes:msgtype, Module:module = INVALID_MODULE, bool:literal = false, any:...)
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
        
        TransMgr_PrintText(SERVER_INDEX, msgformat, MsgType_Server, module, true, translated);
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
        
        TransMgr_PrintText(client, msgformat, msgtype, module, true, translated);
    }
}

/**
 * Translates a boolean value into a text phrase.
 * 
 * @param target        The client index.  LANG_SERVER to use server's language.
 * @param boolean       The boolean value to translate.
 * @param boolphrase    The boolean phrase pair to translate to.
 * @param phrase        The output string with the translated prase.
 * @param maxlen        The max length of the output string.
 * @param lowercase     True to force the output to lower case.
 */
stock TransMgr_TranslateBoolToPhrase(target, bool:boolean, BoolPhrases:boolphrase, String:phrase[], maxlen, bool:lowercase = false)
{
    SetGlobalTransTarget(target);
    
    switch (boolphrase)
    {
        case BoolPhrase_YesNo:
            boolean ? Format(phrase, maxlen, "%t", "Yes") : Format(phrase, maxlen, "%t", "No");
        
        case BoolPhrase_OnOff:
            boolean ? Format(phrase, maxlen, "%t", "On") : Format(phrase, maxlen, "%t", "Off");
        
        case BoolPhrase_EnabledDisabled:
            boolean ? Format(phrase, maxlen, "%t", "_Enabled") : Format(phrase, maxlen, "%t", "_Disabled");
    }
    
    // Force all chars to lowercase.
    if (lowercase)
    {
        for (new x = 0; x < strlen(phrase); x++)
        {
            phrase[x] = CharToLower(phrase[x]);
        }
    }
}

/**
 * Translates a boolean value into a text phrase.
 * 
 * @param target        The client index.  LANG_SERVER to use server's language.
 * @param boolphrase    The boolean phrase pair to translate from.
 * @param phrase        The bool phrase to translate to a bool.
 */
stock PhraseToBool:TransMgr_TranslatePhraseToBool(target, BoolPhrases:boolphrase, const String:phrase[])
{
    SetGlobalTransTarget(target);
    
    decl String:boolphrasetrue[16];
    decl String:boolphrasefalse[16];
    switch (boolphrase)
    {
        case BoolPhrase_YesNo:
        {
            Format(boolphrasetrue, sizeof(boolphrasetrue), "%t", "Yes");
            Format(boolphrasefalse, sizeof(boolphrasefalse), "%t", "No");
        }
        case BoolPhrase_OnOff:
        {
            Format(boolphrasetrue, sizeof(boolphrasetrue), "%t", "On");
            Format(boolphrasefalse, sizeof(boolphrasefalse), "%t", "Off");
        }
        case BoolPhrase_EnabledDisabled:
        {
            Format(boolphrasetrue, sizeof(boolphrasetrue), "%t", "_Enabled");
            Format(boolphrasefalse, sizeof(boolphrasefalse), "%t", "_Disabled");
        }
    }
    
    if (StrEqual(phrase, boolphrasetrue, false))
        return BoolToPhrase_True;
    
    if (StrEqual(phrase, boolphrasefalse, false))
        return BoolToPhrase_False;
    
    return BoolToPhrase_Neither;
}

// **********************************************
//   Private API (For base project files only)
// **********************************************

/**
 * Check if the specified module has translations enabled.
 * This is meant for other base project files to use, modules shouldn't need this.
 * 
 * @param module        The module to check.
 * 
 * @return              True if enabled, false otherwise. 
 */
stock bool:TransMgr_IsEnabled(Module:module)
{
    return bool:GetArrayCell(ModuleMgr_GetModuleArray(module), TRANSLATIONS_DATA_ENABLED);
}

/**
 * Change if translations is enabled for a module.
 * This is meant for other base project files to use, modules shouldn't need this.
 * 
 * @param module        The module to change.
 * @param enable        True to enable translations in this module, false to disable.
 */
stock TransMgr_SetEnabled(Module:module, bool:enable)
{
    SetArrayCell(ModuleMgr_GetModuleArray(module), TRANSLATIONS_DATA_ENABLED, _:enable);
}

/**
 * (Re)loads all project translation files.
 */
stock TransMgr_LoadTranslations()
{
    // Loads extra translations from the main project.inc file.
    TransMgr_LoadExtraTranslations();
    
    decl String:translationfile[PLATFORM_MAX_PATH];
    
    // Load project base component translations.
    TransMgr_BuildTranslationPath(TransFile_ProjectBase, _, translationfile, sizeof(translationfile));
    LoadTranslations(translationfile);
    
    // Load general plugin translations.
    TransMgr_BuildTranslationPath(TransFile_Project, _, translationfile, sizeof(translationfile));
    LoadTranslations(translationfile);
    
    new Module:module;
    
    // Loop through all the modules.
    for (new moduleindex = 0; moduleindex < MODULE_COUNT; moduleindex++)
    {
        // Read moduleindex as a Module type.
        module = Module:moduleindex;
        
        // If the module doesn't have translations enabled, then skip to next module identifier.
        if (!TransMgr_IsEnabled(module))
            continue;
        
        TransMgr_BuildTranslationPath(TransFile_Module, module, translationfile, sizeof(translationfile));
        LoadTranslations(translationfile);
    }
}

/**
 * Return a path to the project's, or module's, translation file.
 * 
 * @param transfile The translation file to build path to.  See enum ProjectTransFiles.
 * @param module    The module of the translation file that is sought.  Leave as INVALID_MODULE for a file not affiliated with any module.
 * @param path      The path to the translation file.
 * @param maxlen    The max length of the output string.
 * @param txt       True to return the .txt extension, false to ignore it.
 */
stock TransMgr_BuildTranslationPath(ProjectTransFiles:transfile, Module:module = INVALID_MODULE, String:path[], maxlen, bool:txt = false)
{
    
    switch (transfile)
    {
        case TransFile_ProjectBase: Format(path, maxlen, "%s/projectbase.phrases", PROJECT_SHORTNAME);
        case TransFile_Project:     Format(path, maxlen, "%s/%s.phrases", PROJECT_SHORTNAME, PROJECT_SHORTNAME);
        case TransFile_Module:
        {
            // Get the module's shortname to use as the translations name.
            decl String:shortname[16];
            ModuleMgr_ReadString(module, ModuleData_ShortName, shortname, sizeof(shortname));
            Format(path, maxlen, "%s/%s.phrases", PROJECT_SHORTNAME, shortname);
        }
    }
    
    if (txt)
        Format(path, maxlen, "%s.txt", path);
}

/**
 * Command callback: project_reload_translations
 * Reloads all translation files used in the project.
 * 
 * @param argc      The number of arguments that the server sent with the command.
 */
public Action:Command_ReloadTranslations(arc)
{
    TransMgr_LoadTranslations();
    
    TransMgr_PrintText(SERVER_INDEX, MsgFormat_Plugin, MsgType_Server, _, false, "TransMgr cmd reloadtranslations");
}
