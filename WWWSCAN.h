// WWWSCAN.h : main header file for the WWWSCAN application
//

#if !defined(AFX_WWWSCAN_H__4BEC54F3_3AFC_4B1D_BBAD_4D106D7A58FB__INCLUDED_)
#define AFX_WWWSCAN_H__4BEC54F3_3AFC_4B1D_BBAD_4D106D7A58FB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CWWWSCANApp:
// See WWWSCAN.cpp for the implementation of this class
//

class CWWWSCANApp : public CWinApp
{
public:
	CWWWSCANApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWWWSCANApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CWWWSCANApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WWWSCAN_H__4BEC54F3_3AFC_4B1D_BBAD_4D106D7A58FB__INCLUDED_)
