/******************************************************** 
* @file    : ImageWidget.cpp
* @brief   : 
* @details : 
* @author  : qh.zhang@atisz.ac.cn
* @date    : 2019-4-23
*********************************************************/
#include <QPainter>
#include <QDebug>
#include <QMouseEvent>
#include <windows.h>
#include <QDir>
#include <QTime>

#include "ImageWidget.h"
#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dctk.h"
#include "ImageDataManager.h"
#include "./BridgeOpenCV/include/itkOpenCVImageBridge.h"
#include "itkFlipImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkIntensityWindowingImageFilter.h"
cv::Point3f ImageWidget::m_ptCurPos;
float   ImageWidget::m_fCurZoom = 1.0;      //当前缩放
int     ImageWidget::m_nCurValue = 0;    //当前真实值
ImageHeaderInfo ImageWidget::m_stImageHeaderInfo;

QVector<int> g_vecScale = { 1,10,100,1000 };//毫米、厘米、10厘米

/******************************************************** 
*  @function :  ImageWidget
*  @brief    :  构造函数 
*  @input    : 
*  @output   : 
*  @return   : 
*********************************************************/
ImageWidget::ImageWidget(QWidget *parent/* = nullptr*/)
    : QWidget(parent)
    , m_ptCross(0.5, 0.5)
    , m_ptOffset(IMAGE_MARGIN, IMAGE_MARGIN)
    , m_stDrawPen(this)
{
    this->setMouseTracking(true);

    bool bRet = connect(&m_stDrawPen, &DrawPen::sigSelectPolygon, this, &ImageWidget::OnSelectPolygonChanged);
    //DrawPen::setCurPenType(PEN_TYPE::PenPolygon);
}

/******************************************************** 
*  @function :  ImageWidget
*  @brief    :  析构函数 
*  @input    : 
*  @output   : 
*  @return   : 
*********************************************************/
ImageWidget::~ImageWidget()
{
}

//获取原始图像的CT值图像
bool GetCTValueImg(cv::Mat& img, int nIntercept, int nSlope)
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

