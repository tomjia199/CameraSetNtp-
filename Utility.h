#pragma once
#include "AbstractDevice.h"
#include <vector>
#include <thread>
#include <regex>
#include <mutex>

#include "include/libxl/libxl.h"
#include <mutex>
#pragma comment(lib,"include/lib/libxlx64/libxl.lib")


#include <boost/asio.hpp>  
#include <boost/bind.hpp>  
#include <boost/date_time/posix_time/posix_time.hpp>  
#include <boost/timer.hpp>
using boost::asio::ip::tcp;
using boost::system::error_code;
using boost::posix_time::milliseconds;


struct PortConnect {
	int nPort;
	bool bIsConnect;
};

class Utility
{
public:
	
	static std::mutex mt;					//����ʵ��
	static int UserThreadNum;			//��ǰ�û��趨���߳���
	static int RunThreadCount;			//�̼߳�����,Ϊ��ʱ,û���߳�������
	static bool isExitThread;			//�߳��˳���־
	static int  RecordNumber;           //��ǰ�̶߳�ȡ�����������
	static int  RecordCount;			//����������
	static const int ThreadArrayNum = 200;	//������߳���������
	static  int TimeOut;               //��ʱʱ��

public:
	static libxl::Book* book;    
	static libxl::Sheet* sheet;

	//��ȡexcel�ļ���������
	static int	   ExcelRowCount(libxl::Sheet* sheet);									 
	//excel д���¼
	static bool	   ExcelWrite(libxl::Sheet* sheet, CString cs_Text, int iRow, int iCol);  
	//excel ��ȡ��¼
	static CString ExecelRead(libxl::Sheet* sheet, int iRow, int iCol);					  
	 //excel ������
	static bool    ExcelInsertRow(libxl::Sheet* sheet, int iRow, int iCol);				 
	 //���ô򿪵�excel�е�tools::book��sheetȫ�ֱ���
	static bool    GetExcelSheetPtr(CString filename);							         
	 //�ж�excel�ļ��Ƿ��Ѿ������������
	static bool    GetExcelState(CString  filename);									 

public:
	//��ʼ��Socket
	static BOOL InitSocket();
	//�Ƿ������
	static  bool isConnect(CString  cs_IP, int  i_Port, int n_seconds);		
	//�����ļ���,��ȡ�ļ���׺
	static CString GetFileExtName(CString FileFullName);								  
	//ִ�д��ļ��Ի���,������ѡ�����ļ���
	static CString GetDir();                                                              
	//�ж�IP��ַ�Ƿ���ȷ
	static bool    isIPaddr(CString IPaddr);											
	//����ȫ�ֱ���
	static void    setVar(int doCount, int RunThreadCount, bool isExitThread, int RecordNumber, int RecordCount, int iThreadNum); 
	//���첽��ʽ���һ��IP�Ķ���˿��Ƿ񿪷�
	static void asyncDetectionIp(CString csIPaddr, std::vector<PortConnect*>& vecPort, int timeOut);
public:
	/*=======================================�豸ϵͳ���� =====================================*/
	//�豸��½  
	static bool Login(AbstractDevice * ad);

	//�豸�˳�
	static void LogOut(AbstractDevice* ad);

	//��ȡ��������������Ϣ, iError(int���͵Ĵ������)
	static bool GetError(AbstractDevice* ad,int iError);

	//��ȡ�豸��ϵͳ��Ϣ
	static bool GetSystemInfo(AbstractDevice* ad);

	//��ȡNTP��������Ϣ
	static bool GetNtpInfo(AbstractDevice* ad);

	//����NTP��������Ϣ
	static bool SetNtpInfo(AbstractDevice* ad);

	//��ȡ�豸��ǰʱ��
	static bool GetTime(AbstractDevice* ad);






};

