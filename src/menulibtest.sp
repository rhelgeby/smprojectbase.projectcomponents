#pragma semicolon 1

#include <sourcemod>
#include "libraries/menulib"

new Handle:menu1;
//new Handle:menu2;
new Handle:menu3;
new Handle:menu4;
new Handle:menu5;
new Handle:menu6;
new Handle:menu7;
 
public OnPluginStart()
{
    menu1 = MenuLib_CreateMenu("Menu1", INVALID_FUNCTION, MenuCallback, "Menu1", false, false, false);
    MenuLib_CreateMenu("Menu2", INVALID_FUNCTION, MenuCallback, "Menu2", false, false, false);     /*menu2 = */
    menu3 = MenuLib_CreateMenu("Menu3", INVALID_FUNCTION, MenuCallback, "Menu3", false, false, false);
    menu4 = MenuLib_CreateMenu("Menu4", INVALID_FUNCTION, MenuCallback, "Menu4", false, false, false);
    menu5 = MenuLib_CreateMenu("Menu5", INVALID_FUNCTION, MenuCallback, "Menu5", false, false, false);
    menu6 = MenuLib_CreateMenu("Menu6", INVALID_FUNCTION, MenuCallback, "Menu6", false, false, false);
    menu7 = MenuLib_CreateMenu("Menu7", INVALID_FUNCTION, MenuCallback2, "Menu7", false, true, false);
    
    MenuLib_AddMenuBtnEx(menu1, "Menu2", "", false, ITEMDRAW_DEFAULT, INVALID_FUNCTION, BtnNextMenu_LinkMenu, "Menu2");
    MenuLib_AddMenuBtnEx(menu1, "Menu3", "", false, ITEMDRAW_DEFAULT, INVALID_FUNCTION, BtnNextMenu_LinkMenu, "Menu3");
    MenuLib_AddMenuBtnEx(menu3, "Menu4", "", false, ITEMDRAW_DEFAULT, INVALID_FUNCTION, BtnNextMenu_LinkMenu, "Menu4");
    MenuLib_AddMenuBtnEx(menu4, "Menu5", "", false, ITEMDRAW_DEFAULT, INVALID_FUNCTION, BtnNextMenu_LinkMenu, "Menu5");
    MenuLib_AddMenuBtnEx(menu5, "Menu6", "", false, ITEMDRAW_DEFAULT, INVALID_FUNCTION, BtnNextMenu_LinkMenu, "Menu6");
    MenuLib_AddMenuBtnEx(menu6, "All Clients", "", false, ITEMDRAW_DEFAULT, PrepClients, BtnNextMenu_None, "");
    MenuLib_AddMenuBtnEx(menu6, "All Clients 2", "", false, ITEMDRAW_DEFAULT, PrepClients2, BtnNextMenu_None, "");
    
	// Deletion test.
    //MenuLib_DeleteMenu(menu2, true);
    //MenuLib_DeleteMenu(menu3, true);
    
    HookEvent("player_spawn", PlayerSpawn);
}

public MenuCallback(Handle:hMenu, MenuAction:action, client, slot)
{
    if (action == MenuAction_Select)
    {
        PrintToServer("Action: %d Client: %N", action, client);
    }
}

public MenuCallback2(Handle:hMenu, MenuAction:action, client, slot)
{
    if (action == MenuAction_Cancel)
    {
        if (slot == MenuCancel_ExitBack)
        {
            new Handle:hClientMenu = MenuLib_CreateClientListMenu(client, "Clients", false, false, INVALID_FUNCTION, INVALID_FUNCTION, BtnNextMenu_LinkMenu, "Menu7", UTILS_FILTER_ALIVE, ClientListFilter);
            MenuLib_SendMenu(hClientMenu, client);
        }
    }
}

public PrepClients(Handle:hMenu, client, slot)
{
    new Handle:hClientMenu = MenuLib_CreateClientListMenu(client, "Clients", false, false, INVALID_FUNCTION, INVALID_FUNCTION, BtnNextMenu_LinkMenu, "Menu7", UTILS_FILTER_ALIVE, ClientListFilter);
    MenuLib_SendMenu(hClientMenu, client, hMenu);
}

public PrepClients2(Handle:hMenu, client, slot)
{
    new Handle:hClientMenu = MenuLib_CreateClientListMenu(client, "Clients", false, false, INVALID_FUNCTION, ClientListHandler, BtnNextMenu_LinkBack, "", UTILS_FILTER_ALIVE);
    MenuLib_SendMenu(hClientMenu, client, hMenu);
}

public PlayerSpawn(Handle:event, const String:name[], bool:dontBroadcast)
{
    new client = GetClientOfUserId(GetEventInt(event, "userid"));
    MenuLib_DisplayMenu(menu1, client);
}

public Action:ClientListFilter(Handle:menu, client, String:buttontxt[], String:buttoninfo[])
{
    Format(buttontxt, 256, "<%s>", buttontxt);
    return Plugin_Changed;
}

public ClientListHandler(Handle:hMenu, MenuAction:action, client, button)
{
    if (action == MenuAction_Select)
    {
        PrintToChat(client, "Selected client %N", MenuLib_GetClientIndex(hMenu, button));
    }
}
