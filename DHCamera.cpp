#include "pch.h"
#include "DHCamera.h"
#include "include/DHGetLastErrorCN.hpp"
//���캯��
DHCamera::DHCamera()
{
	//Զ���豸��IP��ַ
	csLoginIpAddr="";
	//Զ���豸��½�Ķ˿ں�
	nLoginPort=0;
	//��½���û���
	csLoginUserName="";
	//��½����
	csLoginPassWord="";
	//��½�ɹ���ľ��
	hLogin=0;


	//�����豸ʧ�ܷ��صĴ�����Ϣ
	csError="";
	//�豸��ͨ������
	nChannelNumTotal=0;


	//�豸����汾����
	csDeviceSoftWareDate="";
	//�豸�ͺ�
	csDeviceTypeName="";
	//�豸���к�
	csDeviceSerialNumber="";


	//�Ƿ�����NTP������
	bEnableNtp=false;
	//NTP��ַ
	csNtpServerAddr="";
	//NTP�˿ں�
	nNtpServerPort=0;
	//NTP����ʱ������(��λ����)
	nNtpUpdatePeriod=0;
	//NTPʱ��
	csNtpTimeZone="";


	//�豸�ĵ�ǰʱ��
	csTime="";

}
//ִ�е�½ǰ,Ҫ�ȸ������е�csLoginIpAddr,csLoginUserName,csLoginPassWord,iLoginPort��ֵ.
bool DHCamera::deviceLogin()
{
	bool rSuccess = false;
	NET_IN_LOGIN_WITH_HIGHLEVEL_SECURITY stInparam;
	NET_OUT_LOGIN_WITH_HIGHLEVEL_SECURITY stOutparam;  // stOutparam.stuDeviceInfo.
	memset(&stInparam, 0, sizeof(NET_IN_LOGIN_WITH_HIGHLEVEL_SECURITY));
	memset(&stOutparam, 0, sizeof(NET_OUT_LOGIN_WITH_HIGHLEVEL_SECURITY));
	stInparam.dwSize = sizeof(NET_IN_LOGIN_WITH_HIGHLEVEL_SECURITY);
	strncpy(stInparam.szIP, (LPSTR)(LPCTSTR)this->csLoginIpAddr, sizeof(stInparam.szIP) - 1);                    //ΪʲôҪ��1,���source����NULL��strncpy���Ƶ�NULL��ʹû��nҲ��ǰֹͣ
	strncpy(stInparam.szUserName, (LPSTR)(LPCTSTR)this->csLoginUserName, sizeof(stInparam.szUserName) - 1);
	strncpy(stInparam.szPassword, (LPSTR)(LPCTSTR)this->csLoginPassWord, sizeof(stInparam.szPassword) - 1);
	stInparam.nPort = this->nLoginPort;
	stInparam.emSpecCap = EM_LOGIN_SPEC_CAP_TCP;
	stOutparam.dwSize = sizeof(NET_OUT_LOGIN_WITH_HIGHLEVEL_SECURITY);
	//======================================================================================
	NET_PARAM stuNetParm = { 0 };    //NET_PARAM�ṹ��������������������ܶ����͵ĳ�ʱʱ��
	stuNetParm.nConnectTime = Utility::TimeOut; // ��¼ʱ���Խ������ӵĳ�ʱʱ��
	CLIENT_SetNetworkParam(&stuNetParm);
	//CLIENT_SetConnectTime(10000, 1);   //��������������ӳ�ʱʱ�䣬����ͬ�ϵ�.nConnectTime��ͬ
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

//��ȡ��������������Ϣ 
//iError(int���͵Ĵ������)
bool DHCamera::deviceGetError(int iError)
{
	this->csError ="��:"+DHGetLastErrorCN(iError);
	return true;
}

//��ȡ�豸��ϵͳ��Ϣ
bool DHCamera::deviceGetSystemInfo()
{
	if (0 == this->hLogin)
	{
		csError = "δ��½";
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

//��ȡNTP��������Ϣ
bool DHCamera::deviceGetNtpInfo()
{
	//��ȡNTP��Ϣ
	char* szOutBuffer = new char[32 * 1024];
	memset(szOutBuffer, 0, 32 * 1024);
	CFG_NTP_INFO * dhNtpInfo = new CFG_NTP_INFO;
	memset(dhNtpInfo, 0, sizeof(CFG_NTP_INFO));
	
	int i_ret = 0;
	int nerror = 0;
	bool bSuccess = false;
	//================================================================================================��ȡ�豸������Ϣ
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
	//�ͷ�new����Դ
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
		csError = "δ��½";
		return false;
	}
	bool bSuccess = false;
	int nerror = 0;
	char* szOutBuffer = new char[32 * 1024];
	memset(szOutBuffer, 0, 32 * 1024);
	CFG_NTP_INFO * stuNtpDH = new CFG_NTP_INFO; 
	memset(stuNtpDH, 0, sizeof(CFG_NTP_INFO));
	//��ȡ���ݵ�������
	if (CLIENT_GetNewDevConfig(hLogin, CFG_CMD_NTP, -1, szOutBuffer, 32 * 1024, &nerror, Utility::TimeOut))
	{
		//�������ݵ��ṹ
		if (CLIENT_ParseData(CFG_CMD_NTP, (char*)szOutBuffer, stuNtpDH, sizeof(CFG_NTP_INFO), &nerror))
		{
			stuNtpDH->bEnable = TRUE;
			strncpy(stuNtpDH->szAddress, (LPSTR)(LPCSTR)this->csNtpServerAddr, this->csNtpServerAddr.GetLength()+1);
			stuNtpDH->nPort = this->nNtpServerPort;
			stuNtpDH->nUpdatePeriod = this->nNtpUpdatePeriod;
			memset(szOutBuffer, 0, 32 * 1024);
			//����޸Ĺ������ݵ�������
			if (CLIENT_PacketData(CFG_CMD_NTP, (char*)stuNtpDH, sizeof(CFG_NTP_INFO), szOutBuffer, 32 * 1024))
			{
				//���û����������ݵ��豸
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

//��ȡ�豸��ǰʱ��
bool DHCamera::deviceGetTime()
{
	return true;
}


