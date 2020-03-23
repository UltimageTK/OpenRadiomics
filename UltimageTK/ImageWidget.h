/******************************************************** 
* @file    : ImageWidget.h
* @brief   :
* @details :
* @author  : qh.zhang@atisz.ac.cn
* @date    : 2019-4-23
*********************************************************/
#pragma once

#include <QObject>
#include <QWidget>
#include <QImage>
#include "opencv2/opencv.hpp"
#include "GlobalDef.h"
#include "DrawPen.h"

#define MAX_ZOOM 14.4209936106499
#define MIN_ZOOM 0.1015255979947

/******************************************************** 
*  @class   :  ImageWidget
*  @brief   :  brief
*  @details : 
*********************************************************/
class ImageWidget : public QWidget
{
    Q_OBJECT

public:
    ImageWidget(QWidget *parent = nullptr);
    ~ImageWidget();


public:
    //依据切片索引加载
    bool loadImageFromManager(int nIndex);
    //重新加载
    void reload();
    //设置当前窗口是什么视图
    void setViewPlane(VIEW_PLANE emCurPlane) ;
    //设置当前中心位置（光标位置）
    void setCurCenterPos(cv::Point3i ptCurPos);
    //更新缩放比例
    void updateZoom(float fZoom);
    //更新十字光标坐标
    void updateCrossPos(QPointF ptCrossPos);
    //更新缩放比例和中心
    void updateZoomCenter(float fZoom);
    //重置图片偏移量
    void resetOffset(float fZoom);
    //删除目标
    void delTarget(int nIndex) { m_stDrawPen.delTarget(nIndex); };
    //清空当前页面目标
    void clearTargets() { m_stDrawPen.clearTargets(); };
    //保存截图
    bool saveCapture(QString strFileName);
protected:
    QPointF FromWidget2Image(QPoint pt);
    QPoint FromImage2Widget(QPointF pt);

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void resizeEvent(QResizeEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void wheelEvent(QWheelEvent *event);

    void renderProcess();
    void drawParams(QPainter &p);//绘制各参数
    void drawScale(QPainter &p);//绘制比例尺

signals:
    void sigCurPosAndValue(float nX, float nY, float nZ,int nValue);    //十字光标位置和当前点物理值
    void sigCursorAndZoom(int nX, int nY, int nZ, float fZoom);   //缩放鼠标位置和缩放值
    void sigCurZoom(float fZoom);   //缩放鼠标位置和缩放值

public slots:
    void OnLoadFinish(bool bSuccess);
    void OnSelectPolygonChanged(QSizeF size, float fAreaSquare);
private:
    SliceType::Pointer  m_pSlice = nullptr;        //切片操作句柄
    ImageType2D::Pointer    m_pItkImg = nullptr;   //当前图片切片的itk指针
    VIEW_PLANE          m_emCurPlane;   //当前视图是什么视图

    QImage m_imgSrc;        //原图，不做任何处理  
    //QImage m_imgProc;       //该widget内的唯一主图片,用它来处理，处理完了再show，当做缓冲
    QImage m_imgProc;       //该widget显示的大图片
    QImage m_imgLastShow;   //该widget显示的裁剪小图片
    static ImageHeaderInfo m_stImageHeaderInfo;    //图像的头信息
    int m_nCurFrameNum = 0;      //当前Index

    QPointF m_ptCross;                  //十字光标的位置
    QPoint  m_ptOffset;                 //图片与左上角偏移量

    bool    m_bIsMoving = false;
    QPoint  m_ptNewMousePos;
    QPoint  m_ptOldMousePos;

    DrawPen m_stDrawPen;
    QSizeF   m_sizeFSelectPolygon;
    float   m_fAreaSquare = 0.f;
    int     m_nCurScaleIndex = 0;//当前是那个比例尺

    //定位缩放体验不好...暂时不做,就在鼠标选择的点缩放吧
    static  cv::Point3f m_ptCurPos;       //当前十字光标的3D真实坐标，三个视图中这个量是一样的
    static  float   m_fCurZoom;         //当前缩放
    static  int     m_nCurValue;        //当前真实值
};
