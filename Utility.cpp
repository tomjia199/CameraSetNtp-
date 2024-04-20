#include "pch.h"
#include "Utility.h"
#pragma warning(disable:4996)

libxl::Book*  Utility::book;                  //打开excel的book句柄
libxl::Sheet* Utility::sheet;                //打开excel的sheet句柄 
std::mutex    Utility::mt;							//互斥实例
int  Utility::RunThreadCount = 0;				//正在运行的线程总数
bool Utility::isExitThread = false;			//线程是否退出的状态位  子线程检测到true随即RunThreadCount--退出	
int  Utility::RecordNumber = 1;                 //当前线程读取到的数据序号
int  Utility::RecordCount = 0;                  //数据总条数
int  Utility::UserThreadNum = 0;				//定义的线程数组总数
int  Utility::TimeOut = 10 * 1000;


// cs_IP (被测的IP地址),i_Port(被测的端口号),n_seconds(超时时间,单位 秒)
bool Utility::isConnect(CString  cs_IP, int port, int timeout_ms)
{
	const char* ip= (const char *)cs_IP;
	//winsocket初始化代码和清理代码已移动主窗体create和destory事件中
	//WSADATA wsaData;
	//WSAStartup(MAKEWORD(2, 2), &wsaData);

	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET) {
		return false;
	}

	u_long mode = 1;
	ioctlsocket(sock, FIONBIO, &mode); // Set to non-blocking mode  

	sockaddr_in server_addr = {};
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(ip);
	server_addr.sin_port = htons(port);

	if (connect(sock, (sockaddr*)& server_addr, sizeof(server_addr)) < 0) {
		int err = WSAGetLastError();
		if (err != WSAEWOULDBLOCK) {
			closesocket(sock);
			return false;
		}
	}

	fd_set writefds;
	FD_ZERO(&writefds);
	FD_SET(sock, &writefds);

	timeval tv = {};
	tv.tv_sec = timeout_ms / 1000;
	tv.tv_usec = (timeout_ms % 1000) * 1000;

	int result = select(0, NULL, &writefds, NULL, &tv);
	if (result == SOCKET_ERROR) {
		closesocket(sock);
		return false;
	}
	else if (result == 0) {
		closesocket(sock);
		return false;
	}

	int so_error;
	int len = sizeof(so_error);
	if (getsockopt(sock, SOL_SOCKET, SO_ERROR, (char*)& so_error, &len) < 0) {
		closesocket(sock);
		return false;
	}
	closesocket(sock);
	return so_error == 0;
}

//获取excel文件的总行数
int Utility::ExcelRowCount(libxl::Sheet* sheet)
{
	if (sheet)
	{
		int icount = sheet->lastRow();
		return icount;
	}
	return 0;
}

//excel 插入行
bool Utility::ExcelInsertRow(libxl::Sheet* sheet, int iRow, int iCol)
{
	if (sheet)
	{
		if (sheet->insertRow(iRow, iCol))
		{
			return true;
		}
	}
	return false;
}

//excel 写入记录
bool Utility::ExcelWrite(libxl::Sheet* sheet, CString cs_Text, int iRow, int iCol)
{
	if (sheet)
	{
		if (sheet->writeStr(iRow, iCol, (LPSTR)(LPCTSTR)cs_Text, 0))
		{
			return true;
		}
	}
	return false;
}

//excel 读取记录
CString Utility::ExecelRead(libxl::Sheet* sheet, int iRow, int iCol)
{
	if (sheet)
	{

		const char* str1 = sheet->readStr(iRow, iCol, 0);
		CString cs_str;
		cs_str.Format("%s", str1);
		return cs_str;

	}
	return "";
}