/******************************************************** 
*  @function : loadImageFromManager
*  @brief    : 从管理类中读取图片切片
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
bool ImageWidget::loadImageFromManager(int nIndex)
{
    ImageDataManager* pImageDataManager = ImageDataManager::getInstance();
    //bool bRet = pImageDataManager->getImage(nIndex, m_imgSrc, VIEW_PLANE::TransversePlane);
    m_stImageHeaderInfo = pImageDataManager->getImageHeaderInfo();
    if (m_pSlice == nullptr)
    {
        m_pSlice = SliceType::New();
    }

    bool bRet = pImageDataManager->getSliceImage(nIndex, m_pSlice, m_emCurPlane);
    if (bRet==false)
    {
        return false;
    }
    ImageType2D* img2D = m_pSlice->GetOutput();
    //    //结果显示
    itk::FixedArray<bool, 2>flipAxes;
    flipAxes[0] = false;
    flipAxes[1] = true;
    typedef itk::FlipImageFilter<ImageType2D>FlipImageFilterType;
    FlipImageFilterType::Pointer flipFilter = FlipImageFilterType::New();
    if (m_emCurPlane!=TransversePlane)
    {
        flipFilter->SetInput(img2D);
        flipFilter->SetFlipAxes(flipAxes);
        flipFilter->Update();
        img2D = flipFilter->GetOutput();
    }

    int nWinWidth = pImageDataManager->getImageHeaderInfo().nWinWidth;
    int nWinCenter = pImageDataManager->getImageHeaderInfo().nWinCenter;

    typedef itk::MinimumMaximumImageCalculator <ImageType2D> IMinimumMaximumImageCalculatorType;
    IMinimumMaximumImageCalculatorType::Pointer imageCalculatorFilter = IMinimumMaximumImageCalculatorType::New();
    imageCalculatorFilter->SetImage(img2D);
    imageCalculatorFilter->Compute();
    int nSliceMaxValue = imageCalculatorFilter->GetMaximum(); //最大值
    int nSliceMinValue = imageCalculatorFilter->GetMinimum(); //最小值
    
    int n3DMaxValue = nWinCenter + nWinWidth / 2.0f; //最大值
    int n3DMinValue = nWinCenter - nWinWidth / 2.0f; //最小值

    ImageType2D::RegionType inputRegion = img2D->GetLargestPossibleRegion();
    ImageType2D::SizeType size = inputRegion.GetSize();
    for (int i = 0; i < size[0]; i++)
    {
        for (int j = 0; j < size[1]; j++)
        {
            if (i == 189
                && j == 266)
            {
                int nCurRealValue2 = img2D->GetPixel({ i, j });
            }
            int nGrayValue = 0;
            int nCurRealValue = img2D->GetPixel({ i, j });
            if (nCurRealValue<= n3DMinValue)
            {
                nGrayValue = 0;
            }
            else if (nCurRealValue >= n3DMaxValue)
            {
                nGrayValue = 255;
            }
            else
            {
                nGrayValue = (double)(nCurRealValue - n3DMinValue) / nWinWidth * 255;
            }
            img2D->SetPixel({ i, j }, nGrayValue);
        }

    }

    //int nInputMax = (double)(nSliceMaxValue - n3DMinValue) / nWinWidth * 255;
    //int nInputMin = (double)(nSliceMinValue - n3DMinValue) / nWinWidth * 255;

    ////设置窗位和窗宽
    //typedef itk::IntensityWindowingImageFilter <ImageType2D, ImageType2D> IntensityWindowingImageFilterType;
    //IntensityWindowingImageFilterType::Pointer intensityFilter = IntensityWindowingImageFilterType::New();
    //intensityFilter->SetInput(img2D);
    //intensityFilter->SetWindowLevel(nWinWidth, nWinCenter);
    //intensityFilter->SetOutputMinimum(nInputMin);
    //intensityFilter->SetOutputMaximum(nInputMax);
    //intensityFilter->Update();
    //img2D = intensityFilter->GetOutput();
    //int nMa = intensityFilter->GetOutputMaximum();
    //int nMi = intensityFilter->GetOutputMinimum();

    ////设置输出最大最小像素值
    //typedef itk::RescaleIntensityImageFilter< ImageType2D, ImageType2D > RescaleFilterType;
    //RescaleFilterType::Pointer rescaleFilter = RescaleFilterType::New();
    //rescaleFilter->SetInput(intensityFilter->GetOutput());
    //rescaleFilter->SetOutputMinimum(0);
    //rescaleFilter->SetOutputMaximum(255);
    //rescaleFilter->Update();
    //img2D = rescaleFilter->GetOutput();

    cv::Mat matImg = itk::OpenCVImageBridge::ITKImageToCVMat(img2D);
    m_pItkImg = img2D;
    img2D = nullptr;

    matImg.convertTo(matImg, CV_8U);
    int nHeight = 0;
    switch (m_emCurPlane)
    {
    case SagittalPlane:
        nHeight = matImg.rows*(m_stImageHeaderInfo.fPixelSpacingT / m_stImageHeaderInfo.fPixelSpacingW);
        cv::resize(matImg, matImg, cv::Size(matImg.cols, nHeight), (0, 0), (0, 0), cv::INTER_NEAREST);
        break;
    case CoronalPlane:
        nHeight = matImg.rows*(m_stImageHeaderInfo.fPixelSpacingT / m_stImageHeaderInfo.fPixelSpacingW);
        cv::resize(matImg, matImg, cv::Size(matImg.cols, nHeight), (0, 0), (0, 0), cv::INTER_NEAREST);
        break;
    case TransversePlane:
        break;
    default:
        break;
    }
    
    QImage img2Copy(matImg.data, matImg.cols, matImg.rows, matImg.step, QImage::Format_Grayscale8);

    nHeight = img2Copy.height();
    m_imgSrc = img2Copy.copy();
    m_imgProc = m_imgSrc.copy();
    m_stDrawPen.reload(nIndex);

    m_nCurFrameNum = nIndex;
    update();
    return bRet;
}

/******************************************************** 
*  @function : reload
*  @brief    : 重新加载
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void ImageWidget::reload()
{
    loadImageFromManager(m_nCurFrameNum);
}

/******************************************************** 
*  @function : setViewPlane
*  @brief    : 设置当前视图类型
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void ImageWidget::setViewPlane(VIEW_PLANE emCurPlane)
{
   m_emCurPlane = emCurPlane;
   m_stDrawPen.initDraw(m_emCurPlane);
   m_stDrawPen.setImgPointer(&m_imgSrc, &m_imgProc);
}

/******************************************************** 
*  @function : setCurCenterPos
*  @brief    : 设置光标中心
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void ImageWidget::setCurCenterPos(cv::Point3i ptCurPos)
{
    m_ptCurPos.x = (float)ptCurPos.x / (m_stImageHeaderInfo.nWidth);
    m_ptCurPos.y = (float)ptCurPos.y / (m_stImageHeaderInfo.nHeight);
    m_ptCurPos.z = (float)ptCurPos.z / (m_stImageHeaderInfo.nThickNess);
    emit sigCurPosAndValue(m_ptCurPos.x, m_ptCurPos.y, m_ptCurPos.z, m_nCurValue);
}

/******************************************************** 
*  @function : updateZoom
*  @brief    : 更新缩放值
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void ImageWidget::updateZoom(float fZoom)
{
    m_fCurZoom = fZoom;
    m_stDrawPen.setZoom(fZoom);

    update(this->rect());
}

/******************************************************** 
*  @function : updateCrossPos
*  @brief    : 更新光标位置
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void ImageWidget::updateCrossPos(QPointF ptCrossPos)
{
    m_ptCross = ptCrossPos;
    m_stDrawPen.setCross(m_ptCross);

    update();
}

/******************************************************** 
*  @function : updateZoomCenter
*  @brief    : 更新缩放值
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void ImageWidget::updateZoomCenter(float fZoom)
{
    QPoint ptOldOffset = m_ptOffset;
    m_ptOffset.setX(round(m_ptCross.x()*(float)m_imgSrc.width()*m_fCurZoom + (float)ptOldOffset.x() - m_ptCross.x()*m_imgSrc.width()*fZoom));
    m_ptOffset.setY(round(m_ptCross.y()*(float)m_imgSrc.height()*m_fCurZoom + (float)ptOldOffset.y() - m_ptCross.y()*m_imgSrc.height()*fZoom));
    if (m_emCurPlane != TransversePlane)
    {
        m_ptOffset.setY(round((1.0f-m_ptCross.y())*(float)m_imgSrc.height()*m_fCurZoom + (float)ptOldOffset.y() - (1.0f - m_ptCross.y())*m_imgSrc.height()*fZoom));
    }
}

/******************************************************** 
*  @function : resetOffset
*  @brief    : 更新偏移量
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void ImageWidget::resetOffset(float fZoom)
{
    m_ptOffset = QPoint((this->width() - fZoom * m_imgSrc.width()) / 2, (this->height() - fZoom * m_imgSrc.height()) / 2);
}

/******************************************************** 
*  @function : saveCapture
*  @brief    : 保存截图
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
bool ImageWidget::saveCapture(QString strFileName)
{
    if (m_imgLastShow.isNull())
    {
        return false;
    }
    return m_imgLastShow.save(strFileName);
}

/******************************************************** 
*  @function : FromWidget2Image
*  @brief    : 坐标映射，暂时没用到
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
QPointF ImageWidget::FromWidget2Image(QPoint pt)
{
    QPoint ptTemp = pt - m_ptOffset;//此为该点在图片中的坐标
    ptTemp.setX((float)ptTemp.x() / m_imgProc.width());
    ptTemp.setY((float)ptTemp.y() / m_imgProc.height());

    QPointF ptImg((float)ptTemp.x() / m_imgProc.width(), (float)ptTemp.y() / m_imgProc.height());
    return ptImg;
}

/******************************************************** 
*  @function : FromImage2Widget
*  @brief    : 坐标映射，暂时没用到
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
QPoint ImageWidget::FromImage2Widget(QPointF pt)
{
    QPoint ptWidget;
    return ptWidget;
}

/******************************************************** 
*  @function :  paintEvent
*  @brief    :  brief 
*  @input    : 
*  @output   : 
*  @return   : 
*********************************************************/
void ImageWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    //painter.setBrush(QBrush(WIDGET_BK_COLOR, Qt::SolidPattern));//设置画刷形式,背景色 
    //painter.setPen(WIDGET_BORDER_COLOR);  //vs蓝
    //painter.drawRect(0, 0, this->width() - 1, this->height() - 1);
    if (!m_imgSrc.isNull())
    {
        renderProcess();
        QRect rectCut(0 - m_ptOffset.x(), 0 - m_ptOffset.y(), this->width(), this->height());
        m_imgLastShow = m_imgProc.copy(rectCut);
        if (!m_imgProc.isNull())
        {
            //painter.drawImage(m_ptOffset, m_imgProc);
            painter.drawImage(0,0,m_imgLastShow);
        }
        drawParams(painter);
        drawScale(painter);
    }
    painter.setPen(WIDGET_BORDER_COLOR);  //vs蓝
    painter.drawLine(0, 0, this->width() - 1, 0);
    painter.drawLine(0, 0, 0, this->height() - 1);
    painter.drawLine(this->width() - 1, this->height() - 1, 0, this->height() - 1);
    painter.drawLine(this->width() - 1, this->height() - 1, this->width() - 1, 0);

}

