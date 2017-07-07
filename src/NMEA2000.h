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

#ifndef NMEA2000_H_
#define NMEA2000_H_

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include <wx/thread.h>
#include "wx/wx.h"
#endif  // precompiled headers
#include "nmea2000_defs.h"

class nmea2000_frame;
class nmea2000_rx;
class nmea2000_tx;

class PlugIn_Position_Fix_Ex;

class nmea2000 : public wxThreadHelper {
   public:
    nmea2000(void);
    ~nmea2000(void);

    void Init(void);

    inline void setcanif(wxString ifn) {canif = ifn;}
    inline wxString getcanif() {return canif;}
    int getaddress(void) { return myaddress; }
    inline void getconfig(int *un, int * di, int *mf)
	{ *un = uniquenumber; *di = deviceinstance; *mf = manufcode; }
    inline void setconfig(int un, int di, int mf)
	{ uniquenumber = un; deviceinstance = di; manufcode = mf; }
    const nmea2000_desc *get_tx_byindex(int);
    int get_tx_bypgn(int);
    void tx_enable(int, bool);
    const nmea2000_desc *get_rx_byindex(int);
    int get_rx_bypgn(int);
    void rx_enable(int, bool);
    void positionfix(PlugIn_Position_Fix_Ex *);
    void nmeasentence(wxString &);

  protected:
    virtual wxThread::ExitCode Entry();
    void OnThreadUpdate(wxThreadEvent& evt);

  private:
    int sock;
    int myaddress;
    wxString canif;
    int deviceinstance;
    int uniquenumber;
    int manufcode;
    nmea2000_rx *nmea2000_rxP;
    nmea2000_tx *nmea2000_txP;
    enum {
	UNCONF, DOINGCONF, DOCLAIM, CLAIMING, CLAIMED
    } state;
    struct timeval claim_date;
    bool configure();
    void parse_frame(const nmea2000_frame &);
    void handle_address_claim(const nmea2000_frame &);
    void handle_iso_request(const nmea2000_frame &);
    bool send_address_claim();
};

extern nmea2000 *nmea2000P;

#endif
