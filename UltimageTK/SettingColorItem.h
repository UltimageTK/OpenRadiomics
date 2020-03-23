/******************************************************** 
* @file    : SettingColorItem.h
* @brief   : brief
* @details : 
* @author  : qh.zhang@atisz.ac.cn
* @date    : 2019-7-9
*********************************************************/
#pragma once

#include <QWidget>
#include "ui_SettingColorItem.h"

/******************************************************** 
*  @class   :  SettingColorItem
*  @brief   :  һ����ǩ��ɫ��
*  @details : 
*********************************************************/
class SettingColorItem : public QWidget
{
    Q_OBJECT

public:
    SettingColorItem(QWidget *parent = Q_NULLPTR);
    SettingColorItem(QColor color,QString strLabel, QWidget *parent = Q_NULLPTR);
    ~SettingColorItem();

    //��ȡ��ǩ����ɫ
    void GetColorAndLabel(QColor &color, QString &strLabel);
    //��ʾ����
    void SetWarningState(bool bWarning = true);
public slots:
    //ѡ��ĳ����ɫ
    void OnSelectColor();
    //�����µı�ǩ��
    void OnLabelNameChanged(const QString &str);

private:
    Ui::SettingColorItem ui;
    QColor m_curColor;  //��ǰ��ɫ
    QString m_strCurLabel;  //��ǰ��ǩ��
};
