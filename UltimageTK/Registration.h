/******************************************************** 
* @file    : Registration.h
* @brief   : brief
* @details : 
* @author  : qh.zhang@atisz.ac.cn
* @date    : 2019-7-10
*********************************************************/
#pragma once

#include <QDialog>
#include <iostream>
#include <sstream>
#include "ui_Registration.h"


class Registration : public QDialog
{
    Q_OBJECT

public:
    Registration(QWidget *parent = Q_NULLPTR);
    ~Registration();
    //��ʼ��
    bool Init();

    //3d��׼
    bool RegistCalculate3D();
    //2d��׼
    bool RegistCalculate2D();
	//�������
	void changeLanguage();

    //�򿪲���ͼ
    void OnOpenFixedImg();
    //�򿪴���׼ͼ
    void OnOpenMovingImg();
    //�������ͼ��·��
    void OnSetOutputImgPath();
    //��ʼ��׼
    void OnStartCalculate();
    //��ʾ���
    void OnShowResult(bool bSuccess);
signals:
    void sigCalculateFinish(bool bSuccess);
private:
    Ui::Registration ui;
    std::ostringstream m_oss;
    QLabel *m_pLabelLoading = nullptr;
};
