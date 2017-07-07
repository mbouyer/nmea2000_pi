/***************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  NMEA2000 plugin Properties support
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

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
#include "wx/wx.h"
#endif //precompiled headers

#include "ocpnnmea2000_pi.h"
#include "NMEA2000Properties.h"
#include "NMEA2000.h"
#include <tinyxml.h>

NMEA2000Properties *NMEA2000PropertiesP;

NMEA2000Properties::NMEA2000Properties()
{
	LoadConfig();
}

NMEA2000Properties::~NMEA2000Properties()
{
}

void NMEA2000Properties::LoadConfig()
{
    wxString configuration = ocpnnmea2000_pi::StandardPath() + _T("Nmea2000Configuration.xml");                           
    TiXmlDocument doc;

    if(!doc.LoadFile(configuration.mb_str())) {
        wxLogMessage(ocpnnmea2000_pi::ErrMsgPrefix() + wxString(_("Failed to read")) + _T(": ") + configuration);
        return;
    }

    TiXmlHandle root(doc.RootElement());
    for(TiXmlElement* e = root.FirstChild().Element(); e; e = e->NextSiblingElement()) {
        if(!strcasecmp(e->Value(), "Interface")) {
	    const char *name = e->Attribute("Name");
	    if (name != NULL) {
		nmea2000P->setcanif(name);
            }
        } else if(!strcasecmp(e->Value(), "Receive")) {
	    for (u_int i = 0; ; i++) {
		const nmea2000_desc *desc = nmea2000P->get_rx_byindex(i);
		if (desc == NULL)
		    break;
		if (desc->isuser) {
		    bool en = false;
		    e->QueryBoolAttribute(
			wxString::Format(wxT("PGN%d"), desc->pgn), &en);
		    nmea2000P->rx_enable(i, en);
		}
	    }
        } else if(!strcasecmp(e->Value(), "Transmit")) {
	    for (u_int i = 0; ; i++) {
		const nmea2000_desc *desc = nmea2000P->get_tx_byindex(i);
		if (desc == NULL)
		    break;
		if (desc->isuser) {
		    bool en = false;
		    e->QueryBoolAttribute(
			wxString::Format(wxT("PGN%d"), desc->pgn), &en);
		    nmea2000P->tx_enable(i, en);
		} 
	    }
        } else if(!strcasecmp(e->Value(), "NMEA2000")) {
		int uniquenumber, deviceinstance, manufcode;
		nmea2000P->getconfig(&uniquenumber, &deviceinstance, &manufcode);
		e->QueryIntAttribute("UniqueNumber", &uniquenumber);
		e->QueryIntAttribute("DeviceInstance", &deviceinstance);
		e->QueryIntAttribute("NanufacturerCode", &manufcode);
		nmea2000P->setconfig(uniquenumber, deviceinstance, manufcode);
	}
    }
}

void NMEA2000Properties::SaveConfig()
{
    wxString configuration = ocpnnmea2000_pi::StandardPath() + _T("Nmea2000Configuration.xml");                           
    TiXmlDocument doc;
    TiXmlElement *e;
    int uniquenumber, deviceinstance, manufcode;

    TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "utf-8", "" );
    doc.LinkEndChild( decl );

    TiXmlElement * root = new TiXmlElement( "OpenCPNNMEA2000Configuration" );
    doc.LinkEndChild( root );

    char version[24];
    sprintf(version, "%d.%d", PLUGIN_VERSION_MAJOR, PLUGIN_VERSION_MINOR);
    root->SetAttribute("version", version);
    root->SetAttribute("creator", "Opencpn NMEA2000 plugin");
    root->SetAttribute("author", "Manuel Bouyer");

    if (nmea2000P->getcanif().Len() > 0) {
	e = new TiXmlElement( "Interface" );
	e->SetAttribute("Name", nmea2000P->getcanif().c_str());
	root->LinkEndChild(e);
    }
    e = new TiXmlElement( "Receive" );
    for (u_int i = 0; ; i++) {
	const nmea2000_desc *desc = nmea2000P->get_rx_byindex(i);
	if (desc == NULL)
	    break;
	if (desc->isuser) {
	    e->SetAttribute(
			wxString::Format(wxT("PGN%d"), desc->pgn).c_str(), desc->enabled);
	}
    }
    root->LinkEndChild(e);
    e = new TiXmlElement( "Transmit" );
    for (u_int i = 0; ; i++) {
	const nmea2000_desc *desc = nmea2000P->get_tx_byindex(i);
	if (desc == NULL)
	    break;
	if (desc->isuser) {
	    e->SetAttribute(
			wxString::Format(wxT("PGN%d"), desc->pgn).c_str(), desc->enabled);
	}
    }
    root->LinkEndChild(e);
    e = new TiXmlElement( "NMEA2000" );
    nmea2000P->getconfig(&uniquenumber, &deviceinstance, &manufcode);
    e->SetAttribute("UniqueNumber", uniquenumber);
    e->SetAttribute("DeviceInstance", deviceinstance);
    e->SetAttribute("NanufacturerCode", manufcode);
    root->LinkEndChild(e);
    if(!doc.SaveFile(configuration.mb_str()))
        wxLogMessage(ocpnnmea2000_pi::ErrMsgPrefix() + wxString(_("failed to save")) + _T(": ") + configuration);
}
