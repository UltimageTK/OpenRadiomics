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
    //初始化
    bool Init();

    //3d配准
    bool RegistCalculate3D();
    //2d配准
    bool RegistCalculate2D();
	//变更语言
	void changeLanguage();

    //打开参照图
    void OnOpenFixedImg();
    //打开待配准图
    void OnOpenMovingImg();
    //设置输出图像路径
    void OnSetOutputImgPath();
    //开始配准
    void OnStartCalculate();
    //显示结果
    void OnShowResult(bool bSuccess);
signals:
    void sigCalculateFinish(bool bSuccess);
private:
    Ui::Registration ui;
    std::ostringstream m_oss;
    QLabel *m_pLabelLoading = nullptr;
};
