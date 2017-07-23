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

#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <net/if.h>

#include "NMEA2000.h"
#include "nmea2000_defs_tx.h"
#include "nmea2000_defs_rx.h"
#include "NMEA2000Properties.h" 
#include "ocpnnmea2000_pi.h" 

nmea2000 *nmea2000P;

nmea2000::nmea2000(void) {
    myaddress = 0x80;
    srandom(time(NULL));
    // the following may be overriden by the config file
    uniquenumber = random() & 0x1fffff;
    deviceinstance = 0;
    manufcode = 0x7ff;

    nmea2000_rxP = new nmea2000_rx;
    nmea2000_txP = new nmea2000_tx;
}

nmea2000::~nmea2000(void)
{
    if (GetThread() &&      
	GetThread()->IsRunning()) {
	GetThread()->Delete();
	GetThread()->Wait();
    }
    close(sock);
    delete nmea2000_rxP;
    delete nmea2000_txP;
}

void nmea2000::Init() {

    state = UNCONF;

    if ((sock = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
	wxLogSysError(ocpnnmea2000_pi::ErrMsgPrefix() + _("create CAN socket"));
	return;
    }
    nmea2000_txP->setsrc(myaddress);
    nmea2000_txP->iso_address_claim.setdst(NMEA2000_ADDR_GLOBAL);
    nmea2000_txP->iso_address_claim.setdata(uniquenumber, manufcode, 130, 120, deviceinstance, 0);
    nmea2000_txP->iso_address_claim.enabled = 1;
    nmea2000_txP->iso_address_claim.valid = 1;
    if (CreateThread(wxTHREAD_JOINABLE) != wxTHREAD_NO_ERROR) {
        wxLogError(ocpnnmea2000_pi::ErrMsgPrefix() + _("Could not create the management thread"));
	return;
    }
   if (GetThread()->Run() != wxTHREAD_NO_ERROR) {
        wxLogError(ocpnnmea2000_pi::ErrMsgPrefix() + _("Could not run the management thread!"));
	return;
    }
}

wxThread::ExitCode nmea2000::Entry()
{

    while (!GetThread()->TestDestroy()) {
	switch(state) {
	case UNCONF:
	case DOINGCONF:
	        if (configure()) {
	            state = DOCLAIM;
	        } else {
	            state = DOINGCONF;
		    sleep(1);
	        }
	        break;
	case DOCLAIM:
		if (nmea2000_txP->iso_address_claim.send(sock)) {
			state = CLAIMING;
			gettimeofday(&claim_date, NULL);
		} else {
			sleep(1);
		}
		break;
	case CLAIMING:
		struct timeval tv;
		struct timeval tv2;
		// poll checking for addr claim, and timeout
		gettimeofday(&tv, NULL);
		timersub(&tv, &claim_date, &tv2);
		if (tv2.tv_sec >= 1)
			state = CLAIMED;
		/* FALLTHROUGH */
	case CLAIMED:
		// normal operation
		struct timeval timeout;
		fd_set read_set;
		int sret;

		if (nmea2000_txP->n2k_datetime.enabled && state == CLAIMED) {
			timeout.tv_usec = nmea2000_txP->n2k_datetime.update(sock);
			if (timeout.tv_usec == 1000000) {
				timeout.tv_sec=1;
				timeout.tv_usec = 0;
			} else {
				timeout.tv_sec=0;
			}
		} else {
			timeout.tv_sec=1;
			timeout.tv_usec = 0;
		}
		FD_ZERO(&read_set);
		FD_SET(sock, &read_set);
		sret = select(sock + 1, &read_set, NULL, NULL, &timeout);
		switch(sret) {
		case -1:
			wxLogSysError(ocpnnmea2000_pi::ErrMsgPrefix() + _T("select"));
			break;
		case 0:
			break;
		default:
			nmea2000_frame n2kframe;
			int rret = n2kframe.readframe(sock);
			switch(rret) {
			case -1:
				wxLogSysError(ocpnnmea2000_pi::ErrMsgPrefix() + _("read CAN socket"));
				break;
			case 0:
				/* EOF ? */
				break;
			default:
				parse_frame(n2kframe);
				break;
			}
		}
		break;
	default:
		sleep(1);
	}
			
    }
    return (wxThread::ExitCode)0;
}

bool nmea2000::configure()
{
    struct ifreq ifr;
    struct sockaddr_can addr;

    memset(&ifr, 0, sizeof(ifr));
    if (canif.Len() > 0) {
	strcpy(ifr.ifr_name, canif.c_str());
	if (ioctl(sock, SIOCGIFINDEX, &ifr) < 0) {
	    if (state == UNCONF) {
		wxLogSysError(ocpnnmea2000_pi::ErrMsgPrefix() + _("can't get index for CAN interface %s"), canif.c_str());
	    }
	    return false;
        }
        addr.can_family = AF_CAN;
        addr.can_ifindex = ifr.ifr_ifindex;
        if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
	    if (state == UNCONF) {
	        wxLogSysError(ocpnnmea2000_pi::ErrMsgPrefix() + _("can't bind CAN socket to %s"), canif.c_str());
	    }
	    return false;
        }
        return true;
    }
    return false;
}

