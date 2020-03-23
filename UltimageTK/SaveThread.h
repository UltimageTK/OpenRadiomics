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

    //��ע�������ɸ��ָ�ʽ
    bool SaveAsLsr(const QString strPath);
    bool SaveAsNii(const QString strPath);
    bool SaveAsNrrd(const QString strPath);

    //�ѵ㼯ת���ɶ���ζ���
    void Vertex2Polygon(std::list<LabelAnalysis::Vertex> lstVertex, int nWidth, int nHeight, QPolygon& poly);
    //���ֻ�õĶ���Ρ���ɫ��
    void PolygonShader(VIEW_PLANE emPlane, int nIndex, QPolygon poly, int nColorIndex, ImageType3D::Pointer& pImage, const ImageType3D::Pointer &pSrcImage, bool bIsNiiImage = false);

};