//设置打开的excel中的tools::book和sheet全局变量
bool   Utility::GetExcelSheetPtr(CString filename)
{
	CString cs_FileExtName = Utility::GetFileExtName(filename);
	if (cs_FileExtName.MakeLower() == "xls")
	{
		Utility::book = xlCreateBook();
	}
	else if (cs_FileExtName.MakeLower() == "xlsx")
	{
		Utility::book = xlCreateXMLBook();
	}
	else 
	{
		return false;
	}

	Utility::book->setKey("Halil Kural", "windows-2723210a07c4e90162b26966a8jcdboe");
	if (Utility::book)
	{
		if (!Utility::book->load(filename))
		{
			return false;
		}
		Utility::sheet = Utility::book->getSheet(0);
		if (!Utility::sheet)
		{
			return false;
		}
		return true;
	}
	return false;
}


//判断excel文件是否已经被其它程序打开
bool   Utility::GetExcelState(CString  cs_Pathfilename)
{
	struct stat buffer;
	if (stat(cs_Pathfilename, &buffer) != 0)
	{
		AfxMessageBox("\"" + cs_Pathfilename + "\"文件不存在");
		return false;
	}
	time_t seconds = time(0);
	int s = seconds % 60;
	int m = (seconds % 3600) / 60;
	int h = (seconds % (3600 * 24)) / 3600 + 8;
	char timeBuf[128] = { 0 };

	sprintf_s(timeBuf, "\\temp%d-%d-%d.tmp", h, m, s);
	int n = cs_Pathfilename.ReverseFind('\\');
	CString cs_tempFile = cs_Pathfilename.Left(n) + timeBuf;

	if (rename(cs_Pathfilename, cs_tempFile) != 0)
	{
		AfxMessageBox("\"" + cs_Pathfilename + "\"\r\nExcel文件已被其它程序打开！！");
		return false;
	}
	if (0 == rename(cs_tempFile, cs_Pathfilename))
		return true;
}


//执行打开文件对话框,并返回选定的文件名
CString Utility::GetDir()
{
	CString szPath;
	GetModuleFileName(NULL, szPath.GetBufferSetLength(MAX_PATH + 1), MAX_PATH);
	int n = szPath.ReverseFind('\\');
	szPath = szPath.Left(n) + "\\*.xls";
	static TCHAR szFilter[] = "Excel file (*.xlsx; *.xls)|*.xlsx; *.xls||";
	//_T("ALL Files (*.*)|*.*|");
	CFileDialog hFileDlg(TRUE, NULL, NULL, OFN_FILEMUSTEXIST | OFN_READONLY | OFN_PATHMUSTEXIST, szFilter, NULL);
	//CFileDialog hFileDlg(TRUE, NULL, (LPSTR)(LPCTSTR)szPath,OFN_FILEMUSTEXIST | OFN_READONLY | OFN_PATHMUSTEXIST,szFilter,NULL);
	if (hFileDlg.DoModal() == IDOK)
	{
		return hFileDlg.GetPathName();
	}
	return "";

}


//判断IP地址是否正确
bool Utility::isIPaddr(CString IPaddr)
{
	if (IPaddr.IsEmpty())
	{
		return false;
	}
	std::regex pattern("(?=(\\b|\\D))(((\\d{1,2})|(1\\d{1,2})|(2[0-4]\\d)|(25[0-5]))\\.){3}((\\d{1,2})|(1\\d{1,2})|(2[0-4]\\d)|(25[0-5]))(?=(\\b|\\D))");

	return std::regex_match((LPSTR)(LPCTSTR)IPaddr, pattern);  //返回验证的结果

}

void    Utility::setVar(int doCount1, int RunThreadCount1, bool isExitThread1, int RecordNumber1, int RecordCount1, int iThreadNum)
{
	//doCount = doCount1;
	RunThreadCount = RunThreadCount1;
	isExitThread = isExitThread1;
	RecordNumber = RecordNumber1;
	RecordCount = RecordCount1;
	UserThreadNum = iThreadNum;
}

