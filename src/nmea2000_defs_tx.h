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

#ifndef NMEA2000_FRAME_TX_H_
#define NMEA2000_FRAME_TX_H_
#include "nmea2000_frame.h"
#include "nmea2000_defs.h"
#include <array>
#include <time.h>

class NMEA0183;

class nmea2000_frame_tx : public nmea2000_frame, public nmea2000_desc {
    public:
	bool  valid;

	inline nmea2000_frame_tx() :
	    nmea2000_frame(),
	    nmea2000_desc(NULL, false, -1)
	    {valid = 0; }

	inline nmea2000_frame_tx(const char *desc, bool isuser, u_int pgn, u_int pri, u_int len) : nmea2000_frame(), nmea2000_desc(desc, isuser, pgn)
	    {
		valid = 0;
		wxASSERT((len & 0xff) <= 8);
		frame->can_id = ((pri & 0x7) << 26) |
		    (pgn << 8);
		frame->can_id |= CAN_EFF_FLAG;
		frame->can_dlc = len;
	    }

	inline void setsrc(int src)
	    {
		frame->can_id = (frame->can_id & ~0xff) | (src & 0xff);
	    }
	inline void setdst(int dst) {
		wxASSERT(is_pdu1());
		frame->can_id =
		   (frame->can_id & ~0xff00) | ((dst & 0xff) << 8);
	}

	virtual bool send(int);
	virtual void positionfix(PlugIn_Position_Fix_Ex *, int, uint8_t);
	virtual void nmeasentence(NMEA0183 *, int, uint8_t);
};

class nmea2000_fastframe_tx : public nmea2000_frame_tx {
public:
	inline nmea2000_fastframe_tx() : nmea2000_frame_tx(), fastlen(233) { init(); }
	inline nmea2000_fastframe_tx(const char *desc, bool isuser, u_int pgn, u_int pri, u_int len) : nmea2000_frame_tx(desc, isuser, pgn, pri, 8), fastlen(len) { init(); }
	inline ~nmea2000_fastframe_tx() {free(userdata);}
	virtual bool send(int);
protected:
	const int fastlen;
private:
	uint8_t *userdata;
	uint8_t ident;
	inline void init()
	    { userdata = (uint8_t *)malloc(fastlen);
	      data = userdata; ; 
	      ident = 0;
	    }
};

class iso_address_claim_tx : public nmea2000_frame_tx {
    public:
	inline iso_address_claim_tx() : nmea2000_frame_tx("ISO address claim", false, ISO_ADDRESS_CLAIM, NMEA2000_PRIORITY_REQUEST, 8) {} ;

	inline void setdata(u_int uniquenumn, u_int manuf, u_int devfunc, u_int devclass, u_int devinst, u_int systinst)
	{
	    data[0] = (uniquenumn >> 0) & 0xff;
	    data[1] = (uniquenumn >> 8) & 0xff;
	    data[2] = (uniquenumn >> 16) & 0x1f;
	    data[2] |= (manuf << 5) & 0xe0;
	    data[3] = manuf >> 3;
	    data[4] = devinst;
	    data[5] = devfunc;
	    data[6] = devclass << 1;
	    data[7] = 0x80 | (NMEA2000_INDUSTRY_GROUP << 4) | systinst;
	};
};

class n2k_datetime_tx : public nmea2000_frame_tx {
    public:
	inline n2k_datetime_tx() : nmea2000_frame_tx("NMEA2000 local time offset", true, NMEA2000_DATETIME, NMEA2000_PRIORITY_INFO, 8) { init(); }

	int update(int);
    private:
	struct timeval last_update;
	void init(void);
};

class n2k_sogcog_tx : public nmea2000_frame_tx {
    public:
	inline n2k_sogcog_tx() : nmea2000_frame_tx("NMEA2000 COG/SOG", true, NMEA2000_COGSOG, NMEA2000_PRIORITY_INFO, 8) { };
	virtual void positionfix(PlugIn_Position_Fix_Ex *, int, uint8_t);
};

class n2k_navdata_tx : public nmea2000_fastframe_tx {
    public:
	static const int navdata_size = 34;
	inline n2k_navdata_tx() : nmea2000_fastframe_tx("NMEA2000 Navigation Data", true, NMEA2000_NAVDATA, NMEA2000_PRIORITY_INFO, navdata_size) { apb_ok = false; rmb_ok = false; current_bearing = -1;}
	virtual void nmeasentence(NMEA0183 *, int, uint8_t);
	virtual void positionfix(PlugIn_Position_Fix_Ex *, int, uint8_t);
    private:
	bool apb_ok;
	bool rmb_ok;
	double current_bearing;
	double current_sog;
};

class nmea2000_tx {
    public:
	nmea2000_tx();
	~nmea2000_tx();

	const nmea2000_desc *get_byindex(u_int);
	int get_bypgn(int);
	void enable(u_int, bool);

	bool send_frame(int, int);
	void setsrc(int);

	void positionfix(PlugIn_Position_Fix_Ex *, int);
	void nmeasentence(wxString &, int);

	iso_address_claim_tx iso_address_claim;
	n2k_datetime_tx n2k_datetime;
	n2k_sogcog_tx n2k_sogcog;
	n2k_navdata_tx n2k_navdata;
    private:

	std::array<nmea2000_frame_tx *,4> frames_tx = { {
		&iso_address_claim,
		&n2k_datetime,
		&n2k_sogcog,
		&n2k_navdata,
	} };
	uint8_t sid;
	NMEA0183 *m_NMEA0183;
};

#endif // NMEA2000_FRAME_TX_H_
