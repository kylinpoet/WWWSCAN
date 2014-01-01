// WWWSCANDlg.h : header file
//

#if !defined(AFX_WWWSCANDLG_H__2A82BC2D_FFA8_4F98_85D5_0D749738E014__INCLUDED_)
#define AFX_WWWSCANDLG_H__2A82BC2D_FFA8_4F98_85D5_0D749738E014__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CWWWSCANDlg dialog

struct RECVPARAM
{
	CString hostname;
	HWND hwnd;
};


//����ṹ��ר���������ɨ���߳�����Ҫ������
typedef struct
{
	int i;                        //��ʾ�ڼ����߳�
	CString hostname;             //��ǰҪɨ�����վURL
	HWND hwnd;
	CProgressCtrl * m_hprogress;  //����������ָ�봫���̣߳��������ǲ���������
}SCANDATA;


struct WAITTHREADS
{
	HWND hwnd;
	int threadcount;
	HANDLE *hThreads;
};


class CWWWSCANDlg : public CDialog
{
// Construction
public:
	static DWORD WINAPI WaitThreads(LPVOID lpParameter);
	static DWORD WINAPI ScanThreads(LPVOID lpParameter);
	static DWORD WINAPI FileReadThread(LPVOID lpParameter);
	static DWORD WINAPI DNSPOD(LPVOID lpParameter);
	CWWWSCANDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CWWWSCANDlg)
	enum { IDD = IDD_WWWSCAN_DIALOG };
	CProgressCtrl m_progress;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWWWSCANDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CWWWSCANDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBtnStart();
	afx_msg void OnBtnStop();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WWWSCANDLG_H__2A82BC2D_FFA8_4F98_85D5_0D749738E014__INCLUDED_)
