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
#include "nmea2000_defs_tx.h"
#include "nmea0183.h"
#include "ocpn_plugin.h"

void n2k_xte_tx::nmeasentence(NMEA0183  *nmea0183, int sock, uint8_t sid)
{
	double dist;

	if (nmea0183->LastSentenceIDParsed == _T("APB")) {
		dist = nmea0183->Apb.CrossTrackErrorMagnitude * 185200.0;
		uint82frame(sid, 0);
		uint82frame(0, 1);
		if (nmea0183->Apb.DirectionToSteer == Right) {
			uint322frame( -dist, 2);
		} else {
			uint322frame( dist, 2);
		}
		valid = true;
		send(sock);
	}
}
