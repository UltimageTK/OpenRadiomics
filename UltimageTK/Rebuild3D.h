/******************************************************************************
*  Copyright (C), 2017-2019,Advanced Technology Institute of Suzhou. 
*  THIS IS AN UNPUBLISHED WORK CONTAINING CONFIDENTIAL AND PROPRIETARY 
*   
*******************************************************************************
*  File Name: Widget.h
*  Author: cg.zhang@atisz.ac.cn
*  Version: 1.0.0
*  Date: 2020-3-19
*  Description: 三维重建
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

    // 显示三维重建
    void Display3D(const LabelAnalysis::AllLabelInfo& labelInfo);

    // 清空3D界面
    void Clear3DWidget();

protected:
	// 重建
	void Rebuild(const QVector<vtkActor*>& vecActors, LabelAnalysis::FileInfo fileInfo);

	// 创建actor
	vtkActor* CreateActor(vtkPoints* points, QRgb rgb, float fSpacing);

	// 获取所有actor
	void GetActors(const std::map<int, std::map<int, LabelAnalysis::Target>>& targets, QVector<vtkActor*>& vecActor, 
		LabelAnalysis::FileInfo fileInfo, const std::map<std::string, int>& mapLabelProperty, PlaneType planeType);

	// 清理所有actors
	void ClearActors();

private:
	Ui::Build3DClass ui;
	vtkActor* m_actor{nullptr}; // 提供可做返回值的vtkActor指针
	QVector<vtkActor*> m_vecActors; // 存储所有的Actors
	vtkGenericOpenGLRenderWindow* m_renderWindow; // 渲染窗口，不能是局部变量
	vtkRenderer* m_renderer; // 使用类的成员变量，以便删除
    vtkSmartPointer<vtkAxesActor> m_axesActor;
};

#endif
