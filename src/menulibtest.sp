#pragma semicolon 1

#include <sourcemod>
#include "libraries/menulib"

new Handle:menu1;
new Handle:menu2;
new Handle:menu3;
new Handle:menu4;
new Handle:menu5;
new Handle:menu6;

public OnPluginStart()
{
    menu1 = MenuLib_CreateMenu("Menu1");
    menu2 = MenuLib_CreateMenu("Menu2");
    menu3 = MenuLib_CreateMenu("Menu3");
    menu4 = MenuLib_CreateMenu("Menu4");
    menu5 = MenuLib_CreateMenu("Menu5");
    menu6 = MenuLib_CreateMenu("Menu6");
    
    MenuLib_AddMenuBtnEx(menu1, "Menu2", menu2);
    MenuLib_AddMenuBtnEx(menu2, "Nothing");
    MenuLib_AddMenuBtnEx(menu1, "Menu3", menu3);
    MenuLib_AddMenuBtnEx(menu3, "Menu4", menu4);
    MenuLib_AddMenuBtnEx(menu4, "Menu5", menu5);
    MenuLib_AddMenuBtnEx(menu5, "Menu6", menu6);
    MenuLib_AddMenuBtnEx(menu6, "Nothing");
    
    HookEvent("player_spawn", PlayerSpawn);
}

public OnClientPutInServer(client)
{
}

public OnClientDisconnect(client)
{
}

public PlayerSpawn(Handle:event, const String:name[], bool:dontBroadcast)
{
    new client = GetClientOfUserId(GetEventInt(event, "userid"));
    MenuLib_DisplayMenu(menu1, client, MENU_TIME_FOREVER);
}
