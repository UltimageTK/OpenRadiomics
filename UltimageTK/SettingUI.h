/******************************************************** 
* @file    : SettingUI.h
* @brief   : brief
* @details : 
* @author  : qh.zhang@atisz.ac.cn
* @date    : 2019-7-9
*********************************************************/
#pragma once

#include <QWidget>
#include <QDialog>
#include "ui_SettingUI.h"
#include "LabelAnalysis.h"
/******************************************************** 
*  @class   :  SettingUI
*  @brief   :  颜色标签设置页面
*  @details : 
*********************************************************/
class SettingUI : public QDialog
{
    Q_OBJECT

public:
    SettingUI(QDialog *parent = Q_NULLPTR);
    ~SettingUI();
    //初始化
    bool Init(std::map<std::string, int> mapLabelProperty);
    //增加一个标签
    void AddItem(QColor color,QString strLabel);
    //重新载入标签颜色表
    void reload(std::map<std::string, int> mapLabelProperty);
	//变更语言
	void changeLanguage();

protected:
    void closeEvent(QCloseEvent *event);

public slots:
    //增加标签
    void OnAddColorLabel();
    //删除标签
    void OnDelColorLabel();
signals:
    void sigColorMapChanged();
private:
    Ui::SettingUI ui;
};
