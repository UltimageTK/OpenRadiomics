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
    //������Ƭ��������
    bool loadImageFromManager(int nIndex);
    //���¼���
    void reload();
    //���õ�ǰ������ʲô��ͼ
    void setViewPlane(VIEW_PLANE emCurPlane) ;
    //���õ�ǰ����λ�ã����λ�ã�
    void setCurCenterPos(cv::Point3i ptCurPos);
    //�������ű���
    void updateZoom(float fZoom);
    //����ʮ�ֹ������
    void updateCrossPos(QPointF ptCrossPos);
    //�������ű���������
    void updateZoomCenter(float fZoom);
    //����ͼƬƫ����
    void resetOffset(float fZoom);
    //ɾ��Ŀ��
    void delTarget(int nIndex) { m_stDrawPen.delTarget(nIndex); };
    //��յ�ǰҳ��Ŀ��
    void clearTargets() { m_stDrawPen.clearTargets(); };
    //�����ͼ
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
    void drawParams(QPainter &p);//���Ƹ�����
    void drawScale(QPainter &p);//���Ʊ�����

signals:
    void sigCurPosAndValue(float nX, float nY, float nZ,int nValue);    //ʮ�ֹ��λ�ú͵�ǰ������ֵ
    void sigCursorAndZoom(int nX, int nY, int nZ, float fZoom);   //�������λ�ú�����ֵ
    void sigCurZoom(float fZoom);   //�������λ�ú�����ֵ

public slots:
    void OnLoadFinish(bool bSuccess);
    void OnSelectPolygonChanged(QSizeF size, float fAreaSquare);
private:
    SliceType::Pointer  m_pSlice = nullptr;        //��Ƭ�������
    ImageType2D::Pointer    m_pItkImg = nullptr;   //��ǰͼƬ��Ƭ��itkָ��
    VIEW_PLANE          m_emCurPlane;   //��ǰ��ͼ��ʲô��ͼ

    QImage m_imgSrc;        //ԭͼ�������κδ���  
    //QImage m_imgProc;       //��widget�ڵ�Ψһ��ͼƬ,��������������������show����������
    QImage m_imgProc;       //��widget��ʾ�Ĵ�ͼƬ
    QImage m_imgLastShow;   //��widget��ʾ�Ĳü�СͼƬ
    static ImageHeaderInfo m_stImageHeaderInfo;    //ͼ���ͷ��Ϣ
    int m_nCurFrameNum = 0;      //��ǰIndex

    QPointF m_ptCross;                  //ʮ�ֹ���λ��
    QPoint  m_ptOffset;                 //ͼƬ�����Ͻ�ƫ����

    bool    m_bIsMoving = false;
    QPoint  m_ptNewMousePos;
    QPoint  m_ptOldMousePos;

    DrawPen m_stDrawPen;
    QSizeF   m_sizeFSelectPolygon;
    float   m_fAreaSquare = 0.f;
    int     m_nCurScaleIndex = 0;//��ǰ���Ǹ�������

    //��λ�������鲻��...��ʱ����,�������ѡ��ĵ����Ű�
    static  cv::Point3f m_ptCurPos;       //��ǰʮ�ֹ���3D��ʵ���꣬������ͼ���������һ����
    static  float   m_fCurZoom;         //��ǰ����
    static  int     m_nCurValue;        //��ǰ��ʵֵ
};
