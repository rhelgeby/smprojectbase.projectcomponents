/*
 * ============================================================================
 *
 *  Project
 *
 *  File:          versioninfo.inc
 *  Type:          Base
 *  Description:   Command to display Mercurial version information in-game.
 *
 *  Copyright (C) 2009-2010  Greyscale, Richard Helgeby
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
 * Provides the plugin a way to know if the versioninfo.inc is included in the project.
 */
#define PROJECT_VERSIONINFO

#include "project/hgversion"

/**
 * Plugin has loaded.
 */
public VersionInfo_OnPluginStart()
{
    // Define all the module's data as layed out by enum ModuleData in project.inc.
    new moduledata[ModuleData] = {false, "Version Information", "versioninfo", "Command (project_version) to display Mercurial version information in-game."};
    
    // Register the module with this data.
    ModuleMgr_Register(moduledata);
    
    // ************************
    // Done registering module.
    // ************************
    
    // Create a version information print command.
    RegConsoleCmd("project_version", Command_Version, "Prints version info about this project.");
}

/**
 * Command callback: project_version
 * 
 * @param client    The client index.  Or SERVER_INDEX if coming from the server.
 * @param argc      The number of arguments that the client sent with the command.
 */
public Action:Command_Version(client, argc)
{
    decl String:buffer[512];
    decl String:linebuffer[128];
    buffer[0] = 0;
    
    // %24s is a formatting token that lines up text automatically.
    #define FORMATSTRING "%24s: %s\n"
    
    Format(linebuffer, sizeof(linebuffer), "%s\n", PROJECT_VER_PRODUCT_NAME);
    StrCat(buffer, sizeof(buffer), linebuffer);
    
    Format(linebuffer, sizeof(linebuffer), "%s\n \n", PROJECT_VER_COPYRIGHT);
    StrCat(buffer, sizeof(buffer), linebuffer);
    
    Format(linebuffer, sizeof(linebuffer), FORMATSTRING, "Version", PROJECT_VER_VERSION);
    StrCat(buffer, sizeof(buffer), linebuffer);
    
    Format(linebuffer, sizeof(linebuffer), FORMATSTRING, "Compile date", PROJECT_VER_DATE);
    StrCat(buffer, sizeof(buffer), linebuffer);
    
    Format(linebuffer, sizeof(linebuffer), FORMATSTRING, "License", PROJECT_VER_LICENSE);
    StrCat(buffer, sizeof(buffer), linebuffer);
    
    Format(linebuffer, sizeof(linebuffer), FORMATSTRING, "Build", PROJECT_VER_REVISION);
    StrCat(buffer, sizeof(buffer), linebuffer);
    
    Format(linebuffer, sizeof(linebuffer), FORMATSTRING, "Development branch", PROJECT_VER_BRANCH);
    StrCat(buffer, sizeof(buffer), linebuffer);
    
    // Send back reply in the correct place.  (Chat, client console, server console)
    ReplyToCommand(client, buffer);
    
    // Say that we handled the command so CS:S doesn't see it and print "Unknown command"
    return Plugin_Handled;
}
