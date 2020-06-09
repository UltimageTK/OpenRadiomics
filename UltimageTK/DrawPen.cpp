/******************************************************** 
* @file    : DrawPen.cpp
* @brief   : 
* @details : 
* @author  : qh.zhang@atisz.ac.cn
* @date    : 2019-7-3
*********************************************************/
#include <QPainter>
#include <QDebug>
#include <QTime>
#include "DrawPen.h"
#include "ImageDataManager.h"
#include "AllSettings.h"
class ImageWidget;

#define POINT_RADIUS 2
#define LINE_WIDTH 2
#define POINT_LINE_WIDTH 1
#define POINT_SELECT_RADIUS 3
#define DRAWPEN_POINT_COUNTS_LIMIT 20

INTERP_METHOD DrawPen::m_emInterpMethod = IM_None;
QPair<VIEW_PLANE, int> DrawPen::m_pairSelectTarget = qMakePair(VIEW_PLANE::CoronalPlane,-1);

/******************************************************** 
*  @function : DrawPen
*  @brief    : 构造函数
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
DrawPen::DrawPen(QWidget *parent)
    :m_pParent(parent)
{

}

DrawPen::~DrawPen()
{
}

/******************************************************** 
*  @function : initDraw
*  @brief    : 初始化
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
bool DrawPen::initDraw(VIEW_PLANE emPlane)
{
    m_emPlane = emPlane;
    return true;
}

/******************************************************** 
*  @function : setImgPointer
*  @brief    : 设置传入的图片指针
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void DrawPen::setImgPointer(QImage *pImgSrc, QImage *pImgDrawing)
{
    m_pImgSrc = pImgSrc;
    m_pImgDrawing = pImgDrawing;
}

/********************************************************
*  @function : setCross
*  @brief    : 设置十字光标位置
*  @input    :
*  @output   :
*  @return   :
*********************************************************/
void DrawPen::setCross(QPointF ptCross)
{
    m_ptCross = ptCross;
    //DrawGraphics();
}

/********************************************************
*  @function : setZoom
*  @brief    : 设置缩放值
*  @input    :
*  @output   :
*  @return   :
*********************************************************/
void DrawPen::setZoom(float fZoom)
{
    m_fZoom = fZoom;
    //DrawGraphics();
}

/********************************************************
*  @function : reload
*  @brief    : 重新加载图片
*  @input    :
*  @output   :
*  @return   :
*********************************************************/
void DrawPen::reload(int nIndex)
{
    m_nCurFrameIndex = nIndex;
    std::map<int, LabelAnalysis::Target> mapTargetsTmp;
    ImageDataManager::getInstance()->getLabelInfo(m_emPlane, m_nCurFrameIndex, mapTargetsTmp);
    m_stImageHeaderInfo = ImageDataManager::getInstance()->getImageHeaderInfo();

    m_mapTargets.clear();
    for each (auto var in mapTargetsTmp)
    {
        ExPolygonF poly;
        for each (auto ptF in var.second.lstVertex)
        {
            poly.append(QPointF(ptF.fX,ptF.fY));
        }
        poly.bClosed = true;
        poly.strTargetName = var.second.strTargetName;
        m_mapTargets[var.first] = poly;
    }
    m_pairCurTarget.second.reset();
    m_bIsDrawing = false;
    m_bIsMoving = false;
    emit sigSelectPolygon(QSize(0, 0));

    //m_pairSelectTarget = qMakePair(VIEW_PLANE::CoronalPlane, -1);
    //DrawGraphics();
}

/******************************************************** 
*  @function : delTarget
*  @brief    : 删除选中目标
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void DrawPen::delTarget(int nIndex)
{
    if (m_mapTargets.find(nIndex)!=m_mapTargets.end())
    {
        m_mapTargets.erase(nIndex);
    }
    ImageDataManager::getInstance()->updateLabelInfo(m_emPlane, m_nCurFrameIndex, m_mapTargets);
    m_pairSelectTarget = qMakePair(VIEW_PLANE::CoronalPlane, -1);
    emit sigSelectPolygon(QSize(0, 0));
}

/********************************************************
*  @function : clearTargets
*  @brief    : 删除当前页面的目标
*  @input    :
*  @output   :
*  @return   :
*********************************************************/
void DrawPen::clearTargets()
{
    m_mapTargets.clear();
    m_pairSelectTarget = qMakePair(VIEW_PLANE::CoronalPlane, -1);
    emit sigSelectPolygon(QSize(0,0));
    ImageDataManager::getInstance()->updateLabelInfo(m_emPlane, m_nCurFrameIndex, m_mapTargets);
}

