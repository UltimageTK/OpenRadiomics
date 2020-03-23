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
*  @brief   :  ��ɫ��ǩ����ҳ��
*  @details : 
*********************************************************/
class SettingUI : public QDialog
{
    Q_OBJECT

public:
    SettingUI(QDialog *parent = Q_NULLPTR);
    ~SettingUI();
    //��ʼ��
    bool Init(std::map<std::string, int> mapLabelProperty);
    //����һ����ǩ
    void AddItem(QColor color,QString strLabel);
    //���������ǩ��ɫ��
    void reload(std::map<std::string, int> mapLabelProperty);
	//�������
	void changeLanguage();

protected:
    void closeEvent(QCloseEvent *event);

public slots:
    //���ӱ�ǩ
    void OnAddColorLabel();
    //ɾ����ǩ
    void OnDelColorLabel();
signals:
    void sigColorMapChanged();
private:
    Ui::SettingUI ui;
};
