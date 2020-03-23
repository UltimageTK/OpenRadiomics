/******************************************************** 
* @file    : FormatConvert.h
* @brief   : brief
* @details : 
* @author  : qh.zhang@atisz.ac.cn
* @date    : 2019-7-10
*********************************************************/
#pragma once

#include <QDialog>
#include "ui_FormatConvert.h"

//ת����ʽ
enum CONVERT_TYPE
{
    DICOM2NII = 0,
    DICOM2NRRD = 1,
    NRRD2NII = 2,
    NII2NRRD = 3,
};

/******************************************************** 
*  @class   :  FormatConvert
*  @brief   :  ��ʽת������
*  @details : 
*********************************************************/
class FormatConvert : public QDialog
{
    Q_OBJECT

public:
    FormatConvert(QWidget *parent = Q_NULLPTR);
    ~FormatConvert();

    //��ʼ��
    bool Init();
    bool ConnectSignals();

    //����ת������
    void SetConvertType(CONVERT_TYPE emCurConvertType);

    //Dicom�����ļ�ת������������
    bool Dicom2Sth(QString strSrcPath, QString strDstPath);

    //����ת��ʼ��
    bool Dicom2Nii(QString strSrcPath, QString strDstPath);
    bool Dicom2Nrrd(QString strSrcPath, QString strDstPath);
    bool Nrrd2Nii(QString strSrcPath, QString strDstPath);
    bool Nii2Nrrd(QString strSrcPath, QString strDstPath);

    //����ԭʼ��Ŀ��·��
    void SetSrcPath();
    void SetDstPath();
    //��ʼת��
    void StartConvert();

	//�������
	void changeLanguage();
private:
    Ui::FormatConvert ui;

    CONVERT_TYPE m_emCurConvertType = DICOM2NII;//ת����ʽ
};