//根据文件名,获取文件后缀
CString Utility::GetFileExtName(CString cs_FileFullName)
{
	int nPos = cs_FileFullName.ReverseFind('.');
	CString  csFileExtName = cs_FileFullName.Right(cs_FileFullName.GetLength() - nPos - 1); // 获取扩展名
	return csFileExtName;
}

/*=======================================设备系统操作 =====================================*/
//设备登陆  
bool Utility::Login(AbstractDevice* ad)
{
	if (ad->deviceLogin())
	{
		return true;
	}
	return false;
}
void Utility::LogOut(AbstractDevice* ad)
{
	ad->deviceLogOut();
}
//获取错误代码的中文信息 
//iError(int类型的错误代码)
bool  Utility::GetError(AbstractDevice* ad,int iError)
{
	return ad->deviceGetError(iError);
}

//获取设备的系统信息
bool Utility::GetSystemInfo(AbstractDevice* ad)
{
	return ad->deviceGetSystemInfo();
}

//获取NTP服务器信息
bool Utility::GetNtpInfo(AbstractDevice* ad)
{
	return ad->deviceGetNtpInfo();
}

//设置NTP服务器信息
bool Utility::SetNtpInfo(AbstractDevice* ad)
{
	return ad->deviceSetNtpInfo();;
}

//获取设备当前时间
bool Utility::GetTime(AbstractDevice* ad)
{
	return ad->deviceGetTime();
}

/*==============================================OSD操作=============================================*/

//获取时间标签
//ChannelNumber=要获取的通道号大华从0开始,海康从1开始; 


//采用异步方式
//检测端口代码,当检测到一个端口通就立即返回，不再检测等待其它端口，但因为是异步的，检测动作同时进行，这其中也有可能检测出两个以上通的端口
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//如果要应用到检测大华或海康的37777或8000端口来区分厂家，就需要在handle_connect函数内部返回上层一个数据unsigned short success_port。
//使用智能针指当回调结束时，socket会被自动释放。


void handle_connect(const error_code& error, std::shared_ptr<tcp::socket> socket, PortConnect* vecPorts,
	boost::asio::io_service& io_service)
{
	if (!error) {
		vecPorts->bIsConnect = true;
		// 停止io_service  
		io_service.stop();
	}
	else {
		vecPorts->bIsConnect = false;
		// 停止io_service  
		io_service.stop();
	}
}

void check_ports(const std::string& ip_address, std::vector<PortConnect*>& vecPorts, boost::asio::io_service& io_service, milliseconds timeout)
{
	for (auto vecport : vecPorts)
	{
		// 使用std::make_shared创建socket对象，并使用智能指针管理其生命周期
		auto socket_ptr = std::make_shared<tcp::socket>(io_service);
		tcp::endpoint endpoint_iterator(boost::asio::ip::address::from_string(ip_address), vecport->nPort);
		// 尝试连接到端口  
		const error_code  error;
		socket_ptr->async_connect(endpoint_iterator, boost::bind(handle_connect, error, socket_ptr, vecport, std::ref(io_service)));
	}

	// 设置超时  
	boost::asio::deadline_timer timer(io_service, timeout);
	timer.async_wait(   //函数指针或者匿名函数，无论哪种方式，形参必须是const boost::system::error_code& 
		[&io_service](const boost::system::error_code& /*e*/)
		{
			io_service.stop();
		});

	// 运行io_service  
	io_service.run();  //run是阻塞的

}

void Utility::asyncDetectionIp(CString csIPaddr, std::vector<PortConnect*>& vecPort, int timeOut)
{
	boost::asio::io_service io_service;
	milliseconds timeout(timeOut); // 设置超时时间为3秒  
	unsigned short success_port = 0;
	try {
		check_ports(csIPaddr.GetString(), vecPort, io_service, timeout);
	}
	catch (std::exception& e) {
		CString csError;
		csError.Format("Exception: %s", e.what());
	}


}