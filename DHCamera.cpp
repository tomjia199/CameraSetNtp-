#include "pch.h"
#include "DHCamera.h"
#include "include/DHGetLastErrorCN.hpp"
//构造函数
DHCamera::DHCamera()
{
	//远程设备的IP地址
	csLoginIpAddr="";
	//远程设备登陆的端口号
	nLoginPort=0;
	//登陆的用户名
	csLoginUserName="";
	//登陆密码
	csLoginPassWord="";
	//登陆成功后的句柄
	hLogin=0;


	//操作设备失败返回的错误信息
	csError="";
	//设备的通道数量
	nChannelNumTotal=0;


	//设备软件版本日期
	csDeviceSoftWareDate="";
	//设备型号
	csDeviceTypeName="";
	//设备序列号
	csDeviceSerialNumber="";


	//是否开启了NTP服务器
	bEnableNtp=false;
	//NTP地址
	csNtpServerAddr="";
	//NTP端口号
	nNtpServerPort=0;
	//NTP更新时间周期(单位分钟)
	nNtpUpdatePeriod=0;
	//NTP时区
	csNtpTimeZone="";


	//设备的当前时间
	csTime="";

}
//执行登陆前,要先给对像中的csLoginIpAddr,csLoginUserName,csLoginPassWord,iLoginPort赋值.
bool DHCamera::deviceLogin()
{
	bool rSuccess = false;
	NET_IN_LOGIN_WITH_HIGHLEVEL_SECURITY stInparam;
	NET_OUT_LOGIN_WITH_HIGHLEVEL_SECURITY stOutparam;  // stOutparam.stuDeviceInfo.
	memset(&stInparam, 0, sizeof(NET_IN_LOGIN_WITH_HIGHLEVEL_SECURITY));
	memset(&stOutparam, 0, sizeof(NET_OUT_LOGIN_WITH_HIGHLEVEL_SECURITY));
	stInparam.dwSize = sizeof(NET_IN_LOGIN_WITH_HIGHLEVEL_SECURITY);
	strncpy(stInparam.szIP, (LPSTR)(LPCTSTR)this->csLoginIpAddr, sizeof(stInparam.szIP) - 1);                    //为什么要减1,如果source中有NULL，strncpy复制到NULL即使没到n也提前停止
	strncpy(stInparam.szUserName, (LPSTR)(LPCTSTR)this->csLoginUserName, sizeof(stInparam.szUserName) - 1);
	strncpy(stInparam.szPassword, (LPSTR)(LPCTSTR)this->csLoginPassWord, sizeof(stInparam.szPassword) - 1);
	stInparam.nPort = this->nLoginPort;
	stInparam.emSpecCap = EM_LOGIN_SPEC_CAP_TCP;
	stOutparam.dwSize = sizeof(NET_OUT_LOGIN_WITH_HIGHLEVEL_SECURITY);
	//======================================================================================
	NET_PARAM stuNetParm = { 0 };    //NET_PARAM结构里面是网络参数，包含很多类型的超时时间
	stuNetParm.nConnectTime = Utility::TimeOut; // 登录时尝试建立链接的超时时间
	CLIENT_SetNetworkParam(&stuNetParm);
	//CLIENT_SetConnectTime(10000, 1);   //这个方法设置连接超时时间，功能同上的.nConnectTime相同
	this->hLogin = CLIENT_LoginWithHighLevelSecurity(&stInparam, &stOutparam);
	if (0 != this->hLogin)
	{
		this->nChannelNumTotal = stOutparam.stuDeviceInfo.nChanNum;
		rSuccess = true;
	}
	else
	{
		this->deviceGetError((int)CLIENT_GetLastError());
		rSuccess = false;
	}
	return rSuccess;
}

void DHCamera::deviceLogOut()
{
	CLIENT_Logout(this->hLogin);
	this->hLogin = 0;
}

//获取错误代码的中文信息 
//iError(int类型的错误代码)
bool DHCamera::deviceGetError(int iError)
{
	this->csError ="大华:"+DHGetLastErrorCN(iError);
	return true;
}

//获取设备的系统信息
bool DHCamera::deviceGetSystemInfo()
{
	if (0 == this->hLogin)
	{
		csError = "未登陆";
		return false;
	}
	DHDEV_VERSION_INFO* g_stVersionInfo = new DHDEV_VERSION_INFO;
	memset(g_stVersionInfo, 0, sizeof(DHDEV_VERSION_INFO));
	int iRet=0;
	bool rSuccess;
	BOOL bSuccess = CLIENT_QueryDevState(this->hLogin, DH_DEVSTATE_SOFTWARE, (char*)g_stVersionInfo, sizeof(DHDEV_VERSION_INFO), &iRet, Utility::TimeOut);
	if (bSuccess) 
	{
		CString cs_Year, cs_Mon, cs_Day, cs_VerTime;
		DWORD LO = LOWORD(g_stVersionInfo->dwSoftwareBuildDate);
		BYTE  b_mon = HIBYTE(LO);
		BYTE  b_day = LOBYTE(LO);
		cs_Year.Format("%d", HIWORD(g_stVersionInfo->dwSoftwareBuildDate));
		cs_Mon.Format("%d", b_mon);
		cs_Day.Format("%d", b_day);
		cs_VerTime = cs_Year + "-" + cs_Mon + "-" + cs_Day;
		this->csDeviceSoftWareDate = cs_VerTime;
		this->csDeviceTypeName.Format("%s",g_stVersionInfo->szDevType);
		this->csDeviceSerialNumber.Format("%s", g_stVersionInfo->szDevSerialNo);
		rSuccess = true;
	}
	else
	{
		this->deviceGetError((int)CLIENT_GetLastError());
		rSuccess = false;
	}

	if (g_stVersionInfo != nullptr)
	{
		delete g_stVersionInfo;
		g_stVersionInfo = nullptr;
	}
	return rSuccess;
}

