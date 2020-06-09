/******************************************************************************
*  Copyright (C), 2017-2019,Advanced Technology Institute of Suzhou. 
*  THIS IS AN UNPUBLISHED WORK CONTAINING CONFIDENTIAL AND PROPRIETARY 
*   
*******************************************************************************
*  File Name: Widget.h
*  Author: cg.zhang@atisz.ac.cn
*  Version: 1.0.0
*  Date: 2020-3-19
*  Description: ��ά�ؽ�
*  History: 
******************************************************************************/

#ifndef REBUILD3D_H
#define REBUILD3D_H

#include <QtWidgets/QWidget>
#include "ui_Rebuild3D.h"
#include "LabelAnalysis.h"
#include "vtkAxesActor.h"
#include "vtkSmartPointer.h"

class vtkImageData;
class vtkRenderer;
class vtkPoints;
class vtkActor;

enum class PlaneType
{
	PLANE_CORONAL,
	PLANE_SAGITTAL,
	PLANE_TRANSVERSE
};

class Rebuild3D : public QWidget
{
	Q_OBJECT

public:
	Rebuild3D(QWidget *parent = Q_NULLPTR);
	~Rebuild3D();

    // ��ʾ��ά�ؽ�
    void Display3D(const LabelAnalysis::AllLabelInfo& labelInfo);

    // ���3D����
    void Clear3DWidget();

protected:
	// �ؽ�
	void Rebuild(const QVector<vtkActor*>& vecActors, LabelAnalysis::FileInfo fileInfo);

	// ����actor
	vtkActor* CreateActor(vtkPoints* points, QRgb rgb, float fSpacing);

	// ��ȡ����actor
	void GetActors(const std::map<int, std::map<int, LabelAnalysis::Target>>& targets, QVector<vtkActor*>& vecActor, 
		LabelAnalysis::FileInfo fileInfo, const std::map<std::string, int>& mapLabelProperty, PlaneType planeType);

	// ��������actors
	void ClearActors();

private:
	Ui::Build3DClass ui;
	vtkActor* m_actor{nullptr}; // �ṩ��������ֵ��vtkActorָ��
	QVector<vtkActor*> m_vecActors; // �洢���е�Actors
	vtkGenericOpenGLRenderWindow* m_renderWindow; // ��Ⱦ���ڣ������Ǿֲ�����
	vtkRenderer* m_renderer; // ʹ����ĳ�Ա�������Ա�ɾ��
    vtkSmartPointer<vtkAxesActor> m_axesActor;
};

#endif
