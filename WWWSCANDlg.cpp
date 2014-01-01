// WWWSCANDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WWWSCAN.h"
#include "WWWSCANDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//����ÿ���̷ָ߳�ɨ��ʱÿ��������Ŀ
#define MAX_SCAN_LINES 100


int fileline=0;
char filelines[200];
//����������
int progress=0;
int nowprogress=0;

//���廥�����
HANDLE hMutex=CreateMutex(NULL,FALSE,NULL);

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWWWSCANDlg dialog

CWWWSCANDlg::CWWWSCANDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWWWSCANDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWWWSCANDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CWWWSCANDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWWWSCANDlg)
	DDX_Control(pDX, IDC_PROGRESS1, m_progress);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CWWWSCANDlg, CDialog)
	//{{AFX_MSG_MAP(CWWWSCANDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_START, OnBtnStart)
	ON_BN_CLICKED(IDC_BTN_STOP, OnBtnStop)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWWWSCANDlg message handlers

BOOL CWWWSCANDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CWWWSCANDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CWWWSCANDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CWWWSCANDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CWWWSCANDlg::OnBtnStart() 
{
	// TODO: Add your control notification handler code here
	//��շ��ؿ�
	SetDlgItemText(IDC_EDIT_RETURN,"");
	GetDlgItem(IDC_BTN_START)->EnableWindow(FALSE);
	//���Ƚ�������IP��ַ
	CString strHostName;
	RECVPARAM *pRecvParam=new RECVPARAM;
	GetDlgItemText(IDC_EDIT_HOSTNAME,strHostName);

	//�������ʹ��ھ�������������Ժ����ݴ�������IP���߳�
	pRecvParam->hostname=strHostName;
	pRecvParam->hwnd=m_hWnd;

	//����ָʾ��������
	SetDlgItemText(IDC_PROGRESS,"���ڿ�ʼɨ��....");

	//����һ�������������̣߳���ȡ������IP
	HANDLE hThread=CreateThread(NULL,0,DNSPOD,(LPVOID)pRecvParam,0,NULL);
	CloseHandle(hThread);

	if(0==fileline)
	{
		//����һ����ȡ�ļ��������̣߳���ȡ�ֵ������
		HANDLE hFileThread=CreateThread(NULL,0,FileReadThread,(LPVOID)pRecvParam,0,NULL);
		CloseHandle(hFileThread);
		WaitForSingleObject(hFileThread,INFINITE);
    }

	//�ȴ��߳�ִ�н�������ִ��
	WaitForSingleObject(hThread,INFINITE);

	CString url;
	CString geturl;
	GetDlgItemText(IDC_EDIT_HOSTNAME,geturl);
	//ƴ��URL
	url="http://"+geturl;
	url+="/";
	//MessageBox(url);

	//���濪ʼ����ɨ����߳�
	//����64���߳̾����
    HANDLE *hThreads = new HANDLE[64];
	SCANDATA** pScanHosts = new SCANDATA*[64];
	ZeroMemory(pScanHosts,sizeof(SCANDATA*)*64);
		
	//���������е��̱߳�������
	int threadcount=0;
	for(int h=0;h<=63;h++)
    {
		pScanHosts[h] = new SCANDATA;
		pScanHosts[h]->i=h;
		pScanHosts[h]->hostname=url;
		pScanHosts[h]->hwnd=m_hWnd;
		pScanHosts[h]->m_hprogress=&m_progress;
		HANDLE hThread = CreateThread(NULL,0,ScanThreads,(LPVOID)pScanHosts[h],0,NULL);
		

		//ֻͳ���������е��߳�
		if(hThread!=NULL)
			hThreads[threadcount++]=hThread;

	}

	//��������ɨ���߳̾���洢���ȴ��߳���
	WAITTHREADS *WaitThread=new WAITTHREADS;
	WaitThread->hwnd=m_hWnd;
	WaitThread->hThreads=hThreads;
	WaitThread->threadcount=threadcount;

	//�����ȴ��̣߳������ر����д�����ɨ���̣߳�����ԭ��ť
	HANDLE waitthread=CreateThread(NULL,0,WaitThreads,(LPVOID)WaitThread,0,NULL);
	CloseHandle(waitthread);

}

