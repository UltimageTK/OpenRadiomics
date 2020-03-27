/******************************************************** 
* @file    : DataLoad.cpp
* @brief   : 
* @details : 
* @author  : qh.zhang@atisz.ac.cn
* @date    : 2019-4-25
*********************************************************/
#include <QFileInfo>
#include <QDir>
#include "DataLoad.h"
#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dctk.h"
#include "opencv2/opencv.hpp"
#include "ImageDataManager.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkRescaleIntensityImageFilter.h"

#include "itkImageSeriesReader.h"
#include "itkImageFileWriter.h"

#include "itkGDCMImageIOFactory.h"
#include "itkNiftiImageIOFactory.h"
#include "itkJPEGImageIOFactory.h"
#include "itkPNGImageIOFactory.h"
#include "itkBMPImageIOFactory.h"
#include "itkNRRDImageIOFactory.h"

#include "./BridgeOpenCV/include/itkOpenCVImageBridge.h"
/******************************************************** 
*  @class   :  DataLoad
*  @brief   :  ���캯��
*  @details : 
*********************************************************/
DataLoad::DataLoad()
{
}

DataLoad::~DataLoad()
{
}

/******************************************************** 
*  @function : Init
*  @brief    : ��ʼ��
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
bool DataLoad::Init()
{
    //��ʼ����ʱ��ע��һЩҪ�õ���ͼ����𹤳����������Ķ�д�����޷��ɹ�
    itk::GDCMImageIOFactory::RegisterOneFactory();
    itk::NiftiImageIOFactory::RegisterOneFactory();
    itk::PNGImageIOFactory::RegisterOneFactory();
    itk::JPEGImageIOFactory::RegisterOneFactory();
    itk::BMPImageIOFactory::RegisterOneFactory();
    itk::NrrdImageIOFactory::RegisterOneFactory();

    if (m_itkReader == nullptr)
    {
        m_itkReader = SeriesReaderType::New();
    }

    return true;
}

/******************************************************** 
*  @function : GetHUValueImg
*  @brief    : //��ȡԭʼͼ���CTֵͼ��
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
bool GetHUValueImg(cv::Mat& img, int nIntercept, int nSlope)
{
    if (!img.data)
    {
        return false;
    }

    int rows(img.rows);
    int cols(img.cols);
    unsigned short* data = nullptr;

    /*
    ���ȣ���Ҫ��ȡ����DICOM Tag��Ϣ����0028|1052����rescale intercept�ͣ�0028|1053����rescale slope.
    Ȼ��ͨ����ʽ��
    Hu = pixel * slope(1) + intercept(-1024)
    ����õ�CTֵ��
    */
    for (int i = 0; i < rows; ++i)
    {
        data = img.ptr<unsigned short>(i);
        for (int j = 0; j < cols; ++j)
        {
            int temp = data[j] * nSlope + nIntercept;
            if (temp < 0)
            {
                temp = 0;
            }
            data[j] = temp;
        }
    }

    return true;
}
/*
bool DataLoad::loadDcmImage(const QString &strImgPath, QImage &img, ImageHeaderInfo *pImageInfoStru)
{
    DcmFileFormat fileformat;
    OFCondition oc = fileformat.loadFile(strImgPath.toLocal8Bit().data());	//��ȡDicomͼ��
    if (!oc.good())		//�ж�Dicom�ļ��Ƿ��ȡ�ɹ�
    {
        std::cout << "file Load error" << std::endl;
        return false;
    }
    DcmDataset *dataset = fileformat.getDataset();	//�õ�Dicom�����ݼ������е����ݶ��洢�����ݼ�����
    E_TransferSyntax xfer = dataset->getOriginalXfer();	//�õ������﷨

    unsigned short sImgWidth;		//��ȡͼ��Ĵ����
    unsigned short sImgHeight;
    dataset->findAndGetUint16(DCM_Rows, sImgHeight);
    dataset->findAndGetUint16(DCM_Columns, sImgWidth);

    int nIntercept(-1), nSlope(-1);
    OFString strIntercept, strSlope;
    dataset->findAndGetOFString(DCM_RescaleIntercept, strIntercept);//Intercept
    dataset->findAndGetOFString(DCM_RescaleSlope, strSlope);//Slope
    nIntercept = QString::fromLatin1(strIntercept.c_str()).toInt();
    nSlope = QString::fromLatin1(strSlope.c_str()).toInt();

    if (nullptr != pImageInfoStru)
    {
        OFString stPatientName;
        dataset->findAndGetOFString(DCM_PatientName, stPatientName);	//��ȡ��������

        OFString strPatientAge;
        dataset->findAndGetOFString(DCM_PatientAge, strPatientAge);	//��ȡ��������

        OFString strPatientSex;
        dataset->findAndGetOFString(DCM_PatientSex, strPatientSex);	//��ȡ�����Ա�

        unsigned short bit_count(0);
        dataset->findAndGetUint16(DCM_BitsStored, bit_count);	//��ȡ���ص�λ�� bit

        OFString isRGB;
        dataset->findAndGetOFString(DCM_PhotometricInterpretation, isRGB);//DCMͼƬ��ͼ��ģʽ

        unsigned short img_bits(0);
        dataset->findAndGetUint16(DCM_SamplesPerPixel, img_bits);	//��������ռ�ö���byte

        unsigned short sWinCenter, sWinWidth;  //��ȡԴͼ���еĴ�λ�ʹ���
        dataset->findAndGetUint16(DCM_WindowCenter, sWinCenter);
        dataset->findAndGetUint16(DCM_WindowWidth, sWinWidth);

        float fPixelSpacing;
        OFString strPixelSpacing;
        dataset->findAndGetOFString(DCM_PixelSpacing, strPixelSpacing);//DCMͼƬ��ͼ��ģʽ
        fPixelSpacing = QString::fromLatin1(strPixelSpacing.c_str()).toFloat();

        pImageInfoStru->strPatientName = QString::fromLocal8Bit(stPatientName.c_str());
        pImageInfoStru->strPatientAge = QString::fromLocal8Bit(strPatientAge.c_str());
        pImageInfoStru->strPatientSex = QString::fromLocal8Bit(strPatientSex.c_str());
        pImageInfoStru->nWidth = sImgWidth;
        pImageInfoStru->nHeight = sImgHeight;
        pImageInfoStru->fPixelSpacing = fPixelSpacing;
        pImageInfoStru->nIntercept = nIntercept;
        pImageInfoStru->nSlope = nSlope;

    }

    
    DcmElement* element = NULL;    //��ȡdcm�е�����ֵ
    OFCondition result = dataset->findAndGetElement(DCM_PixelData, element);
    if (result.bad() || element == NULL)
    {
        return false;
    }
    Uint16* pixData16;
    result = element->getUint16Array(pixData16);
    if (result.bad())
    {
        return false;
    }

    cv::Mat matImg(sImgWidth, sImgHeight, CV_16UC1, cv::Scalar::all(0));
    unsigned short* data = nullptr;
    for (int i = 0; i < sImgHeight; i++)
    {
        data = matImg.ptr<unsigned short>(i);   //ȡ��ÿһ�е�ͷָ�� Ҳ��ʹ��dst2.at<unsigned short>(i, j) = ?  
        for (int j = 0; j < sImgWidth; j++)
        {
            unsigned short temp = pixData16[i * 512 + j];
            temp = temp == 63536 ? 0 : temp;
            *data++ = temp;
        }
    }
    GetHUValueImg(matImg, nIntercept, nSlope);
    matImg.convertTo(matImg, CV_8U);
    QImage img2Copy(matImg.data, sImgWidth, sImgHeight, matImg.step, QImage::Format_Grayscale8);
    img = img2Copy.copy();
    if (img.isNull())
    {
        return false;
    }
    return true;
}

bool DataLoad::loadNoSuffixImage(const QString &strImgPath, QImage &img, ImageHeaderInfo *pImageInfoStru)
{
    //�����޺�׺����ͼƬ
    //Ŀǰ����Ϊ��dcm
    return loadDcmImage(strImgPath, img, pImageInfoStru);
}

bool DataLoad::loadImage(const QString &strImgPath, QImage &img, ImageHeaderInfo *pImageInfoStru)
{
    //��ʽ�ж�
    QFileInfo fileInfo(strImgPath);
    if (fileInfo.suffix() == "dcm")
    {
        return loadDcmImage(strImgPath, img, pImageInfoStru);
    }
    else if (fileInfo.suffix() == "")
    {
        return loadNoSuffixImage(strImgPath, img, pImageInfoStru);
    }

    return true;
}

bool DataLoad::loadSingleImage(const QString &strFilePath, ImageInfoStru &stImageInfoStru)
{
    ImageHeaderInfo stImageHeaderInfo;
    QImage imgLoadTemp;
    bool bRet = loadImage(m_strCurFileOrDirPath, imgLoadTemp, &stImageHeaderInfo);
    stImageInfoStru.stImageHeaderInfo = stImageHeaderInfo;
    stImageInfoStru.vecImg.push_back(imgLoadTemp);
    return bRet;
}

bool DataLoad::loadImageFromDir(const QString &strDirPath, ImageInfoStru &stImageInfoStru)
{
    //�ж�·���Ƿ����
    QDir dir(strDirPath);
    if (!dir.exists())
    {
        return false;
    }

    QStringList filters;
    //filters << QString("*.dcm");
    dir.setFilter(QDir::Files | QDir::NoSymLinks); //�������͹�������ֻΪ�ļ���ʽ
    dir.setNameFilters(filters);  //�����ļ����ƹ�������ֻΪfilters��ʽ����׺Ϊ.jpeg��ͼƬ��ʽ��

    //��ȡ�ָ���
    QChar separator = QChar('/');
    if (!strDirPath.contains(separator))
    {
        separator = QChar('\\');
    }
    QChar last_char = strDirPath.at(strDirPath.length() - 1);
    if (last_char == separator)
    {
        separator = QChar();
    }

    QStringList lstFilePath;
    foreach(QFileInfo mfi, dir.entryInfoList())
    {
        if (mfi.isFile() && (mfi.suffix() == "dcm" || mfi.suffix() == ""))
        {
            QString file_path = strDirPath + separator + mfi.fileName();
            lstFilePath.append(file_path);
        }
    }
    qSort(lstFilePath.begin(), lstFilePath.end(), [](const QString &s1, const QString &s2) {
        return s1.size() < s2.size() || (s1.size() == s2.size() && s1 < s2);
    });

    bool bRet = false;
    for (int i = 0 ; i< lstFilePath.size();i++)
    {
        QImage imgLoadTemp;
        if (i == 0)
        {
            ImageHeaderInfo stImageHeaderInfo;
            bRet = loadImage(lstFilePath[i], imgLoadTemp, &stImageHeaderInfo);
            stImageInfoStru.stImageHeaderInfo = stImageHeaderInfo;
        }
        else
        {
            bRet = loadImage(lstFilePath[i], imgLoadTemp);
        }
        if (!bRet)
        {
            return false;
        }
        stImageInfoStru.vecImg.push_back(imgLoadTemp);
    }

    return true;
}
*/

