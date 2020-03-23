/******************************************************** 
* @file    : SaveThread.h
* @brief   : brief
* @details : 
* @author  : qh.zhang@atisz.ac.cn
* @date    : 2019-7-10
*********************************************************/
#pragma once

#include <QObject>
#include "GlobalDef.h"
#include "LabelAnalysis.h"

class SaveThread : public QObject
{
    Q_OBJECT

public:
    SaveThread(QObject *parent = nullptr);
    ~SaveThread();

    //标注结果保存成各种格式
    bool SaveAsLsr(const QString strPath);
    bool SaveAsNii(const QString strPath);
    bool SaveAsNrrd(const QString strPath);

    //把点集转换成多边形对象
    void Vertex2Polygon(std::list<LabelAnalysis::Vertex> lstVertex, int nWidth, int nHeight, QPolygon& poly);
    //多会只好的多边形“着色”
    void PolygonShader(VIEW_PLANE emPlane, int nIndex, QPolygon poly, int nColorIndex, ImageType3D::Pointer& pImage, const ImageType3D::Pointer &pSrcImage, bool bIsNiiImage = false);

};
