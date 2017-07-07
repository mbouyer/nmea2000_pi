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

#ifndef __NMEA2000PropertiesDialog__
#define __NMEA2000PropertiesDialog__

#include <wx/bmpcbox.h>
#include <wx/notebook.h>
#include <wx/arrstr.h>

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

class NMEA2000PropertiesDialog : public wxDialog
{
protected:
    void OnNMEA2000PropertiesOKClick( wxCommandEvent& event );
    void OnNMEA2000PropertiesCancelClick( wxCommandEvent& event );
    void NMEA2000PropertiesGetIfList(void);
    void NMEA2000PropertiesGetRxList(void);
    void NMEA2000PropertiesGetTxList(void);
    wxBoxSizer* m_SizerDialogBox;
    wxNotebook* m_notebookProperties;
    wxPanel* m_panelBasicProperties;
    wxFlexGridSizer* m_SizerBasicProperties;
    wxPanel* m_panelIfSelectProperties;
    wxFlexGridSizer* m_SizerIfSelectProperties;
    wxListBox* m_ListIfSelectProperties;
    wxStaticText* m_TextIfSelectProperties;
    wxPanel* m_panelRxSelectProperties;
    wxFlexGridSizer* m_SizerRxSelectProperties;
    wxCheckListBox* m_ListRxSelectProperties;
    wxPanel* m_panelTxSelectProperties;
    wxFlexGridSizer* m_SizerTxSelectProperties;
    wxCheckListBox* m_ListTxSelectProperties;
    wxBoxSizer *bSizer;
    wxButton* m_buttonOK;
    wxButton* m_Cancel;

    wxArrayString *ifList;
    wxArrayString *rxList;
    wxArrayString *txList;

public:
    NMEA2000PropertiesDialog( wxWindow* parent );
    ~NMEA2000PropertiesDialog( void );

private:
    void NMEA2000PropertiesConfig( void );
};

#endif // __NMEA2000PropertiesDialog__