/******************************************************** 
*  @function : renderProcess
*  @brief    : 渲染
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void ImageWidget::renderProcess()
{
    m_stDrawPen.DrawGraphics();
    //显示
    //QPainter pter(&m_imgProc);
}

/******************************************************** 
*  @function : drawParams
*  @brief    : 绘制一些参数值到图片上
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void ImageWidget::drawParams(QPainter &p)
{
    int nCurDrawHeight = 8;
    int nMaxWidth = 0;
    QPen pen(Qt::white);
    p.setPen(pen);
    QString strText;
    //设置字体
    QFont font;
    font.setFamily("Microsoft YaHei");
    font.setPointSize(8);
    QFontMetrics fm(font);
    //字符串所占的像素宽度,高度

    int nMargin = 16;
    int nHeightStep = fm.height();
    QPoint ptText(nMargin, nCurDrawHeight);
    /*
    //Spacing
    strText = QString::fromLocal8Bit("Spacing:") + QString::number(m_stImageHeaderInfo.fPixelSpacingW);
    nCurDrawHeight += nHeightStep;
    ptText.setY(nCurDrawHeight);
    p.drawText(ptText, strText);
    nMaxWidth = (fm.width(strText) > nMaxWidth) ? fm.width(strText) : nMaxWidth;

    //winWidth
    strText = QString::fromLocal8Bit("WW:") + QString::number(m_stImageHeaderInfo.nWinWidth);
    nCurDrawHeight += nHeightStep;
    ptText.setY(nCurDrawHeight);
    p.drawText(ptText, strText);
    nMaxWidth = (fm.width(strText) > nMaxWidth) ? fm.width(strText) : nMaxWidth;

    //winCenter
    strText = QString::fromLocal8Bit("WC:") + QString::number(m_stImageHeaderInfo.nWinCenter);
    nCurDrawHeight += nHeightStep;
    ptText.setY(nCurDrawHeight);
    p.drawText(ptText, strText);
    nMaxWidth = (fm.width(strText) > nMaxWidth) ? fm.width(strText) : nMaxWidth;
*/
    //index
    int nCurIndex = 0;
    int nTotalFrame = 0;
    int nWidth = 0;
    int nHeight = 0;
    switch (m_emCurPlane)
    {
    case SagittalPlane:
        nCurIndex = (m_ptCurPos.x*m_stImageHeaderInfo.nWidth);
        nTotalFrame = m_stImageHeaderInfo.nWidth;
        nWidth = m_stImageHeaderInfo.nHeight;
        nHeight = (double)m_stImageHeaderInfo.nThickNess/m_stImageHeaderInfo.fPixelSpacingW;
        break;
    case CoronalPlane:
        nCurIndex = (m_ptCurPos.y*m_stImageHeaderInfo.nHeight);
        nTotalFrame = m_stImageHeaderInfo.nHeight;
        nWidth = m_stImageHeaderInfo.nWidth;
        nHeight = m_stImageHeaderInfo.nThickNess / m_stImageHeaderInfo.fPixelSpacingW;
        break;
    case TransversePlane:
        nCurIndex = (m_ptCurPos.z*m_stImageHeaderInfo.nThickNess);
        nTotalFrame = m_stImageHeaderInfo.nThickNess;
        nWidth = m_stImageHeaderInfo.nWidth;
        nHeight = m_stImageHeaderInfo.nHeight;
        break;
    default:
        break;
    }
    strText =  QString::number(nCurIndex)+ QString::fromLocal8Bit("/") + QString::number(nTotalFrame-1);
    nCurDrawHeight += nHeightStep;
    ptText.setY(nCurDrawHeight);
    p.drawText(ptText, strText);
    nMaxWidth = (fm.width(strText) > nMaxWidth) ? fm.width(strText) : nMaxWidth;

    //tgt
    if (m_sizeFSelectPolygon.width() != 0
        && m_sizeFSelectPolygon.height() != 0
        && m_fAreaSquare!=0.f)
    {
        QSizeF sizeF(m_sizeFSelectPolygon.width()*(float)nWidth * m_stImageHeaderInfo.fPixelSpacingW,
            m_sizeFSelectPolygon.height()*(float)nHeight * m_stImageHeaderInfo.fPixelSpacingH);
        strText = QString::fromLocal8Bit("Width：") + QString::number(sizeF.width()) + QString::fromLocal8Bit("mm");
        nCurDrawHeight += nHeightStep;
        ptText.setY(nCurDrawHeight);
        p.drawText(ptText, strText);
        nMaxWidth = (fm.width(strText) > nMaxWidth) ? fm.width(strText) : nMaxWidth;

        strText = QString::fromLocal8Bit("Height：") + QString::number(sizeF.height()) + QString::fromLocal8Bit("mm");
        nCurDrawHeight += nHeightStep;
        ptText.setY(nCurDrawHeight);
        p.drawText(ptText, strText);
        nMaxWidth = (fm.width(strText) > nMaxWidth) ? fm.width(strText) : nMaxWidth;

        strText = QString::fromLocal8Bit("Square：") + 
            QString::number(m_fAreaSquare*(float)nWidth*(float)nHeight* m_stImageHeaderInfo.fPixelSpacingW* m_stImageHeaderInfo.fPixelSpacingW)
            + QString::fromLocal8Bit("mm*mm");
        nCurDrawHeight += nHeightStep;
        ptText.setY(nCurDrawHeight);
        p.drawText(ptText, strText);
        nMaxWidth = (fm.width(strText) > nMaxWidth) ? fm.width(strText) : nMaxWidth;
    }

    p.setPen(QPen(QColor(0, 0, 0, 50)));
    p.setBrush(QBrush(QColor(0, 0, 0, 50)));
    p.drawRect(0, 0, nMargin+ nMaxWidth+ nMargin, nCurDrawHeight+ nMargin);

}

