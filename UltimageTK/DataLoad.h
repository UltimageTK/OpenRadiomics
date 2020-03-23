/******************************************************** 
* @file    : DataLoad.h
* @brief   : brief
* @details : 
* @author  : qh.zhang@atisz.ac.cn
* @date    : 2019-7-9
*********************************************************/
#pragma once

#include <QObject>
#include <thread>
#include <QImage>

#include "GlobalDef.h"
#include "ImageDataManager.h"
#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dctk.h"
#include "itkGDCMSeriesFileNames.h"
#include "itkGDCMImageIO.h"
#include "itkImageFileReader.h"
#include "itkNiftiImageIO.h"
class DataLoad : public QObject
{
    Q_OBJECT

public:
    DataLoad();
    ~DataLoad();
    
    bool Init();

protected:
    //bool loadDcmImage(const QString &strImgPath, QImage &img, ImageHeaderInfo *pImageInfoStru = nullptr);
    //bool loadNoSuffixImage(const QString &strImgPath, QImage &img, ImageHeaderInfo *pImageInfoStru = nullptr);
    //bool loadImage(const QString &strImgPath, QImage &img, ImageHeaderInfo *pImageInfoStru = nullptr);
    //bool loadSingleImage(const QString &strFilePath, ImageInfoStru &stImageInfoStru);
    //bool loadImageFromDir(const QString &strDirPath, ImageInfoStru &stImageInfoStru);
    //加载图片序列
    bool loadImageSeries(const QString &strDirPath, ImageHeaderInfo &stImageHeaderInfo);
    //加载Nii文件
    bool loadNiiImage(const QString &strDirPath, ImageHeaderInfo &stImageHeaderInfo);
    //根据tag获取值
    void getValueFromTag(DcmTagKey keyTag, std::string& strValue);
public:
    //开始加载数据
    void startLoadData(const QString &strPath);
    //线程处理函数
    void run();

signals:
    void sigDataLoadFinish(bool bSuccess = true);

private:
    QString m_strCurFileOrDirPath;  //当前加载的文件或文件夹路径
    SeriesReaderType::Pointer m_itkReader = nullptr;    //  读文件句柄
    itk::GDCMSeriesFileNames::Pointer m_nameGenerator =nullptr; //文件生成器
    itk::GDCMImageIO::Pointer m_pItkDicomIO = nullptr;  //文件io句柄

    itk::ImageFileReader<ImageType3D>::Pointer m_itkNiiReader = nullptr;    //nii文件读取句柄
    itk::NiftiImageIO::Pointer m_pItkNiiIO = nullptr;   //nii文件io句柄
};
