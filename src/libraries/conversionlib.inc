/*
 * ============================================================================
 *
 *  <Project Name>
 *
 *  File:          conversionlib.inc
 *  Type:          Library
 *  Description:   A library for converting units into other units.
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

#if defined _conversionlib_included
 #endinput
#endif
#define _conversionlib_included

enum ConvertLibUnits
{
    ConvertLibUnit_GameUnit,
    ConvertLibUnit_Inch,
    ConvertLibUnit_Foot,
    ConvertLibUnit_Yard,
    ConvertLibUnit_Mile,
    ConvertLibUnit_Milimeter,
    ConvertLibUnit_Centimeter,
    ConvertLibUnit_Meter,
    ConvertLibUnit_Kilometer
}

/**
 * Conversion table multiplers between all units.
 * The compiler doesn't like when I index the 2nd dimension with ConvertLibUnits.
 * I bypass this by indexing it as normal integer elements and then reading ConvertLibUnits as an int for the 2nd dimension.
 */
new const Float:g_fConversionFactors[ConvertLibUnits][] =
{
    {   1.0, 0.75, 9.0, 27.0, 47520.0, 19.05, 1.905, 0.01905, 0.00001905                }, // Game units to x
    {   1.3333, 1.0, 0.08333, 0.027778, 0.0000157, 25.4, 2.54, 0.0254, 0.0000254        }, // Inches to x
    {   0.1111, 12.0, 1.0, 0.33333, 0.0001893, 304.8, 30.48, 0.3048, 0.0003048          }, // Feet to x
    {   0.037037, 36.0, 3.0, 1.0, 0.0005681, 914.4, 91.44, 0.9144, 0.0009144            }, // Yards to x
    {   0.000021, 93360.0, 5280.0, 1760.0, 1.0, 1609344.0, 160934.4, 1609.344, 1.609344 }, // Miles to x
    {   0.052493, 0.03937, 0.003281, 0.001094, 0.000000621, 1.0, 0.1, 0.001, 0.000001   }, // Milimeters to x
    {   0.52493, 0.3937, 0.03281, 0.01094, 0.00000621, 10.0, 1.0, 0.01, 0.00001         }, // Centimeters to x
    {   52.493, 39.37, 3.281, 1.094, 0.000621, 1000.0, 100.0, 1.0, 0.001                }, // Meters to x
    {   52493.0, 39370.0, 3281.0, 1094.0, 0.621, 1000000.0, 100000.0, 1000.0, 1.0       }  // Kilometers to x
};

/**
 * The abbreviation for each of the supported units.
 */
new const String:g_strUnits[ConvertLibUnits][] =
{
    "game units",
    "in",
    "ft",
    "yd",
    "mi",
    "mm",
    "cm",
    "m",
    "km"
};

/**
 * Convert a number of x units to a number of y units.
 * See enum ConvertLibUnits at top of file.
 * 
 * @param units_in      The units of the number being inputted.
 * @param units_out     The units to convert the number to.
 * @param number        The number with the units given in 'units_from'.
 * 
 * @return              Result with the units given in 'units_to'.
 */
stock Float:ConvertLib_Convert(ConvertLibUnits:units_in, ConvertLibUnits:units_out, Float:number)
{
    return number * g_fConversionFactors[units_in][_:units_out];
}

/**
 * Looks up the abbreviation for a given symbol from ConvertLibUnits
 * 
 * @param units     The symbol to look up abbreviation for.
 * @param abbrev    The output string for the abbreviation for the units.
 * @param maxlen    The max length of the abbreviation.
 */
stock ConvertLib_SymbolToAbbrev(ConvertLibUnits:units, String:abbrev[], maxlen)
{
    strcopy(abbrev, maxlen, g_strUnits[units]);
}

/**
 * Looks up the abbreviation for certain units.
 * 
 * @param abbrev    The abbreviation to find symbol for.
 * 
 * @return          The symbol for the abbreviation or -1 if symbol can't be found.
 */
stock ConvertLibUnits:ConvertLib_AbbrevToSymbol(const String:abbrev[])
{
    for (new uindex; uindex < sizeof(g_strUnits); uindex++)
    {
        if (StrEqual(abbrev, g_strUnits[ConvertLibUnits:uindex], false))
            return ConvertLibUnits:uindex;
    }
    
    return ConvertLibUnits:-1;
}

/**
 * Detects number and units in a string and outputs as any units.
 * 
 * @param number    The number with units to convert.  E.g. "10cm" or "5 in"
 * @param assume    Units to assume if valid units can't be detected.
 * @param units_out The units to convert to.
 * 
 * @return          The converted value.
 */
stock Float:ConvertLib_DetectAndConvert(const String:number[], ConvertLibUnits:assume, ConvertLibUnits:units_out)
{
    new chr;
    new length = strlen(number);
    
    // Extract number.
    decl String:strNumber[12];
    new Float:flNumber;
    for (chr = 0; chr < length; chr++)
    {
        if (!IsCharNumeric(number[chr]) && number[chr] != '.')
        {
            strcopy(strNumber, chr + 1, number);
            flNumber = StringToFloat(strNumber);
            break;
        }
    }
    
    // Extract units.
    decl String:strUnits[16];
    strcopy(strUnits, sizeof(strUnits), number[chr]);
    TrimString(strUnits);
    
    // Find symbol for abbreviation, if it fails then use value in 'assume'
    new ConvertLibUnits:detected_units = ConvertLib_AbbrevToSymbol(strUnits);
    if (_:detected_units == -1)
        detected_units = assume;
    
    return ConvertLib_Convert(detected_units, units_out, flNumber);
}
