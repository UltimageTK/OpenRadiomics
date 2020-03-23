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
*  @brief   :  一个标签颜色项
*  @details : 
*********************************************************/
class SettingColorItem : public QWidget
{
    Q_OBJECT

public:
    SettingColorItem(QWidget *parent = Q_NULLPTR);
    SettingColorItem(QColor color,QString strLabel, QWidget *parent = Q_NULLPTR);
    ~SettingColorItem();

    //获取标签和颜色
    void GetColorAndLabel(QColor &color, QString &strLabel);
    //显示警告
    void SetWarningState(bool bWarning = true);
public slots:
    //选中某个颜色
    void OnSelectColor();
    //设置新的标签名
    void OnLabelNameChanged(const QString &str);

private:
    Ui::SettingColorItem ui;
    QColor m_curColor;  //当前颜色
    QString m_strCurLabel;  //当前标签名
};
