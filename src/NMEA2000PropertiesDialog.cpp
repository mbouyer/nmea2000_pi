/***************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  NMEA2000 plugin Properties Dialog support
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

#include <sys/param.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <net/if.h>
#include <netcan/can.h>
#include <ifaddrs.h>

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
#include "wx/wx.h"
#endif //precompiled headers

#include "NMEA2000PropertiesDialog.h"
#include "NMEA2000Properties.h"
#include "NMEA2000.h"
#include "version.h"
#include <wx/fontdlg.h>

NMEA2000PropertiesDialog::NMEA2000PropertiesDialog( wxWindow* parent )
:
wxDialog(parent, wxID_ANY, _T("NMEA2000"), wxDefaultPosition, wxSize( -1,-1 ), wxCAPTION|wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER|wxTAB_TRAVERSAL)
{
    int uniquenumber, deviceinstance, manufcode;

    nmea2000P->getconfig(&uniquenumber, &deviceinstance, &manufcode);
    this->SetSizeHints( wxDefaultSize, wxDefaultSize );

    m_SizerDialogBox = new wxBoxSizer( wxVERTICAL );

    m_notebookProperties = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );

    m_panelBasicProperties = new wxPanel( m_notebookProperties, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE|wxTAB_TRAVERSAL );

    m_SizerBasicProperties = new wxFlexGridSizer( 0, 1, 0, 0 );
    m_SizerBasicProperties->SetFlexibleDirection( wxBOTH );
    m_SizerBasicProperties->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

    wxStaticText *staticTextNameVal = new wxStaticText( m_panelBasicProperties, wxID_ANY,
	wxString::Format(_("NMEA2000 plugin %i.%i %s"),
	PLUGIN_VERSION_MAJOR, PLUGIN_VERSION_MINOR, PLUGIN_VERSION_DATE),
	wxDefaultPosition, wxDefaultSize, 0 );
    staticTextNameVal->Wrap( -1 );
    m_SizerBasicProperties->Add( staticTextNameVal, 0, wxALL, 5 );

    wxStaticText *staticTextAddrVal = new wxStaticText( m_panelBasicProperties, wxID_ANY,
	wxString::Format(_("NMEA2000 address: %i"), nmea2000P->getaddress()),
	wxDefaultPosition, wxDefaultSize, 0 );
    staticTextAddrVal->Wrap( -1 );
    m_SizerBasicProperties->Add( staticTextAddrVal, 0, wxALL, 5 );

    wxStaticText *staticTextUNVal = new wxStaticText( m_panelBasicProperties, wxID_ANY,
	wxString::Format(_("NMEA2000 UniqueNumber: %i"), uniquenumber),
	wxDefaultPosition, wxDefaultSize, 0 );
    staticTextUNVal->Wrap( -1 );
    m_SizerBasicProperties->Add( staticTextUNVal, 0, wxALL, 5 );

    wxStaticText *staticTextDIVal = new wxStaticText( m_panelBasicProperties, wxID_ANY,
	wxString::Format(_("NMEA2000 Device Instance: %i"), deviceinstance),
	wxDefaultPosition, wxDefaultSize, 0 );
    staticTextDIVal->Wrap( -1 );
    m_SizerBasicProperties->Add( staticTextDIVal, 0, wxALL, 5 );

    wxStaticText *staticTextMFVal = new wxStaticText( m_panelBasicProperties, wxID_ANY,
	wxString::Format(_("NMEA2000 Manufacturer code: %i"), manufcode),
	wxDefaultPosition, wxDefaultSize, 0 );
    staticTextMFVal->Wrap( -1 );
    m_SizerBasicProperties->Add( staticTextMFVal, 0, wxALL, 5 );
    m_panelBasicProperties->SetSizer(m_SizerBasicProperties);
    m_notebookProperties->AddPage( m_panelBasicProperties, _("Basic"), false );

    NMEA2000PropertiesGetIfList();

    m_panelIfSelectProperties = new wxPanel( m_notebookProperties, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE|wxTAB_TRAVERSAL );

    m_SizerIfSelectProperties = new wxFlexGridSizer( 0, 2, 0, 0 );
    m_SizerIfSelectProperties->SetFlexibleDirection( wxBOTH );
    m_SizerIfSelectProperties->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

    if (ifList->GetCount() > 0) {
        m_ListIfSelectProperties= new wxListBox(m_panelIfSelectProperties, wxID_ANY, wxDefaultPosition, wxDefaultSize, *ifList, wxLB_SINGLE | wxLB_NEEDED_SB | wxLB_SORT);
        m_SizerIfSelectProperties->Add( m_ListIfSelectProperties, 0, wxALL, 5 );
	if (nmea2000P->getcanif().Len() > 0) {
	    m_ListIfSelectProperties->SetStringSelection(nmea2000P->getcanif());
	}
	m_TextIfSelectProperties = NULL;
    } else  {
        m_TextIfSelectProperties = new wxStaticText( m_panelIfSelectProperties, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_TextIfSelectProperties->Wrap(-1);
        m_SizerIfSelectProperties->Add( m_TextIfSelectProperties, 0, wxALL, 5 );
        m_TextIfSelectProperties->SetLabel( _("no interface found") );
	m_ListIfSelectProperties = NULL;
    }
    m_panelIfSelectProperties->SetSizer(m_SizerIfSelectProperties);
    m_notebookProperties->AddPage( m_panelIfSelectProperties, _("Interface"), false );

    m_panelRxSelectProperties = new wxPanel( m_notebookProperties, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE|wxTAB_TRAVERSAL );

    m_SizerRxSelectProperties = new wxFlexGridSizer( 0, 2, 0, 0 );
    m_SizerRxSelectProperties->SetFlexibleDirection( wxBOTH );
    m_SizerRxSelectProperties->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
    NMEA2000PropertiesGetRxList();
    m_SizerRxSelectProperties->Add( m_ListRxSelectProperties, 0, wxALL, 5 );
    m_panelRxSelectProperties->SetSizer(m_SizerRxSelectProperties);
    m_notebookProperties->AddPage( m_panelRxSelectProperties, _("Receive"), false );

    m_panelTxSelectProperties = new wxPanel( m_notebookProperties, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE|wxTAB_TRAVERSAL );

    m_SizerTxSelectProperties = new wxFlexGridSizer( 0, 2, 0, 0 );
    m_SizerTxSelectProperties->SetFlexibleDirection( wxBOTH );
    m_SizerTxSelectProperties->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
    NMEA2000PropertiesGetTxList();
    m_SizerTxSelectProperties->Add( m_ListTxSelectProperties, 0, wxALL, 5 );
    m_panelTxSelectProperties->SetSizer(m_SizerTxSelectProperties);
    m_notebookProperties->AddPage( m_panelTxSelectProperties, _("Transmit"), false );

    m_SizerDialogBox->Add( m_notebookProperties, 1, wxEXPAND | wxALL, 5 );  
 
    bSizer = new wxBoxSizer( wxHORIZONTAL );

    m_buttonOK = new wxButton( this, wxID_ANY, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    bSizer->Add( m_buttonOK, 0, wxALL, 5 );

    m_Cancel = new wxButton( this, wxID_ANY, _("&Cancel"), wxDefaultPosition
    , wxDefaultSize, 0 );
    m_Cancel->SetDefault(); 
    bSizer->Add( m_Cancel, 0, wxALL, 5 );

    m_SizerDialogBox->Add( bSizer, 0, wxALIGN_CENTER, 5 );
    this->SetSizer( m_SizerDialogBox );
    this->Layout();
    m_SizerDialogBox->Fit( this );

    this->Centre( wxBOTH );       
    m_buttonOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NMEA2000PropertiesDialog::OnNMEA2000PropertiesOKClick ), NULL, this );
    m_Cancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(
        NMEA2000PropertiesDialog::OnNMEA2000PropertiesCancelClick ), NULL, this );  
}

NMEA2000PropertiesDialog::~NMEA2000PropertiesDialog( )
{
    // Disconnect Events
    m_buttonOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NMEA2000PropertiesDialog::OnNMEA2000PropertiesOKClick ), NULL, this );
    m_Cancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NMEA2000PropertiesDialog::OnNMEA2000PropertiesCancelClick ), NULL, this );
}

void NMEA2000PropertiesDialog::OnNMEA2000PropertiesOKClick( wxCommandEvent& event )
{
    NMEA2000PropertiesConfig();
    Show( false );
#ifdef __WXOSX__    
    EndModal(wxID_OK);
#endif
    event.Skip();
}

void NMEA2000PropertiesDialog::OnNMEA2000PropertiesCancelClick( wxCommandEvent& event )
{
    Show( false );
#ifdef __WXOSX__    
    EndModal(wxID_OK);
#endif
    event.Skip();
}

void NMEA2000PropertiesDialog::NMEA2000PropertiesConfig(void)
{
	int selected; 
	if (m_ListIfSelectProperties != NULL) {
	    selected = m_ListIfSelectProperties->GetSelection();
	    if (selected == wxNOT_FOUND) {
	        nmea2000P->setcanif("");
	    } else {
	        nmea2000P->setcanif(m_ListIfSelectProperties->GetString(selected));
	    }
	} else {
	    nmea2000P->setcanif("");
	}

	for (u_int i = 0; ; i++) {
		const nmea2000_desc *desc = nmea2000P->get_rx_byindex(i);
		if (desc == NULL)
			break;
		if (desc->isuser) {
			int idx =  m_ListRxSelectProperties->FindString(
			    wxString::Format(wxT("%6d %s"), desc->pgn, desc->descr));
			wxASSERT(idx != wxNOT_FOUND);
			if (m_ListRxSelectProperties->IsChecked(idx)) {
				nmea2000P->rx_enable(i, true);
			} else {
				nmea2000P->rx_enable(i, false);
			}
		}
	}

	for (u_int i = 0; ; i++) {
		const nmea2000_desc *desc = nmea2000P->get_tx_byindex(i);
		if (desc == NULL)
			break;
		if (desc->isuser) {
			int idx =  m_ListTxSelectProperties->FindString(
			    wxString::Format(wxT("%6d %s"), desc->pgn, desc->descr));
			wxASSERT(idx != wxNOT_FOUND);
			if (m_ListTxSelectProperties->IsChecked(idx)) {
				nmea2000P->tx_enable(i, true);
			} else {
				nmea2000P->tx_enable(i, false);
			}
		}
	}

	NMEA2000PropertiesP->SaveConfig();
}

void NMEA2000PropertiesDialog::NMEA2000PropertiesGetIfList(void)
{
    int s;
    int i;
    char *p = NULL;
    struct ifaddrs *ifap, *ifa;

    ifList = new wxArrayString;

    s = socket(AF_CAN, SOCK_RAW, CAN_RAW);
    if (s < 0)
	return;

    if (getifaddrs(&ifap) != 0)
        return;

    for (ifa = ifap; ifa != NULL; ifa = ifa->ifa_next) {
        struct ifreq ifr;
        struct sockaddr_can nam;

        if (p && strcmp(p, ifa->ifa_name) == 0)
            continue;
        p = ifa->ifa_name;

        memset(&ifr, 0, sizeof(ifr));
        strlcpy(ifr.ifr_name, ifa->ifa_name, sizeof(ifr.ifr_name));
        if (ioctl(s, SIOCGIFFLAGS, &ifr) < 0) {
            continue;
        }
        if ((ifr.ifr_flags & IFF_UP) == 0) {
            continue;
        }
        i = if_nametoindex(ifa->ifa_name);
        if (i == 0) {
            continue;
        }
        nam.can_family = AF_CAN;
        nam.can_ifindex = i;
        if (bind(s, (struct sockaddr *)&nam, sizeof(nam)) < 0) {
            continue;
        }
        nam.can_family = AF_CAN;
        nam.can_ifindex = 0;
        (void)bind(s, (struct sockaddr *)&nam, sizeof(nam));
        ifList->Add( wxString::FromAscii(ifa->ifa_name) );
    }
    wxASSERT(NMEA2000PropertiesP != NULL);
    if (nmea2000P->getcanif().Len() > 0 &&
	ifList->Index(nmea2000P->getcanif()) == wxNOT_FOUND) {
	    ifList->Add(nmea2000P->getcanif());
    }
}

void NMEA2000PropertiesDialog::NMEA2000PropertiesGetRxList(void)
{
    rxList = new wxArrayString;
    for (u_int i = 0; ; i++) {
        const nmea2000_desc *desc = nmea2000P->get_rx_byindex(i);
	if (desc == NULL)
		break;
	if (desc->isuser) {
		rxList->Add(wxString::Format(wxT("%6d %s"), desc->pgn, desc->descr));
	}
    }
    m_ListRxSelectProperties= new wxCheckListBox(m_panelRxSelectProperties, wxID_ANY, wxDefaultPosition, wxDefaultSize, *rxList, wxLB_SINGLE | wxLB_NEEDED_SB | wxLB_SORT);
    for (u_int i = 0; ; i++) {
        const nmea2000_desc *desc = nmea2000P->get_rx_byindex(i);
	if (desc == NULL)
		break;
	if (desc->isuser && desc->enabled) {
		int idx =  m_ListRxSelectProperties->FindString(
		    wxString::Format(wxT("%6d %s"), desc->pgn, desc->descr));
		wxASSERT(idx != wxNOT_FOUND);
		m_ListRxSelectProperties->Check(idx);
	}
    }
}

void NMEA2000PropertiesDialog::NMEA2000PropertiesGetTxList(void)
{
    txList = new wxArrayString;
    for (u_int i = 0; ; i++) {
        const nmea2000_desc *desc = nmea2000P->get_tx_byindex(i);
	if (desc == NULL)
		break;
	if (desc->isuser) {
		txList->Add(wxString::Format(wxT("%6d %s"), desc->pgn, desc->descr));
	}
    }
    m_ListTxSelectProperties = new wxCheckListBox(m_panelTxSelectProperties, wxID_ANY, wxDefaultPosition, wxDefaultSize, *txList, wxLB_SINGLE | wxLB_NEEDED_SB | wxLB_SORT);

    for (u_int i = 0; ; i++) {
        const nmea2000_desc *desc = nmea2000P->get_tx_byindex(i);
	if (desc == NULL)
		break;
	if (desc->isuser && desc->enabled) {
		int idx =  m_ListTxSelectProperties->FindString(
		    wxString::Format(wxT("%6d %s"), desc->pgn, desc->descr));
		wxASSERT(idx != wxNOT_FOUND);
		m_ListTxSelectProperties->Check(idx);
	}
    }
}