DWORD WINAPI CWWWSCANDlg::DNSPOD(LPVOID lpParameter)
{
	HWND hwnd;
	hwnd=((RECVPARAM *)lpParameter)->hwnd;
	::SetDlgItemText(hwnd,IDC_PROGRESS,"��ʼ��������....");
	CString strName;
	HOSTENT *pHost;
	SOCKADDR_IN sockaddr;
		
	pHost=gethostbyname(((RECVPARAM *)lpParameter)->hostname);
	if(WSAHOST_NOT_FOUND==::WSAGetLastError() || WSANO_DATA==::WSAGetLastError())
	{
		AfxMessageBox("��������ʧ�ܣ�");
		::SetDlgItemText(hwnd,IDC_PROGRESS,"��������ʧ�ܣ��������������ʽ�Ƿ�������ȷ���߼���������ӵ�Internet��");
		return 0;
	}
	sockaddr.sin_addr.S_un.S_addr = *(DWORD*)pHost->h_addr_list[0];
	strName.Format("%s",inet_ntoa(sockaddr.sin_addr));

	//AfxMessageBox(strName);
	//::SetDlgItemText(hwnd,IDC_EDIT_RETURN,strName);
	::SetDlgItemText(hwnd,IDC_PROGRESS,"����IP���");
    return 0;
}

//����߳�������ȡ�ֵ������
DWORD WINAPI CWWWSCANDlg::FileReadThread(LPVOID lpParameter)
{
	char filecotent[300];
	memset(filecotent,0,300);
	ifstream ifs("cgi.list");
	while(!ifs.eof())
	{
		ifs.getline(filecotent,300);
		::fileline++;
	}
	itoa(::fileline-1,::filelines,10);
	::fileline=::fileline-1;
	::progress=::fileline;
	//AfxMessageBox(filelines);
	return 0;
}




