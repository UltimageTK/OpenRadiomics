/******************************************************** 
* @file    : DrawPen.h
* @brief   : 
* @details : 
* @author  : qh.zhang@atisz.ac.cn
* @date    : 2019-5-8
*********************************************************/
#pragma once

#include <QWidget>
#include "DrawBase.h"
#include "LabelAnalysis.h"

//差之方式
enum INTERP_METHOD
{
    IM_None,//不插值
    IM_Neighbor_Interp,//邻近插值
    IM_Bilinear_Interp, //双线性插值
    IM_Bicubic_Interp,//双三次插值
};

/******************************************************** 
*  @class   :  DrawPen
*  @brief   :  画笔类
*  @details : 
*********************************************************/
class DrawPen : public DrawBase
{
    Q_OBJECT

public:
    DrawPen(QWidget *parent);
    ~DrawPen();

public:
    //初始化
    bool initDraw(VIEW_PLANE  emPlane);
    //设置传入的图片指针
    void setImgPointer(QImage *pImgSrc, QImage *pImgDrawing);
    //设置十字光标位置
    void setCross(QPointF ptCross);
    //设置缩放值
    void setZoom(float fZoom);
    //绘制整个画面
    void DrawGraphics();
    //是否在绘制图像
    bool isDrawing() { return m_bIsDrawing; };
    //是否在移动
    bool isMoving() { return m_bIsMoving; };
    //重新加载图片
    void reload(int nIndex);
    //删除选中目标
    void delTarget(int nIndex);
    //删除当前页面的目标
    void clearTargets();
    static QPair<VIEW_PLANE, int> getCurTarget(){return m_pairSelectTarget;};

    virtual void mousePressEvent(QMouseEvent *event, QPointF ptF, int nValue);
    virtual void mouseMoveEvent(QMouseEvent *event, QPointF ptF, int nValue);
    virtual void mouseReleaseEvent(QMouseEvent *event, QPointF ptF, int nValue);
    virtual void resizeEvent(QResizeEvent *event);
    virtual void wheelEvent(QWheelEvent *event);
protected:
    //绘点
    void drawPoint(QPainter *p, QPointF pt);
    //绘线
    void drawLine(QPainter *p, QPointF pt1,QPointF pt2);
    //绘制透视出来的图层
    void drawLayer(QPainter *p, QPointF pt1, QPointF pt2);
    //绘制多边形
    void drawPolygon(QPainter *p, ExPolygonF poly,bool bDrawRect = false);
    //确定选择的多边形
    void checkSelect(QPointF ptF);
    //生成插值点
    void generateInterpolationPoints(ExPolygonF &poly, INTERP_METHOD emInterpMethod = IM_None);
    //计算多边形面积
    float calcPolygonArea(QPolygonF poly);
signals:
    void sigSelectPolygon(QSizeF size,float fAreaSquare = 0.0f);

private:
    int   m_nCurFrameIndex = 0; //当前画面的帧号
    std::map<int, ExPolygonF> m_mapTargets;//在当前帧的ID和目标结构
    VIEW_PLANE  m_emPlane;  //当前视图类型
    std::pair<int,ExPolygonF> m_pairCurTarget;  //当前选中的目标
    QImage *m_pImgDrawing = nullptr;    //正在绘制的图像
    QImage *m_pImgSrc = nullptr;        //原图，不做任何处理
    QPointF m_ptCross = QPointF(0.5f,0.5f); //光标位置
    float   m_fZoom =1.0;   //缩放值
    int     m_nCurShapeId = 0;  //每一帧的id起始为0
    QPointF m_ptfMoveLastPos;   //拖动的最后一个位置
    QWidget *m_pParent = nullptr;//父窗口，ImageWidget

    static INTERP_METHOD m_emInterpMethod;  //插值方式，暂时没有用到，因为没有加入更多的插值方法
    static QPair<VIEW_PLANE, int> m_pairSelectTarget;   //当前被选中的目标
};
