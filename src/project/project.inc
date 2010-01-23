/*
 * ============================================================================
 *
 *  Project
 *
 *  File:          project.inc
 *  Type:          Base
 *  Description:   Contains defines, enums, etc available to anywhere in the project.
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
 * The amount of registered modules.
 * All integers list than this (and greater than or equal to zero) are valid module IDs.
 */
#define MODULE_COUNT GetArraySize(ModuleManager_GetList())

/**
 * All the data modules need to register.
 * See ModuleManager_Register()
 */
enum ModuleData
{
    bool:ModuleData_Disabled = 0,       /** True if the module is disabled. */
    String:ModuleData_FullName[32],     /** Probably going to be used when displaying modules. */
    String:ModuleData_ShortName[16],    /** Probably going to be used for client interaction. */
    // gunna need more than this..
}
