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
*  @brief   :  构造函数
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
*  @brief    : 初始化
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
bool DataLoad::Init()
{
    //初始化的时候注册一些要用到的图像类别工厂，否则后面的读写操作无法成功
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
*  @brief    : //获取原始图像的CT值图像
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
    首先，需要读取两个DICOM Tag信息，（0028|1052）：rescale intercept和（0028|1053）：rescale slope.
    然后通过公式：
    Hu = pixel * slope(1) + intercept(-1024)
    计算得到CT值。
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
    OFCondition oc = fileformat.loadFile(strImgPath.toLocal8Bit().data());	//读取Dicom图像
    if (!oc.good())		//判断Dicom文件是否读取成功
    {
        std::cout << "file Load error" << std::endl;
        return false;
    }
    DcmDataset *dataset = fileformat.getDataset();	//得到Dicom的数据集，所有的数据都存储在数据集当中
    E_TransferSyntax xfer = dataset->getOriginalXfer();	//得到传输语法

    unsigned short sImgWidth;		//获取图像的窗宽高
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
        dataset->findAndGetOFString(DCM_PatientName, stPatientName);	//获取病人姓名

        OFString strPatientAge;
        dataset->findAndGetOFString(DCM_PatientAge, strPatientAge);	//获取病人年龄

        OFString strPatientSex;
        dataset->findAndGetOFString(DCM_PatientSex, strPatientSex);	//获取病人性别

        unsigned short bit_count(0);
        dataset->findAndGetUint16(DCM_BitsStored, bit_count);	//获取像素的位数 bit

        OFString isRGB;
        dataset->findAndGetOFString(DCM_PhotometricInterpretation, isRGB);//DCM图片的图像模式

        unsigned short img_bits(0);
        dataset->findAndGetUint16(DCM_SamplesPerPixel, img_bits);	//单个像素占用多少byte

        unsigned short sWinCenter, sWinWidth;  //获取源图像中的窗位和窗宽
        dataset->findAndGetUint16(DCM_WindowCenter, sWinCenter);
        dataset->findAndGetUint16(DCM_WindowWidth, sWinWidth);

        float fPixelSpacing;
        OFString strPixelSpacing;
        dataset->findAndGetOFString(DCM_PixelSpacing, strPixelSpacing);//DCM图片的图像模式
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

    
    DcmElement* element = NULL;    //读取dcm中的像素值
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
        data = matImg.ptr<unsigned short>(i);   //取得每一行的头指针 也可使用dst2.at<unsigned short>(i, j) = ?  
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
    //加载无后缀名的图片
    //目前先认为是dcm
    return loadDcmImage(strImgPath, img, pImageInfoStru);
}

bool DataLoad::loadImage(const QString &strImgPath, QImage &img, ImageHeaderInfo *pImageInfoStru)
{
    //格式判断
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
    //判断路径是否存在
    QDir dir(strDirPath);
    if (!dir.exists())
    {
        return false;
    }

    QStringList filters;
    //filters << QString("*.dcm");
    dir.setFilter(QDir::Files | QDir::NoSymLinks); //设置类型过滤器，只为文件格式
    dir.setNameFilters(filters);  //设置文件名称过滤器，只为filters格式（后缀为.jpeg等图片格式）

    //获取分隔符
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
*  @brief    : 加载图片序列
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
            maxValue = imageCalculatorFilter->GetMaximum(); //最大值
            minValue = imageCalculatorFilter->GetMinimum(); //最小值
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
*  @brief    : 加载Nii图片
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
        maxValue = imageCalculatorFilter->GetMaximum(); //最大值
        minValue = imageCalculatorFilter->GetMinimum(); //最小值

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
*  @brief    : 根据tag获取值
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
*  @brief    : 开启线程加载数据
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
*  @brief    : 加载数据线程函数
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void DataLoad::run()
{
    QFileInfo fileInfo(m_strCurFileOrDirPath);

    bool bRet = false;
    ImageHeaderInfo stImageHeaderInfo;
    //如果是文件
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
