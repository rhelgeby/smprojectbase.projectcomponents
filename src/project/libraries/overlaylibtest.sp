#pragma semicolon 1

#include <sourcemod>
#include ""

public OnPluginStart()
{
    
    RegConsoleCmd("addoverlay1", AddOverlay1);
    RegConsoleCmd("addoverlay2", AddOverlay2);
    RegConsoleCmd("addoverlay2", AddOverlay3);
    
    RegConsoleCmd("removeoverlay1", RemoveOverlay1);
    RegConsoleCmd("removeoverlay2", RemoveOverlay2);
    RegConsoleCmd("removeoverlay2", RemoveOverlay3);
}