/******************************************************** 
* @file    : HistogramMatching.cpp
* @brief   : brief
* @details : 
* @author  : qh.zhang@atisz.ac.cn
* @date    : 2019-7-10
*********************************************************/
#include <QMessageBox>
#include <QFileDialog>
#include <thread>
#include "HistogramMatching.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkRescaleIntensityImageFilter.h"

#include "itkImageSeriesReader.h"
#include "itkImageFileWriter.h"
#include "itkGDCMSeriesFileNames.h"
#include "itkGDCMImageIO.h"
#include "itkImageFileReader.h"
#include "itkNiftiImageIO.h"

#include "itkGDCMImageIOFactory.h"
#include "itkNiftiImageIOFactory.h"
#include "itkNRRDImageIOFactory.h"

#include "itkHistogramMatchingImageFilter.h"

/******************************************************** 
*  @function : loadImageSeries
*  @brief    : 加载图片序列
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
bool loadImageSeries(const QString &strDirPath, ImageType3D::Pointer &pImage)
{
    bool bRet = false;

    itk::GDCMImageIO::Pointer pItkDicomIO = itk::GDCMImageIO::New();

    // Get the DICOM filenames from the directory
    itk::GDCMSeriesFileNames::Pointer nameGenerator = itk::GDCMSeriesFileNames::New();
    nameGenerator->SetDirectory(strDirPath.toLocal8Bit().data());

    SeriesReaderType::Pointer itkReader = SeriesReaderType::New();
    try
    {
        typedef std::vector<std::string> seriesIdContainer;
        const seriesIdContainer & seriesUID = nameGenerator->GetSeriesUIDs();

        seriesIdContainer::const_iterator seriesItr = seriesUID.begin();
        seriesIdContainer::const_iterator seriesEnd = seriesUID.end();

        while (seriesItr != seriesEnd)
        {
            seriesItr++;
        }

        typedef std::vector<std::string> fileNamesContainer;
        fileNamesContainer fileNames;

        fileNames = nameGenerator->GetFileNames("");

        try
        {
            itkReader->SetFileNames(fileNames);
            itkReader->SetImageIO(pItkDicomIO);
            itkReader->UpdateLargestPossibleRegion();
            itkReader->Update();
        }
        catch (itk::ExceptionObject &ex)
        {
            std::string str(ex.what());
            std::cout << ex;
            return false;
        }

        ImageType3D::Pointer img = itkReader->GetOutput();
        pImage = img;
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
*  @brief    : 加载nii图
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
bool loadNiiImage(const QString &strDirPath, ImageType3D::Pointer &pImage)
{
    bool bRet = false;

    itk::NiftiImageIO::Pointer  pItkNiiIO = itk::NiftiImageIO::New();

    try
    {

        itk::ImageFileReader<ImageType3D>::Pointer itkNiiReader = itk::ImageFileReader<ImageType3D>::New();

        itkNiiReader->SetFileName(strDirPath.toLocal8Bit().data());
        itkNiiReader->SetImageIO(pItkNiiIO);
        itkNiiReader->UpdateLargestPossibleRegion();

        try
        {
            itkNiiReader->Update();
        }
        catch (itk::ExceptionObject &ex)
        {
            std::string str(ex.what());
            std::cout << ex;
            return false;
        }

        ImageType3D::Pointer img = itkNiiReader->GetOutput();
        pImage = img;
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
*  @function : HistogramMatching
*  @brief    : 构造函数
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
HistogramMatching::HistogramMatching(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);
    QIcon icon(":/UltimageTK/Resources/icon.ico");
    this->setWindowIcon(icon);
}

HistogramMatching::~HistogramMatching()
{
}

/******************************************************** 
*  @function : Init
*  @brief    : 初始化
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
bool HistogramMatching::Init()
{
    //初始化的时候注册一些要用到的图像类别工厂，否则后面的读写操作无法成功
    itk::GDCMImageIOFactory::RegisterOneFactory();
    itk::NiftiImageIOFactory::RegisterOneFactory();
    itk::NrrdImageIOFactory::RegisterOneFactory();

    connect(ui.pushButtonOpenRefImg, &QPushButton::clicked, this, &HistogramMatching::OnOpenRefImg);
    connect(ui.pushButtonOpenMatchImg, &QPushButton::clicked, this, &HistogramMatching::OnOpenMatchImg);
    connect(ui.pushButtonSetOutputPath, &QPushButton::clicked, this, &HistogramMatching::OnSetOutputImgPath);
    connect(ui.pushButtonStartMatching, &QPushButton::clicked, this, &HistogramMatching::OnStartCalculate);
    connect(this, &HistogramMatching::sigCalculateFinish, this, &HistogramMatching::OnShowResult);
    
    if (m_pLabelLoading == nullptr)
    {
        m_pLabelLoading = new QLabel(this);
    }
    QMovie *pMovie = new QMovie(":/UltimageTK/Resources/loading.gif");
    m_pLabelLoading->setMovie(pMovie);
    pMovie->start();
    m_pLabelLoading->hide();
    return true;
}

/******************************************************** 
*  @function : OnOpenRefImg
*  @brief    : 打开ref图
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void HistogramMatching::OnOpenRefImg()
{
    QString strFileName = QFileDialog::getOpenFileName(
        this,
        QString::fromLocal8Bit("Choose File"),
        "",
        "Images (*.dcm *.nii *.nii.gz);;AllFiles(*.*)");
    if (strFileName.isEmpty())
    {
        return;
    }
    
    ui.lineEditRefImg->setText(strFileName);
}

/******************************************************** 
*  @function : OnOpenMatchImg
*  @brief    : 打开待匹配图
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void HistogramMatching::OnOpenMatchImg()
{
    QString strFileName = QFileDialog::getOpenFileName(
        this,
        QString::fromLocal8Bit("Choose File"),
        "",
        "Images (*.dcm *.nii *.nii.gz);;AllFiles(*.*)");

    if (strFileName.isEmpty())
    {
        return;
    }

    
    ui.lineEditMatchImg->setText(strFileName);
}

/******************************************************** 
*  @function : OnSetOutputImgPath
*  @brief    : 设置输出路径
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void HistogramMatching::OnSetOutputImgPath()
{
    QString strFileName = QFileDialog::getSaveFileName(this, QString::fromLocal8Bit("Save File"),
        "",
        tr("Nifti Image (*.nii *.nii.gz)"));
    if (strFileName.isEmpty())
    {
        return;
    }
    ui.lineEditOutputPath->setText(strFileName);

    return ;
}


/******************************************************** 
*  @function : MatchCalculate
*  @brief    : 开始匹配
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
bool HistogramMatching::MatchCalculate()
{
    ImageType3D::Pointer pItkImageRef = ImageType3D::New();
    ImageType3D::Pointer pItkImageMatch = ImageType3D::New();
    

    QString strRefFileName = ui.lineEditRefImg->text();
    QFileInfo fileInfoRef(strRefFileName);
    if (fileInfoRef.completeSuffix() == "dcm"
        || fileInfoRef.completeSuffix() == "")
    {
        QString strDirPath = fileInfoRef.absolutePath();
        loadImageSeries(strDirPath, pItkImageRef);
    }
    else if (fileInfoRef.completeSuffix() == "nii"
        || fileInfoRef.completeSuffix() == "nii.gz")
    {
        loadNiiImage(strRefFileName, pItkImageRef);
    }

    QString strMatchFileName = ui.lineEditMatchImg->text();
    QFileInfo fileInfoMatch(strMatchFileName);
    if (fileInfoMatch.completeSuffix() == "dcm"
        || fileInfoMatch.completeSuffix() == "")
    {
        QString strDirPath = fileInfoMatch.absolutePath();
        loadImageSeries(strDirPath, pItkImageMatch);
    }
    else if (fileInfoMatch.completeSuffix() == "nii"
        || fileInfoMatch.completeSuffix() == "nii.gz")
    {
        loadNiiImage(strMatchFileName, pItkImageMatch);
    }
    

    try
    {
        itk::HistogramMatchingImageFilter<ImageType3D, ImageType3D>::Pointer pHistogramMatchingImageFilter = itk::HistogramMatchingImageFilter<ImageType3D, ImageType3D>::New();
        pHistogramMatchingImageFilter->SetReferenceImage(pItkImageRef);
        pHistogramMatchingImageFilter->SetSourceImage(pItkImageMatch);
        pHistogramMatchingImageFilter->Update();

        typedef  itk::ImageFileWriter<ImageType3D> WriterType;
        WriterType::Pointer writer = WriterType::New();
        writer->SetFileName(ui.lineEditOutputPath->text().toLocal8Bit().data());
        writer->SetInput(pHistogramMatchingImageFilter->GetOutput());

        writer->Update();

    }
    catch (itk::ExceptionObject &ex)
    {
        std::string str(ex.what());
        emit sigCalculateFinish(false);
        return false;
    }
    emit sigCalculateFinish(true);
    return true;
}

/******************************************************** 
*  @function : OnStartCalculate
*  @brief    : 开始计算
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void HistogramMatching::OnStartCalculate()
{
    ui.labelStatus->setText(QString::fromLocal8Bit("<font style = 'color:red;'> Calculating... < / font>"));
    //ui.labelStatus->setText(QString::fromLocal8Bit("计算中,请勿关闭!"));
    ui.labelStatus->repaint();
    m_pLabelLoading->setGeometry(this->width() / 2 - 25, this->height() / 2 - 25, 50, 50);
    m_pLabelLoading->show();
    m_pLabelLoading->repaint();
    std::thread t(&HistogramMatching::MatchCalculate, this);
    t.detach();

}

/******************************************************** 
*  @function : OnShowResult
*  @brief    : 显示结果
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void HistogramMatching::OnShowResult(bool bSuccess)
{
    m_pLabelLoading->hide();
    if (bSuccess)
    {
        QMessageBox::information(this, "Tip", QString::fromLocal8Bit("Save success！"));
    }
    else
    {
        QMessageBox::information(this, "Tip", QString::fromLocal8Bit("Calculate Failed, check params！"));
    }
    ui.labelStatus->setText(QString::fromLocal8Bit("Status."));
}

/********************************************************
*  @function : changeLanguage
*  @brief    : 变更语言
*  @input    :
*  @output   :
*  @return   :
*********************************************************/
void HistogramMatching::changeLanguage()
{
	ui.retranslateUi(this);
}