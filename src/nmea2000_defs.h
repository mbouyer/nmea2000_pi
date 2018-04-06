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

#ifndef NMEA2000_DEFS_H_
#define NMEA2000_DEFS_H_

class nmea2000_desc {
    public:
        const char *descr;
	const bool isuser;
	const int pgn;
	bool enabled;

	inline nmea2000_desc(const char *desc, bool isuser, int pgn) :
	    descr(desc), isuser(isuser), pgn(pgn) {enabled = false;}
	virtual ~nmea2000_desc() {};
};

#define NMEA2000_PRIORITY_HIGH          0
#define NMEA2000_PRIORITY_SECURITY      1
#define NMEA2000_PRIORITY_CONTROL       3
#define NMEA2000_PRIORITY_REQUEST       6
#define NMEA2000_PRIORITY_INFO          6
#define NMEA2000_PRIORITY_ACK           6
#define NMEA2000_PRIORITY_LOW           7

#define NMEA2000_ADDR_GLOBAL    255
#define NMEA2000_ADDR_NULL      254
#define NMEA2000_ADDR_MAX       251

#define NMEA2000_INDUSTRY_GROUP 4

#define ISO_ADDRESS_CLAIM	60928U
#define ISO_REQUEST		59904U

#define NMEA2000_DATETIME	129033U
#define NMEA2000_ATTITUDE	127257U
#define NMEA2000_RATEOFTURN	127251U
#define NMEA2000_COGSOG		129026U
#define NMEA2000_XTE		129283U
#define NMEA2000_NAVDATA	129284U

inline double rad2deg(int rad)
{
	double deg;
	deg = (double)rad / 174.53293F;
	if (deg < 0)
		deg += 360.0F;
	return deg;
}

inline double urad2deg(unsigned int rad)
{
	double deg;
	deg = (double)rad / 174.53293F;
	return deg;
}

inline int deg2rad(double deg)
{
        double rad;

	if (deg > 180)
		deg -= 360;

	rad = deg * 174.53293F;
	return (int)(rad + 0.5);
}

inline unsigned int udeg2rad(double deg)
{
        double rad;

	rad = deg * 174.53293F;
	return (unsigned int)(rad + 0.5);
}

#endif
