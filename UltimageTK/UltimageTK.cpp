/******************************************************** 
* @file    : UltimageTK.cpp
* @brief   : 
* @details : 
* @author  : qh.zhang@atisz.ac.cn
* @date    : 2019-4-24
*********************************************************/

#include <QFileDialog>
#include <QPainter>
#include <QSpinBox> 
#include <QTime>
#include <QMessageBox>
#include <QProcess>
#include <QDebug>
#include "UltimageTK.h"
#include "LabelAnalysis.h"
#include "AllSettings.h"
#include "ConfigHelper.h"

#include "itkGDCMSeriesFileNames.h"
#include "itkGDCMImageIO.h"
#include "itkImageFileReader.h"
#include "itkNiftiImageIO.h"

#include "itkGDCMImageIOFactory.h"
#include "itkNiftiImageIOFactory.h"
#include "itkNRRDImageIOFactory.h"


//用于执行Python的exe需要的头文件
#include "windows.h"
#include "shellapi.h"

/******************************************************** 
*  @function : UltimageTK
*  @brief    : brief
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
UltimageTK::UltimageTK(QWidget *parent)
    : QMainWindow(parent)
    , m_dlgAbout(this)
{
    ui.setupUi(this);
    //this->setWindowTitle("UltimageTK v1.0.1");
}

bool UltimageTK::init()
{
    //初始化信号槽
    bool bRet = true;
    bRet &= connectSignals();
    if (!bRet)
    {
        return false;
    }

    //初始化颜色列表
    std::map<std::string, int> mapLabelProperty;
    ImageDataManager::getInstance()->getLabelProperty(mapLabelProperty);
    bRet &= loadLabel2Combobox(mapLabelProperty);
    bRet &= m_dlgSettingUI.Init(mapLabelProperty);
    AllSettings::getInstance()->UpdateColorMap(mapLabelProperty);
    
    //界面的初始化
    ui.labelSagittalPlane->setVisible(false);
    ui.labelCoronalPlane->setVisible(false);
    ui.labelTransversePlane->setVisible(false);
    ui.pBtnMouse->setDisabled(true);
    ui.widget9->setVisible(false);
    ui.widgetSagittalPlane->setViewPlane(SagittalPlane);
    ui.widgetCoronalPlane->setViewPlane(CoronalPlane);
    ui.widgetTransversePlane->setViewPlane(TransversePlane);

    //初始化相关子界面
    bRet &= m_stThreadDataLoad.Init();
    bRet &= m_dlgFormatConvert.Init();
    bRet &= m_dlgHistogramMatching.Init();
    bRet &= m_dlgRegistration.Init();
    

    ui.pBtnSave->setShortcut(QKeySequence(QLatin1String("Ctrl+S")));
    ui.pBtnDel->setShortcut(QKeySequence(QLatin1String("Del")));
    ui.pBtnFitZoom->setShortcut(QKeySequence(QLatin1String("Ctrl+F")));
    //ui.pBtnTransversePlaneAlone->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_1));
    //ui.pBtnSagittalPlaneAlone->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_2));
    //ui.pBtnCoronalPlaneAlone->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_3));

	//设置语言
	AllConfig stAllConfig;
	ConfigHelper::getInstance()->readConfig(stAllConfig);
	if (stAllConfig.emLanguage == QLocale::Language::Chinese)
	{
		bRet &= m_translator.load("./ultimagetk_zh.qm");
		ui.actionChinese->setChecked(true);
		ui.actionEnglish->setChecked(false);
	}
	else if (stAllConfig.emLanguage == QLocale::Language::English)
	{
		bRet &= m_translator.load("./ultimagetk_en.qm");
		ui.actionChinese->setChecked(false);
		ui.actionEnglish->setChecked(true);
	}
	else
	{
		return false;
	}
	bRet &= qApp->installTranslator(&m_translator);
	if (bRet)
	{
		ui.retranslateUi(this);
        m_dlgSettingUI.changeLanguage();
		m_dlgFormatConvert.changeLanguage();
		m_dlgHistogramMatching.changeLanguage();
		m_dlgRegistration.changeLanguage();
		m_dlgAbout.changeLanguage();

	}

    return bRet;
}

/******************************************************** 
*  @function : connectSignals
*  @brief    : 关联信号槽
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
bool UltimageTK::connectSignals()
{
    bool bRet = true;
    
    bRet &= (bool)connect(ui.actionLoad, &QAction::triggered, this, &UltimageTK::OnLoad);
    bRet &= (bool)connect(ui.actionLoadDir, &QAction::triggered, this, &UltimageTK::OnLoadDir);
    bRet &= (bool)connect(&m_stThreadDataLoad, &DataLoad::sigDataLoadFinish, this, &UltimageTK::OnLoadFinish);

    bRet &= (bool)connect(ui.widgetSagittalPlane, &ImageWidget::sigCurPosAndValue, this, &UltimageTK::OnUpadteCurPosAndValue);
    bRet &= (bool)connect(ui.widgetSagittalPlane, &ImageWidget::sigCurZoom, this, &UltimageTK::OnUpadteCurZoom);
    bRet &= (bool)connect(&m_stThreadDataLoad, &DataLoad::sigDataLoadFinish, ui.widgetSagittalPlane, &ImageWidget::OnLoadFinish);
    
    bRet &= (bool)connect(ui.widgetCoronalPlane, &ImageWidget::sigCurPosAndValue, this, &UltimageTK::OnUpadteCurPosAndValue);
    bRet &= (bool)connect(ui.widgetCoronalPlane, &ImageWidget::sigCurZoom, this, &UltimageTK::OnUpadteCurZoom);
    bRet &= (bool)connect(&m_stThreadDataLoad, &DataLoad::sigDataLoadFinish, ui.widgetCoronalPlane, &ImageWidget::OnLoadFinish);
    
    bRet &= (bool)connect(ui.widgetTransversePlane, &ImageWidget::sigCurPosAndValue, this, &UltimageTK::OnUpadteCurPosAndValue);
    bRet &= (bool)connect(ui.widgetTransversePlane, &ImageWidget::sigCurZoom, this, &UltimageTK::OnUpadteCurZoom);
    bRet &= (bool)connect(&m_stThreadDataLoad, &DataLoad::sigDataLoadFinish, ui.widgetTransversePlane, &ImageWidget::OnLoadFinish);

    bRet &= (bool)connect(ui.pBtnFitZoom, &QPushButton::clicked, this, &UltimageTK::OnFitZoom);
    bRet &= (bool)connect(ui.pBtnMouse, &QPushButton::clicked, this, &UltimageTK::OnSetPenType);
    bRet &= (bool)connect(ui.pBtnPolygon, &QPushButton::clicked, this, &UltimageTK::OnSetPenType);
    bRet &= (bool)connect(ui.pBtnDrawPen, &QPushButton::clicked, this, &UltimageTK::OnSetPenType);
    bRet &= (bool)connect(ui.pBtnSave, &QPushButton::clicked, this, &UltimageTK::OnSaveResult);
    bRet &= (bool)connect(ui.pBtnDel, &QPushButton::clicked, this, &UltimageTK::OnDelTarget);
    bRet &= (bool)connect(ui.pBtnCoronalPlaneClear, &QPushButton::clicked, this, &UltimageTK::OnClearTargets);
    bRet &= (bool)connect(ui.pBtnSagittalPlaneClear, &QPushButton::clicked, this, &UltimageTK::OnClearTargets);
    bRet &= (bool)connect(ui.pBtnTransversePlaneClear, &QPushButton::clicked, this, &UltimageTK::OnClearTargets);
    bRet &= (bool)connect(ui.pBtnTransversePlaneAlone, &QPushButton::clicked, this, &UltimageTK::OnPlaneAlone);
    bRet &= (bool)connect(ui.pBtnSagittalPlaneAlone, &QPushButton::clicked, this, &UltimageTK::OnPlaneAlone);
    bRet &= (bool)connect(ui.pBtnCoronalPlaneAlone, &QPushButton::clicked, this, &UltimageTK::OnPlaneAlone);
    bRet &= (bool)connect(ui.pBtnTransversePlaneCapture, &QPushButton::clicked, this, &UltimageTK::OnCapture);
    bRet &= (bool)connect(ui.pBtnSagittalPlaneCapture, &QPushButton::clicked, this, &UltimageTK::OnCapture);
    bRet &= (bool)connect(ui.pBtnCoronalPlaneCapture, &QPushButton::clicked, this, &UltimageTK::OnCapture);
    
   

    bRet &= (bool)connect(ui.pBtnSetting, &QPushButton::clicked, this, &UltimageTK::OnSetting);
    bRet &= (bool)connect(ui.comboBox, &QComboBox::currentTextChanged, this, &UltimageTK::OnCurColorChanged);
    
    bRet &= (bool)connect(ui.horizontalSliderAlphaValue, &QSlider::valueChanged, this, &UltimageTK::OnAlphaValueChanged);
    bRet &= (bool)connect(ui.spinBoxAlphaValue, SIGNAL(valueChanged(int)), this, SLOT(OnAlphaValueChanged(int)));


    bRet &= (bool)connect(ImageDataManager::getInstance(), &ImageDataManager::sigColorMapChanged, this, &UltimageTK::reloadColorMap);
    
    
    bRet &= (bool)connect(ui.pBtnUpdate3D, &QPushButton::clicked, this, &UltimageTK::OnUpdate3DView);
    
    //另存为
    bRet &= (bool)connect(ui.actionSaveAsLSR, &QAction::triggered, this, &UltimageTK::OnSaveAs);
    bRet &= (bool)connect(ui.actionSaveAsNII, &QAction::triggered, this, &UltimageTK::OnSaveAs);
    bRet &= (bool)connect(ui.actionSaveAsNRRD, &QAction::triggered, this, &UltimageTK::OnSaveAs);

    //格式转换
    bRet &= (bool)connect(ui.actionDICOM_NITFI, &QAction::triggered, this, &UltimageTK::OnFormatConvert);
    bRet &= (bool)connect(ui.actionDICOM_NRRD, &QAction::triggered, this, &UltimageTK::OnFormatConvert);
    bRet &= (bool)connect(ui.actionNRRD_NITFI, &QAction::triggered, this, &UltimageTK::OnFormatConvert);
    bRet &= (bool)connect(ui.actionNITFI_NRRD, &QAction::triggered, this, &UltimageTK::OnFormatConvert);

    //预处理
    bRet &= (bool)connect(ui.actionHistogramMatching, &QAction::triggered, this, &UltimageTK::OnHistogramMatching);
    bRet &= (bool)connect(ui.actionRegister, &QAction::triggered, this, &UltimageTK::OnRegistation);

    //帮助
	bRet &= (bool)connect(ui.action_About, &QAction::triggered, this, &UltimageTK::OnAbout);
	bRet &= (bool)connect(ui.actionChinese, &QAction::triggered, this, &UltimageTK::OnLanguageChanged);
	bRet &= (bool)connect(ui.actionEnglish, &QAction::triggered, this, &UltimageTK::OnLanguageChanged);


    return bRet;
}

/******************************************************** 
*  @function : loadLabel2Combobox
*  @brief    : 加载颜色Combobox列表
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
bool UltimageTK::loadLabel2Combobox(const std::map<std::string, int> &mapLabelProperty)
{
    ui.comboBox->clear();
    for each (auto var in mapLabelProperty)
    {
        QImage image(12, 12, QImage::Format_RGB32);
        QPainter painter(&image);
        QColor color(var.second);
        painter.fillRect(0, 0, 12, 12, color);
        QIcon icon;
        icon.addPixmap(QPixmap::fromImage(image));
        ui.comboBox->addItem(icon, QString::fromLocal8Bit(var.first.c_str()));
    }
    ImageDataManager::getInstance()->updateLabelPpIndex();
    return true;
}

void UltimageTK::reloadColorMap()
{
    std::map<std::string, int> mapLabelProperty;
    ImageDataManager::getInstance()->getLabelProperty(mapLabelProperty);
    loadLabel2Combobox(mapLabelProperty);
    m_dlgSettingUI.reload(mapLabelProperty);
    AllSettings::getInstance()->UpdateColorMap(mapLabelProperty);
}

//QTime atime;
//int nTime = 0;

/******************************************************** 
*  @function : OnLoad
*  @brief    : 载入文件
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void UltimageTK::OnLoad()
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
    disconnect(ui.spinBoxWindow, SIGNAL(valueChanged(int)), this, SLOT(OnWindowLevelChanged()));
    disconnect(ui.spinBoxCenter, SIGNAL(valueChanged(int)), this, SLOT(OnWindowLevelChanged()));
    //ui.widget->loadImage(strFileName);
    QFileInfo file(strFileName);
    if (file.completeSuffix() == "nii"
        || file.completeSuffix() == "nii.gz")
    {
        m_stThreadDataLoad.startLoadData(strFileName);
    }
    else
    {
        QString strDirPath = file.absolutePath();
        m_stThreadDataLoad.startLoadData(strDirPath);
    }

    ui.statusBar->showMessage(QString::fromLocal8Bit("Loading.."));
    //nTime = 0;
    //atime.start();
}

/******************************************************** 
*  @function : OnLoadDir
*  @brief    : 载入文件夹
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void UltimageTK::OnLoadDir()
{
    QString strDirName = QFileDialog::getExistingDirectory(this, QString::fromLocal8Bit("Choose Dir"),
        "/home",
        QFileDialog::ShowDirsOnly
        | QFileDialog::DontResolveSymlinks);
    if (strDirName.isEmpty())
    {
        return;
    }
    disconnect(ui.spinBoxWindow, SIGNAL(valueChanged(int)), this, SLOT(OnWindowLevelChanged()));
    disconnect(ui.spinBoxCenter, SIGNAL(valueChanged(int)), this, SLOT(OnWindowLevelChanged()));
    m_stThreadDataLoad.startLoadData(strDirName);
    ui.statusBar->showMessage(QString::fromLocal8Bit("Loading..."));
    //nTime = 0;
    //atime.start();
}

/******************************************************** 
*  @function : OnUpadteCurPosAndValue
*  @brief    : 更新当前鼠标点的物理值和坐标位置
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void UltimageTK::OnUpadteCurPosAndValue(float fX, float fY, float fZ, int nValue)
{

    ImageHeaderInfo stImageHeaderInfo = ImageDataManager::getInstance()->getImageHeaderInfo();
    ui.labelX->setText(QString::number(int(fX*stImageHeaderInfo.nWidth)));
    ui.labelY->setText(QString::number(int(fY*stImageHeaderInfo.nHeight)));
    ui.labelZ->setText(QString::number(int(fZ*stImageHeaderInfo.nThickNess)));
    ui.labelValue->setText(QString::number(nValue));
    if (sender() != ui.widgetSagittalPlane)
    {
        ui.widgetSagittalPlane->loadImageFromManager(fX*stImageHeaderInfo.nWidth);
        ui.widgetSagittalPlane->updateCrossPos(QPointF(fY, fZ));
    }
    if (sender() != ui.widgetCoronalPlane)
    {
        ui.widgetCoronalPlane->loadImageFromManager(fY*stImageHeaderInfo.nHeight);
        ui.widgetCoronalPlane->updateCrossPos(QPointF(fX, fZ));
    }
    if (sender() != ui.widgetTransversePlane)
    {
        ui.widgetTransversePlane->loadImageFromManager(fZ*stImageHeaderInfo.nThickNess);
        ui.widgetTransversePlane->updateCrossPos(QPointF(fX, fY));
    }
}
//
//void UltimageTK::OnUpadteCurZoom(int nX, int nY, int nZ, float fZoom)
//{
//
//    ui.labelZoom->setText(QString::number(fZoom));
//    ImageHeaderInfo stImageHeaderIndo = ImageDataManager::getInstance()->getImageHeaderInfo();
//    //if (sender() != ui.widgetSagittalPlane)
//    {
//        ui.widgetSagittalPlane->loadImageFromManager(nX);
//        ui.widgetSagittalPlane->updateZoomCenter(QPoint(nY,nZ));
//    }
//    //if (sender() != ui.widgetCoronalPlane)
//    {
//        ui.widgetCoronalPlane->loadImageFromManager(nY);
//        ui.widgetCoronalPlane->updateZoomCenter(QPoint(nX, nZ));
//    }
//    //if (sender() != ui.widgetTransversePlane)
//    {
//        ui.widgetTransversePlane->loadImageFromManager(nZ);
//        ui.widgetTransversePlane->updateZoomCenter(QPoint(nX, nY));
//    }
//}

/******************************************************** 
*  @function : OnUpadteCurZoom
*  @brief    : 更新缩放比例
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void UltimageTK::OnUpadteCurZoom(float fZoom)
{
    ui.labelZoom->setText(QString::number(fZoom));

    ui.widgetSagittalPlane->updateZoomCenter(fZoom);
    ui.widgetCoronalPlane->updateZoomCenter(fZoom);
    ui.widgetTransversePlane->updateZoomCenter(fZoom);

    ui.widgetSagittalPlane->updateZoom(fZoom);
    ui.widgetCoronalPlane->updateZoom(fZoom);
    ui.widgetTransversePlane->updateZoom(fZoom);
}

/******************************************************** 
*  @function : OnLoadFinish
*  @brief    : 加载文件或文件夹结束
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void UltimageTK::OnLoadFinish(bool bLoadSuccess)
{
    //nTime = atime.elapsed();
    if (bLoadSuccess)
    {
        ui.statusBar->showMessage(QString::fromLocal8Bit("Load success."), 5000);
        //ui.statusBar->showMessage(QString::fromLocal8Bit("加载成功.用时") + QString::number(nTime) + QString::fromLocal8Bit("ms"), 5000);
        ImageHeaderInfo stInfo = ImageDataManager::getInstance()->getImageHeaderInfo();
        //ui.labelPName->setText(stInfo.strPatientName);
        ui.labelPAge->setText(stInfo.strPatientAge);
        ui.labelPSex->setText(stInfo.strPatientSex);
        ui.labelModality->setText(stInfo.strFileType);
        ImageHeaderInfo stImageHeaderIndo = ImageDataManager::getInstance()->getImageHeaderInfo();
        ui.spinBoxWindow->setValue(stImageHeaderIndo.nWinWidth);
        ui.spinBoxCenter->setValue(stImageHeaderIndo.nWinCenter);
        connect(ui.spinBoxWindow, SIGNAL(valueChanged(int)), this, SLOT(OnWindowLevelChanged()));
        connect(ui.spinBoxCenter, SIGNAL(valueChanged(int)), this, SLOT(OnWindowLevelChanged()));
    }
    else
    {
        ui.statusBar->showMessage(QString::fromLocal8Bit("Load failed."));
    }
}

/******************************************************** 
*  @function : OnFitZoom
*  @brief    : 自适应显示
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void UltimageTK::OnFitZoom()
{
    ImageHeaderInfo stImageHeaderIndo = ImageDataManager::getInstance()->getImageHeaderInfo();

    float fZoom = 1.0f;
    float f1 = (float)(ui.widgetSagittalPlane->width() - IMAGE_MARGIN * 2) / stImageHeaderIndo.nWidth;
    float f2 = (float)(ui.widgetCoronalPlane->height() - IMAGE_MARGIN * 2) / stImageHeaderIndo.nHeight;
    float f3 = (float)(ui.widgetTransversePlane->height() - IMAGE_MARGIN * 2) / stImageHeaderIndo.nThickNess;
    fZoom = f1 < f2 ? f1 : f2;
    fZoom = fZoom < f3 ? fZoom : f3;

    ui.labelZoom->setText(QString::number(fZoom));

    ui.widgetSagittalPlane->updateZoomCenter(fZoom);
    ui.widgetCoronalPlane->updateZoomCenter(fZoom);
    ui.widgetTransversePlane->updateZoomCenter(fZoom);

    ui.widgetSagittalPlane->resetOffset(fZoom);
    ui.widgetCoronalPlane->resetOffset(fZoom);
    ui.widgetTransversePlane->resetOffset(fZoom);

    ui.widgetSagittalPlane->updateZoom(fZoom);
    ui.widgetCoronalPlane->updateZoom(fZoom);
    ui.widgetTransversePlane->updateZoom(fZoom);
}

/******************************************************** 
*  @function : OnSetPenType
*  @brief    : 设置画笔类型
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void UltimageTK::OnSetPenType()
{
    ui.pBtnMouse->setDisabled(false);
    ui.pBtnPolygon->setDisabled(false);
    ui.pBtnDrawPen->setDisabled(false);
    if (sender() == ui.pBtnMouse)
    {
        ui.pBtnMouse->setDisabled(true);
        DrawPen::setCurPenType(PEN_TYPE::PenNone);
    }
    if (sender() == ui.pBtnPolygon)
    {
        ui.pBtnPolygon->setDisabled(true);
        DrawPen::setCurPenType(PEN_TYPE::PenPolygon);
    }
    if (sender() == ui.pBtnDrawPen)
    {
        ui.pBtnDrawPen->setDisabled(true);
        DrawPen::setCurPenType(PEN_TYPE::PenDrawPen);
    }
}

/******************************************************** 
*  @function : OnSaveResult
*  @brief    : 保存结果
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void UltimageTK::OnSaveResult()
{
    ui.statusBar->showMessage(QString::fromLocal8Bit("Saving..."));
    std::map<std::string, int> mapLabelProperty;
    ImageDataManager::getInstance()->getLabelProperty(mapLabelProperty);
    m_dlgSettingUI.reload(mapLabelProperty);
    bool bRet =ImageDataManager::getInstance()->saveLabelInfo();
    ui.statusBar->showMessage(bRet?QString::fromLocal8Bit("Save succes."): QString::fromLocal8Bit("Save failed."),5000);
}

/******************************************************** 
*  @function : OnDelTarget
*  @brief    : 删除选中的目标
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void UltimageTK::OnDelTarget()
{
    QPair<VIEW_PLANE, int> pairSelectTgt = DrawPen::getCurTarget();
    if (pairSelectTgt.second!=-1)
    {
        switch (pairSelectTgt.first)
        {
        case SagittalPlane:  //矢状面
            ui.widgetSagittalPlane->delTarget(pairSelectTgt.second);
            break;
        case CoronalPlane:   //冠状面
            ui.widgetCoronalPlane->delTarget(pairSelectTgt.second);
            break;
        case TransversePlane:    //横断面
            ui.widgetTransversePlane->delTarget(pairSelectTgt.second);
            break;
        default:
            break;
        }
        ui.widgetSagittalPlane->update();
        ui.widgetCoronalPlane->update();
        ui.widgetTransversePlane->update();
    }
}

/******************************************************** 
*  @function : OnClearTargets
*  @brief    : 清除当前页的目标
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void UltimageTK::OnClearTargets()
{
    if (sender() == ui.pBtnCoronalPlaneClear)
    {
        ui.widgetCoronalPlane->clearTargets();
        ui.widgetCoronalPlane->update();
    }
    if (sender() == ui.pBtnSagittalPlaneClear)
    {
        ui.widgetSagittalPlane->clearTargets();
        ui.widgetSagittalPlane->update();
    }
    if (sender() == ui.pBtnCoronalPlaneClear)
    {
        ui.widgetTransversePlane->clearTargets();
        ui.widgetTransversePlane->update();
    }

}

/******************************************************** 
*  @function : 打开设置页面
*  @brief    : brief
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void UltimageTK::OnSetting()
{
    m_dlgSettingUI.exec();
    reloadColorMap();
}

/******************************************************** 
*  @function : OnCurColorChanged
*  @brief    : 设置画笔颜色
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void UltimageTK::OnCurColorChanged(QString str)
{
    AllSettings::getInstance()->SetCurLabel(str);
}

/******************************************************** 
*  @function : OnAlphaValueChanged
*  @brief    : 设置透明度
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void UltimageTK::OnAlphaValueChanged(int nNewValue)
{
    if (sender() == ui.horizontalSliderAlphaValue)
    {
        ui.spinBoxAlphaValue->setValue(nNewValue);
    }
    if (sender() == ui.spinBoxAlphaValue)
    {
        ui.horizontalSliderAlphaValue->setValue(nNewValue);
    }
    DrawPen::setAlphaValue(nNewValue);
    ui.widgetSagittalPlane->update();
    ui.widgetCoronalPlane->update();
    ui.widgetTransversePlane->update();
}

/******************************************************** 
*  @function : OnWindowLevelChanged
*  @brief    : 设置窗位窗宽
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void UltimageTK::OnWindowLevelChanged()
{
    int nWindow = ui.spinBoxWindow->text().toInt();
    int nCenter = ui.spinBoxCenter->text().toInt();
    ImageDataManager::getInstance()->setWindowLevel(nWindow, nCenter);
    ui.widgetSagittalPlane->reload();
    ui.widgetCoronalPlane->reload();
    ui.widgetTransversePlane->reload();

}

/******************************************************** 
*  @function : OnUpdate3DView
*  @brief    : 更新3D视图
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void UltimageTK::OnUpdate3DView()
{
    
}

/******************************************************** 
*  @function : OnPlaneAlone
*  @brief    : 独占模式
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void UltimageTK::OnPlaneAlone()
{

	AllConfig stAllConfig;
	ConfigHelper::getInstance()->readConfig(stAllConfig);
	QString strExclusive;
	QString strCancelExclusive;
	if (stAllConfig.emLanguage == QLocale::Language::Chinese)
	{
		strExclusive = QString::fromLocal8Bit("独占");
		strCancelExclusive = QString::fromLocal8Bit("取消独占");
	}
	else if (stAllConfig.emLanguage == QLocale::Language::English)
	{
		strExclusive = QString::fromLocal8Bit("Exclusive");
		strCancelExclusive = QString::fromLocal8Bit("Cancel");
	}
	else
	{
		return;
	}

    if (((QPushButton*)sender())->text() == strExclusive)
    {
        if (sender() == ui.pBtnTransversePlaneAlone)
        {
            ui.widgetS->setVisible(false);
            ui.widgetC->setVisible(false);
            ui.pBtnCoronalPlaneAlone->setText(strExclusive);
            ui.pBtnTransversePlaneAlone->setText(strCancelExclusive);
            ui.pBtnSagittalPlaneAlone->setText(strExclusive);
        }
        if (sender() == ui.pBtnSagittalPlaneAlone)
        {
            ui.widgetT->setVisible(false);
            ui.widgetC->setVisible(false);
            ui.pBtnCoronalPlaneAlone->setText(strExclusive);
            ui.pBtnTransversePlaneAlone->setText(strExclusive);
            ui.pBtnSagittalPlaneAlone->setText(strCancelExclusive);
        }
        if (sender() == ui.pBtnCoronalPlaneAlone)
        {
            ui.widgetS->setVisible(false);
            ui.widgetT->setVisible(false);
            ui.pBtnCoronalPlaneAlone->setText(strCancelExclusive);
            ui.pBtnTransversePlaneAlone->setText(strExclusive);
            ui.pBtnSagittalPlaneAlone->setText(strExclusive);
        }
    }
    else
    {
        ui.widgetS->setVisible(true);
        ui.widgetT->setVisible(true);
        ui.widgetC->setVisible(true);
        ui.pBtnCoronalPlaneAlone->setText(strExclusive);
        ui.pBtnTransversePlaneAlone->setText(strExclusive);
        ui.pBtnSagittalPlaneAlone->setText(strExclusive);
        OnFitZoom();
    }

}

/******************************************************** 
*  @function : OnCapture
*  @brief    : 截图
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void UltimageTK::OnCapture()
{
    QString strFileName = QFileDialog::getSaveFileName(this, QString::fromLocal8Bit("Save file"),
        "",
        tr("JPEG Images (*.jpg);;PNG Images (*.png);;BMP Images (*.bmp)"));
    if (strFileName.isEmpty())
    {
        return;
    }
    bool bRet = true;
    if (sender() == ui.pBtnTransversePlaneCapture)
    {
        bRet = ui.widgetTransversePlane->saveCapture(strFileName);
    }
    if (sender() == ui.pBtnSagittalPlaneCapture)
    {
        bRet = ui.widgetSagittalPlane->saveCapture(strFileName);
    }
    if (sender() == ui.pBtnCoronalPlaneCapture)
    {
        bRet = ui.widgetCoronalPlane->saveCapture(strFileName);
    }
    if (bRet)
    {
        QMessageBox::information(this, "Tip", QString::fromLocal8Bit("Save success!"));
    }
    else
    {
        QMessageBox::information(this, "Tip", QString::fromLocal8Bit("Save Failed!"));
    }
}

/******************************************************** 
*  @function : OnSaveAs
*  @brief    : 另存结果
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void UltimageTK::OnSaveAs()
{
    QString strFileName;
    if (sender()==ui.actionSaveAsLSR)
    {
        strFileName = QFileDialog::getSaveFileName(this, QString::fromLocal8Bit("Save file"),
            "",
            tr("Images (*.lsr)"));
        if (strFileName.isEmpty())
        {
            return;
        }
        m_thdSave.SaveAsLsr(strFileName);
    }
    if (sender() == ui.actionSaveAsNII)
    {
        strFileName = QFileDialog::getSaveFileName(this, QString::fromLocal8Bit("Save file"),
            "",
            tr("Images (*.nii *.nii.gz)"));
        if (strFileName.isEmpty())
        {
            return;
        }
        m_thdSave.SaveAsNii(strFileName);
    }
    if (sender() == ui.actionSaveAsNRRD)
    {
        strFileName = QFileDialog::getSaveFileName(this, QString::fromLocal8Bit("Save file"),
            "",
            tr("Images (*.nrrd)"));
        if (strFileName.isEmpty())
        {
            return;
        }
        m_thdSave.SaveAsNrrd(strFileName);
    }
}

/******************************************************** 
*  @function : OnFormatConvert
*  @brief    : 格式转换
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void UltimageTK::OnFormatConvert()
{
    if (sender()== ui.actionDICOM_NITFI)
    {
        m_dlgFormatConvert.SetConvertType(DICOM2NII);
    }
    if (sender() == ui.actionDICOM_NRRD)
    {
        m_dlgFormatConvert.SetConvertType(DICOM2NRRD);
    }
    if (sender() == ui.actionNRRD_NITFI)
    {
        m_dlgFormatConvert.SetConvertType(NRRD2NII);
    }
    if (sender() == ui.actionNITFI_NRRD)
    {
        m_dlgFormatConvert.SetConvertType(NII2NRRD);
    }
    m_dlgFormatConvert.setWindowTitle(((QAction*)sender())->text());
    m_dlgFormatConvert.exec();
}

/******************************************************** 
*  @function : OnHistogramMatching
*  @brief    : 直方图匹配
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void UltimageTK::OnHistogramMatching()
{
    m_dlgHistogramMatching.exec();
}

/******************************************************** 
*  @function : OnRegistation
*  @brief    : 配准
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void UltimageTK::OnRegistation()
{
    //QMessageBox::information(this, "Tip", QString::fromLocal8Bit("模块暂缺!"));
    //return;
    m_dlgRegistration.exec();
}

/******************************************************** 
*  @function : OnAbout
*  @brief    : 关于页面
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void UltimageTK::OnAbout()
{
    m_dlgAbout.exec();
}

/******************************************************** 
*  @function : OnLanguageChanged
*  @brief    : 切换语言
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void UltimageTK::OnLanguageChanged()
{
	//设置语言
	AllConfig stAllConfig;
	ConfigHelper::getInstance()->readConfig(stAllConfig);
	QString strCancelExclusive;
	QString strCancelExclusiveNew;
	if (stAllConfig.emLanguage == QLocale::Language::Chinese)
	{
		strCancelExclusive = QString::fromLocal8Bit("取消独占");
		strCancelExclusiveNew = QString::fromLocal8Bit("Cancel");
	}
	else if (stAllConfig.emLanguage == QLocale::Language::English)
	{
		strCancelExclusive = QString::fromLocal8Bit("Cancel");
		strCancelExclusiveNew = QString::fromLocal8Bit("取消独占");
	}
	else
	{
		return;
	}



	bool bRet = false;
	bRet &= qApp->removeTranslator(&m_translator);

	bool bBtnCoronalPlaneAlone = false;
	bool bBtnTransversePlaneAlone = false;
	bool bBtnSagittalPlaneAlone = false;

	if (ui.pBtnCoronalPlaneAlone->text() == strCancelExclusive)
	{
		bBtnCoronalPlaneAlone = true;
	}
	if (ui.pBtnTransversePlaneAlone->text() == strCancelExclusive)
	{
		bBtnTransversePlaneAlone = true;
	}
	if (ui.pBtnSagittalPlaneAlone->text() == strCancelExclusive)
	{
		bBtnSagittalPlaneAlone = true;
	}

	if (sender() == ui.actionChinese)
	{
		ui.actionChinese->setChecked(true);
		ui.actionEnglish->setChecked(false);
		bRet &= m_translator.load(QString("ultimagetk_zh.qm"));
		stAllConfig.emLanguage = QLocale::Language::Chinese;
	}
	if (sender() == ui.actionEnglish)
	{
		ui.actionChinese->setChecked(false);
		ui.actionEnglish->setChecked(true);
		bRet &= m_translator.load(QString("ultimagetk_en.qm"));
		stAllConfig.emLanguage = QLocale::Language::English;
	}

	bRet = qApp->installTranslator(&m_translator);

	if (bRet)
	{
		ui.retranslateUi(this);
		m_dlgSettingUI.changeLanguage();
		m_dlgFormatConvert.changeLanguage();
		m_dlgHistogramMatching.changeLanguage();
		m_dlgRegistration.changeLanguage();
		m_dlgAbout.changeLanguage();

		ConfigHelper::getInstance()->writeConfig(stAllConfig);
		if (bBtnCoronalPlaneAlone)
		{
			ui.pBtnCoronalPlaneAlone->setText(strCancelExclusiveNew);
		}
		if (bBtnTransversePlaneAlone)
		{
			ui.pBtnTransversePlaneAlone->setText(strCancelExclusiveNew);
		}
		if (bBtnSagittalPlaneAlone)
		{
			ui.pBtnSagittalPlaneAlone->setText(strCancelExclusiveNew);
		}
	}
}

/******************************************************** 
*  @function : closeEvent
*  @brief    : 关闭事件
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void UltimageTK::closeEvent(QCloseEvent *event)
{
    return;
    QMessageBox msgBox;
    msgBox.setText(QString::fromLocal8Bit("Closing..."));
    msgBox.setInformativeText(QString::fromLocal8Bit("是否保存当前标记？\nSave the label?"));
    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Save);
    int ret = msgBox.exec();
    switch (ret)
    {
    case QMessageBox::Save:
        OnSaveResult();
        break;
    case QMessageBox::Discard:
        break;
    case QMessageBox::Cancel:
        event->ignore();
        return;
        break;
    default:
        break;
    }
}

/******************************************************** 
*  @function : keyPressEvent
*  @brief    : 按键响应
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void UltimageTK::keyPressEvent(QKeyEvent *event)
{
   /* if (event->key() == Qt::Key_Delete)
    {
        OnDelTarget();
    }*/
}

