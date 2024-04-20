#include "pch.h"
#include "HCCamera.h"
#include "include/HCGetLastErrorCN.hpp"

HCCamera::HCCamera()
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
	hLogin = -1;


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


	//是否开启了NTP服务器
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

//执行登陆前,要先给对像中的csLoginIpAddr,csLoginUserName,csLoginPassWord,iLoginPort赋值.
bool HCCamera::deviceLogin()
{
	bool bSuccess = false;

	NET_DVR_DEVICEINFO_V30 struDeviceInfo;
	memset(&struDeviceInfo, 0, sizeof(struDeviceInfo));
	NET_DVR_SetConnectTime(Utility::TimeOut, 1);
	//NET_DVR_SetReconnect(10000, true);
	this->hLogin = NET_DVR_Login_V30((LPSTR)(LPCTSTR)this->csLoginIpAddr, this->nLoginPort, (LPSTR)(LPCTSTR)this->csLoginUserName, (LPSTR)(LPCTSTR)this->csLoginPassWord, &struDeviceInfo);
	if (this->hLogin != -1)
	{
		this->nChannelNumTotal = struDeviceInfo.byChanNum;
		bSuccess = true;
	}
	else
	{
		this->deviceGetError((int)NET_DVR_GetLastError());
		bSuccess = false;
	}
	
	return bSuccess;
}

void HCCamera::deviceLogOut()
{
	NET_DVR_Logout(static_cast<LONG>(this->hLogin));
	hLogin = 0;
}

//获取错误代码的中文信息 
//iError(int类型的错误代码)
bool HCCamera::deviceGetError(int nError)
{
	this->csError ="海康:"+HCGetLastErrorCN(nError);
	return true;
}

//获取设备的系统信息
bool HCCamera::deviceGetSystemInfo()
{
	if (-1 == this->hLogin)
	{
		csError = "未登陆";
		return false;
	}
	bool bSuccess = false;

	NET_DVR_DEVICECFG_V40* m_struDeviceCfg = new NET_DVR_DEVICECFG_V40;
	memset(m_struDeviceCfg, 0, sizeof(NET_DVR_DEVICECFG_V40));
	DWORD dwReturned = 0;
	if (NET_DVR_GetDVRConfig(static_cast<LONG>(this->hLogin), NET_DVR_GET_DEVICECFG_V40, 0xFFFFFFFF, m_struDeviceCfg, sizeof(NET_DVR_DEVICECFG_V40), &dwReturned))
	{
		CString  cs_DevTypeName;

		//显示出设备名
		char cTemp[200] = { 0 };
		memset(cTemp, 0, 200);

		memcpy(cTemp, m_struDeviceCfg->byDevTypeName, 24);
		this->csDeviceTypeName.Format("%s", cTemp);

		memcpy(cTemp, m_struDeviceCfg->sSerialNumber, 48);
		this->csDeviceSerialNumber.Format("%s", cTemp);

		this->csDeviceSoftWareDate.Format("%d-%d-%d", (m_struDeviceCfg->dwSoftwareBuildDate >> 16 & 0xffff) + 2000,
			m_struDeviceCfg->dwSoftwareBuildDate >> 8 & 0xff, m_struDeviceCfg->dwSoftwareBuildDate & 0xff);
		bSuccess = true;
	}
	else
	{
		this->deviceGetError((int)NET_DVR_GetLastError());
		bSuccess = false;
	}

	if (m_struDeviceCfg != nullptr)
	{
		delete m_struDeviceCfg;
		m_struDeviceCfg = nullptr;
	}
	return bSuccess;
}

