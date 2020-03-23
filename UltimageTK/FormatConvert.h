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

//转换方式
enum CONVERT_TYPE
{
    DICOM2NII = 0,
    DICOM2NRRD = 1,
    NRRD2NII = 2,
    NII2NRRD = 3,
};

/******************************************************** 
*  @class   :  FormatConvert
*  @brief   :  格式转换界面
*  @details : 
*********************************************************/
class FormatConvert : public QDialog
{
    Q_OBJECT

public:
    FormatConvert(QWidget *parent = Q_NULLPTR);
    ~FormatConvert();

    //初始化
    bool Init();
    bool ConnectSignals();

    //设置转换类型
    void SetConvertType(CONVERT_TYPE emCurConvertType);

    //Dicom类型文件转换成其他类型
    bool Dicom2Sth(QString strSrcPath, QString strDstPath);

    //具体转换始终
    bool Dicom2Nii(QString strSrcPath, QString strDstPath);
    bool Dicom2Nrrd(QString strSrcPath, QString strDstPath);
    bool Nrrd2Nii(QString strSrcPath, QString strDstPath);
    bool Nii2Nrrd(QString strSrcPath, QString strDstPath);

    //设置原始、目标路径
    void SetSrcPath();
    void SetDstPath();
    //开始转换
    void StartConvert();

	//变更语言
	void changeLanguage();
private:
    Ui::FormatConvert ui;

    CONVERT_TYPE m_emCurConvertType = DICOM2NII;//转换方式
};
