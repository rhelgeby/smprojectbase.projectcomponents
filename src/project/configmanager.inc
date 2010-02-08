/*
 * ============================================================================
 *
 *  Project
 *
 *  File:          config.inc
 *  Type:          Base
 *  Description:   Handles all project configs.
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
 * Provides the plugin a way to know if the config manager is included in the project.
 */
#define CONFIG_MANAGER

// ---------------
//     Public
// ---------------

/**
 * The max number of config files a single module can maintain.
 */
#define CM_MODULE_MAX_CONFIGS 3

/**
 * The max size of each of a module's config paths.
 */
#define CM_MODULE_CONFIG_PATH_MAX 128

/**
 * @section Default defines for each config file index.
 */
#define CM_CONFIGINDEX_FIRST    0
#define CM_CONFIGINDEX_SECOND   1
#define CM_CONFIGINDEX_THIRD    2
/**
 * @endsection
 */

/**
 * Config registration errors.
 */
enum ConfigRegistrationResults
{
    RegResult_Success = 0,          /** Config file registered successfully. */
    RegResult_LimitExceeded = -1,   /** The limit on configs/module has been exceeded. */
    RegResult_Invalid = -2,         /** Non-existant config file. */
    RegResult_Duplicate = -3        /** This config file is already registered. */
}

/**
 * Tell the KV cacher what to do.
 */
enum KvCache
{
    KvCache_Continue,   /** Let the cacheing continue. */
    KvCache_Ignore,     /** Pretend this section never existed.  Won't be counted in the return value either. */
    KvCache_Hault       /** Stop cacheing early.  The section currently being cached will be counted. */
}

// ---------------
//     Private
// ---------------

/**
 * The max number of cells needed for each of the config manager's allocated indexes.
 */
#define CM_DATA_MAX_CELLS CM_MODULE_CONFIG_PATH_MAX
// Config manager needs the max number of configs multiplied by the max length of each path.

/**
 * Defines the block of data in the module data arrays that contains config path data.
 */
#define CONFIG_DATA_PATH(%1) g_iCMAllocatedIndexes[%1]

/**
 * Array to store the indexes of the allocated space in the module data arrays for the config manager.
 */
new g_iCMAllocatedIndexes[CM_MODULE_MAX_CONFIGS];

// **********************************************
//                 Forwards
// **********************************************

/**
 * Plugin has loaded.
 */
ConfigMgr_OnPluginStart()
{
    // Allocate 1 index for the data we want to store for each module.
    ModuleMgr_Allocate(CM_MODULE_MAX_CONFIGS, g_iCMAllocatedIndexes);
}

/**
 * A module was just registered.  This is being called before the module has been assigned a module identifier.
 * 
 * @param adtModule The adt array of the module being registered.
 */
stock ConfigMgr_OnModuleRegister(Handle:adtModule)
{
    // Push the config path strings for a module.
    // This is being pushed into our allocated space for config path data.
    for (new pindex = 0; pindex < CM_MODULE_MAX_CONFIGS; pindex++)
        PushArrayString(adtModule, "");
}

/**
 * Base command is printing a module's info.
 * Print the module data allocated by the event manager.
 * Note: |stock| tag will stop this function from being compiled if the base command is disabled.
 * 
 * @param client    The client index the text is being printed to.
 * @param module    The module to print info for.
 * @param msgtype   The type of message to tell Translations Manager to print.
 */
stock ConfigMgr_OnPrintModuleInfo(client, Module:module, MsgTypes:msgtype)
{
}

// **********************************************
//                Public API
// **********************************************

/**
 * Register a config file.
 * 
 * @param module        The module identifier
 * @param configpath    The path to the config file relative to sourcemod/.
 * @param configindex   The index (starting from 0) of this module's config file.
 * 
 * @return              RegResult_Success on success.  See enum ConfigRegistrationResults for other results.
 */
