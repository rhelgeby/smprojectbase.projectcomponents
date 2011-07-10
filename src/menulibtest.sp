#pragma semicolon 1

#include <sourcemod>
#include "libraries/menulib"

new Handle:menu1;
//new Handle:menu2;
new Handle:menu3;
new Handle:menu4;
new Handle:menu5;
new Handle:menu6;

public OnPluginStart()
{
    menu1 = MenuLib_CreateMenu("Menu1", "Menu1", false, false);
    MenuLib_CreateMenu("Menu2", "Menu2", true, false);     /*menu2 = */
    menu3 = MenuLib_CreateMenu("Menu3", "Menu3", false, false);
    menu4 = MenuLib_CreateMenu("Menu4", "Menu4", false, false);
    menu5 = MenuLib_CreateMenu("Menu5", "Menu5", false, false);
    menu6 = MenuLib_CreateMenu("Menu6", "Menu6", false, false);
    //clientmenu = MenuLib_CreateMenu("ClientMenu", "Clients", false, false);
    
    MenuLib_AddMenuBtnEx(menu1, "Menu2", "", false, ITEMDRAW_DEFAULT, INVALID_FUNCTION, BtnNextMenu_LinkMenu, "Menu2");
    MenuLib_AddMenuBtnEx(menu1, "Menu3", "", false, ITEMDRAW_DEFAULT, INVALID_FUNCTION, BtnNextMenu_LinkMenu, "Menu3");
    MenuLib_AddMenuBtnEx(menu3, "Menu4", "", false, ITEMDRAW_DEFAULT, INVALID_FUNCTION, BtnNextMenu_LinkMenu, "Menu4");
    MenuLib_AddMenuBtnEx(menu4, "Menu5", "", false, ITEMDRAW_DEFAULT, INVALID_FUNCTION, BtnNextMenu_LinkMenu, "Menu5");
    MenuLib_AddMenuBtnEx(menu5, "Menu6", "", false, ITEMDRAW_DEFAULT, INVALID_FUNCTION, BtnNextMenu_LinkMenu, "Menu6");
    MenuLib_AddMenuBtnEx(menu6, "All Clients", "", false, ITEMDRAW_DEFAULT, GetFunctionByName(GetMyHandle(), "PrepClients"), BtnNextMenu_None, "");
    MenuLib_AddMenuBtnEx(menu6, "All Clients 2", "", false, ITEMDRAW_DEFAULT, GetFunctionByName(GetMyHandle(), "PrepClients2"), BtnNextMenu_None, "");
    
	// Deletion test.
    //MenuLib_DeleteMenu(menu2, true);
    //MenuLib_DeleteMenu(menu3, true);
    
    HookEvent("player_spawn", PlayerSpawn);
}

public PrepClients(Handle:hMenu, MenuAction:action, client, slot)
{
    MenuLib_SendClientListMenu(client, "Clients", false, "ClientListHandler", BtnNextMenu_None, "", UTILS_FILTER_ALIVE, "ClientListFilter");
}

public PrepClients2(Handle:hMenu, MenuAction:action, client, slot)
{
    MenuLib_SendClientListMenu(client, "Clients", false, "ClientListHandler2", BtnNextMenu_LinkBack, "", UTILS_FILTER_ALIVE);
}

public PlayerSpawn(Handle:event, const String:name[], bool:dontBroadcast)
{
    new client = GetClientOfUserId(GetEventInt(event, "userid"));
    MenuLib_DisplayMenu(menu1, client);
}

public Action:ClientListFilter(client, String:buttontxt[], String:buttoninfo[])
{
    Format(buttontxt, 256, "<%s>", buttontxt);
    return Plugin_Changed;
}

public ClientListHandler(Handle:hMenu, MenuAction:action, client, button)
{
    PrintToChat(client, "Selected client %N", MenuLib_GetClientIndex(hMenu, button));
    MenuLib_SendClientListMenu(client, "Clients", false, "ClientListHandler", BtnNextMenu_None, "", UTILS_FILTER_ALIVE, "ClientListFilter");
}

public ClientListHandler2(Handle:hMenu, MenuAction:action, client, button)
{
    PrintToChat(client, "Selected client %N", MenuLib_GetClientIndex(hMenu, button));
}