/********************************************************
*  @function : mousePressEvent
*  @brief    : 鼠标按下事件
*  @input    :
*  @output   :
*  @return   :
*********************************************************/
void DrawPen::mousePressEvent(QMouseEvent *event, QPointF ptF, int nValue)
{
    //如果没有在绘图，判断是否选择了点或者目标
    if ((event->buttons() & Qt::LeftButton) == Qt::LeftButton)
    {
        if (!m_bIsDrawing)
        {
            checkSelect(ptF);
        }
        //判断是否选中了某个目标
        if (m_pairSelectTarget.first == m_emPlane
            && m_pairSelectTarget.second != -1)
        {
            //选中点,进入点移动模式
            m_bIsMoving = true;
            m_ptfMoveLastPos = ptF;
            m_pParent->setCursor(Qt::ClosedHandCursor);      //范围之外变回原来形状
        }
        else
        {
            m_bIsMoving = false;
        }
    }

    m_ptCross = ptF;

    switch (m_emPenType)
    {
    case PenNone:
        //DrawGraphics();
        break;
    case PenPoint:
        break;
    case PenPolygon:
    case PenDrawPen:
        if ((event->buttons() & Qt::LeftButton) == Qt::LeftButton)
        {
            //判断是否选中了某个目标
            if (m_bIsMoving)
            {
                //do nothing
            }
            else
            {
                while (1)
                {
                    if (m_pairCurTarget.second.size() == 0  
                        && m_mapTargets.find(m_pairCurTarget.first) != m_mapTargets.end())
                    {
                        ++m_pairCurTarget.first;
                    }
                    else
                    {
                        break;
                    }
                }
                if (m_pairCurTarget.second.size()==0)
                {
                    QString strCurLabel;
                    AllSettings::getInstance()->GetCurLabelAndColor(strCurLabel, m_Color);
                    m_pairCurTarget.second.strTargetName = strCurLabel.toLocal8Bit().data();
                }
                m_pairCurTarget.second.append(ptF);
                m_pairCurTarget.second.lstValue.append(nValue);
                m_bIsDrawing = true;
            }

        }
        else if ((event->buttons() & Qt::RightButton) == Qt::RightButton)
        {
            m_bIsDrawing = false;
            if (m_pairCurTarget.second.size() <= 2)
            {
                m_pairCurTarget.second.reset();
                return;
            }

            //插值一下
            generateInterpolationPoints(m_pairCurTarget.second);
            m_pairCurTarget.second.bClosed = true;
            m_pairCurTarget.second.nTargetID = m_pairCurTarget.first;
            m_mapTargets.insert(m_pairCurTarget);
            ImageDataManager::getInstance()->updateLabelInfo(m_emPlane, m_nCurFrameIndex, m_mapTargets);
            m_pairCurTarget.second.reset();
        }
        //DrawGraphics();
        break;
    case MeasureRuler:
        break;
    case MeasurePolygon:
        break;
    case MeasureCircle:
        break;
    case MeasureOval:
        break;
    default:
        break;
    }
}

