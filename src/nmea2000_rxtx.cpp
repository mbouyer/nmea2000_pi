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
#include "nmea2000_defs_rx.h"
#include "nmea0183.h"
#include "ocpnnmea2000_pi.h"

bool nmea2000_rx::handle(const nmea2000_frame &n2kf)
{
	int pgn = n2kf.getpgn();

	for (u_int i = 0; i < frames_rx.size(); i++) {
		if (frames_rx[i]->pgn == pgn) {
			if (frames_rx[i]->enabled)
				return frames_rx[i]->handle(n2kf);
			return false;
		}
	}
	return false;
}

const nmea2000_desc * nmea2000_rx::get_byindex(u_int i) {
	if (i >= frames_rx.size()) {
		return NULL;
	};
	return frames_rx[i];
}

int nmea2000_rx::get_bypgn(int pgn) {
	for (u_int i = 0; i < frames_rx.size(); i++) {
		if (frames_rx[i]->pgn == pgn) {
			return i;
		}
	}
	return -1;
}

void nmea2000_rx::enable(u_int i, bool en)
{
	if (i < frames_rx.size()) {
		frames_rx[i]->enabled = en;
	}
}

nmea2000_tx::nmea2000_tx()
{
	sid = 0;
	m_NMEA0183 = new NMEA0183;
};

nmea2000_tx::~nmea2000_tx()
{
	delete m_NMEA0183;
}

const nmea2000_desc *nmea2000_tx::get_byindex(u_int i) {
	if (i >= frames_tx.size()) {
		return NULL;
	};
	return frames_tx[i];
}

int nmea2000_tx::get_bypgn(int pgn) {
	for (u_int i = 0; i < frames_tx.size(); i++) {
		if (frames_tx[i]->pgn == pgn) {
			return i;
		}
	}
	return -1;
}

void nmea2000_tx::enable(u_int i, bool en)
{
	if (i < frames_tx.size()) {
		frames_tx[i]->enabled = en;
	}
}

bool nmea2000_tx::send_frame(int sock, int pgn) {
	for (u_int i = 0; i < frames_tx.size(); i++) {
		if (frames_tx[i]->pgn == pgn) {
			if (frames_tx[i]->enabled) {
				return frames_tx[i]->send(sock);
			} else {
				return false;
			}
		}
	}
	return false;
}

void nmea2000_tx::setsrc(int src) {
	for (u_int i = 0; i < frames_tx.size(); i++) {
		frames_tx[i]->setsrc(src);
	}
}

void nmea2000_tx::positionfix(PlugIn_Position_Fix_Ex *fix, int sock) {
	for (u_int i = 0; i < frames_tx.size(); i++) {
		if (frames_tx[i]->enabled)
			frames_tx[i]->positionfix(fix, sock, sid);
	}
	sid++;
};

void nmea2000_tx::nmeasentence(wxString &sentence, int sock) {
	*m_NMEA0183 << sentence;

	if (!m_NMEA0183->PreParse()) {
		return;
	}
	if (!m_NMEA0183->Parse()) {
		return;
	}

	for (u_int i = 0; i < frames_tx.size(); i++) {
		if (frames_tx[i]->enabled)
			frames_tx[i]->nmeasentence(m_NMEA0183, sock, sid);
	}
	sid++;
};

bool nmea2000_frame_tx::send(int sock) {
	if (!valid)
		return false;

	if (write(sock, frame, sizeof(struct can_frame)) < 0) {
		wxLogSysError(ocpnnmea2000_pi::ErrMsgPrefix() + _T("send %s"), descr);
		return false;
	}
	return true;
}

void nmea2000_frame_tx::positionfix(PlugIn_Position_Fix_Ex *fix, int sock, uint8_t sid)
{
	return;
}

void nmea2000_frame_tx::nmeasentence(NMEA0183 *s, int sock, uint8_t sid)
{
	return;
}

bool nmea2000_fastframe_tx::send(int sock)
{
	int i;
	int n;
	if (!valid)
		return false;

	for (i = 0, n = 0; i < fastlen; n++) {
		frame->data[0] = (ident << 5) | n ;
		if (n == 0) {
			frame->data[1] = fastlen;
			memcpy(&frame->data[2], &data[i], 6);
			i += 6;
			frame->can_dlc = 8;
		} else {
			int remain = fastlen - i;
			if (remain > 7)
				remain = 7;
			memcpy(&frame->data[1], &data[i], remain);
			frame->can_dlc = remain + 1;
			i += remain;
		}
		if (write(sock, frame, sizeof(struct can_frame)) < 0) {
			wxLogSysError(ocpnnmea2000_pi::ErrMsgPrefix() + _T("send %s (%d)"), descr, n);
			return false;
		}
	}
	ident = (ident + 1) & 0x7;
	return true;
}
