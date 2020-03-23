/******************************************************** 
* @file    : DrawBase.h
* @brief   : 
* @details : 
* @author  : qh.zhang@atisz.ac.cn
* @date    : 2019-5-8
*********************************************************/
#pragma once
#include <QObject>
#include <QMouseEvent>
#include <QPolygon>
#include <QColor>
#include <QLineF>
#include "GlobalDef.h"

#define SELECT_LINE_COLOR       QColor(0,255,0)
#define DRWAING_POINT_COLOR     QColor(255,255,0)
#define COMMON_POINT_LINE_COLOR     QColor(255,255,0)

//画笔类型
enum PEN_TYPE
{
    PenNone,
    PenPoint,
    PenPolygon, //多边形
    PenDrawPen, //画笔
    MeasureRuler,
    MeasurePolygon,
    MeasureCircle,
    MeasureOval,
};

class ExLineF :public QLineF
{
public:
    std::string strTargetName;
};

class ExPolygonF :public QPolygonF
{
public:
    std::string strTargetName;
    std::string strTargetDisc;
    int         nTargetID;
    int         nTargetType = PenPolygon;
    bool bClosed = false;   //是否已封闭
    bool bIsSelected = false;   //是否被选中
    int  nSelectPoint = -1;     //当前被选中的点
    QVector<int> lstValue;  //存所有valve值
    void reset()
    {
        strTargetName = "";
        strTargetDisc = "";
        nTargetID = -1;
        bClosed = false;
        bIsSelected = false;
        nSelectPoint = -1;
        this->clear();
        lstValue.clear();
    }
};

class DrawBase : public QObject
{
    Q_OBJECT
public:
    DrawBase();
    ~DrawBase();

    //virtual bool initDraw();

    PEN_TYPE getCurPenType();
    static void setCurPenType(PEN_TYPE emPenType);
    static void setAlphaValue(int nAlphaValue);
    //virtual void mousePressEvent(QMouseEvent *event);
    //virtual void mouseMoveEvent(QMouseEvent *event);
    //virtual void mouseReleaseEvent(QMouseEvent *event);
    //virtual void resizeEvent(QResizeEvent *event);
protected:
    virtual void DrawGraphics() = 0;
protected:
    static PEN_TYPE    m_emPenType;    //画笔类型
    static   int      m_nAlphaValue; //透明度
    QColor   m_Color;      //画笔颜色
    int         m_nSize;        //画笔宽度
    ImageHeaderInfo     m_stImageHeaderInfo;    //图片序列头信息
    bool            m_bIsDrawing = false;
    bool            m_bIsMoving = false;
};