void nmea2000::parse_frame(const nmea2000_frame &n2kf)
{
	if (n2kf.is_pdu1() &&
	    n2kf.getdst() != myaddress &&
	    n2kf.getdst() != NMEA2000_ADDR_GLOBAL) {
		return;
	}
	switch(n2kf.getpgn()) {
	case ISO_ADDRESS_CLAIM:
		handle_address_claim(n2kf);
		break;
	case ISO_REQUEST:
		handle_iso_request(n2kf);
		break;
	default:
		nmea2000_rxP->handle(n2kf);
		break;
	}
	return;
}

void nmea2000::handle_address_claim(const nmea2000_frame &n2kf)
{
	if (n2kf.getsrc() != myaddress)
		return;
	for (int i = 7; i >= 0; i--) {
	    if (n2kf.getdata()[i] < nmea2000_txP->iso_address_claim.getdata()[i]) {
		// we loose
		myaddress++;
		if (myaddress >= NMEA2000_ADDR_MAX)
			myaddress = 0;
		nmea2000_txP->setsrc(myaddress);
		state = DOCLAIM;
		return;
	    }
	    if (n2kf.getdata()[i] > nmea2000_txP->iso_address_claim.getdata()[i])
		break;
	}
	// defend our address. if we can't right now restart the whole process
	if (!nmea2000_txP->iso_address_claim.send(sock)) 
		state = DOCLAIM;
}

void nmea2000::handle_iso_request(const nmea2000_frame &n2kf)
{
	int pgn;
	if (n2kf.getlen() < 3)
		return;

	pgn = n2kf.frame2uint24(0);
	if (nmea2000_txP->get_bypgn(pgn) < 0) {
		return;
	}
	nmea2000_txP->send_frame(sock, pgn);
}

void nmea2000::OnThreadUpdate(wxThreadEvent& evt) {
}

const nmea2000_desc *nmea2000::get_tx_byindex(int i) {
	return nmea2000_txP->get_byindex(i);
}

int nmea2000::get_tx_bypgn(int pgn) {
	return nmea2000_txP->get_bypgn(pgn);
}

void nmea2000::tx_enable(int i, bool en) {
	nmea2000_txP->enable(i, en);
}
	
const nmea2000_desc *nmea2000::get_rx_byindex(int i) {
	return nmea2000_rxP->get_byindex(i);
}

int nmea2000::get_rx_bypgn(int pgn) {
	return nmea2000_rxP->get_bypgn(pgn);
}

void nmea2000::rx_enable(int i, bool en) {
	nmea2000_rxP->enable(i, en);
}

void nmea2000::positionfix(PlugIn_Position_Fix_Ex *fix)
{
	if (state == CLAIMED) 
		nmea2000_txP->positionfix(fix, sock);
}

void nmea2000::nmeasentence(wxString &sentence)
{
	if (state == CLAIMED) 
		nmea2000_txP->nmeasentence(sentence, sock);
}
