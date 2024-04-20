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
	/*=======================================�豸ϵͳ���� =====================================*/
	//�豸��½  
	virtual bool deviceLogin() override;
	//�豸�˳���½
	virtual void deviceLogOut() override;
	//��ȡ��������������Ϣ 
	//iError(int���͵Ĵ������)
	virtual bool deviceGetError(int iError) override;

	//��ȡ�豸��ϵͳ��Ϣ
	virtual bool deviceGetSystemInfo() override;

	//��ȡNTP��������Ϣ
	virtual bool deviceGetNtpInfo() override;
	//����NTP��������Ϣ
	virtual bool deviceSetNtpInfo() override;


	//��ȡ�豸��ǰʱ��
	virtual bool deviceGetTime() override;


	/*==============================================OSD����=============================================*/

	//��ȡʱ���ǩ
	//ChannelNumber=Ҫ��ȡ��ͨ���Ŵ󻪴�0��ʼ,������1��ʼ; 


};