/******************************************************** 
*  @function : loadImageSeries
*  @brief    : ����ͼƬ����
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
bool DataLoad::loadImageSeries(const QString &strDirPath, ImageHeaderInfo &stImageHeaderInfo)
{
    bool bRet = true;
    if (m_pItkDicomIO == nullptr)
    {
        m_pItkDicomIO = itk::GDCMImageIO::New();
    }

    // Get the DICOM filenames from the directory
    if (m_nameGenerator == nullptr)
    {
        m_nameGenerator = itk::GDCMSeriesFileNames::New();
    }
    m_nameGenerator->SetDirectory(strDirPath.toLocal8Bit().data());

    try
    {
        typedef std::vector<std::string> seriesIdContainer;
        const seriesIdContainer & seriesUID = m_nameGenerator->GetSeriesUIDs();

        seriesIdContainer::const_iterator seriesItr = seriesUID.begin();
        seriesIdContainer::const_iterator seriesEnd = seriesUID.end();

        while (seriesItr != seriesEnd)
        {
            seriesItr++;
        }

        typedef std::vector<std::string> fileNamesContainer;
        fileNamesContainer fileNames;

        fileNames = m_nameGenerator->GetFileNames("");

        m_itkReader->SetFileNames(fileNames);
        m_itkReader->SetImageIO(m_pItkDicomIO);
        m_itkReader->UpdateLargestPossibleRegion();

        try
        {
            m_itkReader->Update();
        }
        catch (itk::ExceptionObject &ex)
        {
            std::string str(ex.what());
            std::cout << ex;
            return false;
        }

        char* pParams = new char[256];
        m_pItkDicomIO->GetPatientName(pParams);
        stImageHeaderInfo.strPatientName = pParams;
        memset(pParams, 0, 256);
        m_pItkDicomIO->GetPatientAge(pParams);
        stImageHeaderInfo.strPatientAge = pParams;
        memset(pParams, 0, 256);
        m_pItkDicomIO->GetPatientSex(pParams);
        stImageHeaderInfo.strPatientSex = pParams;
        memset(pParams, 0, 256);
        m_pItkDicomIO->GetModality(pParams);
        stImageHeaderInfo.strFileType = pParams;
        delete[] pParams;
        pParams = nullptr;

        std::string strWindowCenter;
        getValueFromTag(DCM_WindowCenter, strWindowCenter);
        std::string strWindowWidth;
        getValueFromTag(DCM_WindowWidth, strWindowWidth); 
        std::string strSlope;
        getValueFromTag(DCM_RescaleSlope, strSlope);
        std::string strIntercept;
        getValueFromTag(DCM_RescaleIntercept, strIntercept);
        std::string strALinePixelSpacing;
        getValueFromTag(DCM_ALinePixelSpacing, strALinePixelSpacing);

        ImageType3D::PixelType  maxValue, minValue;
        std::string strSmallestImagePixelValue;
        //getValueFromTag(DCM_SmallestImagePixelValue, strSmallestImagePixelValue);
        //minValue = std::atoi(strSmallestImagePixelValue.c_str());
        std::string strLargestImagePixelValue;
        //getValueFromTag(DCM_LargestImagePixelValue, strLargestImagePixelValue);
        //maxValue = std::atoi(strLargestImagePixelValue.c_str());

        double dS1 = m_pItkDicomIO->GetSpacing(SagittalPlane);
        double dS2 = m_pItkDicomIO->GetSpacing(CoronalPlane);
        double dS3 = m_pItkDicomIO->GetSpacing(TransversePlane);
        ImageType3D *img = m_itkReader->GetOutput();
        itk::Size<3> sliceSize = img->GetLargestPossibleRegion().GetSize();

        if (strSmallestImagePixelValue.empty() || strLargestImagePixelValue.empty())
        {
            typedef itk::MinimumMaximumImageCalculator <ImageType3D> IMinimumMaximumImageCalculatorType;
            IMinimumMaximumImageCalculatorType::Pointer imageCalculatorFilter = IMinimumMaximumImageCalculatorType::New();
            imageCalculatorFilter->SetImage(img);
            imageCalculatorFilter->Compute();
            maxValue = imageCalculatorFilter->GetMaximum(); //���ֵ
            minValue = imageCalculatorFilter->GetMinimum(); //��Сֵ
        }
        


        stImageHeaderInfo.nWidth = sliceSize[SagittalPlane];
        stImageHeaderInfo.nHeight = sliceSize[CoronalPlane];
        stImageHeaderInfo.nThickNess = sliceSize[TransversePlane];
        stImageHeaderInfo.strFilePath = strDirPath;
        stImageHeaderInfo.fPixelSpacingW = dS1;
        stImageHeaderInfo.fPixelSpacingH = dS2;
        stImageHeaderInfo.fPixelSpacingT = dS3;
        //stImageHeaderInfo.nWinCenter = std::atoi(strWindowCenter.c_str());
        //stImageHeaderInfo.nWinWidth = std::atoi(strWindowWidth.c_str());
        stImageHeaderInfo.nWinCenter = (maxValue + minValue) / 2;
        stImageHeaderInfo.nWinWidth = maxValue - minValue;
        stImageHeaderInfo.nSlope = std::atoi(strSlope.c_str());
        stImageHeaderInfo.nIntercept = std::atoi(strIntercept.c_str());

        ImageDataManager::getInstance()->setHeaderInfo(stImageHeaderInfo);
        ImageDataManager::getInstance()->setImageSerial(img);
        bRet = ImageDataManager::getInstance()->loadLabelInfo(strDirPath,IMAGE_TYPE::IMG_DCM);
    }
    catch (itk::ExceptionObject &ex)
    {
        std::string str(ex.what());
        std::cout << ex;
        return 0;
    }


    return bRet;
}

/******************************************************** 
*  @function : loadNiiImage
*  @brief    : ����NiiͼƬ
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
bool DataLoad::loadNiiImage(const QString &strDirPath, ImageHeaderInfo &stImageHeaderInfo)
{
    bool bRet = true;
    if (m_pItkNiiIO== nullptr)
    {
        m_pItkNiiIO = itk::NiftiImageIO::New();
    }

    try
    {
        if (m_itkNiiReader == nullptr)
        {
            m_itkNiiReader = itk::ImageFileReader<ImageType3D>::New();
        }
       

        m_itkNiiReader->SetFileName(strDirPath.toLocal8Bit().data());
        m_itkNiiReader->SetImageIO(m_pItkNiiIO);
        m_itkNiiReader->UpdateLargestPossibleRegion();

        try
        {
            m_itkNiiReader->Update();
        }
        catch (itk::ExceptionObject &ex)
        {
            std::string str(ex.what());
            std::cout << ex;
            return false;
        }

        /*char* pParams = new char[256];
        m_pItkNiiIO->GetPatientName(pParams);
        stImageHeaderInfo.strPatientName = pParams;
        memset(pParams, 0, 256);
        m_pItkNiiIO->GetPatientAge(pParams);
        stImageHeaderInfo.strPatientAge = pParams;
        memset(pParams, 0, 256);
        m_pItkNiiIO->GetPatientSex(pParams);
        stImageHeaderInfo.strPatientSex = pParams;
        delete[] pParams;
        pParams = nullptr;

        std::string strWindowCenter;
        getValueFromTag(DCM_WindowCenter, strWindowCenter);
        std::string strWindowWidth;
        getValueFromTag(DCM_WindowWidth, strWindowWidth);
        std::string strSlope;
        getValueFromTag(DCM_RescaleSlope, strSlope);
        std::string strIntercept;
        getValueFromTag(DCM_RescaleIntercept, strIntercept);
        std::string strALinePixelSpacing;
        getValueFromTag(DCM_ALinePixelSpacing, strALinePixelSpacing);*/

        double dS1 = m_pItkNiiIO->GetSpacing(SagittalPlane);
        double dS2 = m_pItkNiiIO->GetSpacing(CoronalPlane);
        double dS3 = m_pItkNiiIO->GetSpacing(TransversePlane);

        ImageType3D *img = m_itkNiiReader->GetOutput();
        itk::Size<3> sliceSize = img->GetLargestPossibleRegion().GetSize();
        ImageType3D::PixelType pixelValue, maxValue, minValue;
        typedef itk::MinimumMaximumImageCalculator <ImageType3D> IMinimumMaximumImageCalculatorType;
        IMinimumMaximumImageCalculatorType::Pointer imageCalculatorFilter = IMinimumMaximumImageCalculatorType::New();
        imageCalculatorFilter->SetImage(img);
        imageCalculatorFilter->Compute();
        maxValue = imageCalculatorFilter->GetMaximum(); //���ֵ
        minValue = imageCalculatorFilter->GetMinimum(); //��Сֵ

        stImageHeaderInfo.nWidth = sliceSize[SagittalPlane];
        stImageHeaderInfo.nHeight = sliceSize[CoronalPlane];
        stImageHeaderInfo.nThickNess = sliceSize[TransversePlane];
        stImageHeaderInfo.strFilePath = strDirPath;
        stImageHeaderInfo.fPixelSpacingW = dS1;
        stImageHeaderInfo.fPixelSpacingH = dS2;
        stImageHeaderInfo.fPixelSpacingT = dS3;
        //stImageHeaderInfo.nWinCenter = std::atoi(strWindowCenter.c_str());
        //stImageHeaderInfo.nWinWidth = std::atoi(strWindowWidth.c_str());
        stImageHeaderInfo.nWinCenter = (maxValue + minValue) / 2;
        stImageHeaderInfo.nWinWidth = maxValue - minValue;
        /*stImageHeaderInfo.nSlope = std::atoi(strSlope.c_str());
        stImageHeaderInfo.nIntercept = std::atoi(strIntercept.c_str());*/

        ImageDataManager::getInstance()->setImageSerial(img);
        ImageDataManager::getInstance()->setHeaderInfo(stImageHeaderInfo);
        bRet = ImageDataManager::getInstance()->loadLabelInfo(strDirPath, IMAGE_TYPE::IMG_NII);
    }
    catch (itk::ExceptionObject &ex)
    {
        std::string str(ex.what());
        std::cout << ex;
        return 0;
    }


    return bRet;
}

