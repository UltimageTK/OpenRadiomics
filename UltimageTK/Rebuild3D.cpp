/******************************************************************************
*  Copyright (C), 2017-2019,Advanced Technology Institute of Suzhou. 
*  THIS IS AN UNPUBLISHED WORK CONTAINING CONFIDENTIAL AND PROPRIETARY 
*   
*******************************************************************************
*  File Name: Widget.cpp
*  Author: cg.zhang@atisz.ac.cn
*  Version: 1.0.0
*  Date: 2020-3-19
*  Description: 三维重建
*  History: 
******************************************************************************/

#include "Rebuild3D.h"
#include "vtkGenericOpenGLRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkRenderWindowInteractor.h"
#include "QVTKOpenGLWidget.h"
#include "vtkCamera.h"
#include "vtkProperty.h"
#include "vtkDataSetMapper.h"
#include "vtkTriangleFilter.h"
#include "vtkLinearExtrusionFilter.h"
#include "vtkAutoInit.h"

using namespace LabelAnalysis;

VTK_MODULE_INIT(vtkRenderingOpenGL2)
VTK_MODULE_INIT(vtkInteractionStyle)
VTK_MODULE_INIT(vtkRenderingFreeType)

Rebuild3D::Rebuild3D(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	m_renderWindow = vtkGenericOpenGLRenderWindow::New();
	m_renderer = vtkRenderer::New();
	m_renderer->GetActiveCamera()->Azimuth(30);
	m_renderer->GetActiveCamera()->Elevation(30);
	m_renderer->GetActiveCamera()->Dolly(1.0);
	m_renderWindow->AddRenderer(m_renderer);
	ui.qvtkWidget3D->SetRenderWindow(m_renderWindow);


    // 设置三维坐标轴
    m_axesActor = vtkSmartPointer<vtkAxesActor>::New();
}

Rebuild3D::~Rebuild3D()
{
	ClearActors();

	if (nullptr != m_renderWindow)
	{
		m_renderWindow->Delete();
		m_renderWindow = nullptr;
	}

	if (nullptr == m_renderer)
	{
		m_renderer->Delete();
        m_renderer = nullptr;
	}
}

//////////////////////////////////////////////////////////////////////////////// 
// Function: Rebuild
// Description: 三维重建
// Parameters:const QVector<vtkActor * > & vecActors
// Parameters:LabelAnalysis::FileInfo fileInfo
// Return: void
// Remarks: 
////////////////////////////////////////////////////////////////////////////////
void Rebuild3D::Rebuild(const QVector<vtkActor*>& vecActors, LabelAnalysis::FileInfo fileInfo)
{
	for (const auto& actor : vecActors)
	{
		m_renderer->AddActor(actor);
	}

	auto maxLength = [](double a, double b, double c) 
	{
		return a > b ? (a > c ? a : c) : (b > c ? b : c);
	};

	auto dLength = maxLength(fileInfo.nWidth, fileInfo.nHeight, fileInfo.nThickness * fileInfo.fSpacing);
    m_axesActor->SetTotalLength(dLength, dLength, dLength);
    m_axesActor->SetPosition(0, 0, 0);
    m_axesActor->SetShaftType(0);
    m_axesActor->SetAxisLabels(0);
    m_axesActor->SetCylinderRadius(0.02);
	m_renderer->AddActor(m_axesActor);

	// Generate an interesting view
	m_renderer->ResetCamera();
	m_renderer->ResetCameraClippingRange();

	// 显示
	m_renderWindow->Render();
}

//////////////////////////////////////////////////////////////////////////////// 
// Function: CreateActor
// Description: 创建actor
// Parameters:vtkPoints * points
// Parameters:QRgb rgb
// Parameters:float fSpacing
// Return: vtkActor*
// Remarks: 
////////////////////////////////////////////////////////////////////////////////
vtkActor* Rebuild3D::CreateActor(vtkPoints* points, QRgb rgb, float fSpacing)
{
	vtkIdType num = points->GetNumberOfPoints();
	vtkIdType* cellId = new vtkIdType[num];
    
	for (size_t i = 0; i < num; i++)
	{
		cellId[i] = static_cast<vtkIdType>(i);
	}

	vtkSmartPointer<vtkCellArray> cellArray = vtkSmartPointer<vtkCellArray>::New();

	cellArray->InsertNextCell(num, cellId);
	delete[] cellId;

	vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
	polyData->SetPoints(points);
	polyData->SetPolys(cellArray);

	// Apply linear extrusion 
	vtkSmartPointer<vtkLinearExtrusionFilter> extrude = vtkSmartPointer<vtkLinearExtrusionFilter>::New();
	extrude->SetInputData(polyData);
	extrude->SetExtrusionTypeToNormalExtrusion();
	extrude->SetVector(0, 0, fSpacing);
	//extrude->SetScaleFactor(0.5);

	vtkSmartPointer<vtkTriangleFilter> triangleFilter = vtkSmartPointer<vtkTriangleFilter>::New();
	triangleFilter->SetInputConnection(extrude->GetOutputPort());

	vtkSmartPointer<vtkDataSetMapper> mapper = vtkSmartPointer<vtkDataSetMapper>::New();
	mapper->SetInputConnection(triangleFilter->GetOutputPort());

	m_actor = vtkActor::New();
	m_actor->SetMapper(mapper);
	m_actor->GetProperty()->SetColor(QColor(rgb).red(), QColor(rgb).green(), QColor(rgb).blue());
	return m_actor;
}


