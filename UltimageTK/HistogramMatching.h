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
    
    //��ʼ��
    bool Init();
    //��ʼƥ��
    bool MatchCalculate();
    
    //��refͼ
    void OnOpenRefImg();
    //�򿪴�ƥ��ͼ
    void OnOpenMatchImg();
    //�������·��
    void OnSetOutputImgPath();
    //��ʼ����
    void OnStartCalculate();
    //��ʾ���
    void OnShowResult(bool bSuccess);
	//�������
	void changeLanguage();
signals:
    void sigCalculateFinish(bool bSuccess);
private:
    Ui::HistogramMatching ui;
    QLabel *m_pLabelLoading = nullptr;  //  �ȴ��Ķ�ͼ

    //ImageType3D::Pointer m_pItkImageRef = nullptr;
    //ImageType3D::Pointer m_pItkImageMatch = nullptr;
    //ImageType3D::Pointer m_pItkImageOutput = nullptr;

};