/******************************************************** 
*  @function : drawScale
*  @brief    : 绘制比例尺
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void ImageWidget::drawScale(QPainter &p)
{
    QPen pen(Qt::white);
    p.setPen(pen);
    int nScaleLeftStart = 16;
    int nStepHeight = this->height() - 23;
    int nStratEndHeight = this->height() - 28;
    int nLineHeight = this->height() - 20;
    //计算各个比例尺绘制完后的像素值
    int nPixelWidth = 0;
    for (int i = 0 ;i<g_vecScale.size(); i++)
    {
        nPixelWidth = (float)g_vecScale[i] / m_stImageHeaderInfo.fPixelSpacingW*m_fCurZoom;
        if (nPixelWidth > this->width() / 10
            || nPixelWidth>= 40)
        {
            m_nCurScaleIndex = i;
            break;
        }
    }

    if (nPixelWidth>(this->width()*1/3)&&m_nCurScaleIndex>0)
    {
        m_nCurScaleIndex -= 1;
        nPixelWidth = (float)g_vecScale[m_nCurScaleIndex] / m_stImageHeaderInfo.fPixelSpacingW*m_fCurZoom;
    }
    QString strText;
    if (m_nCurScaleIndex == 0)
    {
        strText = QString::fromLocal8Bit("1mm");
    }
    if (m_nCurScaleIndex == 1)
    {
        strText = QString::fromLocal8Bit("1cm");
    }
    if (m_nCurScaleIndex == 2)
    {
        strText = QString::fromLocal8Bit("10cm");
    }
    if (m_nCurScaleIndex == 3)
    {
        strText = QString::fromLocal8Bit("1m");
    }
    if (nPixelWidth % 10 == 0)
    {
        for (int i = 1; i < 10; i++)
        {
            int nDistX = nScaleLeftStart + i * nPixelWidth / 10;
            p.drawLine(nDistX, nStepHeight, nDistX, nLineHeight);
        }
    }
    else if (nPixelWidth%5 == 0)
    {
        for (int i = 1; i < 5; i++)
        {
            int nDistX = nScaleLeftStart + i * nPixelWidth / 5;
            p.drawLine(nDistX, nStepHeight, nDistX, nLineHeight);
        }
    }
    else if (nPixelWidth % 2 == 0)
    {
        int nDistX = nScaleLeftStart + nPixelWidth / 2;
        p.drawLine(nDistX, nStepHeight, nDistX, nLineHeight);
    }
    else 
    {

    }

    p.drawText(nScaleLeftStart, nStratEndHeight, strText);
    p.drawLine(nScaleLeftStart, nLineHeight, nScaleLeftStart + nPixelWidth, nLineHeight);//横线
    p.drawLine(nScaleLeftStart, nStratEndHeight, nScaleLeftStart, nLineHeight);//起点
    p.drawLine(nScaleLeftStart+ nPixelWidth, nStratEndHeight, nScaleLeftStart + nPixelWidth, nLineHeight);//终点


}

/******************************************************** 
*  @function :  resizeEvent
*  @brief    :  brief 
*  @input    : 
*  @output   : 
*  @return   : 
*********************************************************/
void ImageWidget::resizeEvent(QResizeEvent *event)
{
    QSize deltaSize = (event->size() - event->oldSize())/2;
    //m_ptOffset = QPoint((this->width() - m_fCurZoom * m_imgSrc.width()) / 2, (this->height() - m_fCurZoom * m_imgSrc.height()) / 2);
    m_ptOffset.setX(m_ptOffset.x() + deltaSize.width());
    m_ptOffset.setY(m_ptOffset.y() + deltaSize.height());

    update();
}