//获取NTP服务器信息
bool HCCamera::deviceGetNtpInfo()
{
	//获取NTP信息
	if (-1 == this->hLogin)
	{
		csError = "未登陆";
		return false;
	}
	bool bSuccess = false;

	NET_DVR_NTPPARA * struNTPPARA = new NET_DVR_NTPPARA;
	memset(struNTPPARA, 0, sizeof(NET_DVR_NTPPARA));
	DWORD dwReturnLen;
	if (NET_DVR_GetDVRConfig(static_cast<LONG>(this->hLogin), NET_DVR_GET_NTPCFG, -1, struNTPPARA, sizeof(NET_DVR_NTPPARA), &dwReturnLen))
	{
		CString NTP_ENABLE, NTPADDR, NTPPORT, UPDATETIME, TIMEZONE, cTimeDifferenceH, cTimeDifferenceM;
		if (1 == struNTPPARA->byEnableNTP)
			this->bEnableNtp = true;
		else
			this->bEnableNtp = false;

		this->csNtpServerAddr.Format("%s", struNTPPARA->sNTPServer);
		this->nNtpServerPort = struNTPPARA->wNtpPort;
		this->nNtpUpdatePeriod = struNTPPARA->wInterval;

		if (struNTPPARA->cTimeDifferenceH >= 0)
			cTimeDifferenceH.Format("GMT+%.2d", struNTPPARA->cTimeDifferenceH);
		else
			cTimeDifferenceH.Format("GMT-%.2d", struNTPPARA->cTimeDifferenceH);

		cTimeDifferenceM.Format("%.2d", struNTPPARA->cTimeDifferenceM);
	    this->csNtpTimeZone = cTimeDifferenceH + ":" + cTimeDifferenceM;
		bSuccess = true;
	}
	else
	{
		this->deviceGetError((int)NET_DVR_GetLastError());
		bSuccess = false;
	}

	if (struNTPPARA != nullptr)
	{
		delete struNTPPARA;
		struNTPPARA = nullptr;
	}
	return bSuccess;
}

bool HCCamera::deviceSetNtpInfo()
{
	if (-1 == this->hLogin)
	{
		csError = "未登陆";
		return false;
	}
	bool bSuccess = false;

	NET_DVR_NTPPARA* struNTPPARA = new NET_DVR_NTPPARA;
	memset(struNTPPARA, 0, sizeof(NET_DVR_NTPPARA));
	DWORD dwReturnLen;
	if (NET_DVR_GetDVRConfig(static_cast<LONG>(this->hLogin), NET_DVR_GET_NTPCFG, -1, struNTPPARA, sizeof(NET_DVR_NTPPARA), &dwReturnLen))
	{
		//设置是否使能NTP
		if (this->bEnableNtp)
			struNTPPARA->byEnableNTP = 1;
		else
			struNTPPARA->byEnableNTP = 0;
		//设置NTP服务器地址
		char* strAddr = (LPSTR)(LPCTSTR)this->csNtpServerAddr;
		int len = strlen(strAddr);

		//将自定义类中的NTP服务器IP地址复制到目标字段中.
		strncpy((char*)(struNTPPARA->sNTPServer), strAddr, len + 1);
		//设置NTP服务器服务端口
		struNTPPARA->wNtpPort = this->nNtpServerPort;
		//设置NTP更新时间,单位(分钟)
		struNTPPARA->wInterval = this->nNtpUpdatePeriod;

		struNTPPARA->cTimeDifferenceH = 8;
		struNTPPARA->cTimeDifferenceM = 0;

		if (NET_DVR_SetDVRConfig(static_cast<LONG>(this->hLogin), NET_DVR_SET_NTPCFG, -1, struNTPPARA, sizeof(NET_DVR_NTPPARA)))
		{
			bSuccess = true;
		}
		else
		{
			this->deviceGetError((int)NET_DVR_GetLastError());
			bSuccess = false;
		}
	}
	else
	{
		this->deviceGetError((int)NET_DVR_GetLastError());
		bSuccess = false;
	}

	if (struNTPPARA != nullptr)
	{
		delete struNTPPARA;
		struNTPPARA = nullptr;
	}
	return bSuccess;
}

//获取设备当前时间
bool HCCamera::deviceGetTime()
{
	return true;
}