//ɨ��URL���̣߳���ഴ��64��
DWORD WINAPI CWWWSCANDlg::ScanThreads(LPVOID lpParameter)
{
	CString url;
	CProgressCtrl * T_progress;
	//�߳�ID
	int id;
	id=((SCANDATA *)lpParameter)->i;

	T_progress=((SCANDATA *)lpParameter)->m_hprogress;
	T_progress->SetRange(0,::fileline);

	//����ǰ�벿��ʵʱ��ʾ������
	char ThreadId[10];
	itoa(id,ThreadId,10);
	CString threadi;
	threadi+="Thread ID";
	threadi+=ThreadId;
	
	//���嵱ǰ�߳���������������������
	int readline=1;
	int currentline=1;

	HWND hwnd=((SCANDATA *)lpParameter)->hwnd;

	//hostname��¼ɨ������������
	CString hostname=((SCANDATA *)lpParameter)->hostname;
	//�趨���ļ���ָ��,����ָ��λ��
	FILE *pFile=fopen("cgi.list","r");
	CStdioFile sf(pFile);

	//scanurl������Ǵ��ֵ��ж�ȡ����URL
	char scanurl[300];
	memset(scanurl,0,300);

	//��λÿ���̳߳�ʼ��ʱ���ڵ���
	for(readline;readline<=MAX_SCAN_LINES*id+1&&currentline<=::fileline;readline++)
	{
		//��ȡһ�У�ÿ���߳�ͨ�������ҵ��Լ���ʼ����
		sf.ReadString(scanurl,300);
		currentline++;
	}


	//����������ݣ���ʾ�ھ�̬�ı������棬�����û��鿴
	CString strProgress;
	strProgress="Thread ";
	strProgress+=ThreadId;
	strProgress+=" :";

	//���URL��ɨ��ʱ��ʱ���ɵģ�ÿ�ζ����
	CString hostname1;
	//�������������ɨ��ʱ��ʱ���ɵģ�ÿ�ζ����
	CString strProgress1;


	//����洢���ؽ�����ַ���
	CString urlstring;
	CString tempstring;
	char returnurl[40000];

	strProgress1=strProgress;

	for(int n=0;currentline<=::fileline;n++)
	{
		//����������
    	readline=1;
		if(n>0)
		{
			//ÿ��һ��������ļ�ָ��Ҫ����ƶ�6301��
			for(int tempcount=1;tempcount<=6301;tempcount++)
			{
				//���жϵ�ǰ���ǲ���С�������������������ˣ�Ӧ����ֹѭ��
				if(currentline<=::fileline)
				{
					//�����ƶ��ļ�ָ�룬������ǰ������1
					sf.ReadString(scanurl,300);
		            currentline++;
				}
				else
				{
					break;
				}
			}
		}
		for(;readline<=100&&currentline<=::fileline;readline++)
		{
			hostname1=hostname+scanurl;
			//����ػ�����ʼɨ��
			CInternetSession sess;
     		CHttpFile* pF=(CHttpFile*)sess.OpenURL(hostname1);

			//�������Ϊ�գ�˵������ʧ�ܣ���Ҫ�������ʧ������
			if(!pF)
			{
				AfxThrowInternetException(1);
				hostname1.Empty();
				memset(scanurl,0,300);
				sf.ReadString(scanurl,300);
			
				//ÿ��һ�н���ǰ��������1�����浱ǰ�к�
			    currentline++;
				pF->Close();
				sess.Close();

				//ͬһ��ʱ��ֻ����һ���߳��޸Ľ���������
				WaitForSingleObject(::hMutex,INFINITE);
				::nowprogress++;
				ReleaseMutex(::hMutex);
				continue;
			}


			strProgress1+=hostname1;
			::SetDlgItemText(hwnd,IDC_NOW_PROGRESS,strProgress1);
			DWORD dwStatusCode; 

			//ȡ�����ص�״̬�룬�������ж�
			pF->QueryInfoStatusCode(dwStatusCode);

			urlstring=hostname1;
			if(200==dwStatusCode || 403==dwStatusCode)
			{
				tempstring=hostname1;

				//ͬһ��ʱ��ֻ����һ���߳��޸ķ��ؿ��еĽ��
				WaitForSingleObject(::hMutex,INFINITE);
				memset(returnurl,0,40000);
				::GetDlgItemText(hwnd,IDC_EDIT_RETURN,returnurl,40000);
				urlstring=returnurl;
				if(strlen(returnurl))
				   urlstring+="\r\n";
				urlstring+=tempstring;
				::SetDlgItemText(hwnd,IDC_EDIT_RETURN,urlstring);

				ReleaseMutex(::hMutex);
			}

			strProgress1=strProgress;
			hostname1.Empty();
			memset(scanurl,0,300);
			sf.ReadString(scanurl,300);

			//ͬһ��ʱ��ֻ����һ���߳��޸Ľ���������
			WaitForSingleObject(::hMutex,INFINITE);
			::nowprogress++;
			ReleaseMutex(::hMutex);

			pF->Close();
			sess.Close();

			//ÿ��һ�н���ǰ��������1�����浱ǰ�к�
			currentline++;

			CString progress_string;
			int progress_now;
			char pro[300];
			memset(pro,0,300);

			//ͬһ��ʱ��ֻ����һ���߳��޸Ľ���������
			WaitForSingleObject(::hMutex,INFINITE);

			progress_now=::nowprogress*100/::progress;
			T_progress->SetPos(::nowprogress);
			itoa(progress_now,pro,10);
			progress_string+=pro;
			progress_string+="%";
			::SetDlgItemText(hwnd,IDC_PROGRESS,progress_string);

			ReleaseMutex(::hMutex);
		}
	}
	//hostnameΪ��ǰɨ���url
	//AfxMessageBox(hostname);
	fclose(pFile);
	//::SetDlgItemText(hwnd,IDC_EDIT_RETURN,szData);
	return 0;
}

void CWWWSCANDlg::OnBtnStop() 
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_BTN_START)->EnableWindow(TRUE);
}



//����߳�Ϊ�ȴ��̣߳������ȴ�ɨ���߳�ȫ���������
DWORD WINAPI CWWWSCANDlg::WaitThreads(LPVOID lpParameter)
{
	HANDLE *hThreads = new HANDLE[64];
	HWND hwnd;
	HWND hButton;
	int threadcount;

	//�������ɨ���̵߳ľ��
	hThreads=((WAITTHREADS *)lpParameter)->hThreads;
	hwnd=((WAITTHREADS *)lpParameter)->hwnd;
	threadcount=((WAITTHREADS *)lpParameter)->threadcount;



	hButton=::GetDlgItem(hwnd,IDC_BTN_START);
	//�ȴ����е��߳̽���������
    WaitForMultipleObjects(threadcount,hThreads,MAXIMUM_WAIT_OBJECTS,INFINITE); 
	
	//���е��߳̽���������˵��ɨ�������Ѿ���ɣ�����ť��Ϊ����ʹ��״̬
	::EnableWindow(hButton,TRUE);
	::SetDlgItemText(hwnd,IDC_PROGRESS,"100%");
	AfxMessageBox("ɨ����ȫ��������");
	::SetDlgItemText(hwnd,IDC_PROGRESS,"ɨ����ȫ��������");
	return 0;
}
