/***************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  NMEA2000 plugin Properties support
 * Author:   Manuel Bouyer
 *
 ***************************************************************************
 *   Copyright (C) 2017 by Manuel Bouyer
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,  USA.             *
 ***************************************************************************
 */

#ifndef __NMEA2000Properties__
#define __NMEA2000Properties__

#include <wx/arrstr.h>

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

class NMEA2000Properties
{
public:
    NMEA2000Properties( void );
    ~NMEA2000Properties( void );
    void SaveConfig( void );
private:
    void LoadConfig( void );
};

extern NMEA2000Properties *NMEA2000PropertiesP;
#endif // __NMEA2000Properties__
