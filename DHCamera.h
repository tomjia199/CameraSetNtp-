#pragma once
//#include "AbstractDevice.h"
#include "Utility.h"
#include "include/dh/dhconfigsdk.h"
#include "include/dh/dhnetsdk.h"
#pragma comment(lib,"include/lib/dhx64/dhnetsdk.lib")
#pragma comment(lib,"include/lib/dhx64/dhconfigsdk.lib")


class DHCamera :public AbstractDevice   //�̳�AbstractDevice,�������ض����з���.
{
public:
	DHCamera();
public:
	/*=======================================�豸ϵͳ���� =====================================*/
	//�豸��½  
	virtual bool deviceLogin() override;

	//�豸�˳�
	virtual void deviceLogOut() override;

	//��ȡ��������������Ϣ ,iError(int���͵Ĵ������)
	virtual bool deviceGetError(int iError) override;
	
	//��ȡ�豸��ϵͳ��Ϣ
	virtual bool deviceGetSystemInfo() override;

	//��ȡNTP��������Ϣ
	virtual bool deviceGetNtpInfo() override;
	//��ȡNTP��������Ϣ
	virtual bool deviceSetNtpInfo() override;


	//��ȡ�豸��ǰʱ��
	virtual bool deviceGetTime() override;


	/*==============================================OSD����=============================================*/

	//��ȡʱ���ǩ
	//ChannelNumber=Ҫ��ȡ��ͨ���Ŵ󻪴�0��ʼ,������1��ʼ; 

};