stock ConfigRegistrationResults:ConfigMgr_Register(Module:module, const String:configpath[], &configindex = -1)
{
    decl String:modulefullname[32];
    ModuleMgr_ReadString(module, ModuleData_FullName, modulefullname, sizeof(modulefullname));
    
    // Check if the module has registered the max number of config files.
    configindex = ConfigMgr_FindNextIndex(module);
    if (configindex == -1)
    {
        LogError("[Config Manager] Module \"%s\" can't register more than %d config files!", modulefullname, CM_MODULE_MAX_CONFIGS);
        return RegResult_LimitExceeded;
    }
    
    // Check if the file is valid.
    decl String:fullconfigpath[PLATFORM_MAX_PATH];
    BuildPath(Path_SM, fullconfigpath, sizeof(fullconfigpath), configpath);
    if (!FileExists(fullconfigpath))
    {
        LogError("[Config Manager] Module \"%s\" trying to register a non-existant config file! (%s)", modulefullname, fullconfigpath);
        return RegResult_Invalid;
    }
    
    // Check if this path is already registered.
    if (ConfigMgr_PathToIndex(module, configpath) > -1)
    {
        LogError("[Config Manager] Module \"%s\" can't register a config file (%s) more than once!", modulefullname, configpath);
        return RegResult_Duplicate;
    }
    
    // Write the config path in the module's config path data.
    ConfigMgr_WriteConfigPath(module, configindex, configpath);
    
    return RegResult_Success;
}

/**
 * This function only works for the following KV format:
 * "root"
 * {  
 *     "sectionname"
 *     {
 *         "option1"   "anyvalue"
 *         "option2"   "anyvalue"
 *         ...
 *     }
 *     "sectionname2"
 *     {
 *         "option1"   "anyvalue"
 *         "option2"   "anyvalue"
 *         ...
 *     }
 * }
 * This function will simply loop through each section name and then
 * call the function given to give it a chance to cache each option within it.
 * 
 * Cache function prototype:
 * 
 * @param kv            The keyvalues handle of the config file. (Don't close this)
 * @param sectionindex  The index of the current keyvalue section, starting from 0.
 * @param sectionname   The name of the current keyvalue section.
 * 
 * @return              See enum KvCache.
 *  
 * public KvCache:funcCache(Handle:kv, sectionindex, const String:sectionname[])
 *  
 * Note: SM has plans to remove the need for 'public' on this private function.
 * 
 * @param module        The module identifier to get the config info from.
 * @param configindex   The index, starting from 0, of the config file path.
 * @param funcCache     The name of the function that will be doing the caching. (see prototype above)
 * 
 * @return              The number of sections in the keyvalue file.  
 */
stock ConfigMgr_CacheKv(Module:module, configindex, const String:funcCache[])
{
    decl String:configpath[CM_MODULE_CONFIG_PATH_MAX];
    ConfigMgr_ReadConfigPath(module, configindex, configpath, sizeof(configpath));
    
    decl String:fullconfigpath[PLATFORM_MAX_PATH];
    BuildPath(Path_SM, fullconfigpath, sizeof(fullconfigpath), configpath);
    
    new Handle:kv = CreateKeyValues("");
    if (!FileToKeyValues(kv, fullconfigpath))
    {
        LogError("Keyvalue config file \"%s\" couldn't be loaded.", fullconfigpath);
        return 0;
    }
    
    KvRewind(kv);
    if (!KvGotoFirstSubKey(kv))
        return 0;
    
    new count;
    decl String:sectionname[64];
    do
    {
        // Get the sectionname to push to the cache func.
        KvGetSectionName(kv, sectionname, sizeof(sectionname));
        
        // Forward to the cache function.
        Call_StartFunction(GetMyHandle(), GetFunctionByName(GetMyHandle(), funcCache));
        
        // Push parameters.
        Call_PushCell(kv);
        Call_PushCell(count);
        Call_PushString(sectionname);
        
        // Finish
        new KvCache:result;
        Call_Finish(result);
        
        count++;
        
        switch (result)
        {
            case KvCache_Ignore: { count--; continue; }
            case KvCache_Hault: break;
        }
    } while (KvGotoNextKey(kv));
    
    CloseHandle(kv);
    
    return count;
}

/**
 * Caches each line of a text file into an adt array.
 * 
 * @param module        The module identifier.
 * @param configindex   The index, starting from 0, of the config file path.
 * @param maxlen        The max length of each string.
 * @param count         This will be set to the number of lines cached.
 * 
 * @return              A handle to a newly created adt array.  INVALID_HANDLE on failure.
 *                      Don't forget to close this when you're done!
 */
