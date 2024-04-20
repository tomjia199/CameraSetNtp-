#pragma once


class AbstractDevice
{
	public:
		//Զ���豸��IP��ַ
		CString csLoginIpAddr;
		//Զ���豸��½�Ķ˿ں�
		int     nLoginPort;
		//��½���û���
		CString csLoginUserName;
		//��½����
		CString csLoginPassWord;
		//��½�ɹ���ľ��
		long long  hLogin;


		//�����豸ʧ�ܷ��صĴ�����Ϣ
		CString csError;
		//�豸��ͨ������
		int     nChannelNumTotal;


		//�豸����汾����
		CString csDeviceSoftWareDate;
		//�豸�ͺ�
		CString csDeviceTypeName;
		//�豸���к�
		CString csDeviceSerialNumber;


		//�Ƿ�����NTP������
		bool    bEnableNtp;
		//NTP��ַ
		CString csNtpServerAddr;
		//NTP�˿ں�
		int     nNtpServerPort;
		//NTP����ʱ������(��λ����)
		int     nNtpUpdatePeriod;
		//NTPʱ��
		CString csNtpTimeZone;

		//�豸�ĵ�ǰʱ��
		CString csTime;

	public:
		//virtual void initVarible() 
		 void initVarible()
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
			hLogin = 0;


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


			//�Ƿ���NTP������
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
		/*=======================================�豸ϵͳ���� =====================================*/
		//�豸��½  
		virtual bool deviceLogin() = 0;

		//�豸�˳���½
		virtual void deviceLogOut() = 0;
		
		//��ȡ��������������Ϣ,iError(int���͵Ĵ������)
		virtual bool deviceGetError(int iError) = 0;
	
		//��ȡ�豸��ϵͳ��Ϣ
		virtual bool deviceGetSystemInfo() = 0;
	
		//��ȡNTP��������Ϣ
		virtual bool deviceGetNtpInfo() = 0;
		//����NTP��������Ϣ
		virtual bool deviceSetNtpInfo() = 0;


		//��ȡ�豸��ǰʱ��
		virtual bool deviceGetTime() = 0;
	
	
	

};




