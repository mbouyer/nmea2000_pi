/******************************************************************************
 * $Id: ocpnnmea2000_pi.cpp,v 1.0 2011/02/26 01:54:37 nohal Exp $
 *
 * Project:  OpenCPN
 * Purpose:  OpenCPNNMEA2000 Plugin
 * Author:   Manuel Bouyer
 *
 ***************************************************************************
 *   Copyright (C) 2012 by Manuel Bouyer
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

bool nmea2000_attitude_rx::handle(const nmea2000_frame &f)
{
	NMEA0183 m_NMEA0183;
	SENTENCE snt;
	int rad = f.frame2int16(1);

	m_NMEA0183.TalkerID = _T("HC");
	m_NMEA0183.Hdm.DegreesMagnetic = rad2deg(rad);
	m_NMEA0183.Hdm.Write(snt);

	PushNMEABuffer(snt.Sentence);
	return true;
}
