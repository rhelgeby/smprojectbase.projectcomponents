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
    menu1 = MenuLib_CreateMenu("Menu1", "Menu1", false);
    menu2 = MenuLib_CreateMenu("Menu2", "Menu2", true);
    menu3 = MenuLib_CreateMenu("Menu3", "Menu3", false);
    menu4 = MenuLib_CreateMenu("Menu4", "Menu4", false);
    menu5 = MenuLib_CreateMenu("Menu5", "Menu5", false);
    menu6 = MenuLib_CreateMenu("Menu6", "Menu6", false);
    clientmenu = MenuLib_CreateMenu("ClientMenu", "Clients", false);
	
	// Test lookup
    new Handle:lookedup = MenuLib_FindMenuById("Menu1");
    if (menu1 == lookedup)  PrintToServer("Passed looked up test 1!");
    else                    PrintToServer("Failed looked up test 1!");
    
    MenuLib_AddMenuBtnEx(menu1, "Menu2", "", false, ITEMDRAW_DEFAULT, INVALID_FUNCTION, BtnNextMenu_LinkMenu, menu2);
    MenuLib_AddMenuBtnEx(menu1, "Menu3", "", false, ITEMDRAW_DEFAULT, INVALID_FUNCTION, BtnNextMenu_LinkMenu, menu3);
    MenuLib_AddMenuBtnEx(menu3, "Menu4", "", false, ITEMDRAW_DEFAULT, INVALID_FUNCTION, BtnNextMenu_LinkMenu, menu4);
    MenuLib_AddMenuBtnEx(menu4, "Menu5", "", false, ITEMDRAW_DEFAULT, INVALID_FUNCTION, BtnNextMenu_LinkMenu, menu5);
    MenuLib_AddMenuBtnEx(menu5, "Menu6", "", false, ITEMDRAW_DEFAULT, INVALID_FUNCTION, BtnNextMenu_LinkMenu, menu6);
    MenuLib_AddMenuBtnEx(menu6, "All Clients", "", false, ITEMDRAW_DEFAULT, GetFunctionByName(GetMyHandle(), "PrepClients"), BtnNextMenu_LinkMenu, clientmenu);
    MenuLib_AddMenuBtnEx(menu6, "All Clients 2", "", false, ITEMDRAW_DEFAULT, GetFunctionByName(GetMyHandle(), "PrepClients2"), BtnNextMenu_LinkMenu, clientmenu);
    
	// Deletion test.
    //MenuLib_DeleteMenu(menu2, true);
    MenuLib_DeleteMenu(menu3, true);
    
    // Test lookup again.
    lookedup = MenuLib_FindMenuById("Menu2");
    if (menu2 == lookedup)  PrintToServer("Passed looked up test 2!");
    else                    PrintToServer("Failed looked up test!");
    
    lookedup = MenuLib_FindMenuById("clientmenu");
    if (lookedup == INVALID_HANDLE) PrintToServer("Passed looked up test 3!");
    else                            PrintToServer("Failed looked up test!");
    
    HookEvent("player_spawn", PlayerSpawn);
}

public PrepClients(Handle:hMenu, MenuAction:action, client, slot)
{
    MenuLib_GenerateClientMenu(client, clientmenu, "ClientListHandler", BtnNextMenu_LinkCurrent, INVALID_HANDLE, UTILS_FILTER_ALIVE);
}

public PrepClients2(Handle:hMenu, MenuAction:action, client, slot)
{
    MenuLib_GenerateClientMenu(client, clientmenu, "ClientListHandler", BtnNextMenu_LinkBack, INVALID_HANDLE, UTILS_FILTER_ALIVE);
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
