/********************************************************
* @file    : Registration.cpp
* @brief   :
* @details :
* @author  : qh.zhang@atisz.ac.cn
* @date    : 2019-7-1
*********************************************************/
#include <QMessageBox>
#include <QFileDialog>
#include <QMovie>
#include <thread>
#include "Registration.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkRescaleIntensityImageFilter.h"

#include "itkImageSeriesReader.h"
#include "itkImageFileWriter.h"
#include "itkGDCMSeriesFileNames.h"
#include "itkGDCMImageIO.h"
#include "itkImageFileReader.h"
#include "itkNiftiImageIO.h"
#include "itkPNGImageIO.h"

#include "itkGDCMImageIOFactory.h"
#include "itkNiftiImageIOFactory.h"
#include "itkNRRDImageIOFactory.h"
#include "itkPNGImageIOFactory.h"

#include "itkImageRegistrationMethod.h"
#include "itkTranslationTransform.h"
#include "itkMeanSquaresImageToImageMetric.h"
#include "itkRegularStepGradientDescentOptimizer.h"

#include "itkResampleImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkSubtractImageFilter.h"
#include "itkVersorRigid3DTransform.h"

#include "itkImageRegistrationMethodv4.h"
#include "itkMeanSquaresImageToImageMetricv4.h"

#include "itkVersorRigid3DTransform.h"
#include "itkCenteredTransformInitializer.h"
#include "itkRegularStepGradientDescentOptimizerv4.h"

Registration::Registration(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);
    QIcon icon(":/UltimageTK/Resources/icon.ico");
    this->setWindowIcon(icon);
}

Registration::~Registration()
{
}