/******************************************************** 
*  @function : mouseMoveEvent
*  @brief    : 鼠标移动事件
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void DrawPen::mouseMoveEvent(QMouseEvent *event, QPointF ptF, int nValue)
{
    if (m_bIsDrawing)
    {
        switch (m_emPenType)
        {
        case PenNone:
            break;
        case PenPoint:
            break;
        case PenPolygon:
            break;
        case PenDrawPen:
            if ((event->buttons() & Qt::LeftButton) == Qt::LeftButton)
            {
                if (m_bIsDrawing)
                {
                    m_pairCurTarget.second.append(ptF);
                    m_pairCurTarget.second.lstValue.append(nValue);
                    //qDebug() << ptF;
                    //DrawGraphics();
                }
            }
            break;
        case MeasureRuler:
            break;
        case MeasurePolygon:
            break;
        case MeasureCircle:
            break;
        case MeasureOval:
            break;
        default:
            break;
        }
    }
    else if (m_bIsMoving)
    {
        if ((event->buttons() & Qt::LeftButton) == Qt::LeftButton
            && m_pairSelectTarget.first == m_emPlane
            && m_pairSelectTarget.second != -1
            )
        {
            QPointF ptfDist = ptF - m_ptfMoveLastPos;//位移
            m_ptfMoveLastPos = ptF;
            ExPolygonF& poly = m_mapTargets[m_pairSelectTarget.second];
            if (poly.nSelectPoint!=-1)
            {
                poly[poly.nSelectPoint] = poly[poly.nSelectPoint] + ptfDist;

            }
            else
            {
                ExPolygonF polyTemp = poly;
                for (int i = 0; i < poly.size(); i++)
                {
                    polyTemp[i] = poly[i] + ptfDist;
                    if (polyTemp[i].rx() > 1
                        || polyTemp[i].ry() > 1
                        || polyTemp[i].rx() < 0
                        || polyTemp[i].ry() < 0
                        )
                    {
                        return;
                    }
                }
                poly = polyTemp;
            }
        }
        ImageDataManager::getInstance()->updateLabelInfo(m_emPlane, m_nCurFrameIndex, m_mapTargets);

    }
    else
    {
        m_ptCross = ptF;
    }
}

/******************************************************** 
*  @function : mouseReleaseEvent
*  @brief    : 鼠标释放事件
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void DrawPen::mouseReleaseEvent(QMouseEvent *event, QPointF ptF, int nValue)
{
    m_bIsMoving = false;
    switch (m_emPenType)
    {
    case PenNone:
        break;
    case PenPoint:
        break;
    case PenPolygon:
        break;
    case PenDrawPen:
        break;
    case MeasureRuler:
        break;
    case MeasurePolygon:
        break;
    case MeasureCircle:
        break;
    case MeasureOval:
        break;
    default:
        break;
    }
}

/******************************************************** 
*  @function : resizeEvent
*  @brief    : 
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void DrawPen::resizeEvent(QResizeEvent *event)
{
    //需要调整尺寸, 并且重新绘制
    //DrawGraphics();
}

/******************************************************** 
*  @function : wheelEvent
*  @brief    : brief
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void DrawPen::wheelEvent(QWheelEvent *event)
{
    //需要调整尺寸, 并且重新绘制
    //DrawGraphics();
}

/******************************************************** 
*  @function : DrawGraphics
*  @brief    : 整个画面绘制
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void DrawPen::DrawGraphics()
{
    QList<ExLineF> lstWTargets;
    QList<ExLineF> lstHTargets;
    ImageDataManager::getInstance()->getOtherPlaneTargetWH(m_emPlane, lstWTargets, lstHTargets, m_nCurFrameIndex);

    QPainter painter;
    QTime atime;

    *m_pImgDrawing = m_pImgSrc->convertToFormat(QImage::Format_RGB32);

    *m_pImgDrawing = m_pImgDrawing->scaled(m_pImgSrc->size()*m_fZoom, Qt::KeepAspectRatio);

    //设置画笔画刷
    QPen penCommonLine;
    QBrush brushNone;
    penCommonLine.setColor(Qt::red); //改变颜色
    penCommonLine.setWidth(LINE_WIDTH);

    QPen penCommenPoint;
    penCommenPoint.setColor(COMMON_POINT_LINE_COLOR); //改变颜色
    penCommenPoint.setWidth(POINT_LINE_WIDTH);

    //正在绘制的画笔
    QPen penDrawingLine;
    penDrawingLine.setColor(m_Color);
    penDrawingLine.setWidth(LINE_WIDTH);

    QPen penDrawingPoint;
    penDrawingPoint.setColor(DRWAING_POINT_COLOR);
    penDrawingPoint.setWidth(POINT_RADIUS);
    QBrush brushDrawingPoint(Qt::red);

    QPen penSelectLine;
    penSelectLine.setColor(SELECT_LINE_COLOR);
    penSelectLine.setWidth(LINE_WIDTH);

    painter.begin(m_pImgDrawing);

    for each (auto var in lstWTargets)
    {
        QColor clrCurTgt = AllSettings::getInstance()->GetColorByLabel(QString::fromLocal8Bit(var.strTargetName.c_str()));
        clrCurTgt.setAlpha(m_nAlphaValue);
        painter.setPen(QPen(clrCurTgt));
        QColor clrCurTgtBrush = clrCurTgt;
        clrCurTgtBrush.setAlpha(m_nAlphaValue);
        painter.setBrush(clrCurTgtBrush);
        drawLayer(&painter, var.p1(), var.p2());
    }
    for each (auto var in lstHTargets)
    {
        QColor clrCurTgt = AllSettings::getInstance()->GetColorByLabel(QString::fromLocal8Bit(var.strTargetName.c_str()));
        clrCurTgt.setAlpha(m_nAlphaValue);
        painter.setPen(QPen(clrCurTgt));
        QColor clrCurTgtBrush = clrCurTgt;
        clrCurTgtBrush.setAlpha(m_nAlphaValue);
        painter.setBrush(clrCurTgtBrush);
        drawLayer(&painter, var.p1(), var.p2());
    }

    //绘制已完成的图形
    for each (auto varShape in m_mapTargets)
    {
        QColor clrCurTgt = AllSettings::getInstance()->GetColorByLabel(QString::fromLocal8Bit(varShape.second.strTargetName.c_str()));
        QColor clrCurTgtBrush = clrCurTgt;
        clrCurTgtBrush.setAlpha(m_nAlphaValue);
        penCommonLine.setColor(clrCurTgt);
        //绘图
        painter.setPen(penCommonLine);
        painter.setBrush(clrCurTgtBrush);
        //当前选中框
        if (m_pairSelectTarget.first == m_emPlane
            &&m_pairSelectTarget.second == varShape.first)
        {
            painter.setPen(penSelectLine);
            painter.setBrush(brushNone);
            drawPolygon(&painter, varShape.second,true);
        }
        else
        {
            drawPolygon(&painter, varShape.second);
        }
        //绘点
        if (varShape.second.size() < DRAWPEN_POINT_COUNTS_LIMIT)
        {
            for (int i = 0; i < varShape.second.size(); i++)
            {
                if (varShape.second.nSelectPoint == i)
                {
                    painter.setPen(penDrawingPoint);
                    painter.setBrush(brushDrawingPoint);
                }
                else
                {
                    painter.setPen(penCommenPoint);
                }
                //painter.drawEllipse(QPoint(varShape.second[i].rx()*m_pImgDrawing->width(), varShape.second[i].ry()*m_pImgDrawing->height()), POINT_RADIUS, POINT_RADIUS);
                drawPoint(&painter, varShape.second[i]);
            }
        }
    }

    //绘制正在绘制的图形
    //绘图
    painter.setPen(penDrawingLine);
    drawPolygon(&painter, m_pairCurTarget.second);

    painter.setPen(penDrawingPoint);
    painter.setBrush(brushDrawingPoint);

    if (m_pairCurTarget.second.size()< DRAWPEN_POINT_COUNTS_LIMIT)
    {
        //绘点
        for (int i = 0; i < m_pairCurTarget.second.size(); i++)
        {
            drawPoint(&painter, m_pairCurTarget.second[i]);
        }
    }

    //画十字
    {
        painter.setPen(QPen(QBrush(WIDGET_CROSS_COLOR), 1, Qt::DashDotLine));//设置画刷形式,背景色 

        drawLine(&painter, QPointF(0.0f, m_ptCross.ry()), QPointF(1.0f, m_ptCross.ry()));
        drawLine(&painter, QPointF(m_ptCross.rx(), 0), QPointF(m_ptCross.rx(), 1.0f));
    }

    painter.end();
}

/******************************************************** 
*  @function : drawPoint
*  @brief    : 画点
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void DrawPen::drawPoint(QPainter *p, QPointF pt)
{
    if (m_emPlane != TransversePlane)
    {
        pt = QPointF(pt.rx(), 1.0f - pt.ry());
    }
    p->drawEllipse(QPoint(pt.rx()*m_pImgDrawing->width(), pt.ry()*m_pImgDrawing->height()), POINT_RADIUS, POINT_RADIUS);
}

/******************************************************** 
*  @function : drawLine
*  @brief    : 画线
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void DrawPen::drawLine(QPainter *p, QPointF pt1, QPointF pt2)
{
    if (m_emPlane != TransversePlane)
    {
        pt1 = QPointF(pt1.rx(), 1.0f - pt1.ry());
        pt2 = QPointF(pt2.rx(), 1.0f - pt2.ry());
    }
    p->drawLine(QPoint(round(pt1.rx()*m_pImgDrawing->width()), round(pt1.ry()*m_pImgDrawing->height())),
        QPoint(round(pt2.rx()*m_pImgDrawing->width()), round(pt2.ry()*m_pImgDrawing->height())));
}

/******************************************************** 
*  @function : drawLayer
*  @brief    : 绘制透视出来的图层
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void DrawPen::drawLayer(QPainter *p, QPointF pt1, QPointF pt2)
{
    if (m_emPlane != TransversePlane)
    {
        pt1 = QPointF(pt1.rx(), 1.0f - pt1.ry());
        pt2 = QPointF(pt2.rx(), 1.0f - pt2.ry());
    }
    QRect rect;
    float fSpacing = m_stImageHeaderInfo.fPixelSpacingT/ m_stImageHeaderInfo.fPixelSpacingW;

    //横线
    if (abs(pt1.x()-pt2.x())<0.000001)
    {
        QPoint ptTopLeft = QPoint(round(pt1.rx()*m_pImgDrawing->width()), round(pt1.ry()*m_pImgDrawing->height()));
        QPoint ptBottomRight = QPoint(round(pt2.rx()*m_pImgDrawing->width() + m_fZoom), round(pt2.ry()*m_pImgDrawing->height()));
        switch (m_emPlane)
        {
        case SagittalPlane:
        case CoronalPlane:
            ptTopLeft = QPoint(round(pt1.rx()*m_pImgDrawing->width()), round(pt1.ry()*m_pImgDrawing->height()));
            ptBottomRight = QPoint(round(pt2.rx()*m_pImgDrawing->width() + m_fZoom* fSpacing), round(pt2.ry()*m_pImgDrawing->height()));
            break;
        case TransversePlane:
            break;
        default:
            break;
        }
        rect.setTopLeft(ptTopLeft);
        rect.setBottomRight(ptBottomRight);
    }
    //竖线
    else if (abs(pt1.y() - pt2.y()) < 0.000001)
    {
        QPoint ptTopLeft = QPoint(round(pt1.rx()*m_pImgDrawing->width()), round(pt1.ry()*m_pImgDrawing->height()));
        QPoint ptBottomRight = QPoint(round(pt2.rx()*m_pImgDrawing->width()), round(pt2.ry()*m_pImgDrawing->height() + m_fZoom));
        switch (m_emPlane)
        {
        case SagittalPlane:
        case CoronalPlane:
            ptTopLeft = ptTopLeft = QPoint(round(pt1.rx()*m_pImgDrawing->width()), round(pt1.ry()*m_pImgDrawing->height()));
            ptBottomRight = QPoint(round(pt2.rx()*m_pImgDrawing->width()), round(pt2.ry()*m_pImgDrawing->height() + m_fZoom*fSpacing));
            break;
        case TransversePlane:
            break;
        default:
            break;
        }
        rect.setTopLeft(ptTopLeft);
        rect.setBottomRight(ptBottomRight);
    }
    else
    {
        return;
    }
    p->drawRect(rect);
}

/******************************************************** 
*  @function : drawPolygon
*  @brief    : 绘制多边形
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void DrawPen::drawPolygon(QPainter *p, ExPolygonF poly, bool bDrawRect/* = false*/)
{
    QPolygon polyTmp;
    QPolygonF polyTmp4Area;
    for (int i = 0 ; i < poly.size();i++)
    {
        if (m_emPlane != TransversePlane)
        {
            polyTmp.append(QPoint(round(poly[i].rx()*m_pImgDrawing->width()), round((1.0f-poly[i].ry())*m_pImgDrawing->height())));
        }
        else
        {
            polyTmp.append(QPoint(round(poly[i].rx()*m_pImgDrawing->width()), round(poly[i].ry()*m_pImgDrawing->height())));
        }
    }
    if (poly.bClosed)
    {
        polyTmp.append(polyTmp.first());
        p->drawPolygon(polyTmp);
        polyTmp4Area = poly;
        polyTmp4Area.append(polyTmp4Area.first());
        if (bDrawRect)
        {
            p->drawRect(polyTmp.boundingRect());
            float fAreaSquare = calcPolygonArea(polyTmp4Area);
            emit sigSelectPolygon(polyTmp4Area.boundingRect().size(), fAreaSquare);
        }
    }
    else
    {
        p->drawPolyline(polyTmp);
    }

}

