#include "pch.h"
#include "HCCamera.h"
#include "include/HCGetLastErrorCN.hpp"

HCCamera::HCCamera()
{
	//Զ���豸��IP��ַ
	csLoginIpAddr = "";
	//Զ���豸��½�Ķ˿ں�
	nLoginPort = 0;
	//��½���û���
	csLoginUserName = "";
	//��½����
	csLoginPassWord = "";
	//��½�ɹ���ľ��
	hLogin = -1;


	//�����豸ʧ�ܷ��صĴ�����Ϣ
	csError = "";
	//�豸��ͨ������
	nChannelNumTotal = 0;


	//�豸����汾����
	csDeviceSoftWareDate = "";
	//�豸�ͺ�
	csDeviceTypeName = "";
	//�豸���к�
	csDeviceSerialNumber = "";


	//�Ƿ�����NTP������
	bEnableNtp = false;
	//NTP��ַ
	csNtpServerAddr = "";
	//NTP�˿ں�
	nNtpServerPort = 0;
	//NTP����ʱ������(��λ����)
	nNtpUpdatePeriod = 0;
	//NTPʱ��
	csNtpTimeZone = "";


	//�豸�ĵ�ǰʱ��
	csTime = "";

}

//ִ�е�½ǰ,Ҫ�ȸ������е�csLoginIpAddr,csLoginUserName,csLoginPassWord,iLoginPort��ֵ.
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

//��ȡ��������������Ϣ 
//iError(int���͵Ĵ������)
bool HCCamera::deviceGetError(int nError)
{
	this->csError ="����:"+HCGetLastErrorCN(nError);
	return true;
}

//��ȡ�豸��ϵͳ��Ϣ
bool HCCamera::deviceGetSystemInfo()
{
	if (-1 == this->hLogin)
	{
		csError = "δ��½";
		return false;
	}
	bool bSuccess = false;

	NET_DVR_DEVICECFG_V40* m_struDeviceCfg = new NET_DVR_DEVICECFG_V40;
	memset(m_struDeviceCfg, 0, sizeof(NET_DVR_DEVICECFG_V40));
	DWORD dwReturned = 0;
	if (NET_DVR_GetDVRConfig(static_cast<LONG>(this->hLogin), NET_DVR_GET_DEVICECFG_V40, 0xFFFFFFFF, m_struDeviceCfg, sizeof(NET_DVR_DEVICECFG_V40), &dwReturned))
	{
		CString  cs_DevTypeName;

		//��ʾ���豸��
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

//��ȡNTP��������Ϣ
bool HCCamera::deviceGetNtpInfo()
{
	//��ȡNTP��Ϣ
	if (-1 == this->hLogin)
	{
		csError = "δ��½";
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
		csError = "δ��½";
		return false;
	}
	bool bSuccess = false;

	NET_DVR_NTPPARA* struNTPPARA = new NET_DVR_NTPPARA;
	memset(struNTPPARA, 0, sizeof(NET_DVR_NTPPARA));
	DWORD dwReturnLen;
	if (NET_DVR_GetDVRConfig(static_cast<LONG>(this->hLogin), NET_DVR_GET_NTPCFG, -1, struNTPPARA, sizeof(NET_DVR_NTPPARA), &dwReturnLen))
	{
		//�����Ƿ�ʹ��NTP
		if (this->bEnableNtp)
			struNTPPARA->byEnableNTP = 1;
		else
			struNTPPARA->byEnableNTP = 0;
		//����NTP��������ַ
		char* strAddr = (LPSTR)(LPCTSTR)this->csNtpServerAddr;
		int len = strlen(strAddr);

		//���Զ������е�NTP������IP��ַ���Ƶ�Ŀ���ֶ���.
		strncpy((char*)(struNTPPARA->sNTPServer), strAddr, len + 1);
		//����NTP����������˿�
		struNTPPARA->wNtpPort = this->nNtpServerPort;
		//����NTP����ʱ��,��λ(����)
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

//��ȡ�豸��ǰʱ��
bool HCCamera::deviceGetTime()
{
	return true;
}

