#pragma semicolon 1

#include <sourcemod>
#include "libraries/menulib"

new Handle:menu1;
new Handle:menu2;
new Handle:menu3;
new Handle:menu4;
new Handle:menu5;
new Handle:menu6;
new Handle:clientmenu;

public OnPluginStart()
{
    menu1 = MenuLib_CreateMenu("Menu1");
    menu2 = MenuLib_CreateMenu("Menu2");
    menu3 = MenuLib_CreateMenu("Menu3");
    menu4 = MenuLib_CreateMenu("Menu4");
    menu5 = MenuLib_CreateMenu("Menu5");
    menu6 = MenuLib_CreateMenu("Menu6");
    clientmenu = MenuLib_CreateMenu("Clients");
    
    MenuLib_AddMenuBtnEx(menu1, "Menu2", "", ITEMDRAW_DEFAULT, INVALID_FUNCTION, BtnNextMenu_LinkBack, menu2);
    MenuLib_AddMenuBtnEx(menu1, "Menu3", "", ITEMDRAW_DEFAULT, INVALID_FUNCTION, BtnNextMenu_LinkMenu, menu3);
    MenuLib_AddMenuBtnEx(menu3, "Menu4", "", ITEMDRAW_DEFAULT, INVALID_FUNCTION, BtnNextMenu_LinkMenu, menu4);
    MenuLib_AddMenuBtnEx(menu4, "Menu5", "", ITEMDRAW_DEFAULT, INVALID_FUNCTION, BtnNextMenu_LinkMenu, menu5);
    MenuLib_AddMenuBtnEx(menu5, "Menu6", "", ITEMDRAW_DEFAULT, INVALID_FUNCTION, BtnNextMenu_LinkMenu, menu6);
    MenuLib_AddMenuBtnEx(menu6, "All Clients", "", ITEMDRAW_DEFAULT, GetFunctionByName(GetMyHandle(), "PrepClients"), BtnNextMenu_LinkMenu, clientmenu);
    MenuLib_AddMenuBtnEx(menu6, "All Clients 2", "", ITEMDRAW_DEFAULT, GetFunctionByName(GetMyHandle(), "PrepClients2"), BtnNextMenu_LinkMenu, clientmenu);
    
    HookEvent("player_spawn", PlayerSpawn);
}

public OnClientPutInServer(client)
{
}

public OnClientDisconnect(client)
{
}

public PrepClients(Handle:hMenu, MenuAction:action, client, slot)
{
    MenuLib_GenerateClientMenu(client, clientmenu, GetFunctionByName(GetMyHandle(), "ClientListHandler"), BtnNextMenu_LinkCurrent, INVALID_HANDLE, UTILS_FILTER_ALIVE);
}

public PrepClients2(Handle:hMenu, MenuAction:action, client, slot)
{
    MenuLib_GenerateClientMenu(client, clientmenu, GetFunctionByName(GetMyHandle(), "ClientListHandler"), BtnNextMenu_LinkBack, INVALID_HANDLE, UTILS_FILTER_ALIVE);
}

public PlayerSpawn(Handle:event, const String:name[], bool:dontBroadcast)
{
    new client = GetClientOfUserId(GetEventInt(event, "userid"));
    MenuLib_DisplayMenu(menu1, client);
}

public ClientListHandler(Handle:hMenu, MenuAction:action, client, buttonindex)
{
    PrintToChat(client, "Selected client %N", MenuLib_GetClientIndex(hMenu, buttonindex));
}