/******************************************************** 
*  @function :  mousePressEvent
*  @brief    :  brief 
*  @input    : 
*  @output   : 
*  @return   : 
*********************************************************/
void ImageWidget::mousePressEvent(QMouseEvent *event)
{
    if (KEY_DOWN(VK_CONTROL)
        || (event->buttons() & Qt::MidButton) == Qt::MidButton)
    {
        m_bIsMoving = true;
        m_ptOldMousePos = event->pos();
        this->setCursor(Qt::ClosedHandCursor);      //范围之外变回原来形状
    }

    if (!m_imgProc.isNull() && !KEY_DOWN(VK_CONTROL))
    {

        QPoint ptTmp = event->pos();
        QPointF pt = event->pos() - m_ptOffset;
        if (pt.rx()>m_imgProc.width())
        {
            pt.setX(m_imgProc.width());
        }
        if (pt.rx() < 0)
        {
            pt.setX(0.f);
        }
        if (pt.ry() > m_imgProc.height())
        {
            pt.setY(m_imgProc.height());
        }
        if (pt.ry() < 0)
        {
            pt.setY(0.f);
        }
        
        m_ptCross = QPointF(pt.rx()/ m_imgProc.width(), pt.ry() / m_imgProc.height());
        if (m_emCurPlane != TransversePlane)
        {
            m_ptCross.setY(1.0f - m_ptCross.ry());
        }
        cv::Point3i ptCurPos;
        ptCurPos.x = round(m_ptCurPos.x*(m_stImageHeaderInfo.nWidth));
        ptCurPos.y = round(m_ptCurPos.y*(m_stImageHeaderInfo.nHeight));
        ptCurPos.z = round(m_ptCurPos.z*(m_stImageHeaderInfo.nThickNess));

        switch (m_emCurPlane)
        {
        case SagittalPlane:
        {
            ptCurPos.y = round(m_ptCross.rx()*(m_stImageHeaderInfo.nHeight));
            ptCurPos.z = round(m_ptCross.ry()*(m_stImageHeaderInfo.nThickNess));
            //m_nCurValue = m_pItkImg->GetPixel({ ptCurPos.y,ptCurPos.z });
        }
            break;
        case CoronalPlane:
        {
            ptCurPos.x = round(m_ptCross.rx()*(m_stImageHeaderInfo.nWidth));
            ptCurPos.z = round(m_ptCross.ry()*(m_stImageHeaderInfo.nThickNess));
            //m_nCurValue = m_pItkImg->GetPixel({ ptCurPos.x,ptCurPos.z });
        }
            break;
        case TransversePlane:
        {
            ptCurPos.x = round(m_ptCross.rx()*(m_stImageHeaderInfo.nWidth));
            ptCurPos.y = round(m_ptCross.ry()*(m_stImageHeaderInfo.nHeight));
            //m_nCurValue = m_pItkImg->GetPixel({ ptCurPos.x,ptCurPos.y });
        }
            break;
        default:
            break;
        }

        ptCurPos.x = (ptCurPos.x == m_stImageHeaderInfo.nWidth) ? m_stImageHeaderInfo.nWidth - 1 : ptCurPos.x;
        ptCurPos.y = (ptCurPos.y == m_stImageHeaderInfo.nHeight) ? m_stImageHeaderInfo.nHeight - 1 : ptCurPos.y;
        ptCurPos.z = (ptCurPos.z == m_stImageHeaderInfo.nThickNess) ? m_stImageHeaderInfo.nThickNess - 1 : ptCurPos.z;
        m_nCurValue = ImageDataManager::getInstance()->getPixelValue(ptCurPos.x, ptCurPos.y, ptCurPos.z);
        setCurCenterPos(ptCurPos);

        QPointF ptCrossF(m_ptCross);
        /*if (m_emCurPlane!=TransversePlane)
        {
            ptCrossF = QPointF(m_ptCross.rx(), 1.0f - m_ptCross.ry());
        }*/
        m_stDrawPen.mousePressEvent(event, ptCrossF, m_nCurValue);

        //emit sigCurPosAndZoom(m_ptCurPos.x, m_ptCurPos.y, m_ptCurPos.z, m_fCurZoom, m_nCurValue);
    }
    update();
}

