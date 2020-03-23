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
*  @brief   :  配置文件中的各项
*  @details : 
*********************************************************/
struct AllConfig
{
	QLocale::Language emLanguage;	//用户名
	AllConfig()
	{
		emLanguage = QLocale::Language::Chinese;
	}
};

/******************************************************** 
*  @class   :  ConfigHelper
*  @brief   :  配置文件帮助单例
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
	//读配置文件
	bool readConfig(AllConfig &stAllConfig);
	//写配置文件
	bool writeConfig(const AllConfig &stAllConfig);

private:
	static ConfigHelper* m_pInstance;	//单例句柄
	AllConfig	m_stAllConfig;	//所有配置信息
};

