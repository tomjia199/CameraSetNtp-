#pragma once

#include <mutex>
#include "fstream"
#include <thread>
#include <regex>


class AbstractDevice
{
	public:
		//远程设备的IP地址
		CString csLoginIpAddr;
		//远程设备登陆的端口号
		int     iLoginPort;
		//登陆的用户名
		CString csLoginUserName;
		//登陆密码
		CString csLoginPassWord;
		//登陆成功后的句柄
		LONG    hLogin;
		//操作设备失败返回的错误信息
		CString csError;
		//设备的通道数量
		int     iChannelNumTotal;
		//设备软件版本日期
		CString csDeviceSoftWareDate;
		//设备型号
		CString csDeviceTypeName;
		//设备序列号
		CString csSerialNumber;
		//是否开启了NTP服务器
		bool    enableNtp; 
		//NTP地址
		CString csNtpServerAddr;
		//NTP端口号
		int     csNtpServerPort;
		//NTP更新时间周期(单位分钟)
		int     csNtpUpdatePeriod;
		//设备的当前时间
		CString csTime;
		//是否使能时间标签
		bool enableTimeTitle;
		//是否显示星期
		bool enableShowWeek; 
		//时间标签的X坐标
		int  csTimeLeft;
		//时间标签Y坐标
		int  csTimeTop;
		//是否使能了通道名称
		bool enableChannelNameTitle;
		//通道名称
		CString csChannelName;
		//通道名称X坐标
		CString csChannelNameLeft;
		//通道名称Y坐标.
		CString csChannelNameTop;
		//是否使能自定义的名称         
		bool enableCustomNameTitle;
		//自定义名字
		CString csCustomName;
		//自定义X坐标
		CString csCustomNameLeft;
		//自定义Y坐标
		CString csCustomNameTop;
		//自定义排列方式
		CString csCustomNameTextAlign;
	public:
		
		/*=======================================设备系统操作 =====================================*/
		//设备登陆  
		virtual LONG deviceLogin()=0;
		
		//获取错误代码的中文信息 
		//iError(int类型的错误代码)
		virtual void deviceGetError(int iError) = 0;
	
		//获取设备的系统信息
		virtual bool deviceGetSystemInfo() = 0;
	
		//获取NTP服务器信息
		virtual bool deviceGetNtpInfo() = 0;
		
		//获取设备当前时间
		virtual bool deviceGetTime() = 0;
	
	
		/*==============================================OSD操作=============================================*/
	
		//获取时间标签
		//ChannelNumber=要获取的通道号大华从0开始,海康从1开始; 
		virtual bool deviceGetTimeTitle(int ChannelNumber) = 0;
	
		//获取通道名标签
		virtual bool deviceGetChannelNameTitle(int ChannelNumber) = 0;
	
		//获取自定义标签信息
		virtual bool deviceGetCustomNameTitle(int ChannelNumber) = 0;




};




