#pragma once


class AbstractDevice
{
	public:
		//远程设备的IP地址
		CString csLoginIpAddr;
		//远程设备登陆的端口号
		int     nLoginPort;
		//登陆的用户名
		CString csLoginUserName;
		//登陆密码
		CString csLoginPassWord;
		//登陆成功后的句柄
		long long  hLogin;


		//操作设备失败返回的错误信息
		CString csError;
		//设备的通道数量
		int     nChannelNumTotal;


		//设备软件版本日期
		CString csDeviceSoftWareDate;
		//设备型号
		CString csDeviceTypeName;
		//设备序列号
		CString csDeviceSerialNumber;


		//是否开启了NTP服务器
		bool    bEnableNtp;
		//NTP地址
		CString csNtpServerAddr;
		//NTP端口号
		int     nNtpServerPort;
		//NTP更新时间周期(单位分钟)
		int     nNtpUpdatePeriod;
		//NTP时区
		CString csNtpTimeZone;

		//设备的当前时间
		CString csTime;

	public:
		//virtual void initVarible() 
		 void initVarible()
		{
			//远程设备的IP地址
			csLoginIpAddr = "";
			//远程设备登陆的端口号
			nLoginPort = 0;
			//登陆的用户名
			csLoginUserName = "";
			//登陆密码
			csLoginPassWord = "";
			//登陆成功后的句柄
			hLogin = 0;


			//操作设备失败返回的错误信息
			csError = "";
			//设备的通道数量
			nChannelNumTotal = 0;


			//设备软件版本日期
			csDeviceSoftWareDate = "";
			//设备型号
			csDeviceTypeName = "";
			//设备序列号
			csDeviceSerialNumber = "";


			//是否开启NTP服务器
			bEnableNtp = false;
			//NTP地址
			csNtpServerAddr = "";
			//NTP端口号
			nNtpServerPort = 0;
			//NTP更新时间周期(单位分钟)
			nNtpUpdatePeriod = 0;
			//NTP时区
			csNtpTimeZone = "";


			//设备的当前时间
			csTime = "";

		}
		/*=======================================设备系统操作 =====================================*/
		//设备登陆  
		virtual bool deviceLogin() = 0;

		//设备退出登陆
		virtual void deviceLogOut() = 0;
		
		//获取错误代码的中文信息,iError(int类型的错误代码)
		virtual bool deviceGetError(int iError) = 0;
	
		//获取设备的系统信息
		virtual bool deviceGetSystemInfo() = 0;
	
		//获取NTP服务器信息
		virtual bool deviceGetNtpInfo() = 0;
		//设置NTP服务器信息
		virtual bool deviceSetNtpInfo() = 0;


		//获取设备当前时间
		virtual bool deviceGetTime() = 0;
	
	
	

};