/******************************************************** 
*  @function : Init
*  @brief    : 初始化
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
bool Registration::Init()
{
    //初始化的时候注册一些要用到的图像类别工厂，否则后面的读写操作无法成功
    itk::GDCMImageIOFactory::RegisterOneFactory();
    itk::NiftiImageIOFactory::RegisterOneFactory();
	itk::NrrdImageIOFactory::RegisterOneFactory();
	itk::PNGImageIOFactory::RegisterOneFactory();

    connect(ui.pushButtonOpenFixedImg, &QPushButton::clicked, this, &Registration::OnOpenFixedImg);
    connect(ui.pushButtonOpenMovingImg, &QPushButton::clicked, this, &Registration::OnOpenMovingImg);
    connect(ui.pushButtonSetOutputPath, &QPushButton::clicked, this, &Registration::OnSetOutputImgPath);
    connect(ui.pushButtonStartRegisting, &QPushButton::clicked, this, &Registration::OnStartCalculate);
    bool b = connect(this, &Registration::sigCalculateFinish, this, &Registration::OnShowResult);

    QIntValidator* aIntValidator = new QIntValidator;
    aIntValidator->setRange(1, 10000);
    ui.lineEditIterNum->setValidator(aIntValidator);

    QIntValidator* aPixelIntValidator = new QIntValidator;
    aPixelIntValidator->setRange(-10000, 10000);
    ui.lineEditDefaultPixel->setValidator(aPixelIntValidator);

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
*  @function : OnOpenFixedImg
*  @brief    : 打开参照图
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void Registration::OnOpenFixedImg()
{
    QString strFileName = QFileDialog::getOpenFileName(
        this,
        QString::fromLocal8Bit("Choose File"),
        "",
		//"Images (*.dcm *.nii *.nii.gz);;AllFiles(*.*)");
		"Images (*.nii *.nii.gz);;AllFiles(*.*)");
    if (strFileName.isEmpty())
    {
        return;
    }
   
    ui.lineEditFixedImg->setText(strFileName);
}

/******************************************************** 
*  @function : OnOpenMovingImg
*  @brief    : 打开待配准图
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void Registration::OnOpenMovingImg()
{
    QString strFileName = QFileDialog::getOpenFileName(
        this,
        QString::fromLocal8Bit("Choose File"),
        "",
        //"Images (*.dcm *.nii *.nii.gz);;AllFiles(*.*)");
		"Images (*.nii *.nii.gz);;AllFiles(*.*)");

    if (strFileName.isEmpty())
    {
        return;
    }

    ui.lineEditRegImg->setText(strFileName);
    
}

/******************************************************** 
*  @function : OnSetOutputImgPath
*  @brief    : 设置输出图像路径
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void Registration::OnSetOutputImgPath()
{
    QString strFileName = QFileDialog::getSaveFileName(this, QString::fromLocal8Bit("Save File"),
        "",
        tr("Nifti Image (*.nii *.nii.gz)"));
    if (strFileName.isEmpty())
    {
        return;
    }
    ui.lineEditOutputPath->setText(strFileName);

    return;
}

/******************************************************** 
*  @function : OnStartCalculate
*  @brief    : 开始配准
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void Registration::OnStartCalculate()
{
    ui.labelStatus->setText(QString::fromLocal8Bit("<font style = 'color:red;'> Calculating... < / font>"));
    ui.labelStatus->repaint();
    ui.textEdit->clear();
    m_oss.clear();
    m_pLabelLoading->setGeometry(this->width() / 2 - 25, this->height() / 2 - 25, 50, 50);
    m_pLabelLoading->show();
    m_pLabelLoading->repaint();

    ui.textEdit->setText(QString::fromLocal8Bit("Start Calculate..."));
    std::thread t(&Registration::RegistCalculate3D,this);
    t.detach();


}

/******************************************************** 
*  @function : 
*  @brief    : 显示结果
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void Registration::OnShowResult(bool bSuccess)
{
    m_pLabelLoading->hide();
    if (bSuccess)
    {
        m_oss << std::endl << "Regist Success" << std::endl;
        //QMessageBox::information(this, "Tip", QString::fromLocal8Bit("保存成功！"));
    }
    else
    {
        m_oss << std::endl << "Regist Failed." << std::endl;
        //QMessageBox::information(this, "Tip", QString::fromLocal8Bit("计算失败，请检查参数！"));
    }
    ui.textEdit->append(m_oss.str().c_str());
    ui.labelStatus->setText(QString::fromLocal8Bit("Status."));
}


/******************************************************** 
*  @function : RegistCalculate3D
*  @brief    : 3d配准
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
bool Registration::RegistCalculate3D()
{
    const unsigned int                          Dimension = 3;
    typedef  float                              PixelType;
    typedef itk::Image< PixelType, Dimension >  FixedImageType;
    typedef itk::Image< PixelType, Dimension >  MovingImageType;
    typedef itk::VersorRigid3DTransform< double > TransformType;

    typedef itk::RegularStepGradientDescentOptimizerv4<double>    OptimizerType;
    typedef itk::MeanSquaresImageToImageMetricv4<
        FixedImageType,
        MovingImageType >   MetricType;
    typedef itk::ImageRegistrationMethodv4<
        FixedImageType,
        MovingImageType,
        TransformType >           RegistrationType;

    MetricType::Pointer         metric = MetricType::New();
    OptimizerType::Pointer      optimizer = OptimizerType::New();
    RegistrationType::Pointer   registration = RegistrationType::New();

    registration->SetMetric(metric);
    registration->SetOptimizer(optimizer);

    TransformType::Pointer  initialTransform = TransformType::New();
    // Software Guide : EndCodeSnippet

    typedef itk::ImageFileReader< FixedImageType  > FixedImageReaderType;
    typedef itk::ImageFileReader< MovingImageType > MovingImageReaderType;
    FixedImageReaderType::Pointer  fixedImageReader = FixedImageReaderType::New();
    MovingImageReaderType::Pointer movingImageReader = MovingImageReaderType::New();

    fixedImageReader->SetFileName(ui.lineEditFixedImg->text().toLocal8Bit().data());
    movingImageReader->SetFileName(ui.lineEditRegImg->text().toLocal8Bit().data());

    registration->SetFixedImage(fixedImageReader->GetOutput());
    registration->SetMovingImage(movingImageReader->GetOutput());

    typedef itk::CenteredTransformInitializer<
        TransformType,
        FixedImageType,
        MovingImageType >  TransformInitializerType;
    TransformInitializerType::Pointer initializer =
        TransformInitializerType::New();
    initializer->SetTransform(initialTransform);
    initializer->SetFixedImage(fixedImageReader->GetOutput());
    initializer->SetMovingImage(movingImageReader->GetOutput());
    initializer->MomentsOn();
    initializer->InitializeTransform();
 
    typedef TransformType::VersorType  VersorType;
    typedef VersorType::VectorType     VectorType;
    VersorType     rotation;
    VectorType     axis;
    axis[0] = 0.0;
    axis[1] = 0.0;
    axis[2] = 1.0;
    const double angle = 0;
    rotation.Set(axis, angle);
    initialTransform->SetRotation(rotation);

    registration->SetInitialTransform(initialTransform);

    typedef OptimizerType::ScalesType       OptimizerScalesType;
    OptimizerScalesType optimizerScales(initialTransform->GetNumberOfParameters());
    const double translationScale = 1.0 / 1000.0;
    optimizerScales[0] = 1.0;
    optimizerScales[1] = 1.0;
    optimizerScales[2] = 1.0;
    optimizerScales[3] = translationScale;
    optimizerScales[4] = translationScale;
    optimizerScales[5] = translationScale;
    optimizer->SetScales(optimizerScales);
    optimizer->SetNumberOfIterations(ui.lineEditIterNum->text().toInt());
    optimizer->SetLearningRate(ui.doubleSpinBoxLearningRate->text().toFloat());
    optimizer->SetMinimumStepLength(ui.doubleSpinBoxMinStep->text().toFloat());
    optimizer->SetReturnBestParametersAndValue(true);

    // One level registration process without shrinking and smoothing.
    //
    const unsigned int numberOfLevels = 1;

    RegistrationType::ShrinkFactorsArrayType shrinkFactorsPerLevel;
    shrinkFactorsPerLevel.SetSize(1);
    shrinkFactorsPerLevel[0] = 1;

    RegistrationType::SmoothingSigmasArrayType smoothingSigmasPerLevel;
    smoothingSigmasPerLevel.SetSize(1);
    smoothingSigmasPerLevel[0] = 0;

    registration->SetNumberOfLevels(numberOfLevels);
    registration->SetSmoothingSigmasPerLevel(smoothingSigmasPerLevel);
    registration->SetShrinkFactorsPerLevel(shrinkFactorsPerLevel);

    try
    {
        registration->Update();
        std::cout << "Optimizer stop condition: " << registration->GetOptimizer()->GetStopConditionDescription() << std::endl;
        //m_oss << "Optimizer stop condition: " << registration->GetOptimizer()->GetStopConditionDescription() << std::endl;
        
    }
    catch (itk::ExceptionObject & err)
    {
        m_oss << "ExceptionObject caught !" << std::endl;
        m_oss << err << std::endl;

        emit sigCalculateFinish(false);
        return EXIT_FAILURE;
    }

    const TransformType::ParametersType finalParameters =
        registration->GetOutput()->Get()->GetParameters();

    const double versorX = finalParameters[0];
    const double versorY = finalParameters[1];
    const double versorZ = finalParameters[2];
    const double finalTranslationX = finalParameters[3];
    const double finalTranslationY = finalParameters[4];
    const double finalTranslationZ = finalParameters[5];
    const unsigned int numberOfIterations = optimizer->GetCurrentIteration();
    const double bestValue = optimizer->GetValue();


    // Print out results
    //
    m_oss <<  std::endl;
    m_oss << "Result = " << std::endl;
    m_oss << " versor X      = " << versorX << std::endl;
    m_oss << " versor Y      = " << versorY << std::endl;
    m_oss << " versor Z      = " << versorZ << std::endl;
    m_oss << " Translation X = " << finalTranslationX << std::endl;
    m_oss << " Translation Y = " << finalTranslationY << std::endl;
    m_oss << " Translation Z = " << finalTranslationZ << std::endl;
    m_oss << " Iterations    = " << numberOfIterations << std::endl;
    m_oss << " Metric value  = " << bestValue << std::endl;

    TransformType::Pointer finalTransform = TransformType::New();

    finalTransform->SetFixedParameters(registration->GetOutput()->Get()->GetFixedParameters());
    finalTransform->SetParameters(finalParameters);

    TransformType::MatrixType matrix = finalTransform->GetMatrix();
    TransformType::OffsetType offset = finalTransform->GetOffset();
    m_oss << "Matrix = " << std::endl << matrix << std::endl;
    m_oss << "Offset = " << std::endl << offset << std::endl;

    typedef itk::ResampleImageFilter<
        MovingImageType,
        FixedImageType >    ResampleFilterType;

    ResampleFilterType::Pointer resampler = ResampleFilterType::New();

    resampler->SetTransform(finalTransform);
    resampler->SetInput(movingImageReader->GetOutput());

    FixedImageType::Pointer fixedImage = fixedImageReader->GetOutput();

    resampler->SetSize(fixedImage->GetLargestPossibleRegion().GetSize());
    resampler->SetOutputOrigin(fixedImage->GetOrigin());
    resampler->SetOutputSpacing(fixedImage->GetSpacing());
    resampler->SetOutputDirection(fixedImage->GetDirection());
    resampler->SetDefaultPixelValue(ui.lineEditDefaultPixel->text().toInt());

    typedef  unsigned short                                          OutputPixelType;
    typedef itk::Image< OutputPixelType, Dimension >                OutputImageType;
    typedef itk::CastImageFilter< FixedImageType, OutputImageType > CastFilterType;
    typedef itk::ImageFileWriter< OutputImageType >                 WriterType;

    WriterType::Pointer      writer = WriterType::New();
    CastFilterType::Pointer  caster = CastFilterType::New();

    writer->SetFileName(ui.lineEditOutputPath->text().toLocal8Bit().data());

    caster->SetInput(resampler->GetOutput());
    writer->SetInput(caster->GetOutput());
    writer->Update();
    /*
    typedef itk::SubtractImageFilter<
        FixedImageType,
        FixedImageType,
        FixedImageType > DifferenceFilterType;
    DifferenceFilterType::Pointer difference = DifferenceFilterType::New();

    typedef itk::RescaleIntensityImageFilter<
        FixedImageType,
        OutputImageType >   RescalerType;
    RescalerType::Pointer intensityRescaler = RescalerType::New();

    difference->SetInput1(fixedImageReader->GetOutput());
    difference->SetInput2(resampler->GetOutput());
    intensityRescaler->SetInput(difference->GetOutput());
    intensityRescaler->SetOutputMinimum(0);
    intensityRescaler->SetOutputMaximum(255);



    resampler->SetDefaultPixelValue(1);

    WriterType::Pointer writer2 = WriterType::New();
    writer2->SetInput(intensityRescaler->GetOutput());

    // Compute the difference image between the
    // fixed and resampled moving image.
    //if (argc > 5)
    {
        writer2->SetFileName(".\ImageRegistration8DifferenceAfter.nii");
        writer2->Update();
    }

    typedef itk::IdentityTransform< double, Dimension > IdentityTransformType;
    IdentityTransformType::Pointer identity = IdentityTransformType::New();
    // Compute the difference image between the
    // fixed and moving image before registration.
    //if (argc > 4)
    {
        resampler->SetTransform(identity);
        writer2->SetFileName("./ImageRegistration8DifferenceBefore.nii");
        writer2->Update();
    }

    typedef itk::Image< OutputPixelType, 2 > OutputSliceType;
    typedef itk::ExtractImageFilter<
        OutputImageType,
        OutputSliceType > ExtractFilterType;
    ExtractFilterType::Pointer extractor = ExtractFilterType::New();
    extractor->SetDirectionCollapseToSubmatrix();
    extractor->InPlaceOn();

    FixedImageType::RegionType inputRegion =
        fixedImage->GetLargestPossibleRegion();
    FixedImageType::SizeType  size = inputRegion.GetSize();
    FixedImageType::IndexType start = inputRegion.GetIndex();

    // Select one slice as output
    size[2] = 0;
    start[2] = 10;
    FixedImageType::RegionType desiredRegion;
    desiredRegion.SetSize(size);
    desiredRegion.SetIndex(start);
    extractor->SetExtractionRegion(desiredRegion);
    typedef itk::ImageFileWriter< OutputSliceType > SliceWriterType;
    SliceWriterType::Pointer sliceWriter = SliceWriterType::New();
    sliceWriter->SetInput(extractor->GetOutput());
    //if (argc > 6)
    {
        extractor->SetInput(caster->GetOutput());
        resampler->SetTransform(identity);
        sliceWriter->SetFileName("./ImageRegistration8Output.png");
        sliceWriter->Update();
    }
    //if (argc > 7)
    {
        extractor->SetInput(intensityRescaler->GetOutput());
        resampler->SetTransform(identity);
        sliceWriter->SetFileName(strDifferenceBeforeRegistration);
        sliceWriter->Update();
    }
    //if (argc > 8)
    {
        resampler->SetTransform(finalTransform);
        sliceWriter->SetFileName(strDifferenceAfterRegistrationImage);
        sliceWriter->Update();
    }
    //if (argc > 9)
    {
        extractor->SetInput(caster->GetOutput());
        resampler->SetTransform(finalTransform);
        sliceWriter->SetFileName("./outputSlice.dcm");
        sliceWriter->Update();
    }
 */   

    emit sigCalculateFinish(true);
    return true;
}

/******************************************************** 
*  @function : RegistCalculate2D
*  @brief    : 2d配准
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
bool Registration::RegistCalculate2D()
{

    return true;
}
/********************************************************
*  @function : changeLanguage
*  @brief    : 变更语言
*  @input    :
*  @output   :
*  @return   :
*********************************************************/
void Registration::changeLanguage()
{
	ui.retranslateUi(this);
}