stock Handle:ConfigMgr_CacheFile(Module:module, configindex, maxlen, &count = 0)
{
    decl String:configpath[CM_MODULE_CONFIG_PATH_MAX];
    ConfigMgr_ReadConfigPath(module, configindex, configpath, sizeof(configpath));
    
    decl String:fullconfigpath[PLATFORM_MAX_PATH];
    BuildPath(Path_SM, fullconfigpath, sizeof(fullconfigpath), configpath);
    
    new Handle:hConfig = OpenFile(configpath, "r");
    if (hConfig == INVALID_HANDLE)
    {
        LogError("Config file \"%s\" couldn't be loaded.", fullconfigpath);
        return INVALID_HANDLE;
    }
    
    // Create the array we are going to return.
    new Handle:hCache = CreateArray(maxlen);
    
    decl String:line[maxlen];
    count = 0;
    while(ReadFileLine(hConfig, line, maxlen))
    {
        // Cut out comments at the end of a line.
        if (StrContains(line, "//") > -1)
        {
            SplitString(line, "//", line, maxlen);
        }
        TrimString(line);
        
        if (line[0] == 0)
            continue;
        
        // Push line into array.
        PushArrayString(hCache, line);
        count++;
    }
    
    // We're done this file, so we can destory it from memory. 
    CloseHandle(hConfig);
    
    // Return the handle to the cache.
    return hCache;
}

// **********************************************
//   Private API (For base project files only)
// **********************************************

/**
 * Finds the next available index to store a config path.
 * 
 * @param module    The module identifier.
 * 
 * @return          The next available index.  -1 if none are available.
 */
stock ConfigMgr_FindNextIndex(Module:module)
{
    new String:configpaths[CM_MODULE_MAX_CONFIGS][CM_MODULE_CONFIG_PATH_MAX];
    ConfigMgr_ReadConfigPaths(module, configpaths);
    
    // Loop through each config path index.
    for (new cpindex = 0; cpindex < sizeof(configpaths); cpindex++)
    {
        if (configpaths[cpindex][0] == 0)
            return cpindex;
    }
    
    return -1;
}

/**
 * Returns the config path index given the path.
 * 
 * @param module    The module to get the path index for.
 * @param path      The path to the config file. (relative to sourcemod)
 * 
 * @return      The config path index.  -1 if the path doesn't exist.
 */
stock ConfigMgr_PathToIndex(Module:module, const String:configpath[])
{
    new String:configpaths[CM_MODULE_MAX_CONFIGS][CM_MODULE_CONFIG_PATH_MAX];
    ConfigMgr_ReadConfigPaths(module, configpaths);
    
    // Loop through each config path index.
    for (new cpindex = 0; cpindex < sizeof(configpaths); cpindex++)
    {
        if (StrEqual(configpath, configpaths[cpindex], false))
            return cpindex;
    }
    
    return -1;
}

/**
 * Reads a config path registered by a module.
 * 
 * @param module        The module to check.
 * @param configindex   The index of the config path starting from 0 and ending at CM_MODULE_MAX_CONFIGS - 1. 
 * @param configpath    The path to the config file.
 * @param maxlen        The max length of the output string.
 */
stock ConfigMgr_ReadConfigPath(Module:module, configindex, String:configpath[], maxlen)
{
    // Get the config path.
    GetArrayString(ModuleMgr_GetModuleArray(module), CONFIG_DATA_PATH(configindex), configpath, maxlen);
}

/**
 * Reads the config paths registered by a module.
 * 
 * @param module        The module to check.
 * @param configpaths   The paths to all the module's registered configs.
 */
stock ConfigMgr_ReadConfigPaths(Module:module, String:configpaths[][])
{
    // Get all of the config paths.
    for (new pindex = 0; pindex < CM_MODULE_MAX_CONFIGS; pindex++)
        GetArrayString(ModuleMgr_GetModuleArray(module), CONFIG_DATA_PATH(pindex), configpaths[pindex], CM_MODULE_CONFIG_PATH_MAX);
}

/**
 * Sets a config path for module.
 * 
 * @param module        The module whose config path to set.
 * @param configindex   The index of the config path starting from 0 and ending at CM_MODULE_MAX_CONFIGS - 1. 
 * @param configpath    The path to the config file.
 */
stock ConfigMgr_WriteConfigPath(Module:module, configindex, const String:configpath[])
{
    // Get the config path.
    SetArrayString(ModuleMgr_GetModuleArray(module), CONFIG_DATA_PATH(configindex), configpath);
}

/**
 * Reads the config paths registered by a module.
 * 
 * @param module        The module to check.
 * @param configpaths   The paths to all the module's registered configs.
 */
stock ConfigMgr_WriteConfigPaths(Module:module, String:configpaths[][])
{
    // Set all of the config paths.
    for (new pindex = 0; pindex < CM_MODULE_MAX_CONFIGS; pindex++)
        SetArrayString(ModuleMgr_GetModuleArray(module), CONFIG_DATA_PATH(pindex), configpaths[pindex]);
}
