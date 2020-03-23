/******************************************************** 
* @file    : AllSettings.cpp
* @brief   : 
* @details : 
* @author  : qh.zhang@atisz.ac.cn
* @date    : 2019-5-20
*********************************************************/
#include "AllSettings.h"

AllSettings* AllSettings::m_pInstance = nullptr;

/******************************************************** 
*  @function : ���캯��
*  @brief    : brief
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
AllSettings::AllSettings()
{
   
}

AllSettings::~AllSettings()
{
}

/******************************************************** 
*  @function : UpdateColorMap
*  @brief    : ���±�ǩ��ɫ��
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void AllSettings::UpdateColorMap(const std::map<std::string, int> mapLabelProperty)
{
    if (mapLabelProperty.empty())
    {
        return;
    }
    m_mapColor.clear();
    for each (auto var in mapLabelProperty)
    {
        m_mapColor[QString::fromLocal8Bit(var.first.c_str())] = var.second;
    }
    m_strCurLabel = m_mapColor.firstKey();
}


/******************************************************** 
*  @function : GetColorByLabel
*  @brief    : ���ݱ�ǩ����ȡ��ɫ
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
QColor AllSettings::GetColorByLabel(QString str)
{
    if (m_mapColor.find(str) != m_mapColor.end())
    {
        return m_mapColor[str];
    }
    else
    {
        return QColor(255, 255, 255);
    }
}

/******************************************************** 
*  @function : SetCurLabel
*  @brief    : ���µ�ǰ��ǩ��
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void AllSettings::SetCurLabel(QString strLabel)
{
    m_strCurLabel = strLabel;
}

/******************************************************** 
*  @function : GetCurLabelAndColor
*  @brief    : ��ȡ��ǰ��ǩ����ɫ
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void AllSettings::GetCurLabelAndColor(QString &str, QColor &color)
{
    str = m_strCurLabel;
    color = m_mapColor[m_strCurLabel];
}

/******************************************************** 
*  @function : GetCurColor
*  @brief    : ��ȡ��ǰ��ɫ
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
QColor AllSettings::GetCurColor()
{
    return m_mapColor[m_strCurLabel];
}

