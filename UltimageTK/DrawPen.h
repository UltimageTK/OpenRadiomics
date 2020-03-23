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

//��֮��ʽ
enum INTERP_METHOD
{
    IM_None,//����ֵ
    IM_Neighbor_Interp,//�ڽ���ֵ
    IM_Bilinear_Interp, //˫���Բ�ֵ
    IM_Bicubic_Interp,//˫���β�ֵ
};

/******************************************************** 
*  @class   :  DrawPen
*  @brief   :  ������
*  @details : 
*********************************************************/
class DrawPen : public DrawBase
{
    Q_OBJECT

public:
    DrawPen(QWidget *parent);
    ~DrawPen();

public:
    //��ʼ��
    bool initDraw(VIEW_PLANE  emPlane);
    //���ô����ͼƬָ��
    void setImgPointer(QImage *pImgSrc, QImage *pImgDrawing);
    //����ʮ�ֹ��λ��
    void setCross(QPointF ptCross);
    //��������ֵ
    void setZoom(float fZoom);
    //������������
    void DrawGraphics();
    //�Ƿ��ڻ���ͼ��
    bool isDrawing() { return m_bIsDrawing; };
    //�Ƿ����ƶ�
    bool isMoving() { return m_bIsMoving; };
    //���¼���ͼƬ
    void reload(int nIndex);
    //ɾ��ѡ��Ŀ��
    void delTarget(int nIndex);
    //ɾ����ǰҳ���Ŀ��
    void clearTargets();
    static QPair<VIEW_PLANE, int> getCurTarget(){return m_pairSelectTarget;};

    virtual void mousePressEvent(QMouseEvent *event, QPointF ptF, int nValue);
    virtual void mouseMoveEvent(QMouseEvent *event, QPointF ptF, int nValue);
    virtual void mouseReleaseEvent(QMouseEvent *event, QPointF ptF, int nValue);
    virtual void resizeEvent(QResizeEvent *event);
    virtual void wheelEvent(QWheelEvent *event);
protected:
    //���
    void drawPoint(QPainter *p, QPointF pt);
    //����
    void drawLine(QPainter *p, QPointF pt1,QPointF pt2);
    //����͸�ӳ�����ͼ��
    void drawLayer(QPainter *p, QPointF pt1, QPointF pt2);
    //���ƶ����
    void drawPolygon(QPainter *p, ExPolygonF poly,bool bDrawRect = false);
    //ȷ��ѡ��Ķ����
    void checkSelect(QPointF ptF);
    //���ɲ�ֵ��
    void generateInterpolationPoints(ExPolygonF &poly, INTERP_METHOD emInterpMethod = IM_None);
    //�����������
    float calcPolygonArea(QPolygonF poly);
signals:
    void sigSelectPolygon(QSizeF size,float fAreaSquare = 0.0f);

private:
    int   m_nCurFrameIndex = 0; //��ǰ�����֡��
    std::map<int, ExPolygonF> m_mapTargets;//�ڵ�ǰ֡��ID��Ŀ��ṹ
    VIEW_PLANE  m_emPlane;  //��ǰ��ͼ����
    std::pair<int,ExPolygonF> m_pairCurTarget;  //��ǰѡ�е�Ŀ��
    QImage *m_pImgDrawing = nullptr;    //���ڻ��Ƶ�ͼ��
    QImage *m_pImgSrc = nullptr;        //ԭͼ�������κδ���
    QPointF m_ptCross = QPointF(0.5f,0.5f); //���λ��
    float   m_fZoom =1.0;   //����ֵ
    int     m_nCurShapeId = 0;  //ÿһ֡��id��ʼΪ0
    QPointF m_ptfMoveLastPos;   //�϶������һ��λ��
    QWidget *m_pParent = nullptr;//�����ڣ�ImageWidget

    static INTERP_METHOD m_emInterpMethod;  //��ֵ��ʽ����ʱû���õ�����Ϊû�м������Ĳ�ֵ����
    static QPair<VIEW_PLANE, int> m_pairSelectTarget;   //��ǰ��ѡ�е�Ŀ��
};