//获取NTP服务器信息
bool DHCamera::deviceGetNtpInfo()
{
	//获取NTP信息
	char* szOutBuffer = new char[32 * 1024];
	memset(szOutBuffer, 0, 32 * 1024);
	CFG_NTP_INFO * dhNtpInfo = new CFG_NTP_INFO;
	memset(dhNtpInfo, 0, sizeof(CFG_NTP_INFO));
	
	int i_ret = 0;
	int nerror = 0;
	bool bSuccess = false;
	//================================================================================================获取设备参数信息
	if (CLIENT_GetNewDevConfig(hLogin, CFG_CMD_NTP, -1, szOutBuffer, 32 * 1024, &nerror, Utility::TimeOut))
	{
		int nRetLen = 0;
		if (CLIENT_ParseData(CFG_CMD_NTP, (char*)szOutBuffer, dhNtpInfo, sizeof(CFG_NTP_INFO), &nRetLen))
		{
			this->bEnableNtp = dhNtpInfo->bEnable;
			this->nNtpServerPort = dhNtpInfo->nPort;
			this->csNtpServerAddr.Format("%s", dhNtpInfo->szAddress);
			this->nNtpUpdatePeriod = dhNtpInfo->nUpdatePeriod;
			if (13 == dhNtpInfo->emTimeZoneType)
				this->csNtpTimeZone = "GMT+08:00";
			else
				this->csNtpTimeZone.Format("%d", dhNtpInfo->emTimeZoneType);
			bSuccess = true;

		}
		else
		{
			this->deviceGetError((int)CLIENT_GetLastError());
			bSuccess = false;

		}
	}
	else
	{
		this->deviceGetError((int)CLIENT_GetLastError());
		bSuccess = false;
	}
	//释放new的资源
	if (szOutBuffer != nullptr)
	{
		delete []szOutBuffer;
		szOutBuffer = nullptr;
	}
	if (dhNtpInfo != nullptr)
	{
		delete dhNtpInfo;
		dhNtpInfo = nullptr;
	}

	return bSuccess;
}

bool DHCamera::deviceSetNtpInfo()
{

	if (-1 == this->hLogin)
	{
		csError = "未登陆";
		return false;
	}
	bool bSuccess = false;
	int nerror = 0;
	char* szOutBuffer = new char[32 * 1024];
	memset(szOutBuffer, 0, 32 * 1024);
	CFG_NTP_INFO * stuNtpDH = new CFG_NTP_INFO; 
	memset(stuNtpDH, 0, sizeof(CFG_NTP_INFO));
	//获取数据到缓冲区
	if (CLIENT_GetNewDevConfig(hLogin, CFG_CMD_NTP, -1, szOutBuffer, 32 * 1024, &nerror, Utility::TimeOut))
	{
		//解析数据到结构
		if (CLIENT_ParseData(CFG_CMD_NTP, (char*)szOutBuffer, stuNtpDH, sizeof(CFG_NTP_INFO), &nerror))
		{
			stuNtpDH->bEnable = TRUE;
			strncpy(stuNtpDH->szAddress, (LPSTR)(LPCSTR)this->csNtpServerAddr, this->csNtpServerAddr.GetLength()+1);
			stuNtpDH->nPort = this->nNtpServerPort;
			stuNtpDH->nUpdatePeriod = this->nNtpUpdatePeriod;
			memset(szOutBuffer, 0, 32 * 1024);
			//打包修改过的数据到缓冲区
			if (CLIENT_PacketData(CFG_CMD_NTP, (char*)stuNtpDH, sizeof(CFG_NTP_INFO), szOutBuffer, 32 * 1024))
			{
				//设置缓冲区的数据到设备
				if (CLIENT_SetNewDevConfig(hLogin, CFG_CMD_NTP, -1, szOutBuffer, 32 * 1024, &nerror, &nerror, Utility::TimeOut))
				{

					bSuccess = true;
				}
				else
				{
					this->deviceGetError((int)CLIENT_GetLastError());
					bSuccess = false;
				}
			}
			else
			{
				this->deviceGetError((int)CLIENT_GetLastError());
				bSuccess = false;
			}
		}
		else
		{
			this->deviceGetError((int)CLIENT_GetLastError());
			bSuccess = false;
		}
	}
	else
	{
		this->deviceGetError((int)CLIENT_GetLastError());
		bSuccess = false;
	}

	

	if (stuNtpDH != nullptr)
	{
		delete stuNtpDH;
		stuNtpDH = nullptr;
	}
	return bSuccess;
	return false;
}

//获取设备当前时间
bool DHCamera::deviceGetTime()
{
	return true;
}


