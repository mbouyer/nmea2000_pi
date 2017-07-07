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
#include "ocpnnmea2000_pi.h"

void n2k_datetime_tx::init()
{
    time_t t = time(NULL);
    struct tm lt = {0};

    localtime_r(&t, &lt);

    uint162frame(lt.tm_gmtoff / 60, 6);
}

int n2k_datetime_tx::update(int sock) {
	struct timeval tv;
	int time;

	if (gettimeofday(&tv, NULL) <0) {
		wxLogSysError(ocpnnmea2000_pi::ErrMsgPrefix() + _T("gettimeofday"));
		valid = 0;
		return 1000000;
	}
	if (tv.tv_sec != last_update.tv_sec) {
		last_update = tv;
		uint162frame(tv.tv_sec / (3600 * 24), 0 );
		time  = tv.tv_sec % (3600 * 24) * 10000;
		time += tv.tv_usec / 100;
		uint322frame(time, 2 );
		valid = 1;
		send(sock);
	}
	// return the time until next tick, in us */
	return (1000000 - tv.tv_usec);
}