/******************************************************** 
*  @function :  
*  @brief    :  brief 
*  @input    : 
*  @output   : 
*  @return   : 
*********************************************************/
void ImageWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_pItkImg != nullptr)
    {
        if ((m_bIsMoving&&KEY_DOWN(VK_CONTROL))
            ||(event->buttons() & Qt::MidButton) == Qt::MidButton)
        {
            QPoint ptMove = event->pos() - m_ptOldMousePos;
            m_ptOldMousePos = event->pos();
            m_ptOffset += ptMove;
            update();
        }
        QPointF ptF = event->pos() - m_ptOffset;
        //QPoint pt = event->pos() - m_ptOffset;
        if (ptF.rx() > m_imgProc.width())
        {
            ptF.setX(m_imgProc.width());
        }
        if (ptF.rx() < 0)
        {
            ptF.setX(0.f);
        }
        if (ptF.ry() > m_imgProc.height())
        {
            ptF.setY(m_imgProc.height());
        }
        if (ptF.ry() < 0)
        {
            ptF.setY(0.f);
        }
        int nValue = -1;
        ptF = QPointF(ptF.rx() / m_imgProc.width(), ptF.ry() / m_imgProc.height());
        if (m_emCurPlane != TransversePlane)
        {
            ptF.setY(1.0f - ptF.y());
        }
        try
        {
            cv::Point3i ptCurPos;
            ptCurPos.x = round(m_ptCurPos.x*(m_stImageHeaderInfo.nWidth));
            ptCurPos.y = round(m_ptCurPos.y*(m_stImageHeaderInfo.nHeight));
            ptCurPos.z = round(m_ptCurPos.z*(m_stImageHeaderInfo.nThickNess));


            switch (m_emCurPlane)
            {
            case SagittalPlane:
            {
                ptCurPos.y = round(ptF.rx()*(m_stImageHeaderInfo.nHeight));
                ptCurPos.z = round(ptF.ry()*(m_stImageHeaderInfo.nThickNess));
                ptCurPos.x = ptCurPos.x >= m_stImageHeaderInfo.nWidth ? m_stImageHeaderInfo.nWidth - 1 : ptCurPos.x;
                ptCurPos.y = ptCurPos.y >= m_stImageHeaderInfo.nHeight ? m_stImageHeaderInfo.nHeight - 1 : ptCurPos.y;
                ptCurPos.z = ptCurPos.z >= m_stImageHeaderInfo.nThickNess ? m_stImageHeaderInfo.nThickNess - 1 : ptCurPos.z;
                //nValue = m_pItkImg->GetPixel({ ptCurPos.y,ptCurPos.z });
                nValue = ImageDataManager::getInstance()->getPixelValue(ptCurPos.x, ptCurPos.y, ptCurPos.z);
            }
            break;
            case CoronalPlane:
            {
                ptCurPos.x = round(ptF.rx()*(m_stImageHeaderInfo.nWidth));
                ptCurPos.z = round(ptF.ry()*(m_stImageHeaderInfo.nThickNess));
                ptCurPos.x = ptCurPos.x >= m_stImageHeaderInfo.nWidth ? m_stImageHeaderInfo.nWidth - 1 : ptCurPos.x;
                ptCurPos.y = ptCurPos.y >= m_stImageHeaderInfo.nHeight ? m_stImageHeaderInfo.nHeight - 1 : ptCurPos.y;
                ptCurPos.z = ptCurPos.z >= m_stImageHeaderInfo.nThickNess ? m_stImageHeaderInfo.nThickNess - 1 : ptCurPos.z;
                //nValue = m_pItkImg->GetPixel({ ptCurPos.x,ptCurPos.z });
                nValue = ImageDataManager::getInstance()->getPixelValue(ptCurPos.x, ptCurPos.y, ptCurPos.z);
            }
            break;
            case TransversePlane:
            {
                ptCurPos.x = round(ptF.rx()*(m_stImageHeaderInfo.nWidth));
                ptCurPos.y = round(ptF.ry()*(m_stImageHeaderInfo.nHeight));
                ptCurPos.x = ptCurPos.x >= m_stImageHeaderInfo.nWidth ? m_stImageHeaderInfo.nWidth - 1 : ptCurPos.x;
                ptCurPos.y = ptCurPos.y >= m_stImageHeaderInfo.nHeight ? m_stImageHeaderInfo.nHeight - 1 : ptCurPos.y;
                ptCurPos.z = ptCurPos.z >= m_stImageHeaderInfo.nThickNess ? m_stImageHeaderInfo.nThickNess - 1 : ptCurPos.z;
                //nValue = m_pItkImg->GetPixel({ ptCurPos.x,ptCurPos.y });
                nValue = ImageDataManager::getInstance()->getPixelValue(ptCurPos.x, ptCurPos.y, ptCurPos.z);
            }
            break;
            default:
                break;
            }

        }
        catch(...)
        {
            return;
        }
        if (!(KEY_DOWN(VK_CONTROL))&&
            (m_stDrawPen.isDrawing()
            || m_stDrawPen.isMoving()))
        {
            m_stDrawPen.mouseMoveEvent(event, ptF, nValue);
            update();
        }
    }
    

    return;
}

