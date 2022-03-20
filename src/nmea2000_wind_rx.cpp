/******************************************************************************
 * $Id: ocpnnmea2000_pi.cpp,v 1.0 2011/02/26 01:54:37 nohal Exp $
 *
 * Project:  OpenCPN
 * Purpose:  OpenCPNNMEA2000 Plugin
 * Author:   Manuel Bouyer
 *
 ***************************************************************************
 *   Copyright (C) 2021 by Manuel Bouyer
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

#include "NMEA2000.h"
#include "nmea2000_defs_rx.h"
#include "nmea0183.h"
#include "ocpn_plugin.h"

bool nmea2000_wind_rx::handle(const nmea2000_frame &f)
{
	NMEA0183 m_NMEA0183;
	SENTENCE snt;
	uint16_t speed = f.frame2uint16(1);
	uint16_t dir = f.frame2uint16(3);
	uint8_t ref = f.frame2uint16(5);

	if (ref == 2) 
		m_NMEA0183.Mwv.Reference = _T("R");
	else if (ref == 3) 
		m_NMEA0183.Mwv.Reference = _T("T");
	else
		return true;
	m_NMEA0183.TalkerID = _T("WI");
	m_NMEA0183.Mwv.WindAngle = urad2deg(dir);
	m_NMEA0183.Mwv.WindSpeed = (double)speed / 100.0;
	m_NMEA0183.Mwv.WindSpeedUnits = _T("M");
	m_NMEA0183.Mwv.IsDataValid = NTrue;
	m_NMEA0183.Mwv.Write(snt);

	PushNMEABuffer(snt.Sentence);
	return true;
}
