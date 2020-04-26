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

static wxString last_To;
static uint32_t wp_id;

void n2k_navdata_tx::nmeasentence(NMEA0183  *nmea0183, int sock, uint8_t sid)
{
	uint8_t byte;
	int16_t speed;
	double error;

	if (nmea0183->LastSentenceIDParsed == _T("APB")) {
		apb_ok = true;
	} else if (nmea0183->LastSentenceIDParsed == _T("RMB")) {
		rmb_ok = true;
	} else {
		return;
	}
	if (apb_ok == false || rmb_ok == false || current_bearing < 0) {
		return;
	}

	/* check that data are coherent */
	if (nmea0183->Apb.To != nmea0183->Rmb.To)
		return;

	if (last_To != nmea0183->Apb.To) {
		wp_id++;
		last_To = nmea0183->Apb.To;
	}

	uint82frame(sid, 0);
	uint322frame(nmea0183->Rmb.RangeToDestinationNauticalMiles * 185200.0, 1);
	byte = 0 << 6; /* true */
	if (nmea0183->Apb.IsPerpendicular == NTrue)
		byte |= 1 << 4;
	if (nmea0183->Apb.IsArrivalCircleEntered == NTrue)
		byte |= 1 << 2;
	/* byte |= 0 << 0 calculation type */
	uint82frame(byte, 5);
	uint322frame(0, 6); // ETA Time
	uint162frame(0, 10); // ETA Date
	uint162frame(udeg2rad(nmea0183->Apb.BearingOriginToDestination), 12);
	uint162frame(udeg2rad(nmea0183->Apb.BearingPresentPositionToDestination), 14);
	uint322frame(1, 16); // Origin Waypoint Number
	uint322frame(wp_id, 20); // Destination Waypoint Number
	int322frame(0, 24); // Destination Latitude
	int322frame(0, 28); // Destination Longitude
	error = abs(nmea0183->Apb.BearingPresentPositionToDestination - current_bearing);
	/*
	 * DestinationClosingVelocityKnots is in fact SOG - this may be
	 * an error in opencpn
	 */
	speed = cos(error / 180.0 * 3.1415927) * current_sog * 1852.0 / 36.0;
	int162frame(speed, 32); // Destination Longitude
	valid = true;

	send(sock);
	apb_ok = rmb_ok = false;
}

void n2k_navdata_tx::positionfix(PlugIn_Position_Fix_Ex *fix, int sock, uint8_t sid) {
	current_bearing = fix->Cog;
	current_sog = fix->Sog;
}