/******************************************************** 
*  @function :  mousePressEvent
*  @brief    :  brief 
*  @input    : 
*  @output   : 
*  @return   : 
*********************************************************/
void ImageWidget::mouseReleaseEvent(QMouseEvent *event)
{
    m_bIsMoving = false;
    this->setCursor(Qt::ArrowCursor);      //范围之外变回原来形状
}

/******************************************************** 
*  @function :  wheelEvent
*  @brief    :  brief 
*  @input    : 
*  @output   : 
*  @return   : 
*********************************************************/
void ImageWidget::wheelEvent(QWheelEvent *event)
{
    int numDegrees = event->delta() / 8;//滚动的角度，*8就是鼠标滚动的距离
    int numSteps = numDegrees / 15;//滚动的步数，*15就是鼠标滚动的角度
    if (numSteps == 0 || m_pItkImg == nullptr)
    {
        return;
    }
    cv::Point3i ptCurPos;
    ptCurPos.x = round(m_ptCurPos.x*(m_stImageHeaderInfo.nWidth));
    ptCurPos.y = round(m_ptCurPos.y*(m_stImageHeaderInfo.nHeight));
    ptCurPos.z = round(m_ptCurPos.z*(m_stImageHeaderInfo.nThickNess));
    //control按下，说明要放大缩小
    if (KEY_DOWN(VK_CONTROL))
    {
        float fZoom = 1.0;
        if (numSteps > 0)
        {
            fZoom = m_fCurZoom * 1.1;
            if (fZoom > 15.f)
            {
                fZoom = MAX_ZOOM;
            }
            else
            {
                fZoom = fZoom;
            }
        }
        else
        {
            fZoom = m_fCurZoom / 1.1;
            if (fZoom < 0.1f)
            {
                fZoom = MIN_ZOOM;
            }
            else
            {
                fZoom = fZoom;
            }
        }

        cv::Point3i ptMousePos = m_ptCurPos;
        //QPointF ptCursor = event->posF();
        QPoint ptCursor = event->pos() - m_ptOffset;

        //防边界超出
        {
            if (ptCursor.rx() > m_imgProc.width())
            {
                ptCursor.setX(m_imgProc.width());
            }
            if (ptCursor.rx() < 0)
            {
                ptCursor.setX(0.f);
            }
            if (ptCursor.ry() > m_imgProc.height())
            {
                ptCursor.setY(m_imgProc.height());
            }
            if (ptCursor.ry() < 0)
            {
                ptCursor.setY(0.f);
            }
        }
        
        //根据不同的视图设置当前鼠标所在位置的真实值
        switch (m_emCurPlane)
        {
        case SagittalPlane:
        {
            ptMousePos.y = ptCursor.x();
            ptMousePos.z = ptCursor.y();
        }
        break;
        case CoronalPlane:
        {
            ptMousePos.x = ptCursor.x();
            ptMousePos.z = ptCursor.y();
        }
        break;
        case TransversePlane:
        {
            ptMousePos.x = ptCursor.x();
            ptMousePos.y = ptCursor.y();
        }
        break;
        default:
            break;
        }

        emit sigCurZoom(fZoom);
        update();
    }
    else
    {
        int nTmpIndex = -1;
        //上下滑动更换图片
        switch (m_emCurPlane)
        {
        case SagittalPlane:
        {
            nTmpIndex = ptCurPos.x;
        }
        break;
        case CoronalPlane:
        {
            nTmpIndex = ptCurPos.y;
        }
        break;
        case TransversePlane:
        {
            nTmpIndex = ptCurPos.z;
        }
        break;
        default:
            break;
        }
        nTmpIndex = numSteps < 0 ? (nTmpIndex + 1) : (nTmpIndex - 1);
        if (loadImageFromManager(nTmpIndex))
        {
            switch (m_emCurPlane)
            {
            case SagittalPlane:
            {
                ptCurPos.x = nTmpIndex;
            }
            break;
            case CoronalPlane:
            {
                ptCurPos.y = nTmpIndex;
            }
            break;
            case TransversePlane:
            {
                ptCurPos.z = nTmpIndex;
            }
            break;
            default:
                break;
            }
        }
        setCurCenterPos(ptCurPos);
        update();
    }

}