/******************************************************** 
*  @function : checkSelect
*  @brief    : 确定选择的多边形
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void DrawPen::checkSelect(QPointF ptF)
{
    for(auto it = m_mapTargets.begin(); it!= m_mapTargets.end(); it++)
    {
        it->second.nSelectPoint = -1;
    }

    ExPolygonF* pCurPoly = nullptr;
    if (m_pairSelectTarget.first == m_emPlane
        &&m_pairSelectTarget.second !=-1)
    {
        pCurPoly = &m_mapTargets[m_pairSelectTarget.second];
    }
    if (pCurPoly!=nullptr)
    {
        QPoint pt(ptF.rx()*m_pImgDrawing->width(), ptF.ry()*m_pImgDrawing->height());
        for (int i = 0; i < (*pCurPoly).size(); i++)
        {
            QPoint pt2Compare((*pCurPoly)[i].rx()*m_pImgDrawing->width(), (*pCurPoly)[i].ry()*m_pImgDrawing->height());
            if (fabs(pt.rx() - pt2Compare.rx()) <= 5 && fabs(pt.ry() - pt2Compare.ry()) <= 5)
            {
                (*pCurPoly).nSelectPoint = i;
                return;
            }
        }
    }


    QVector<int> vecSelectIds;
    ExPolygonF ploySelect;
    for each (auto& var in m_mapTargets)
    {
        ploySelect = var.second;
        ploySelect.append(ploySelect.first());
        if (ploySelect.containsPoint(ptF, Qt::OddEvenFill))
        {
            vecSelectIds.append(var.first);
        }
    }
    if (vecSelectIds.isEmpty())
    {
        m_pairSelectTarget.first = m_emPlane;
        m_pairSelectTarget.second = -1;
        emit sigSelectPolygon(QSize(0, 0));
        m_bIsMoving = false;
        return;
    }
    //如果就一个 或者 之前不在这个视图
    if (vecSelectIds.size() == 1 || m_pairSelectTarget.first != m_emPlane)
    {
        m_pairSelectTarget.first = m_emPlane;
        m_pairSelectTarget.second = vecSelectIds[0];
        return;
    }
    //大于两个多边形被选中
    if (vecSelectIds.size() > 1)
    {
        m_pairSelectTarget.first = m_emPlane;
        //判断是否包含了之前被选中的那个
        if (vecSelectIds.contains(m_pairSelectTarget.second))
        {
            //如果包含，而且不是最后一个，往下一层选
            if (vecSelectIds.indexOf(m_pairSelectTarget.second) != vecSelectIds.size() - 1)
            {
                m_pairSelectTarget.second = vecSelectIds[vecSelectIds.indexOf(m_pairSelectTarget.second)+1];
            }
            else//如果是最后一个，就一个循环了，再设置选中第一个
            {
                m_pairSelectTarget.second = vecSelectIds[0];
            }
        }//没包含过，直接选中第一层的
        else
        {
            m_pairSelectTarget.second = vecSelectIds[0];
        }

    }

}

/******************************************************** 
*  @function : generateInterpolationPoints
*  @brief    : 生成插值点
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void DrawPen::generateInterpolationPoints(ExPolygonF &poly, INTERP_METHOD emInterpMethod /*= IM_NONE*/)
{
    switch (emInterpMethod)
    {
    case IM_None:
        break;
    case IM_Neighbor_Interp:
        break;
    case IM_Bilinear_Interp:
        break;
    case IM_Bicubic_Interp:
        break;
    default:
        break;
    }
}

/******************************************************** 
*  @function : calcPolygonArea
*  @brief    : 计算多边形面积
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
float DrawPen::calcPolygonArea(QPolygonF poly)
{
    int  i_count = poly.size();
    float area_temp = 0.f;
    for (int i = 0; i < i_count; i++)
    {
        area_temp = area_temp + (poly[i].rx()*poly[(i + 1) % i_count].ry() - poly[(i + 1) % i_count].rx()*poly[i].ry());
    }
    return abs(0.5*area_temp);
}
