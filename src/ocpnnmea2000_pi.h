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

#ifndef _OpenCPNNMEA2000PI_H_
#define _OpenCPNNMEA2000PI_H_

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include <wx/glcanvas.h>
#include <wx/fileconf.h>
#include "wx/wx.h"
#endif  // precompiled headers

#include "version.h"

#define MY_API_VERSION_MAJOR 1
#define MY_API_VERSION_MINOR 14

#include "ocpn_plugin.h"
#include "NMEA2000.h"

//----------------------------------------------------------------------------------------------------------
//    The PlugIn Class Definition
//----------------------------------------------------------------------------------------------------------

class ocpnnmea2000_pi : public opencpn_plugin_114 {
   public:
    ocpnnmea2000_pi(void *ppimgr);
    ~ocpnnmea2000_pi(void);

    //    The required PlugIn Methods
    int Init(void);
    bool DeInit(void);

    int GetAPIVersionMajor();
    int GetAPIVersionMinor();
    int GetPlugInVersionMajor();
    int GetPlugInVersionMinor();
    // wxBitmap *GetPlugInBitmap();
    wxString GetCommonName();
    wxString GetShortDescription();
    wxString GetLongDescription();

    //    Optional plugin overrides
    void SetNMEASentence(wxString &sentence);
    void SetAISSentence(wxString &sentence);
    void SetPluginMessage(wxString &message_id, wxString &message_body);
    void SetPositionFixEx(PlugIn_Position_Fix_Ex &pfix);
    void SetSignalKSentence(wxString &sentence);

    void ShowPreferencesDialog(wxWindow* parent);

    static wxString StandardPath();
    static wxString ErrMsgPrefix();

  protected:

  private:
    wxWindow *m_parent_window;
    wxFileConfig *m_pNmea2000Config;
};

#endif
