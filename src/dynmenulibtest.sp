#pragma semicolon 1
#include <sourcemod>
#include "libraries/dynmenulib"

public OnPluginStart()
{
    new menuData[DynMenuData];
    DynMenuLib_CreateMenu(menuData);
    DynMenuLib_DeleteMenu("");
}