/******************************************************** 
*  @function : getValueFromTag
*  @brief    : ����tag��ȡֵ
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void DataLoad::getValueFromTag(DcmTagKey keyTag, std::string& strValue)
{
    QString strTagSpacing(keyTag.toString().c_str());
    strTagSpacing = strTagSpacing.mid(1, strTagSpacing.size() - 2);
    strTagSpacing.replace(',', '|');
    m_pItkDicomIO->GetValueFromTag(strTagSpacing.toLocal8Bit().data(), strValue);
}

/******************************************************** 
*  @function : startLoadData
*  @brief    : �����̼߳�������
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void DataLoad::startLoadData(const QString &strPath)
{
    m_strCurFileOrDirPath = strPath;

    std::thread thrAnalysis(&DataLoad::run, this);
    if (thrAnalysis.joinable())
    {
        thrAnalysis.detach();
    }

}

/******************************************************** 
*  @function : run
*  @brief    : ���������̺߳���
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void DataLoad::run()
{
    QFileInfo fileInfo(m_strCurFileOrDirPath);

    bool bRet = false;
    ImageHeaderInfo stImageHeaderInfo;
    //������ļ�
    if (fileInfo.isFile())
    {
        if (fileInfo.completeSuffix() == "nii"
            ||fileInfo.completeSuffix() == "nii.gz")
        {
            bRet = loadNiiImage(m_strCurFileOrDirPath, stImageHeaderInfo);
        }
        //bRet = loadSingleImage(m_strCurFileOrDirPath, stImageHeaderInfo);
    }
    else if (fileInfo.isDir())
    {
        //bRet = loadImageFromDir(m_strCurFileOrDirPath,stImageInfoStru);
        bRet = loadImageSeries(m_strCurFileOrDirPath, stImageHeaderInfo);
    }

    emit sigDataLoadFinish(bRet);
}