//////////////////////////////////////////////////////////////////////////////// 
// Function: GetActors
// Description: 获取所有actor
// Parameters:const std::map<int, std::map<int, LabelAnalysis::Target>> & targets
// Parameters:QVector<vtkActor * > & vecActors
// Parameters:LabelAnalysis::FileInfo fileInfo
// Parameters:const std::map<std::string
// Parameters:int> & mapLabelProperty
// Parameters:PlaneType planeType
// Return: void
// Remarks: 
////////////////////////////////////////////////////////////////////////////////
void Rebuild3D::GetActors(const std::map<int, std::map<int, LabelAnalysis::Target>>& targets, QVector<vtkActor*>& vecActors, 
	LabelAnalysis::FileInfo fileInfo, const std::map<std::string, int>& mapLabelProperty, PlaneType planeType)
{
	for (const auto& itTargets : targets)
	{
		for (const auto& itVertex : itTargets.second)
		{
			vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
			QRgb rgb = static_cast<QRgb>(mapLabelProperty.at(itVertex.second.strTargetName));
			auto it = itVertex.second.lstVertex.begin();
			for (size_t i = 0; i < itVertex.second.lstVertex.size(); i++, it++)
			{
				switch (planeType)
				{
				case PlaneType::PLANE_CORONAL:
				{
					points->InsertPoint(i, it->fX * fileInfo.nWidth, itTargets.first * fileInfo.fSpacing, it->fY * fileInfo.nHeight);
					break;
				}
				case PlaneType::PLANE_SAGITTAL:
				{
					points->InsertPoint(i, it->fY * fileInfo.nHeight, it->fX * fileInfo.nWidth, itTargets.first * fileInfo.fSpacing);
					break;
				}
				case PlaneType::PLANE_TRANSVERSE:
				{
					points->InsertPoint(i, it->fX * fileInfo.nWidth, it->fY * fileInfo.nHeight, itTargets.first * fileInfo.fSpacing);
					break;
				}
				default:
					break;
				}
			}
			vecActors.push_back(CreateActor(points, rgb, fileInfo.fSpacing));
		}
	}
}

//////////////////////////////////////////////////////////////////////////////// 
// Function: Display3D
// Description: 显示三维重建
// Parameters:const LabelAnalysis::AllLabelInfo & labelInfo
// Return: void
// Remarks: 
////////////////////////////////////////////////////////////////////////////////
void Rebuild3D::Display3D(const LabelAnalysis::AllLabelInfo& labelInfo)
{
    Clear3DWidget();
	ClearActors();

	GetActors(labelInfo.mapCPTargets, m_vecActors, labelInfo.stFileInfo, labelInfo.mapLabelProperty, PlaneType::PLANE_CORONAL);
	GetActors(labelInfo.mapSPTargets, m_vecActors, labelInfo.stFileInfo, labelInfo.mapLabelProperty, PlaneType::PLANE_SAGITTAL);
	GetActors(labelInfo.mapTPTargets, m_vecActors, labelInfo.stFileInfo, labelInfo.mapLabelProperty, PlaneType::PLANE_TRANSVERSE);

	Rebuild(m_vecActors, labelInfo.stFileInfo);
}

/******************************************************** 
*  @function : Clear3DWidget
*  @brief    : 清空3D界面上的所有东西
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void Rebuild3D::Clear3DWidget()
{
	m_renderer->RemoveAllViewProps();
	m_renderWindow->Render();
}

//////////////////////////////////////////////////////////////////////////////// 
// Function: ClearActors
// Description: 清理所有actors
// Return: void
// Remarks: 
////////////////////////////////////////////////////////////////////////////////
void Rebuild3D::ClearActors()
{
	for (auto& actor : m_vecActors)
	{
		actor->Delete();
		actor = nullptr;
	}

	m_vecActors.clear();
}
