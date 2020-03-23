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
*  @brief    : ������
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
bool ConfigHelper::readConfig(AllConfig &stAllConfig)
{
	//���úø�ʽ�ͱ���
	QSettings stSetting(CONFIG_FILE, QSettings::IniFormat);
	stSetting.setIniCodec("GBK");

	//��AllConfig���ж�ȡ������
	stSetting.beginGroup("AllConfig");
	m_stAllConfig.emLanguage = (QLocale::Language)stSetting.value("Language").toInt();
	stSetting.endGroup();

	//��У��һ�£�����û����ǿյģ����ش���
	if (m_stAllConfig.emLanguage<=0)
	{
		return false;
	}
	stAllConfig = m_stAllConfig;
	return true;
}

/******************************************************** 
*  @function : writeConfig
*  @brief    : д����
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
bool ConfigHelper::writeConfig(const AllConfig &stAllConfig)
{
	//���úø�ʽ�ͱ���
	QSettings stSetting(CONFIG_FILE, QSettings::IniFormat);
	stSetting.setIniCodec("GBK");

	m_stAllConfig = stAllConfig;
	//дAllConfig���и�����
	stSetting.beginGroup("AllConfig");
	stSetting.setValue("Language", m_stAllConfig.emLanguage);
	stSetting.endGroup();

	return true;
}
