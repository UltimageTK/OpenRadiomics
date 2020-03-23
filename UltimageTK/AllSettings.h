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

    //������ɫ��
    void UpdateColorMap(const std::map<std::string, int> mapLabelProperty);
    //���ݱ�ǩ��ȡ��ɫ
    QColor GetColorByLabel(QString str);
    //���õ�ǰ��ǩ��
    void SetCurLabel(QString strLabel);
    //��ȡ��ǰ��ǩ����ɫ
    void GetCurLabelAndColor(QString &str, QColor &color);
    //��ȡ��ǰ��ɫ
    QColor GetCurColor();

private:
    AllSettings();
    ~AllSettings();

private:
    static AllSettings* m_pInstance;

    QMap<QString, QColor> m_mapColor;   //��ɫ��ǩ��
    QString               m_strCurLabel;    //��ǰ��ǩ��

};
