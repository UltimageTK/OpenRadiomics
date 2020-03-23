/******************************************************** 
* @file    : ImageWidgetDef.h
* @brief   : 对ImageWidget中的一些常用量做定义
* @details : 
* @author  : qh.zhang@atisz.ac.cn
* @date    : 2019-4-24
*********************************************************/
#pragma once

#define KEY_DOWN(VK_NONAME) ((GetAsyncKeyState(VK_NONAME) & 0x8000) ? 1:0) 

//图片类型定义
enum IMAGE_TYPE
{
    IMG_DCM,   //dcm图片
    IMG_NII,   //nii图片
};


#define IMAGE_MARGIN 8  //图片到ImageWidget边缘的距离

#define WIDGET_BORDER_COLOR QColor(0,122,204)       //widget边缘颜色
#define WIDGET_BK_COLOR QColor(100,100,100)         //背景色
//#define WIDGET_BK_COLOR QColor(0,0,0)         //背景色
//#define WIDGET_CROSS_COLOR QColor(76, 76, 255)      //十字架颜色
#define WIDGET_CROSS_COLOR QColor(0, 255, 0)      //十字架颜色


enum VIEW_PLANE
{
    SagittalPlane,  //矢状面
    CoronalPlane,   //冠状面
    TransversePlane,    //横断面

};

//图片头信息
struct ImageHeaderInfo
{
    QString strPatientName;
    QString strPatientAge = "0";
    QString strPatientSex = "M";  //男M，女F
    QString strFilePath;
    QString strFileType;
    int nWidth = 0;
    int nHeight = 0;
    int nThickNess = 0;
    float fPixelSpacingW = 0.f;
    float fPixelSpacingH = 0.f;
    float fPixelSpacingT = 0.f;
    int nIntercept = 0;
    int nSlope = 0;
    int nWinCenter = 0;
    int nWinWidth = 0;
};


#include "itkImageFileReader.h"
#include "itkExtractImageFilter.h"
#include "itkImageSeriesReader.h"

//定义ITK
typedef itk::Image<short, 3>                  ImageType3D;    //3D图像类型
typedef itk::Image<short, 2>                  ImageType2D;    //2D图像类型
typedef itk::ExtractImageFilter<ImageType3D, ImageType2D> SliceType;    //切片类型
typedef itk::ImageSeriesReader< ImageType3D >  SeriesReaderType;    //读序列类型
