/******************************************************** 
* @file    : ConfigHelper.h
* @brief   : brief
* @details : 
* @author  : qh.zhang@atisz.ac.cn
* @date    : 2019-8-6
*********************************************************/
#pragma once

#include <QLocale>

/******************************************************** 
*  @struct  :  AllConfig
*  @brief   :  �����ļ��еĸ���
*  @details : 
*********************************************************/
struct AllConfig
{
	QLocale::Language emLanguage;	//�û���
	AllConfig()
	{
		emLanguage = QLocale::Language::Chinese;
	}
};

/******************************************************** 
*  @class   :  ConfigHelper
*  @brief   :  �����ļ���������
*  @details : 
*********************************************************/
class ConfigHelper
{
	ConfigHelper();
	~ConfigHelper();

public:
	static ConfigHelper* getInstance()
	{
		if (nullptr == m_pInstance)
		{
			m_pInstance = new ConfigHelper;
		}
		return m_pInstance;
	}

	static void releaseInstance()
	{
		if (nullptr != m_pInstance)
		{
			delete m_pInstance;
			m_pInstance = nullptr;
		}
	}


public:
	//�������ļ�
	bool readConfig(AllConfig &stAllConfig);
	//д�����ļ�
	bool writeConfig(const AllConfig &stAllConfig);

private:
	static ConfigHelper* m_pInstance;	//�������
	AllConfig	m_stAllConfig;	//����������Ϣ
};

