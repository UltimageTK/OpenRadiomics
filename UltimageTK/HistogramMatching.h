/******************************************************** 
* @file    : HistogramMatching.h
* @brief   : brief
* @details : 
* @author  : qh.zhang@atisz.ac.cn
* @date    : 2019-7-10
*********************************************************/
#pragma once

#include <QDialog>
#include <QMovie>
#include "ui_HistogramMatching.h"
#include "GlobalDef.h"

class HistogramMatching : public QDialog
{
    Q_OBJECT

public:
    HistogramMatching(QWidget *parent = Q_NULLPTR);
    ~HistogramMatching();
    
    //初始化
    bool Init();
    //开始匹配
    bool MatchCalculate();
    
    //打开ref图
    void OnOpenRefImg();
    //打开待匹配图
    void OnOpenMatchImg();
    //设置输出路径
    void OnSetOutputImgPath();
    //开始计算
    void OnStartCalculate();
    //显示结果
    void OnShowResult(bool bSuccess);
	//变更语言
	void changeLanguage();
signals:
    void sigCalculateFinish(bool bSuccess);
private:
    Ui::HistogramMatching ui;
    QLabel *m_pLabelLoading = nullptr;  //  等待的动图

    //ImageType3D::Pointer m_pItkImageRef = nullptr;
    //ImageType3D::Pointer m_pItkImageMatch = nullptr;
    //ImageType3D::Pointer m_pItkImageOutput = nullptr;

};
