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


//定义每条线程分割扫描时每块的最大数目
#define MAX_SCAN_LINES 100


int fileline=0;
char filelines[200];
//进度条变量
int progress=0;
int nowprogress=0;

//定义互斥对象
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
	//清空返回框
	SetDlgItemText(IDC_EDIT_RETURN,"");
	GetDlgItem(IDC_BTN_START)->EnableWindow(FALSE);
	//首先解析出其IP地址
	CString strHostName;
	RECVPARAM *pRecvParam=new RECVPARAM;
	GetDlgItemText(IDC_EDIT_HOSTNAME,strHostName);

	//将域名和窗口句柄保存下来，以后将数据传给解析IP的线程
	pRecvParam->hostname=strHostName;
	pRecvParam->hwnd=m_hWnd;

	//设置指示区的文字
	SetDlgItemText(IDC_PROGRESS,"正在开始扫描....");

	//创建一个解析域名的线程，获取主机的IP
	HANDLE hThread=CreateThread(NULL,0,DNSPOD,(LPVOID)pRecvParam,0,NULL);
	CloseHandle(hThread);

	if(0==fileline)
	{
		//创建一个获取文件行数的线程，获取字典的行数
		HANDLE hFileThread=CreateThread(NULL,0,FileReadThread,(LPVOID)pRecvParam,0,NULL);
		CloseHandle(hFileThread);
		WaitForSingleObject(hFileThread,INFINITE);
    }

	//等待线程执行结束后再执行
	WaitForSingleObject(hThread,INFINITE);

	CString url;
	CString geturl;
	GetDlgItemText(IDC_EDIT_HOSTNAME,geturl);
	//拼接URL
	url="http://"+geturl;
	url+="/";
	//MessageBox(url);

	//下面开始创建扫描的线程
	//创建64个线程句柄数
    HANDLE *hThreads = new HANDLE[64];
	SCANDATA** pScanHosts = new SCANDATA*[64];
	ZeroMemory(pScanHosts,sizeof(SCANDATA*)*64);
		
	//将正常运行的线程保存起来
	int threadcount=0;
	for(int h=0;h<=63;h++)
    {
		pScanHosts[h] = new SCANDATA;
		pScanHosts[h]->i=h;
		pScanHosts[h]->hostname=url;
		pScanHosts[h]->hwnd=m_hWnd;
		pScanHosts[h]->m_hprogress=&m_progress;
		HANDLE hThread = CreateThread(NULL,0,ScanThreads,(LPVOID)pScanHosts[h],0,NULL);
		

		//只统计正常运行的线程
		if(hThread!=NULL)
			hThreads[threadcount++]=hThread;

	}

	//将创建的扫描线程句柄存储到等待线程中
	WAITTHREADS *WaitThread=new WAITTHREADS;
	WaitThread->hwnd=m_hWnd;
	WaitThread->hThreads=hThreads;
	WaitThread->threadcount=threadcount;

	//创建等待线程，由他关闭所有创建的扫描线程，并复原按钮
	HANDLE waitthread=CreateThread(NULL,0,WaitThreads,(LPVOID)WaitThread,0,NULL);
	CloseHandle(waitthread);

}

DWORD WINAPI CWWWSCANDlg::DNSPOD(LPVOID lpParameter)
{
	HWND hwnd;
	hwnd=((RECVPARAM *)lpParameter)->hwnd;
	::SetDlgItemText(hwnd,IDC_PROGRESS,"开始解析域名....");
	CString strName;
	HOSTENT *pHost;
	SOCKADDR_IN sockaddr;
		
	pHost=gethostbyname(((RECVPARAM *)lpParameter)->hostname);
	if(WSAHOST_NOT_FOUND==::WSAGetLastError() || WSANO_DATA==::WSAGetLastError())
	{
		AfxMessageBox("解析域名失败！");
		::SetDlgItemText(hwnd,IDC_PROGRESS,"解析域名失败！请检查你的域名格式是否输入正确或者计算机已连接到Internet中");
		return 0;
	}
	sockaddr.sin_addr.S_un.S_addr = *(DWORD*)pHost->h_addr_list[0];
	strName.Format("%s",inet_ntoa(sockaddr.sin_addr));

	//AfxMessageBox(strName);
	//::SetDlgItemText(hwnd,IDC_EDIT_RETURN,strName);
	::SetDlgItemText(hwnd,IDC_PROGRESS,"解析IP完成");
    return 0;
}

//这个线程用来获取字典的行数
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




//扫描URL的线程，最多创建64个
DWORD WINAPI CWWWSCANDlg::ScanThreads(LPVOID lpParameter)
{
	CString url;
	CProgressCtrl * T_progress;
	//线程ID
	int id;
	id=((SCANDATA *)lpParameter)->i;

	T_progress=((SCANDATA *)lpParameter)->m_hprogress;
	T_progress->SetRange(0,::fileline);

	//构造前半部分实时显示的数据
	char ThreadId[10];
	itoa(id,ThreadId,10);
	CString threadi;
	threadi+="Thread ID";
	threadi+=ThreadId;
	
	//定义当前线程在其区域内所读到的行
	int readline=1;
	int currentline=1;

	HWND hwnd=((SCANDATA *)lpParameter)->hwnd;

	//hostname记录扫描主机的域名
	CString hostname=((SCANDATA *)lpParameter)->hostname;
	//设定好文件的指针,读到指定位置
	FILE *pFile=fopen("cgi.list","r");
	CStdioFile sf(pFile);

	//scanurl定义的是从字典中读取到的URL
	char scanurl[300];
	memset(scanurl,0,300);

	//定位每个线程初始化时所在的行
	for(readline;readline<=MAX_SCAN_LINES*id+1&&currentline<=::fileline;readline++)
	{
		//读取一行，每个线程通过参数找到自己开始的行
		sf.ReadString(scanurl,300);
		currentline++;
	}


	//构造进度数据，显示在静态文本框里面，方便用户查看
	CString strProgress;
	strProgress="Thread ";
	strProgress+=ThreadId;
	strProgress+=" :";

	//这个URL是扫描时临时生成的，每次都会变
	CString hostname1;
	//这个进度数据是扫描时临时生成的，每次都会变
	CString strProgress1;


	//定义存储返回结果的字符串
	CString urlstring;
	CString tempstring;
	char returnurl[40000];

	strProgress1=strProgress;

	for(int n=0;currentline<=::fileline;n++)
	{
		//将变量重置
    	readline=1;
		if(n>0)
		{
			//每次一块结束后，文件指针要向后移动6301行
			for(int tempcount=1;tempcount<=6301;tempcount++)
			{
				//先判断当前行是不是小于最大行数，如果超过了，应该终止循环
				if(currentline<=::fileline)
				{
					//向下移动文件指针，并将当前行数加1
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
			//构造回话并开始扫描
			CInternetSession sess;
     		CHttpFile* pF=(CHttpFile*)sess.OpenURL(hostname1);

			//如果返回为空，说明请求失败，需要丢弃这个失败请求
			if(!pF)
			{
				AfxThrowInternetException(1);
				hostname1.Empty();
				memset(scanurl,0,300);
				sf.ReadString(scanurl,300);
			
				//每读一行将当前行数加上1，保存当前行号
			    currentline++;
				pF->Close();
				sess.Close();

				//同一个时刻只能有一个线程修改进度条变量
				WaitForSingleObject(::hMutex,INFINITE);
				::nowprogress++;
				ReleaseMutex(::hMutex);
				continue;
			}


			strProgress1+=hostname1;
			::SetDlgItemText(hwnd,IDC_NOW_PROGRESS,strProgress1);
			DWORD dwStatusCode; 

			//取出返回的状态码，并作出判断
			pF->QueryInfoStatusCode(dwStatusCode);

			urlstring=hostname1;
			if(200==dwStatusCode || 403==dwStatusCode)
			{
				tempstring=hostname1;

				//同一个时刻只能有一个线程修改返回框中的结果
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

			//同一个时刻只能有一个线程修改进度条变量
			WaitForSingleObject(::hMutex,INFINITE);
			::nowprogress++;
			ReleaseMutex(::hMutex);

			pF->Close();
			sess.Close();

			//每读一行将当前行数加上1，保存当前行号
			currentline++;

			CString progress_string;
			int progress_now;
			char pro[300];
			memset(pro,0,300);

			//同一个时刻只能有一个线程修改进度条进度
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
	//hostname为当前扫描的url
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



//这个线程为等待线程，用来等待扫描线程全部完成任务
DWORD WINAPI CWWWSCANDlg::WaitThreads(LPVOID lpParameter)
{
	HANDLE *hThreads = new HANDLE[64];
	HWND hwnd;
	HWND hButton;
	int threadcount;

	//获得所有扫描线程的句柄
	hThreads=((WAITTHREADS *)lpParameter)->hThreads;
	hwnd=((WAITTHREADS *)lpParameter)->hwnd;
	threadcount=((WAITTHREADS *)lpParameter)->threadcount;



	hButton=::GetDlgItem(hwnd,IDC_BTN_START);
	//等待所有的线程结束其任务
    WaitForMultipleObjects(threadcount,hThreads,MAXIMUM_WAIT_OBJECTS,INFINITE); 
	
	//所有的线程结束工作后说明扫描任务已经完成，将按钮设为可以使用状态
	::EnableWindow(hButton,TRUE);
	::SetDlgItemText(hwnd,IDC_PROGRESS,"100%");
	AfxMessageBox("扫描已全部结束！");
	::SetDlgItemText(hwnd,IDC_PROGRESS,"扫描已全部结束！");
	return 0;
}
