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
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif  // precompiled headers

#include <wx/stdpaths.h>
#include "ocpnnmea2000_pi.h"
#include "NMEA2000Properties.h" 
#include "NMEA2000PropertiesDialog.h" 

// the class factories, used to create and destroy instances of the PlugIn

extern "C" DECL_EXP opencpn_plugin *create_pi(void *ppimgr) {
    return new ocpnnmea2000_pi(ppimgr);
}

extern "C" DECL_EXP void destroy_pi(opencpn_plugin *p) { delete p; }

//---------------------------------------------------------------------------------------------------------
//
//    OpenCPNNMEA2000 PlugIn Implementation
//
//---------------------------------------------------------------------------------------------------------

NMEA2000PropertiesDialog *g_pOCPNN2KPropDialog; 

//---------------------------------------------------------------------------------------------------------
//
//          PlugIn initialization and de-init
//
//---------------------------------------------------------------------------------------------------------

ocpnnmea2000_pi::ocpnnmea2000_pi(void *ppimgr) : opencpn_plugin_114(ppimgr) {
	NMEA2000PropertiesP = NULL;
	nmea2000P = NULL;
}

ocpnnmea2000_pi::~ocpnnmea2000_pi(void)
{
    wxASSERT(NMEA2000PropertiesP == NULL);
    wxASSERT(nmea2000P == NULL);
}

int ocpnnmea2000_pi::Init(void) {
    AddLocaleCatalog(_T("opencpn-ocpnnmea2000_pi"));

    m_parent_window = GetOCPNCanvasWindow();

    wxASSERT(nmea2000P == NULL);
    nmea2000P = new nmea2000;

    wxASSERT(NMEA2000PropertiesP == NULL);
    NMEA2000PropertiesP = new NMEA2000Properties;
    nmea2000P->Init();

    return ( WANTS_NMEA_SENTENCES | WANTS_NMEA_EVENTS | WANTS_AIS_SENTENCES |
            WANTS_PLUGIN_MESSAGING | WANTS_SIGNALK_SENTENCES | WANTS_PREFERENCES );
}

bool ocpnnmea2000_pi::DeInit(void) {
    if (nmea2000P != NULL) {
    	delete nmea2000P;
	nmea2000P = NULL;
    }

    wxASSERT(NMEA2000PropertiesP != NULL);
    delete NMEA2000PropertiesP;
    NMEA2000PropertiesP = NULL;

    return true;
}

int ocpnnmea2000_pi::GetAPIVersionMajor() { return MY_API_VERSION_MAJOR; }

int ocpnnmea2000_pi::GetAPIVersionMinor() { return MY_API_VERSION_MINOR; }

int ocpnnmea2000_pi::GetPlugInVersionMajor() { return PLUGIN_VERSION_MAJOR; }

int ocpnnmea2000_pi::GetPlugInVersionMinor() { return PLUGIN_VERSION_MINOR; }

wxString ocpnnmea2000_pi::GetCommonName() { return _T("OpenCPNNMEA2000"); }

wxString ocpnnmea2000_pi::GetShortDescription() {
    return _("NNMEA2000 PlugIn for OpenCPN");
}

wxString ocpnnmea2000_pi::GetLongDescription() {
    return _(
        "Send and receive NMEA2000 messages"
        );
}

void ocpnnmea2000_pi::SetNMEASentence(wxString &sentence) {
	nmea2000P->nmeasentence(sentence);
}

void ocpnnmea2000_pi::SetAISSentence(wxString &sentence) {
}

void ocpnnmea2000_pi::SetPluginMessage(wxString &message_id,
                                       wxString &message_body) {
}

void ocpnnmea2000_pi::SetPositionFixEx(PlugIn_Position_Fix_Ex &pfix) {
	nmea2000P->positionfix(&pfix);
}

void ocpnnmea2000_pi::SetSignalKSentence(wxString &sentence) {
}

#if 0
wxBitmap *ocpnnmea2000_pi::GetPlugInBitmap()
{
      return _img_pi;
}
#endif

void ocpnnmea2000_pi::ShowPreferencesDialog(wxWindow* parent)
{
    wxASSERT(nmea2000P != NULL);
    if( NULL == g_pOCPNN2KPropDialog )
        g_pOCPNN2KPropDialog = new NMEA2000PropertiesDialog( parent );

    g_pOCPNN2KPropDialog->ShowModal();

    delete g_pOCPNN2KPropDialog;
    g_pOCPNN2KPropDialog = NULL;
}

wxString ocpnnmea2000_pi::StandardPath()
{
    wxStandardPathsBase& std_path = wxStandardPathsBase::Get();
    wxString s = wxFileName::GetPathSeparator();
 
#ifdef __WXMSW__
    wxString stdPath  = std_path.GetConfigDir();
#endif
#ifdef __WXGTK__
    wxString stdPath  = std_path.GetUserDataDir();
#endif
#ifdef __WXOSX__ 
    wxString stdPath  = (std_path.GetUserConfigDir() + s + _T("opencpn"));
#endif

    stdPath += s + _T("plugins");     
    if (!wxDirExists(stdPath))
      wxMkdir(stdPath);

    stdPath += s + _T("nmea2000");    

    if (!wxDirExists(stdPath))
      wxMkdir(stdPath);

    stdPath += s; // is this necessary?
    return stdPath;
}

wxString ocpnnmea2000_pi::ErrMsgPrefix()
{
	static wxString emp("NMEA2000: ");
	return emp;
}
