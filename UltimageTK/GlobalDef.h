/******************************************************** 
* @file    : ImageWidgetDef.h
* @brief   : ��ImageWidget�е�һЩ������������
* @details : 
* @author  : qh.zhang@atisz.ac.cn
* @date    : 2019-4-24
*********************************************************/
#pragma once

#define KEY_DOWN(VK_NONAME) ((GetAsyncKeyState(VK_NONAME) & 0x8000) ? 1:0) 

//ͼƬ���Ͷ���
enum IMAGE_TYPE
{
    IMG_DCM,   //dcmͼƬ
    IMG_NII,   //niiͼƬ
};


#define IMAGE_MARGIN 8  //ͼƬ��ImageWidget��Ե�ľ���

#define WIDGET_BORDER_COLOR QColor(0,122,204)       //widget��Ե��ɫ
#define WIDGET_BK_COLOR QColor(100,100,100)         //����ɫ
//#define WIDGET_BK_COLOR QColor(0,0,0)         //����ɫ
//#define WIDGET_CROSS_COLOR QColor(76, 76, 255)      //ʮ�ּ���ɫ
#define WIDGET_CROSS_COLOR QColor(0, 255, 0)      //ʮ�ּ���ɫ


enum VIEW_PLANE
{
    SagittalPlane,  //ʸ״��
    CoronalPlane,   //��״��
    TransversePlane,    //�����

};

//ͼƬͷ��Ϣ
struct ImageHeaderInfo
{
    QString strPatientName;
    QString strPatientAge = "0";
    QString strPatientSex = "M";  //��M��ŮF
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

//����ITK
typedef itk::Image<short, 3>                  ImageType3D;    //3Dͼ������
typedef itk::Image<short, 2>                  ImageType2D;    //2Dͼ������
typedef itk::ExtractImageFilter<ImageType3D, ImageType2D> SliceType;    //��Ƭ����
typedef itk::ImageSeriesReader< ImageType3D >  SeriesReaderType;    //����������
