
// CameraToolsDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "CameraTools.h"
#include "CameraToolsDlg.h"
#include "afxdialogex.h"
#include "DHCamera.h"
#include "HCCamera.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif

boost::timer tm1;

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CCameraToolsDlg 对话框



CCameraToolsDlg::CCameraToolsDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CAMERATOOLS_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCameraToolsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

}

BEGIN_MESSAGE_MAP(CCameraToolsDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_RUN, &CCameraToolsDlg::OnBnClickedButtonRun)
	ON_BN_CLICKED(IDC_OPENFILE, &CCameraToolsDlg::OnBnClickedOpenfile)
	ON_WM_DESTROY()
	ON_WM_DROPFILES()
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CCameraToolsDlg::OnBnClickedButtonStop)
	ON_BN_CLICKED(IDOK, &CCameraToolsDlg::OnBnClickedOk)
	//ON_MESSAGE(WM_DISPLAY_CHANGE1, OnDisplayChange1)  //自定义的消息映射到函数
	//ON_MESSAGE(WM_DISPLAY_CHANGE2, OnDisplayChange2)  //自定义的消息映射到函数

	ON_BN_CLICKED(IDC_SETNTP_BUTTON, &CCameraToolsDlg::OnBnClickedSetntpButton)
END_MESSAGE_MAP()


// CCameraToolsDlg 消息处理程序

BOOL CCameraToolsDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	GetDlgItem(IDC_USERNAME)->SetWindowTextA("admin");
	GetDlgItem(IDC_THREADNUM)->SetWindowTextA("10");
	GetDlgItem(IDC_TIMEOUT)->SetWindowTextA("3000");
	Utility::TimeOut = 3000;
	//大华SDK初使化
	CLIENT_Init(NULL, 0);
	NET_PARAM stuNetParam = { 0 };
	stuNetParam.nWaittime = Utility::TimeOut;		// 等待超时时间(毫秒为单位)，为0默认5000ms 
	stuNetParam.nConnectTime = Utility::TimeOut;	//连接超时时间(毫秒为单位)，为0默认1500ms 
	stuNetParam.nConnectTryNum = 3;		//连接尝试次数，为0默认1次 
	stuNetParam.nGetDevInfoTime = Utility::TimeOut;//获取设备信息超时时间，为0默认1000ms. 

	CLIENT_SetNetworkParam(&stuNetParam);
	//海康SDK初使化
	NET_DVR_Init();
	NET_DVR_SetConnectTime(Utility::TimeOut, 1);
	NET_DVR_SetReconnect(Utility::TimeOut, true);

	//加载套接字库
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	wVersionRequested = MAKEWORD(2, 2);
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0)
	{
		AfxMessageBox("Winsocket初始化失败");
	}


	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CCameraToolsDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	//清理大华
	CLIENT_Cleanup();
	//清理海康
	NET_DVR_Cleanup();
	WSACleanup();
}

void CCameraToolsDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CCameraToolsDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CCameraToolsDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void ReadNTPInfo(CString csUserName, CString csPassWord, CString csFileName, CCameraToolsDlg* that, bool bPwdMode)
{
	

	Utility::mt.lock();
	Utility::RunThreadCount++;  //全局线程线+1;
	CString csThreadID;
	csThreadID.Format("%d", Utility::RunThreadCount);
	Utility::mt.unlock();

	CString csDoNum;
	CString csError = "";	
	int iColPos;
	CString csIpAddr="";

	//建立大华和海康的实例对像
	DHCamera* dhCamera = new DHCamera;  
	HCCamera* hcCamera = new HCCamera;
	AbstractDevice* deviceCamera = nullptr;


	while (!Utility::isExitThread)
	{
		int i;
		//如果当前操作的记录号已经>=总记录数,则退出循环
		Utility::mt.lock();
		try
		{
			if (Utility::RecordNumber >= Utility::RecordCount)
			{
				Utility::isExitThread = true;
				Utility::mt.unlock();
				break;
			}
			//获取的当前操作行数
			i = Utility::RecordNumber;
			Utility::RecordNumber++;
			csIpAddr = Utility::ExecelRead(Utility::sheet, i, 0);
			//判断读取的IP是否正确
			if (!Utility::isIPaddr(csIpAddr))
			{
				Utility::mt.unlock();
				continue;
			}

			if (bPwdMode)
			{
				iColPos = 3;
				csUserName = Utility::ExecelRead(Utility::sheet, i, 1);
				csPassWord = Utility::ExecelRead(Utility::sheet, i, 2);
			}
			else
			{
				iColPos = 1;
			}

			//读取厂商,如果厂商不为空,则跳过这个IP
			CString sText = Utility::ExecelRead(Utility::sheet, i, iColPos);
			if (sText!="(null)")
			{
				Utility::mt.unlock();
				continue;
			}
			Utility::mt.unlock();
		}
		catch (...)
		{
			Utility::mt.unlock();
		}

		CString Factory = "";
		//多线程同时检测一个IP地址的多个端口状态。
		//Utility::threadDetection(csIpAddr,vecPorts, Utility::TimeOut);
		//用异步的方式检测一个IP地址的多个端口状态,只有有一个端口是通的就保存这个端口并立即返回.
		std::vector<PortConnect*> vecPort;
		PortConnect dh, hk;
		dh.nPort = 37777; dh.bIsConnect = false;
		hk.nPort = 8000;  hk.bIsConnect = false;
		vecPort.push_back(&dh);
		vecPort.push_back(&hk);
		Utility::asyncDetectionIp(csIpAddr, vecPort, Utility::TimeOut);


		//大华设备
		if (dh.bIsConnect)
		{
			Factory = "大华";
			deviceCamera = dhCamera;
			deviceCamera->nLoginPort = 37777;
		}
		//海康设备
		else if (hk.bIsConnect)
		{
			Factory = "海康";
			deviceCamera = hcCamera;
			deviceCamera->nLoginPort = 8000;
		}
		else
		{
			Factory = "检测失败";
		}
		
		if (Factory == "大华" || Factory == "海康")
		{
			
			Utility::mt.lock();
			Utility::ExcelWrite(Utility::sheet, Factory, i, iColPos);
			Utility::mt.unlock();

			deviceCamera->csLoginIpAddr = csIpAddr;   
			deviceCamera->csLoginUserName = csUserName;
			deviceCamera->csLoginPassWord = csPassWord;
			

			if (Utility::Login(deviceCamera))
			{
				//获取设备型号:
				if (Utility::GetSystemInfo(deviceCamera))
				{
					//设备型号
					Utility::mt.lock();
					Utility::ExcelWrite(Utility::sheet, deviceCamera->csDeviceTypeName, i, iColPos + 1);
					Utility::mt.unlock();
				}
				else
				{
					//当获取失败时,方法会将对像实例中的csError属性变量置为错误信息.
					csError = deviceCamera->csError;
					Utility::mt.lock();
					Utility::ExcelWrite(Utility::sheet, csError, i, iColPos + 2);
					Utility::mt.unlock();
				}

				//获取NTP信息  
				if (Utility::GetNtpInfo(deviceCamera))
				{
					CString NTP_ENABLE, NTPADDR, NTPPORT, UPDATETIME, TIMEZONE;
					if (deviceCamera->bEnableNtp)
						NTP_ENABLE = "开启";
					else
						NTP_ENABLE = "未开启";
					NTPADDR = deviceCamera->csNtpServerAddr;
					NTPPORT.Format("%d", deviceCamera->nNtpServerPort);
					UPDATETIME.Format("%d", deviceCamera->nNtpUpdatePeriod);
					TIMEZONE = deviceCamera->csNtpTimeZone;
					Utility::mt.lock();
					Utility::ExcelWrite(Utility::sheet, NTP_ENABLE, i, iColPos + 2);
					Utility::ExcelWrite(Utility::sheet, NTPADDR, i, iColPos + 3);
					Utility::ExcelWrite(Utility::sheet, NTPPORT, i, iColPos + 4);
					Utility::ExcelWrite(Utility::sheet, UPDATETIME, i, iColPos + 5);
					Utility::ExcelWrite(Utility::sheet, TIMEZONE, i, iColPos + 6);
					Utility::mt.unlock();
				}
				else
				{
					csError = deviceCamera->csError;
					Utility::mt.lock();
					Utility::ExcelWrite(Utility::sheet, csError, i, iColPos + 2);
					Utility::mt.unlock();
				}




				Utility::LogOut(deviceCamera);
			}
			else
			{
				csError = deviceCamera->csError;
				Utility::mt.lock();
				Utility::ExcelWrite(Utility::sheet, csError, i, iColPos + 1);
				Utility::mt.unlock();
			}
		}
		else 
		{
			Utility::mt.lock();
			Utility::ExcelWrite(Utility::sheet, "检测失败", i, iColPos);
			Utility::mt.unlock();
		}

		CString csRecordNumber;
		csRecordNumber.Format("%d", Utility::RecordNumber - 1);
		CString runThreadNum;
		runThreadNum.Format("%d", Utility::RunThreadCount);
		that->GetDlgItem(IDC_STATUS)->SetWindowTextA("已处理:" + csRecordNumber + "   处理状态:进行中......" + "      线程数量: " + runThreadNum);
		CProgressCtrl* ProGress = (CProgressCtrl*)that->GetDlgItem(IDC_PROGRESS1);
		ProGress->SetPos(Utility::RecordNumber);


	}
	Utility::mt.lock();
	Utility::RunThreadCount--;
	if (Utility::RunThreadCount <= 0)
	{
		Utility::RunThreadCount = 0;
		CString cstime1;
		cstime1.Format("耗时 %.2f 秒", tm1.elapsed());
		that->GetDlgItem(IDC_BUTTON_RUN)->EnableWindow(TRUE);
		that->GetDlgItem(IDC_STATUS)->SetWindowTextA("已处理:" + csDoNum + "   已全部处理完成," + cstime1);
		if (Utility::book != nullptr)
		{
			Utility::book->save(csFileName);
			Utility::book->release();
			Utility::book = nullptr;
		}
	}
	Utility::mt.unlock();

	//释放大华的实例对像
	if (dhCamera != nullptr)
	{
		delete dhCamera;
		dhCamera = nullptr;
	}
	//释放大华的实例对像
	if (hcCamera != nullptr)
	{
		delete hcCamera;
		hcCamera = nullptr;
	}


}

struct NtpSetup
{
	char szNtpServer[256]; //地址
	int  nPort;            //端口号
	int nUpdatePeriod;     //更新间隔（分钟）
};



void CCameraToolsDlg::OnBnClickedButtonRun()
{
	// TODO: 在此添加控件通知处理程序代码
	// TODO: 在此添加控件通知处理程序代码
	CString csUserName, csPassWord, csFileName;
	GetDlgItem(IDC_USERNAME)->GetWindowTextA(csUserName);
	GetDlgItem(IDC_PASSWORD)->GetWindowTextA(csPassWord);
	GetDlgItem(IDC_FILENAME)->GetWindowTextA(csFileName);
	CProgressCtrl* ProGress = (CProgressCtrl*)GetDlgItem(IDC_PROGRESS1);

	bool  PwdMode = ((CButton*)GetDlgItem(IDC_CHECK_PWDFROMFILE))->GetCheck();
	if (!PwdMode && csPassWord.IsEmpty())
	{
		AfxMessageBox("请输入密码或选择从文件读取");
		return;
	}



	if (!Utility::GetExcelSheetPtr(csFileName) || !Utility::GetExcelState(csFileName))
	{
		AfxMessageBox("打开excel文件出错");
		return;
	}

	GetDlgItem(IDC_BUTTON_RUN)->EnableWindow(FALSE);
	ProGress->SetPos(0);

	GetDlgItem(IDC_BUTTON_RUN)->EnableWindow(FALSE);
	if (Utility::isIPaddr(Utility::ExecelRead(Utility::sheet, 0, 0)))
	{
		Utility::ExcelInsertRow(Utility::sheet, 0, 0);
	}
	int iRowCount = Utility::ExcelRowCount(Utility::sheet);  //获取数据总数
	ProGress->SetRange(0, iRowCount);
	int iThreadNum = GetDlgItemInt(IDC_THREADNUM, NULL, FALSE);
	Utility::TimeOut=(UINT)GetDlgItemInt(IDC_TIMEOUT, NULL, FALSE);

	if (iThreadNum == 0 || iThreadNum > iRowCount)
	{
		iThreadNum = iRowCount;
	}

	Utility::setVar(0, 0, false, 1, iRowCount, iThreadNum);

	if (PwdMode)
	{
		int poscur = 0;
		Utility::ExcelWrite(Utility::sheet, "IP地址", 0, 0);
		Utility::ExcelWrite(Utility::sheet, "账号", 0, 1);
		Utility::ExcelWrite(Utility::sheet, "密码", 0, 2);

		Utility::ExcelWrite(Utility::sheet, "厂商", 0, 3);
		Utility::ExcelWrite(Utility::sheet, "设备型号", 0, 4);
		Utility::ExcelWrite(Utility::sheet, "NTP状态", 0, 5);
		Utility::ExcelWrite(Utility::sheet, "NTP地址", 0, 6);
		Utility::ExcelWrite(Utility::sheet, "NTP端口", 0, 7);
		Utility::ExcelWrite(Utility::sheet, "NTP更新", 0, 8);
		Utility::ExcelWrite(Utility::sheet, "NTP时区", 0, 9);
	}
	else
	{
		int poscur = 0;
		Utility::ExcelWrite(Utility::sheet, "IP地址", 0, 0);

		Utility::ExcelWrite(Utility::sheet, "厂商", 0, 1);
		Utility::ExcelWrite(Utility::sheet, "设备型号", 0, 2);
		Utility::ExcelWrite(Utility::sheet, "NTP状态", 0, 3);
		Utility::ExcelWrite(Utility::sheet, "NTP地址", 0, 4);
		Utility::ExcelWrite(Utility::sheet, "NTP端口", 0, 5);
		Utility::ExcelWrite(Utility::sheet, "NTP更新", 0, 6);
		Utility::ExcelWrite(Utility::sheet, "NTP时区", 0, 7);
	}

	tm1.restart(); //计时器重新开始计时
	std::vector<std::thread> threads;   //声名一个thread类的vector容器,用vector容器来动态创建线程数量.
	for (int t = 0; t < Utility::UserThreadNum; t++)
	{
		threads.push_back(std::thread(ReadNTPInfo, csUserName, csPassWord, csFileName, this, PwdMode));
	}
	for (unsigned int i = 0; i < threads.size(); i++)
	{
		threads[i].detach();  //非阻塞式线程,脱离
		//threads[i].join();    //阻塞模式线程,需线程都结束才能返回
	}
}



void CCameraToolsDlg::OnBnClickedOpenfile()
{
	// TODO: 在此添加控件通知处理程序代码
	GetDlgItem(IDC_FILENAME)->SetWindowTextA(Utility::GetDir());
}





void CCameraToolsDlg::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	TCHAR szPath[MAX_PATH] = { 0 };
	UINT nCount = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);

	for (UINT idx = 0; idx < nCount; ++idx)
	{
		DragQueryFile(hDropInfo, idx, szPath, MAX_PATH);
		//MessageBox(szPath);//以消息盒子形式显示路径
		//SetDlgItemText(IDC_EDIT1, szPath);//在编辑框内显示路径	
	}
	CString a = (CString)szPath;
	GetDlgItem(IDC_FILENAME)->SetWindowTextA(a);
	DragFinish(hDropInfo);
	CDialogEx::OnDropFiles(hDropInfo);
}


void CCameraToolsDlg::OnBnClickedButtonStop()
{
	// TODO: 在此添加控件通知处理程序代码
	Utility::mt.lock();
	Utility::isExitThread = true;;
	Utility::mt.unlock();

}


void CCameraToolsDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	Utility::mt.lock();
	if (Utility::RunThreadCount > 0)
	{
		
		if (Utility::book != nullptr)
		{
			CString	csFileName;
			GetDlgItem(IDC_FILENAME)->GetWindowTextA(csFileName);
			Utility::book->save(csFileName);
			Utility::book->release();
			Utility::book = nullptr;
		}
		AfxMessageBox("有线程在工作,任务未处理完成,已强制退出");
	}
	Utility::mt.unlock();
	CDialogEx::OnOK();
}



void SetNTPInfo(CString csUserName, CString csPassWord, CString csFileName, CCameraToolsDlg* that, bool bPwdMode, NtpSetup stuNtp)
{


	Utility::mt.lock();
	Utility::RunThreadCount++;  //全局线程线+1;
	CString csThreadID;
	csThreadID.Format("%d", Utility::RunThreadCount);
	Utility::mt.unlock();

	CString csDoNum;
	CString csError = "";
	int iColPos;
	CString csIpAddr = "";

	//建立大华和海康的实例对像
	DHCamera* dhCamera = new DHCamera;
	HCCamera* hcCamera = new HCCamera;
	AbstractDevice* deviceCamera = nullptr;


	while (!Utility::isExitThread)
	{
		int i;
		//如果当前操作的记录号已经>=总记录数,则退出循环
		Utility::mt.lock();
		try
		{
			if (Utility::RecordNumber >= Utility::RecordCount)
			{
				Utility::isExitThread = true;
				Utility::mt.unlock();
				break;
			}
			//获取的当前操作行数
			i = Utility::RecordNumber;
			Utility::RecordNumber++;
			csIpAddr = Utility::ExecelRead(Utility::sheet, i, 0);
			//判断读取的IP是否正确
			if (!Utility::isIPaddr(csIpAddr))
			{
				Utility::mt.unlock();
				continue;
			}

			if (bPwdMode)
			{
				iColPos = 3;
				csUserName = Utility::ExecelRead(Utility::sheet, i, 1);
				csPassWord = Utility::ExecelRead(Utility::sheet, i, 2);
			}
			else
			{
				iColPos = 1;
			}

			//读取厂商,如果厂商不为空,则跳过这个IP
			CString sText = Utility::ExecelRead(Utility::sheet, i, iColPos);
			if (sText != "(null)")
			{
				Utility::mt.unlock();
				continue;
			}
			Utility::mt.unlock();
		}
		catch (...)
		{
			Utility::mt.unlock();
		}

		CString Factory = "";
		//多线程同时检测一个IP地址的多个端口状态。
		//Utility::threadDetection(csIpAddr,vecPorts, Utility::TimeOut);
		//用异步的方式检测一个IP地址的多个端口状态,只有有一个端口是通的就保存这个端口并立即返回.
		std::vector<PortConnect*> vecPort;
		PortConnect dh, hk;
		dh.nPort = 37777; dh.bIsConnect = false;
		hk.nPort = 8000;  hk.bIsConnect = false;
		vecPort.push_back(&dh);
		vecPort.push_back(&hk);
		Utility::asyncDetectionIp(csIpAddr, vecPort, Utility::TimeOut);


		//大华设备
		if (dh.bIsConnect)
		{
			Factory = "大华";
			deviceCamera = dhCamera;
			deviceCamera->nLoginPort = 37777;
		}
		//海康设备
		else if (hk.bIsConnect)
		{
			Factory = "海康";
			deviceCamera = hcCamera;
			deviceCamera->nLoginPort = 8000;
		}
		else
		{
			Factory = "检测失败";
		}

		if (Factory == "大华" || Factory == "海康")
		{

			Utility::mt.lock();
			Utility::ExcelWrite(Utility::sheet, Factory, i, iColPos);
			Utility::mt.unlock();

			deviceCamera->csLoginIpAddr = csIpAddr;
			deviceCamera->csLoginUserName = csUserName;
			deviceCamera->csLoginPassWord = csPassWord;
			deviceCamera->bEnableNtp = true;
			deviceCamera->csNtpServerAddr = stuNtp.szNtpServer;
			deviceCamera->nNtpServerPort = stuNtp.nPort;
			deviceCamera->nNtpUpdatePeriod = stuNtp.nUpdatePeriod;

			if (Utility::Login(deviceCamera))
			{
				//获取设备型号:
				if (Utility::GetSystemInfo(deviceCamera))
				{
					//设备型号
					Utility::mt.lock();
					Utility::ExcelWrite(Utility::sheet, deviceCamera->csDeviceTypeName, i, iColPos + 1);
					Utility::mt.unlock();
				}
				else
				{
					//当获取失败时,方法会将对像实例中的csError属性变量置为错误信息.
					csError = deviceCamera->csError;
					Utility::mt.lock();
					Utility::ExcelWrite(Utility::sheet, csError, i, iColPos + 2);
					Utility::mt.unlock();
				}

				//设置NTP信息  
				if (Utility::SetNtpInfo(deviceCamera))
				{
					
					Utility::mt.lock();
					Utility::ExcelWrite(Utility::sheet, "设置成功", i, iColPos + 2);
					Utility::mt.unlock();
				}
				else
				{
					csError = deviceCamera->csError;
					Utility::mt.lock();
					Utility::ExcelWrite(Utility::sheet, csError, i, iColPos + 2);
					Utility::mt.unlock();
				}
				Utility::LogOut(deviceCamera);
			}
			else
			{
				csError = deviceCamera->csError;
				Utility::mt.lock();
				Utility::ExcelWrite(Utility::sheet, csError, i, iColPos + 1);
				Utility::mt.unlock();
			}
		}
		else
		{
			Utility::mt.lock();
			Utility::ExcelWrite(Utility::sheet, "检测失败", i, iColPos);
			Utility::mt.unlock();
		}

		CString csRecordNumber;
		csRecordNumber.Format("%d", Utility::RecordNumber - 1);
		CString runThreadNum;
		runThreadNum.Format("%d", Utility::RunThreadCount);
		that->GetDlgItem(IDC_STATUS)->SetWindowTextA("已处理:" + csRecordNumber + "   处理状态:进行中......" + "      线程数量: " + runThreadNum);
		CProgressCtrl* ProGress = (CProgressCtrl*)that->GetDlgItem(IDC_PROGRESS1);
		ProGress->SetPos(Utility::RecordNumber);


	}
	Utility::mt.lock();
	Utility::RunThreadCount--;
	if (Utility::RunThreadCount <= 0)
	{
		Utility::RunThreadCount = 0;
		CString cstime1;
		cstime1.Format("耗时 %.2f 秒", tm1.elapsed());
		that->GetDlgItem(IDC_BUTTON_RUN)->EnableWindow(TRUE);
		that->GetDlgItem(IDC_STATUS)->SetWindowTextA("已处理:" + csDoNum + "   已全部处理完成," + cstime1);
		if (Utility::book != nullptr)
		{
			Utility::book->save(csFileName);
			Utility::book->release();
			Utility::book = nullptr;
		}
	}
	Utility::mt.unlock();

	//释放大华的实例对像
	if (dhCamera != nullptr)
	{
		delete dhCamera;
		dhCamera = nullptr;
	}
	//释放大华的实例对像
	if (hcCamera != nullptr)
	{
		delete hcCamera;
		hcCamera = nullptr;
	}


}


void CCameraToolsDlg::OnBnClickedSetntpButton()
{
	// TODO: 在此添加控件通知处理程序代码
	CString csUserName, csPassWord, csFileName,csNtpServer;
	GetDlgItem(IDC_USERNAME)->GetWindowTextA(csUserName);
	GetDlgItem(IDC_PASSWORD)->GetWindowTextA(csPassWord);
	GetDlgItem(IDC_FILENAME)->GetWindowTextA(csFileName);
	GetDlgItem(IDC_NTPSERVER_EDIT)->GetWindowTextA(csNtpServer);
	NtpSetup stuNtp;
	strncpy(stuNtp.szNtpServer,(LPSTR)(LPCSTR)csNtpServer, csNtpServer.GetLength()+1);
	stuNtp.nPort = GetDlgItemInt(IDC_NTPSERVERPORT_EDIT);
	stuNtp.nUpdatePeriod = GetDlgItemInt(IDC_UPDATE_EDIT);
	CProgressCtrl* ProGress = (CProgressCtrl*)GetDlgItem(IDC_PROGRESS1);

	bool  PwdMode = ((CButton*)GetDlgItem(IDC_CHECK_PWDFROMFILE))->GetCheck();
	if (!PwdMode && csPassWord.IsEmpty())
	{
		AfxMessageBox("请输入密码或选择从文件读取");
		return;
	}



	if (!Utility::GetExcelSheetPtr(csFileName) || !Utility::GetExcelState(csFileName))
	{
		AfxMessageBox("打开excel文件出错");
		return;
	}

	GetDlgItem(IDC_BUTTON_RUN)->EnableWindow(FALSE);
	ProGress->SetPos(0);

	GetDlgItem(IDC_SETNTP_BUTTON)->EnableWindow(FALSE);

	if (Utility::isIPaddr(Utility::ExecelRead(Utility::sheet, 0, 0)))
	{
		Utility::ExcelInsertRow(Utility::sheet, 0, 0);
	}
	int iRowCount = Utility::ExcelRowCount(Utility::sheet);  //获取数据总数

	ProGress->SetRange(0, iRowCount);
	int iThreadNum = GetDlgItemInt(IDC_THREADNUM, NULL, FALSE);
	Utility::TimeOut = (UINT)GetDlgItemInt(IDC_TIMEOUT, NULL, FALSE);

	if (iThreadNum == 0 || iThreadNum > iRowCount)
	{
		iThreadNum = iRowCount;
	}

	Utility::setVar(0, 0, false, 1, iRowCount, iThreadNum);
	if (PwdMode)
	{
		int poscur = 0;
		Utility::ExcelWrite(Utility::sheet, "IP地址", 0, 0);
		Utility::ExcelWrite(Utility::sheet, "账号", 0, 1);
		Utility::ExcelWrite(Utility::sheet, "密码", 0, 2);

		Utility::ExcelWrite(Utility::sheet, "厂商", 0, 3);
		Utility::ExcelWrite(Utility::sheet, "设备型号", 0, 4);
		Utility::ExcelWrite(Utility::sheet, "操作结果", 0, 5);

	}
	else
	{
		int poscur = 0;
		Utility::ExcelWrite(Utility::sheet, "IP地址", 0, 0);

		Utility::ExcelWrite(Utility::sheet, "厂商", 0, 1);
		Utility::ExcelWrite(Utility::sheet, "设备型号", 0, 2);
		Utility::ExcelWrite(Utility::sheet, "操作结果", 0, 3);

	}

	tm1.restart(); //计时器重新开始计时
	std::vector<std::thread> threads;   //声名一个thread类的vector容器,用vector容器来动态创建线程数量.
	for (int t = 0; t < Utility::UserThreadNum; t++)
	{
		threads.push_back(std::thread(SetNTPInfo, csUserName, csPassWord, csFileName, this, PwdMode,stuNtp));
	}
	for (unsigned int i = 0; i < threads.size(); i++)
	{
		threads[i].detach();  //非阻塞式线程,脱离
	}

}
