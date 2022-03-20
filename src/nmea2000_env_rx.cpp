/******************************************************************************
 * $Id: ocpnnmea2000_pi.cpp,v 1.0 2011/02/26 01:54:37 nohal Exp $
 *
 * Project:  OpenCPN
 * Purpose:  OpenCPNNMEA2000 Plugin
 * Author:   Manuel Bouyer
 *
 ***************************************************************************
 *   Copyright (C) 2021 by Manuel Bouyer
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

bool nmea2000_env_rx::handle(const nmea2000_frame &f)
{
	NMEA0183 m_NMEA0183;
	SENTENCE snt;
	uint8_t src = f.frame2uint8(1);
	uint16_t temp = f.frame2uint16(2);
	uint16_t hum = f.frame2uint16(4);
	uint16_t press = f.frame2uint16(6);
	int count = 0;

	if (temp != 0xffff) {
		m_NMEA0183.Xdr.TransducerInfo[count].TransducerType = _T("C");
		m_NMEA0183.Xdr.TransducerInfo[count].MeasurementData = 
			((double)temp / 100.0) - 273.15;
		m_NMEA0183.Xdr.TransducerInfo[count].UnitOfMeasurement = _T("C");
		switch(src & 0x3f) {
		case 0: // sea
			m_NMEA0183.Xdr.TransducerInfo[count].TransducerName =
			    _T("ENV_WATER_T");
			break;
		case 1: // outside
			m_NMEA0183.Xdr.TransducerInfo[count].TransducerName =
			    _T("ENV_OUTAIR_T");
			break;
		case 2: // inside
		default:
			m_NMEA0183.Xdr.TransducerInfo[count].TransducerName =
			    _T("TempAir");
			break;
		}
		count++;
	}
	if (hum != 0xffff) {
		m_NMEA0183.Xdr.TransducerInfo[count].TransducerType = _T("H");
		m_NMEA0183.Xdr.TransducerInfo[count].MeasurementData = 
			(double)hum / 250.0;
		m_NMEA0183.Xdr.TransducerInfo[count].UnitOfMeasurement = _T("P");
		switch(src & 0xc0) {
		case 0x00: // inside
			m_NMEA0183.Xdr.TransducerInfo[count].TransducerName =
			    _T("ENV_INAIR_H");
			break;
		case 0x40: // outside
			m_NMEA0183.Xdr.TransducerInfo[count].TransducerName =
			    _T("ENV_OUTAIR_H");
			break;
		default:
			m_NMEA0183.Xdr.TransducerInfo[count].TransducerName =
			    _T("HumAir");
			break;
		}
		count++;
	}
	if (press != 0xffff) {
		m_NMEA0183.Xdr.TransducerInfo[count].TransducerType = _T("P");
		m_NMEA0183.Xdr.TransducerInfo[count].MeasurementData =
		    (double)press / 1000;
		m_NMEA0183.Xdr.TransducerInfo[count].UnitOfMeasurement = _T("B");
		m_NMEA0183.Xdr.TransducerInfo[count].TransducerName =
			    _T("Barometer");
		count++;
	}
	if (count == 0)
		return true;
	m_NMEA0183.TalkerID = _T("WI");
	m_NMEA0183.Xdr.TransducerCnt = count;
	m_NMEA0183.Xdr.Write(snt);

	PushNMEABuffer(snt.Sentence);
	return true;
}
