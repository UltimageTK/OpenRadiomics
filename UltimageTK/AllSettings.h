/******************************************************** 
* @file    : AllSettings.h
* @brief   : 
* @details : 
* @author  : qh.zhang@atisz.ac.cn
* @date    : 2019-5-20
*********************************************************/
#pragma once
#include <QColor>
#include <QMap>

/******************************************************** 
*  @class   :  AllSettings
*  @brief   :  brief
*  @details : 
*********************************************************/
class AllSettings
{
public:

    static AllSettings* getInstance()
    {
        if (nullptr == m_pInstance)
        {
            m_pInstance = new AllSettings;
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

    //更新颜色表
    void UpdateColorMap(const std::map<std::string, int> mapLabelProperty);
    //根据标签获取颜色
    QColor GetColorByLabel(QString str);
    //设置当前标签名
    void SetCurLabel(QString strLabel);
    //获取当前标签和颜色
    void GetCurLabelAndColor(QString &str, QColor &color);
    //获取当前颜色
    QColor GetCurColor();

private:
    AllSettings();
    ~AllSettings();

private:
    static AllSettings* m_pInstance;

    QMap<QString, QColor> m_mapColor;   //颜色标签表
    QString               m_strCurLabel;    //当前标签名

};
