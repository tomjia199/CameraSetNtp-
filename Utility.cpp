#include "pch.h"
#include "Utility.h"
#pragma warning(disable:4996)

libxl::Book*  Utility::book;                  //��excel��book���
libxl::Sheet* Utility::sheet;                //��excel��sheet��� 
std::mutex    Utility::mt;							//����ʵ��
int  Utility::RunThreadCount = 0;				//�������е��߳�����
bool Utility::isExitThread = false;			//�߳��Ƿ��˳���״̬λ  ���̼߳�⵽true�漴RunThreadCount--�˳�	
int  Utility::RecordNumber = 1;                 //��ǰ�̶߳�ȡ�����������
int  Utility::RecordCount = 0;                  //����������
int  Utility::UserThreadNum = 0;				//������߳���������
int  Utility::TimeOut = 10 * 1000;


// cs_IP (�����IP��ַ),i_Port(����Ķ˿ں�),n_seconds(��ʱʱ��,��λ ��)
bool Utility::isConnect(CString  cs_IP, int port, int timeout_ms)
{
	const char* ip= (const char *)cs_IP;
	//winsocket��ʼ�����������������ƶ�������create��destory�¼���
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

//��ȡexcel�ļ���������
int Utility::ExcelRowCount(libxl::Sheet* sheet)
{
	if (sheet)
	{
		int icount = sheet->lastRow();
		return icount;
	}
	return 0;
}

//excel ������
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

//excel д���¼
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

//excel ��ȡ��¼
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

//���ô򿪵�excel�е�tools::book��sheetȫ�ֱ���
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


//�ж�excel�ļ��Ƿ��Ѿ������������
bool   Utility::GetExcelState(CString  cs_Pathfilename)
{
	struct stat buffer;
	if (stat(cs_Pathfilename, &buffer) != 0)
	{
		AfxMessageBox("\"" + cs_Pathfilename + "\"�ļ�������");
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
		AfxMessageBox("\"" + cs_Pathfilename + "\"\r\nExcel�ļ��ѱ���������򿪣���");
		return false;
	}
	if (0 == rename(cs_tempFile, cs_Pathfilename))
		return true;
}


//ִ�д��ļ��Ի���,������ѡ�����ļ���
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


//�ж�IP��ַ�Ƿ���ȷ
bool Utility::isIPaddr(CString IPaddr)
{
	if (IPaddr.IsEmpty())
	{
		return false;
	}
	std::regex pattern("(?=(\\b|\\D))(((\\d{1,2})|(1\\d{1,2})|(2[0-4]\\d)|(25[0-5]))\\.){3}((\\d{1,2})|(1\\d{1,2})|(2[0-4]\\d)|(25[0-5]))(?=(\\b|\\D))");

	return std::regex_match((LPSTR)(LPCTSTR)IPaddr, pattern);  //������֤�Ľ��

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

//�����ļ���,��ȡ�ļ���׺
CString Utility::GetFileExtName(CString cs_FileFullName)
{
	int nPos = cs_FileFullName.ReverseFind('.');
	CString  csFileExtName = cs_FileFullName.Right(cs_FileFullName.GetLength() - nPos - 1); // ��ȡ��չ��
	return csFileExtName;
}

/*=======================================�豸ϵͳ���� =====================================*/
//�豸��½  
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
//��ȡ��������������Ϣ 
//iError(int���͵Ĵ������)
bool  Utility::GetError(AbstractDevice* ad,int iError)
{
	return ad->deviceGetError(iError);
}

//��ȡ�豸��ϵͳ��Ϣ
bool Utility::GetSystemInfo(AbstractDevice* ad)
{
	return ad->deviceGetSystemInfo();
}

//��ȡNTP��������Ϣ
bool Utility::GetNtpInfo(AbstractDevice* ad)
{
	return ad->deviceGetNtpInfo();
}

//����NTP��������Ϣ
bool Utility::SetNtpInfo(AbstractDevice* ad)
{
	return ad->deviceSetNtpInfo();;
}

//��ȡ�豸��ǰʱ��
bool Utility::GetTime(AbstractDevice* ad)
{
	return ad->deviceGetTime();
}

/*==============================================OSD����=============================================*/

//��ȡʱ���ǩ
//ChannelNumber=Ҫ��ȡ��ͨ���Ŵ󻪴�0��ʼ,������1��ʼ; 


//�����첽��ʽ
//���˿ڴ���,����⵽һ���˿�ͨ���������أ����ټ��ȴ������˿ڣ�����Ϊ���첽�ģ���⶯��ͬʱ���У�������Ҳ�п��ܼ�����������ͨ�Ķ˿�
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//���ҪӦ�õ����󻪻򺣿���37777��8000�˿������ֳ��ң�����Ҫ��handle_connect�����ڲ������ϲ�һ������unsigned short success_port��
//ʹ��������ָ���ص�����ʱ��socket�ᱻ�Զ��ͷš�


void handle_connect(const error_code& error, std::shared_ptr<tcp::socket> socket, PortConnect* vecPorts,
	boost::asio::io_service& io_service)
{
	if (!error) {
		vecPorts->bIsConnect = true;
		// ֹͣio_service  
		io_service.stop();
	}
	else {
		vecPorts->bIsConnect = false;
		// ֹͣio_service  
		io_service.stop();
	}
}

void check_ports(const std::string& ip_address, std::vector<PortConnect*>& vecPorts, boost::asio::io_service& io_service, milliseconds timeout)
{
	for (auto vecport : vecPorts)
	{
		// ʹ��std::make_shared����socket���󣬲�ʹ������ָ���������������
		auto socket_ptr = std::make_shared<tcp::socket>(io_service);
		tcp::endpoint endpoint_iterator(boost::asio::ip::address::from_string(ip_address), vecport->nPort);
		// �������ӵ��˿�  
		const error_code  error;
		socket_ptr->async_connect(endpoint_iterator, boost::bind(handle_connect, error, socket_ptr, vecport, std::ref(io_service)));
	}

	// ���ó�ʱ  
	boost::asio::deadline_timer timer(io_service, timeout);
	timer.async_wait(   //����ָ����������������������ַ�ʽ���βα�����const boost::system::error_code& 
		[&io_service](const boost::system::error_code& /*e*/)
		{
			io_service.stop();
		});

	// ����io_service  
	io_service.run();  //run��������

}

void Utility::asyncDetectionIp(CString csIPaddr, std::vector<PortConnect*>& vecPort, int timeOut)
{
	boost::asio::io_service io_service;
	milliseconds timeout(timeOut); // ���ó�ʱʱ��Ϊ3��  
	unsigned short success_port = 0;
	try {
		check_ports(csIPaddr.GetString(), vecPort, io_service, timeout);
	}
	catch (std::exception& e) {
		CString csError;
		csError.Format("Exception: %s", e.what());
	}


}