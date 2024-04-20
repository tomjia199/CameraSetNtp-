#pragma once

#include <mutex>
#include "fstream"
#include <thread>
#include <regex>


class AbstractDevice
{
	public:
		//Զ���豸��IP��ַ
		CString csLoginIpAddr;
		//Զ���豸��½�Ķ˿ں�
		int     iLoginPort;
		//��½���û���
		CString csLoginUserName;
		//��½����
		CString csLoginPassWord;
		//��½�ɹ���ľ��
		LONG    hLogin;
		//�����豸ʧ�ܷ��صĴ�����Ϣ
		CString csError;
		//�豸��ͨ������
		int     iChannelNumTotal;
		//�豸����汾����
		CString csDeviceSoftWareDate;
		//�豸�ͺ�
		CString csDeviceTypeName;
		//�豸���к�
		CString csSerialNumber;
		//�Ƿ�����NTP������
		bool    enableNtp; 
		//NTP��ַ
		CString csNtpServerAddr;
		//NTP�˿ں�
		int     csNtpServerPort;
		//NTP����ʱ������(��λ����)
		int     csNtpUpdatePeriod;
		//�豸�ĵ�ǰʱ��
		CString csTime;
		//�Ƿ�ʹ��ʱ���ǩ
		bool enableTimeTitle;
		//�Ƿ���ʾ����
		bool enableShowWeek; 
		//ʱ���ǩ��X����
		int  csTimeLeft;
		//ʱ���ǩY����
		int  csTimeTop;
		//�Ƿ�ʹ����ͨ������
		bool enableChannelNameTitle;
		//ͨ������
		CString csChannelName;
		//ͨ������X����
		CString csChannelNameLeft;
		//ͨ������Y����.
		CString csChannelNameTop;
		//�Ƿ�ʹ���Զ��������         
		bool enableCustomNameTitle;
		//�Զ�������
		CString csCustomName;
		//�Զ���X����
		CString csCustomNameLeft;
		//�Զ���Y����
		CString csCustomNameTop;
		//�Զ������з�ʽ
		CString csCustomNameTextAlign;
	public:
		
		/*=======================================�豸ϵͳ���� =====================================*/
		//�豸��½  
		virtual LONG deviceLogin()=0;
		
		//��ȡ��������������Ϣ 
		//iError(int���͵Ĵ������)
		virtual void deviceGetError(int iError) = 0;
	
		//��ȡ�豸��ϵͳ��Ϣ
		virtual bool deviceGetSystemInfo() = 0;
	
		//��ȡNTP��������Ϣ
		virtual bool deviceGetNtpInfo() = 0;
		
		//��ȡ�豸��ǰʱ��
		virtual bool deviceGetTime() = 0;
	
	
		/*==============================================OSD����=============================================*/
	
		//��ȡʱ���ǩ
		//ChannelNumber=Ҫ��ȡ��ͨ���Ŵ󻪴�0��ʼ,������1��ʼ; 
		virtual bool deviceGetTimeTitle(int ChannelNumber) = 0;
	
		//��ȡͨ������ǩ
		virtual bool deviceGetChannelNameTitle(int ChannelNumber) = 0;
	
		//��ȡ�Զ����ǩ��Ϣ
		virtual bool deviceGetCustomNameTitle(int ChannelNumber) = 0;




};




