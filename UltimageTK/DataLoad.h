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
    //����ͼƬ����
    bool loadImageSeries(const QString &strDirPath, ImageHeaderInfo &stImageHeaderInfo);
    //����Nii�ļ�
    bool loadNiiImage(const QString &strDirPath, ImageHeaderInfo &stImageHeaderInfo);
    //����tag��ȡֵ
    void getValueFromTag(DcmTagKey keyTag, std::string& strValue);
public:
    //��ʼ��������
    void startLoadData(const QString &strPath);
    //�̴߳�����
    void run();

signals:
    void sigDataLoadFinish(bool bSuccess = true);

private:
    QString m_strCurFileOrDirPath;  //��ǰ���ص��ļ����ļ���·��
    SeriesReaderType::Pointer m_itkReader = nullptr;    //  ���ļ����
    itk::GDCMSeriesFileNames::Pointer m_nameGenerator =nullptr; //�ļ�������
    itk::GDCMImageIO::Pointer m_pItkDicomIO = nullptr;  //�ļ�io���

    itk::ImageFileReader<ImageType3D>::Pointer m_itkNiiReader = nullptr;    //nii�ļ���ȡ���
    itk::NiftiImageIO::Pointer m_pItkNiiIO = nullptr;   //nii�ļ�io���
};
