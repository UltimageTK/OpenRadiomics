/******************************************************** 
* @file    : FormatConvert.cpp
* @brief   : brief
* @details : 
* @author  : qh.zhang@atisz.ac.cn
* @date    : 2019-7-10
*********************************************************/
#include <QMessageBox>
#include <QFileDialog>
#include <QIcon>
#include "FormatConvert.h"
#include "GlobalDef.h"
#include "itkGDCMSeriesFileNames.h"
#include "itkGDCMImageIO.h"
#include "itkNiftiImageIO.h"
#include "itkNrrdImageIO.h"

#include "itkGDCMImageIOFactory.h"
#include "itkNiftiImageIOFactory.h"
#include "itkNrrdImageIOFactory.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "ConfigHelper.h"

/******************************************************** 
*  @function : FormatConvert
*  @brief    : 构造函数
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
FormatConvert::FormatConvert(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);

    QIcon icon(":/UltimageTK/Resources/icon.ico");
    this->setWindowIcon(icon);
}

FormatConvert::~FormatConvert()
{
}

/******************************************************** 
*  @function : Init
*  @brief    : 初始化
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
bool FormatConvert::Init()
{
    //初始化的时候注册一些要用到的图像类别工厂，否则后面的读写操作无法成功
    itk::GDCMImageIOFactory::RegisterOneFactory();
    itk::NiftiImageIOFactory::RegisterOneFactory();
    itk::NrrdImageIOFactory::RegisterOneFactory();

    if (!ConnectSignals())
    {
        return false;
    }
    return true;
}

/******************************************************** 
*  @function : ConnectSignals
*  @brief    : 连接信号槽
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
bool FormatConvert::ConnectSignals()
{
    bool bRet = true;
    bRet &= (bool)connect(ui.pBtnSelectSrcPath, &QPushButton::clicked, this, &FormatConvert::SetSrcPath);
    bRet &= (bool)connect(ui.pBtnSetDstPath, &QPushButton::clicked, this, &FormatConvert::SetDstPath);
    bRet &= (bool)connect(ui.pBtnStartConvert, &QPushButton::clicked, this, &FormatConvert::StartConvert);

    return bRet;
}

/******************************************************** 
*  @function : SetConvertType
*  @brief    : 设置转换模式
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void FormatConvert::SetConvertType(CONVERT_TYPE emCurConvertType)
{
    m_emCurConvertType = emCurConvertType;
	AllConfig stAllConfig;
	ConfigHelper::getInstance()->readConfig(stAllConfig);

    switch (m_emCurConvertType)
    {
    case DICOM2NII:
		if (stAllConfig.emLanguage == QLocale::Language::Chinese)
		{
			ui.labelSrcPath->setText(QString::fromLocal8Bit("DICOM文件目录"));
			ui.labelDstPath->setText(QString::fromLocal8Bit("NII目标路径"));
		}
		else if (stAllConfig.emLanguage == QLocale::Language::English)
		{
			ui.labelSrcPath->setText(QString::fromLocal8Bit("DICOM File Dir"));
			ui.labelDstPath->setText(QString::fromLocal8Bit("NII File Path"));
		}
        break;
    case DICOM2NRRD:
		if (stAllConfig.emLanguage == QLocale::Language::Chinese)
		{
			ui.labelSrcPath->setText(QString::fromLocal8Bit("DICOM文件目录"));
			ui.labelDstPath->setText(QString::fromLocal8Bit("NRRD目标路径"));
		}
		else if (stAllConfig.emLanguage == QLocale::Language::English)
		{
			ui.labelSrcPath->setText(QString::fromLocal8Bit("DICOM File Dir"));
			ui.labelDstPath->setText(QString::fromLocal8Bit("NRRD File Path"));
		}
        break;
    case NRRD2NII:
		if (stAllConfig.emLanguage == QLocale::Language::Chinese)
		{
			ui.labelSrcPath->setText(QString::fromLocal8Bit("NRRD文件路径"));
			ui.labelDstPath->setText(QString::fromLocal8Bit("NII目标路径"));
		}
		else if (stAllConfig.emLanguage == QLocale::Language::English)
		{
			ui.labelSrcPath->setText(QString::fromLocal8Bit("NRRD File Path"));
			ui.labelDstPath->setText(QString::fromLocal8Bit("NII File Path"));
		}
        break;
    case NII2NRRD:
		if (stAllConfig.emLanguage == QLocale::Language::Chinese)
		{
			ui.labelSrcPath->setText(QString::fromLocal8Bit("NII文件路径"));
			ui.labelDstPath->setText(QString::fromLocal8Bit("NRRD目标路径"));
		}
		else if (stAllConfig.emLanguage == QLocale::Language::English)
		{
			ui.labelSrcPath->setText(QString::fromLocal8Bit("NII File Path"));
			ui.labelDstPath->setText(QString::fromLocal8Bit("NRRD File Path"));
		}
        break;
    default:
        break;
    }

    ui.lineEditDicomPath->setText("");
    ui.lineEditNiiPath->setText("");
}

/******************************************************** 
*  @function : Dicom2Sth
*  @brief    : Dicom类型文件转换成其他类型
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
bool FormatConvert::Dicom2Sth(QString strDirPath, QString strDstPath)
{
    SeriesReaderType::Pointer itkReader = nullptr;
    itk::GDCMSeriesFileNames::Pointer nameGenerator = nullptr;
    itk::GDCMImageIO::Pointer pItkDicomIO = nullptr;
    if (itkReader == nullptr)
    {
        itkReader = SeriesReaderType::New();
    }

    if (pItkDicomIO == nullptr)
    {
        pItkDicomIO = itk::GDCMImageIO::New();
    }

    // Get the DICOM filenames from the directory
    if (nameGenerator == nullptr)
    {
        nameGenerator = itk::GDCMSeriesFileNames::New();
    }
    nameGenerator->SetDirectory(strDirPath.toLocal8Bit().data());

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

        itkReader->SetFileNames(fileNames);
        itkReader->SetImageIO(pItkDicomIO);
        itkReader->UpdateLargestPossibleRegion();

        try
        {
            itkReader->Update();
        }
        catch (itk::ExceptionObject &ex)
        {
            std::string str(ex.what());
            return false;
        }

        ImageType3D *img = itkReader->GetOutput();

        typedef  itk::ImageFileWriter<ImageType3D> WriterType;
        WriterType::Pointer writer = WriterType::New();
        writer->SetFileName(strDstPath.toLocal8Bit().data());
        writer->SetInput(img);
        try
        {
            writer->Update();
        }
        catch (itk::ExceptionObject &ex)
        {
            std::string str(ex.what());
            return false;
        }
    }
    catch (itk::ExceptionObject &ex)
    {
        std::string str(ex.what());
        return false;
    }
    return true;
    
}

/******************************************************** 
*  @function : Dicom2Nii
*  @brief    : brief
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
bool FormatConvert::Dicom2Nii(QString strSrcPath, QString strDstPath)
{
    return Dicom2Sth(strSrcPath, strDstPath);
}

/******************************************************** 
*  @function : Dicom2Nrrd
*  @brief    : brief
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
bool FormatConvert::Dicom2Nrrd(QString strSrcPath, QString strDstPath)
{
    return Dicom2Sth(strSrcPath, strDstPath);
}

/******************************************************** 
*  @function : Nrrd2Nii
*  @brief    : brief
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
bool FormatConvert::Nrrd2Nii(QString strSrcPath, QString strDstPath)
{
    itk::ImageFileReader< ImageType3D >::Pointer itkReader = nullptr;
    itk::NrrdImageIO::Pointer pItkNrrdIO = nullptr;
    if (pItkNrrdIO == nullptr)
    {
        pItkNrrdIO = itk::NrrdImageIO::New();
    }

    try
    {
        if (itkReader == nullptr)
        {
            itkReader = itk::ImageFileReader<ImageType3D>::New();
        }


        itkReader->SetFileName(strSrcPath.toLocal8Bit().data());
        itkReader->SetImageIO(pItkNrrdIO);
        itkReader->UpdateLargestPossibleRegion();

        try
        {
            itkReader->Update();
        }
        catch (itk::ExceptionObject &ex)
        {
            std::string str(ex.what());
            std::cout << ex;
            return false;
        }

        ImageType3D *img = itkReader->GetOutput();

        typedef  itk::ImageFileWriter<ImageType3D> WriterType;
        WriterType::Pointer writer = WriterType::New();
        writer->SetFileName(strDstPath.toLocal8Bit().data());
        writer->SetInput(img);
        try
        {
            writer->Update();
        }
        catch (itk::ExceptionObject &ex)
        {
            std::string str(ex.what());
            return false;
        }
    }
    catch (itk::ExceptionObject &ex)
    {
        std::string str(ex.what());
        std::cout << ex;
        return false;
    }


    return true;
}

/******************************************************** 
*  @function : Nii2Nrrd
*  @brief    : brief
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
bool FormatConvert::Nii2Nrrd(QString strSrcPath, QString strDstPath)
{
    itk::ImageFileReader< ImageType3D >::Pointer itkReader = nullptr;
    itk::NiftiImageIO::Pointer pItkNiiIO = nullptr;
    if (pItkNiiIO == nullptr)
    {
        pItkNiiIO = itk::NiftiImageIO::New();
    }

    try
    {
        if (itkReader == nullptr)
        {
            itkReader = itk::ImageFileReader<ImageType3D>::New();
        }


        itkReader->SetFileName(strSrcPath.toLocal8Bit().data());
        itkReader->SetImageIO(pItkNiiIO);
        itkReader->UpdateLargestPossibleRegion();

        try
        {
            itkReader->Update();
        }
        catch (itk::ExceptionObject &ex)
        {
            std::string str(ex.what());
            std::cout << ex;
            return false;
        }

        ImageType3D *img = itkReader->GetOutput();

        typedef  itk::ImageFileWriter<ImageType3D> WriterType;
        WriterType::Pointer writer = WriterType::New();
        writer->SetFileName(strDstPath.toLocal8Bit().data());
        writer->SetInput(img);
        try
        {
            writer->Update();
        }
        catch (itk::ExceptionObject &ex)
        {
            std::string str(ex.what());
            return false;
        }
    }
    catch (itk::ExceptionObject &ex)
    {
        std::string str(ex.what());
        std::cout << ex;
        return false;
    }


    return true;
}

/******************************************************** 
*  @function : SetSrcPath
*  @brief    : 设置原始路径
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void FormatConvert::SetSrcPath()
{
    QString strDirName;
    switch (m_emCurConvertType)
    {
    case DICOM2NII:
    case DICOM2NRRD:
        strDirName = QFileDialog::getExistingDirectory(this, QString::fromLocal8Bit("Choose dir"),
            "",
            QFileDialog::ShowDirsOnly
            | QFileDialog::DontResolveSymlinks);
        break;
    case NRRD2NII:
        strDirName = QFileDialog::getOpenFileName(this, tr("Open File"),
            "",
            tr("Images (*.nrrd)"));
        break;
    case NII2NRRD:
        strDirName = QFileDialog::getOpenFileName(this, tr("Open File"),
            "",
            tr("Images (*.nii *nii.gz)"));
        break;
    default:
        break;
    }
    
    ui.lineEditDicomPath->setText(strDirName);
}

/******************************************************** 
*  @function : SetDstPath
*  @brief    : 设置目标路径
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void FormatConvert::SetDstPath()
{
    QString strFileName;
    switch (m_emCurConvertType)
    {
    case DICOM2NII:
    case NRRD2NII:
        strFileName = QFileDialog::getSaveFileName(this, QString::fromLocal8Bit("Save File"),
            "",
            tr("Images (*.nii *.nii.gz)"));
        break;
    case DICOM2NRRD:
    case NII2NRRD:
        strFileName = QFileDialog::getSaveFileName(this, QString::fromLocal8Bit("Save File"),
            "",
            tr("Images (*.nrrd)"));
        break;
    default:
        break;
    }
    
    if (strFileName.isEmpty())
    {
        return;
    }
    ui.lineEditNiiPath->setText(strFileName);
}

/******************************************************** 
*  @function : StartConvert
*  @brief    : 开始转换
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void FormatConvert::StartConvert()
{
    QString strSrcPath = ui.lineEditDicomPath->text();
    QString strDstPath = ui.lineEditNiiPath->text();
    QDir dir(strSrcPath);
    QFileInfo fileInfoSrc(strSrcPath);
    QString strSffSrc = fileInfoSrc.completeSuffix().toLower();
    QFileInfo fileInfoDst(strDstPath);
    QString strSffDst = fileInfoDst.completeSuffix().toLower();
    bool bRet = true; 
    switch (m_emCurConvertType)
    {
    case DICOM2NII:
        if (!dir.exists()
            || (strSffDst != "nii" && strSffDst != "nii.gz")
            )
        {
            QMessageBox::information(this, "Waring", QString::fromLocal8Bit("请确认路径无误！\nPlease check path!"));
            return;
        }
        bRet = Dicom2Nii(strSrcPath, strDstPath);
        break;
    case DICOM2NRRD:
        if (!dir.exists()
            || (strSffDst != "nrrd")
            )
        {
            QMessageBox::information(this, "Waring", QString::fromLocal8Bit("请确认路径无误！\nPlease check path!"));
            return;
        }
        bRet = Dicom2Nrrd(strSrcPath, strDstPath);
        break;
    case NRRD2NII:
        if (strSffSrc!="nrrd"
            || (strSffDst != "nii" && strSffDst != "nii.gz")
            )
        {
            QMessageBox::information(this, "Waring", QString::fromLocal8Bit("请确认路径无误！\nPlease check path!"));
            return;
        }
        bRet = Nrrd2Nii(strSrcPath, strDstPath);
        break;
    case NII2NRRD:
        if ((strSffSrc != "nii" && strSffSrc != "nii.gz")
            || strSffDst != "nrrd"
            )
        {
            QMessageBox::information(this, "Waring", QString::fromLocal8Bit("请确认路径无误！\nPlease check path!"));
            return;
        }
        bRet = Nii2Nrrd(strSrcPath, strDstPath);
        break;
    default:
        break;
    }

    if (bRet)
    {
        QMessageBox::information(this, "Info", QString::fromLocal8Bit("Convert Success！"));
    }
    else
    {
        QMessageBox::information(this, "Info", QString::fromLocal8Bit("Convert Failed！"));
    }

}

/********************************************************
*  @function : changeLanguage
*  @brief    : 变更语言
*  @input    :
*  @output   :
*  @return   :
*********************************************************/
void FormatConvert::changeLanguage()
{
	ui.retranslateUi(this);
}