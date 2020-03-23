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

//��������
enum PEN_TYPE
{
    PenNone,
    PenPoint,
    PenPolygon, //�����
    PenDrawPen, //����
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
    bool bClosed = false;   //�Ƿ��ѷ��
    bool bIsSelected = false;   //�Ƿ�ѡ��
    int  nSelectPoint = -1;     //��ǰ��ѡ�еĵ�
    QVector<int> lstValue;  //������valveֵ
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
    static PEN_TYPE    m_emPenType;    //��������
    static   int      m_nAlphaValue; //͸����
    QColor   m_Color;      //������ɫ
    int         m_nSize;        //���ʿ��
    ImageHeaderInfo     m_stImageHeaderInfo;    //ͼƬ����ͷ��Ϣ
    bool            m_bIsDrawing = false;
    bool            m_bIsMoving = false;
};