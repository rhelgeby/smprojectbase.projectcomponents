#pragma semicolon 1

#include <sourcemod>
#include "libraries/overlaylib"

public OnConfigsExecuted()
{
    OverlayLib_Register("materials/overlays/zr/humans_win", 0, true);
    OverlayLib_Register("materials/overlays/zr/zombies_win", 0, true);
    OverlayLib_Register("materials/overlays/zr/cleric_zr_view", 0, true);
    
    RegConsoleCmd("addoverlay1", AddOverlay1);
    RegConsoleCmd("addoverlay2", AddOverlay2);
    RegConsoleCmd("addoverlay3", AddOverlay3);
    
    RegConsoleCmd("removeoverlay1", RemoveOverlay1);
    RegConsoleCmd("removeoverlay2", RemoveOverlay2);
    RegConsoleCmd("removeoverlay3", RemoveOverlay3);
}

public OnClientPutInServer(client)
{
    OverlayLib_ClientInit(client);
}

public OnClientDisconnect(client)
{
    OverlayLib_ClientDestroyInfo(client);
}

public Action:AddOverlay1(client, argc)
{
    OverlayLib_AddOverlay(client, 0);
    ReplyToCommand(client, "added humanswin priority %d", OverlayLib_ReadPriority(0));
    return Plugin_Handled;
}

public Action:AddOverlay2(client, argc)
{
    OverlayLib_AddOverlay(client, 1);
    ReplyToCommand(client, "added zombieswin priority %d", OverlayLib_ReadPriority(1));
    return Plugin_Handled;
}

public Action:AddOverlay3(client, argc)
{
    OverlayLib_AddOverlay(client, 2);
    ReplyToCommand(client, "added cleric priority %d", OverlayLib_ReadPriority(2));
    return Plugin_Handled;
}

public Action:RemoveOverlay1(client, argc)
{
    OverlayLib_RemoveOverlay(client, 0);
    ReplyToCommand(client, "removed humanswin");
    return Plugin_Handled;
}

public Action:RemoveOverlay2(client, argc)
{
    OverlayLib_RemoveOverlay(client, 1);
    ReplyToCommand(client, "removed zombieswin");
    return Plugin_Handled;
}

public Action:RemoveOverlay3(client, argc)
{
    OverlayLib_RemoveOverlay(client, 2);
    ReplyToCommand(client, "removed cleric");
    return Plugin_Handled;
}

public OnGameFrame()
{
    for (new client = 1; client < MaxClients; client++)
    {
        if (!IsClientInGame(client))
            continue;
        
        OverlayLib_UpdateClient(client);
    }
}
