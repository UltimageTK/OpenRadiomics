/******************************************************** 
* @file    : ConfigHelper.cpp
* @brief   : brief
* @details : 
* @author  : qh.zhang@atisz.ac.cn
* @date    : 2019-8-6
*********************************************************/
#include <QSettings>
#include "ConfigHelper.h"

#define CONFIG_FILE "./config.ini"

ConfigHelper * ConfigHelper::m_pInstance = nullptr;
ConfigHelper::ConfigHelper()
{
}


ConfigHelper::~ConfigHelper()
{
}

/******************************************************** 
*  @function : readConfig
*  @brief    : 读配置
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
bool ConfigHelper::readConfig(AllConfig &stAllConfig)
{
	//设置好格式和编码
	QSettings stSetting(CONFIG_FILE, QSettings::IniFormat);
	stSetting.setIniCodec("GBK");

	//在AllConfig组中读取各个项
	stSetting.beginGroup("AllConfig");
	m_stAllConfig.emLanguage = (QLocale::Language)stSetting.value("Language").toInt();
	stSetting.endGroup();

	//简单校验一下，如果用户名是空的，返回错误
	if (m_stAllConfig.emLanguage<=0)
	{
		return false;
	}
	stAllConfig = m_stAllConfig;
	return true;
}

/******************************************************** 
*  @function : writeConfig
*  @brief    : 写配置
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
bool ConfigHelper::writeConfig(const AllConfig &stAllConfig)
{
	//设置好格式和编码
	QSettings stSetting(CONFIG_FILE, QSettings::IniFormat);
	stSetting.setIniCodec("GBK");

	m_stAllConfig = stAllConfig;
	//写AllConfig组中各个项
	stSetting.beginGroup("AllConfig");
	stSetting.setValue("Language", m_stAllConfig.emLanguage);
	stSetting.endGroup();

	return true;
}
