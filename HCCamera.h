#pragma once
//#include "AbstractDevice.h"
#include "Utility.h"
#include "include/hc/HCNetSDK.h"
#pragma comment(lib,"include/lib/hcx64/HCNetSDK.lib")


class HCCamera:public AbstractDevice
{
public:
	HCCamera();
public:
	/*=======================================设备系统操作 =====================================*/
	//设备登陆  
	virtual bool deviceLogin() override;
	//设备退出登陆
	virtual void deviceLogOut() override;
	//获取错误代码的中文信息 
	//iError(int类型的错误代码)
	virtual bool deviceGetError(int iError) override;

	//获取设备的系统信息
	virtual bool deviceGetSystemInfo() override;

	//获取NTP服务器信息
	virtual bool deviceGetNtpInfo() override;
	//设置NTP服务器信息
	virtual bool deviceSetNtpInfo() override;


	//获取设备当前时间
	virtual bool deviceGetTime() override;


	/*==============================================OSD操作=============================================*/

	//获取时间标签
	//ChannelNumber=要获取的通道号大华从0开始,海康从1开始; 


};

