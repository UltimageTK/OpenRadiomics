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
*  @function : 构造函数
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
*  @brief    : 更新标签颜色表
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
*  @brief    : 根据标签名获取颜色
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
*  @brief    : 更新当前标签名
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
*  @brief    : 获取当前标签和颜色
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
*  @brief    : 获取当前颜色
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
QColor AllSettings::GetCurColor()
{
    return m_mapColor[m_strCurLabel];
}

