/******************************************************************************
 * $Id: ocpnnmea2000_pi.h,v 1.0 2011/02/26 01:54:37 nohal Exp $
 *
 * Project:  OpenCPN
 * Purpose:  OpenCPNNMEA2000 Plugin
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

#ifndef NMEA2000_FRAME_RX_H_
#define NMEA2000_FRAME_RX_H_
#include "nmea2000_frame.h"
#include "nmea2000_defs.h"
#include <array>

class nmea2000_frame_rx : public nmea2000_desc {
    public:
	inline nmea2000_frame_rx() :
	    nmea2000_desc(NULL, false, -1) { enabled = false; }

	inline nmea2000_frame_rx(const char *desc, bool isuser, int pgn) :
	    nmea2000_desc(desc, isuser, pgn) { enabled = false; }
	virtual ~nmea2000_frame_rx() {};

	virtual bool handle(const nmea2000_frame &) { return false;}
};

class nmea2000_attitude_rx : public nmea2000_frame_rx {
    public:
	inline nmea2000_attitude_rx() :
	    nmea2000_frame_rx("NMEA2000 attitude", true, NMEA2000_ATTITUDE) {};
	virtual ~nmea2000_attitude_rx() {};
	bool handle(const nmea2000_frame &f);
};

class nmea2000_rx {
    public:
	inline nmea2000_rx() {};

	bool handle(const nmea2000_frame &);
	const nmea2000_desc *get_byindex(u_int);
	int get_bypgn(int);
	void enable(u_int, bool);

    private:
	nmea2000_attitude_rx attitude;

	std::array<nmea2000_frame_rx *,1> frames_rx = { {
	    &attitude,
	} };
};

#endif // NMEA2000_FRAME_RX_H_