/******************************************************** 
*  @function : OnLoadFinish
*  @brief    : 加载结束响应
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void ImageWidget::OnLoadFinish(bool bSuccess)
{
    if (bSuccess)
    {
        ImageDataManager* pImageDataManager = ImageDataManager::getInstance();
        m_stImageHeaderInfo = pImageDataManager->getImageHeaderInfo();
        int nIndex = 0;

        //加载的时候十字光标位置和鼠标位置全部为中心点
        m_ptCurPos.x = 0.5;
        m_ptCurPos.y = 0.5;
        m_ptCurPos.z = 0.5;

        //m_ptMousePos = m_ptCurPos;

        switch (m_emCurPlane)
        {
        case SagittalPlane:
        {
            nIndex = m_ptCurPos.x*(m_stImageHeaderInfo.nWidth - 1);
            if (!loadImageFromManager(nIndex))
            {
                return;
            }
            m_nCurValue = m_pItkImg->GetPixel({ (int)(m_ptCurPos.y*m_stImageHeaderInfo.nHeight),(int)(m_ptCurPos.z*m_stImageHeaderInfo.nThickNess) });
        }
        break;
        case CoronalPlane:
        {
            nIndex = m_ptCurPos.y*(m_stImageHeaderInfo.nHeight - 1);
            if (!loadImageFromManager(nIndex))
            {
                return;
            }
            m_nCurValue = m_pItkImg->GetPixel({ (int)(m_ptCurPos.x*m_stImageHeaderInfo.nWidth),(int)(m_ptCurPos.z*m_stImageHeaderInfo.nThickNess) });
        }
        break;
        case TransversePlane:
        {
            nIndex = m_ptCurPos.z*(m_stImageHeaderInfo.nThickNess - 1);
            if (!loadImageFromManager(nIndex))
            {
                return;
            }
            m_nCurValue = m_pItkImg->GetPixel({ (int)(m_ptCurPos.x*m_stImageHeaderInfo.nWidth),(int)(m_ptCurPos.y*m_stImageHeaderInfo.nHeight) });
        }
        break;
        default:
            break;
        }

        //设置好偏移量
        m_nCurValue = ImageDataManager::getInstance()->getPixelValue((int)(m_ptCurPos.x*m_stImageHeaderInfo.nWidth), (int)(m_ptCurPos.y*m_stImageHeaderInfo.nHeight), (int)(m_ptCurPos.z*m_stImageHeaderInfo.nThickNess));

        emit sigCurPosAndValue(m_ptCurPos.x, m_ptCurPos.y, m_ptCurPos.z, m_nCurValue);
        float f1 = (float)(this->width() - IMAGE_MARGIN * 2) / m_stImageHeaderInfo.nWidth;
        float f2 = (float)(this->height() - IMAGE_MARGIN * 2) / m_stImageHeaderInfo.nHeight;
        float f3 = (float)(this->height() - IMAGE_MARGIN * 2) / m_stImageHeaderInfo.nThickNess;
        m_fCurZoom = f1 < f2 ? f1 : f2;
        m_fCurZoom = m_fCurZoom < f3 ? m_fCurZoom : f3;
        m_ptOffset = QPoint((this->width()- m_fCurZoom *m_imgSrc.width())/2, (this->height() - m_fCurZoom * m_imgSrc.height()) / 2);
        //emit sigCursorAndZoom(m_ptMousePos.x*m_stImageHeaderInfo.nWidth, m_ptMousePos.y*m_stImageHeaderInfo.nHeight, m_ptMousePos.z*m_stImageHeaderInfo.nThickNess, m_fCurZoom);
        emit sigCurZoom(m_fCurZoom);
        update();
    }
}

/******************************************************** 
*  @function : OnSelectPolygonChanged
*  @brief    : 切换了选中的多边形，更新大小和面积
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void ImageWidget::OnSelectPolygonChanged(QSizeF size, float fAreaSquare)
{
    m_sizeFSelectPolygon = size;
    m_fAreaSquare = fAreaSquare;
}
