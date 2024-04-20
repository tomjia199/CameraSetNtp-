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
	
	static std::mutex mt;					//互斥实例
	static int UserThreadNum;			//当前用户设定的线程数
	static int RunThreadCount;			//线程计数器,为零时,没有线程在运行
	static bool isExitThread;			//线程退出标志
	static int  RecordNumber;           //当前线程读取到的数据序号
	static int  RecordCount;			//数据总条数
	static const int ThreadArrayNum = 200;	//定义的线程数组总数
	static  int TimeOut;               //超时时间

public:
	static libxl::Book* book;    
	static libxl::Sheet* sheet;

	//获取excel文件的总行数
	static int	   ExcelRowCount(libxl::Sheet* sheet);									 
	//excel 写入记录
	static bool	   ExcelWrite(libxl::Sheet* sheet, CString cs_Text, int iRow, int iCol);  
	//excel 读取记录
	static CString ExecelRead(libxl::Sheet* sheet, int iRow, int iCol);					  
	 //excel 插入行
	static bool    ExcelInsertRow(libxl::Sheet* sheet, int iRow, int iCol);				 
	 //设置打开的excel中的tools::book和sheet全局变量
	static bool    GetExcelSheetPtr(CString filename);							         
	 //判断excel文件是否已经被其它程序打开
	static bool    GetExcelState(CString  filename);									 

public:
	//初始化Socket
	static BOOL InitSocket();
	//是否可连接
	static  bool isConnect(CString  cs_IP, int  i_Port, int n_seconds);		
	//根据文件名,获取文件后缀
	static CString GetFileExtName(CString FileFullName);								  
	//执行打开文件对话框,并返回选定的文件名
	static CString GetDir();                                                              
	//判断IP地址是否正确
	static bool    isIPaddr(CString IPaddr);											
	//设置全局变量
	static void    setVar(int doCount, int RunThreadCount, bool isExitThread, int RecordNumber, int RecordCount, int iThreadNum); 
	//用异步方式检测一个IP的多个端口是否开放
	static void asyncDetectionIp(CString csIPaddr, std::vector<PortConnect*>& vecPort, int timeOut);
public:
	/*=======================================设备系统操作 =====================================*/
	//设备登陆  
	static bool Login(AbstractDevice * ad);

	//设备退出
	static void LogOut(AbstractDevice* ad);

	//获取错误代码的中文信息, iError(int类型的错误代码)
	static bool GetError(AbstractDevice* ad,int iError);

	//获取设备的系统信息
	static bool GetSystemInfo(AbstractDevice* ad);

	//获取NTP服务器信息
	static bool GetNtpInfo(AbstractDevice* ad);

	//设置NTP服务器信息
	static bool SetNtpInfo(AbstractDevice* ad);

	//获取设备当前时间
	static bool GetTime(AbstractDevice* ad);






};

