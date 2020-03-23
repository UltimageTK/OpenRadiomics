/******************************************************** 
* @file    : SaveThread.cpp
* @brief   : brief
* @details : 
* @author  : qh.zhang@atisz.ac.cn
* @date    : 2019-7-10
*********************************************************/
#include <QFileInfo>
#include "SaveThread.h"
#include "ImageDataManager.h"
#include "itkMeanImageFilter.h"
#include "itkImageFileWriter.h"
#include "itkMeanImageFilter.h"

SaveThread::SaveThread(QObject *parent)
{
}

SaveThread::~SaveThread()
{
}

/******************************************************** 
*  @function : SaveAsLsr
*  @brief    : 保存为lsr文件
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
bool SaveThread::SaveAsLsr(const QString strPath)
{
    QFileInfo fileInfo(strPath);
    if (fileInfo.suffix() != "lsr")
    {
        return false;
    }
    LabelAnalysis::AllLabelInfo stAllLabelInfo;
    ImageDataManager::getInstance()->getAllLabelInfo(stAllLabelInfo);
    bool bRet = LabelAnalysis::WriteLabelFile(strPath.toLocal8Bit().data(), stAllLabelInfo);

    return bRet;
}

/******************************************************** 
*  @function : SaveAsNii
*  @brief    : 保存为nii格式
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
bool SaveThread::SaveAsNii(const QString strPath)
{
    QFileInfo fileInfo(strPath);
    if (fileInfo.completeSuffix() != "nii"
        && fileInfo.completeSuffix() != "nii.gz")
    {
        return false;
    }
    ImageType3D::Pointer pImageNii = ImageType3D::New();
    ImageType3D::Pointer pImageSrcClone = ImageType3D::New();
    //获取文件信息
    bool bRet = ImageDataManager::getInstance()->getImageSerial(pImageSrcClone);
    LabelAnalysis::AllLabelInfo stAllLabelInfo;
    ImageDataManager::getInstance()->getAllLabelInfo(stAllLabelInfo);
    std::map<std::string, int> mapLabelIndex;
    ImageDataManager::getInstance()->getLabelPpMap(mapLabelIndex);

    itk::Size<3> sliceSize = pImageSrcClone->GetLargestPossibleRegion().GetSize();
    int nWidth = sliceSize[SagittalPlane];
    int nHeight = sliceSize[CoronalPlane];
    int nThickNess = sliceSize[TransversePlane];

    pImageNii->SetRegions(pImageSrcClone->GetBufferedRegion());
    pImageNii->Allocate();
    pImageNii->FillBuffer(itk::NumericTraits< ImageType3D::PixelType >::Zero);
    pImageNii->CopyInformation(pImageSrcClone);


    //bool bRet = ImageDataManager::getInstance()->getNiiImage(pImageNii);

    //将结构化信息转换为Nii文件信息
    for each (auto varFrame in stAllLabelInfo.mapSPTargets)
    {
        for each (auto varTarget in varFrame.second)
        {
            QPolygon poly;
            Vertex2Polygon(varTarget.second.lstVertex, nHeight, nThickNess, poly);
            int nColorIndex = mapLabelIndex.find(varTarget.second.strTargetName)->second;
            PolygonShader(SagittalPlane, varFrame.first, poly, nColorIndex, pImageNii, pImageSrcClone, true);
        }
    }
    for each (auto varFrame in stAllLabelInfo.mapCPTargets)
    {
        for each (auto varTarget in varFrame.second)
        {
            QPolygon poly;
            Vertex2Polygon(varTarget.second.lstVertex, nWidth, nThickNess, poly);
            int nColorIndex = mapLabelIndex.find(varTarget.second.strTargetName)->second;
            PolygonShader(CoronalPlane, varFrame.first, poly, nColorIndex, pImageNii, pImageSrcClone, true);
        }
    }

    for each (auto varFrame in stAllLabelInfo.mapTPTargets)
    {
        for each (auto varTarget in varFrame.second)
        {
            QPolygon poly;
            Vertex2Polygon(varTarget.second.lstVertex, nWidth, nHeight, poly);
            int nColorIndex = mapLabelIndex.find(varTarget.second.strTargetName)->second;
            PolygonShader(TransversePlane, varFrame.first, poly, nColorIndex, pImageNii, pImageSrcClone, true);
        }
    }

    if (!bRet)
    {
        return false;
    }
    typedef  itk::ImageFileWriter<ImageType3D> WriterType;
    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(strPath.toLocal8Bit().data());
    writer->SetInput(pImageNii);
    try
    {
        writer->Update();
    }
    catch (itk::ExceptionObject &ex)
    {
        std::string str(ex.what());
        return false;
    }
    return true;
}

/******************************************************** 
*  @function : SaveAsNrrd
*  @brief    : 保存为nrrd格式
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
bool SaveThread::SaveAsNrrd(const QString strPath)
{
    QFileInfo fileInfo(strPath);
    if (fileInfo.suffix()!="nrrd")
    {
        return false;
    }
    ImageType3D::Pointer pImageNrrd = ImageType3D::New();
    //bool bRet = ImageDataManager::getInstance()->getNrrdImage(pImageNrrd);
    ImageType3D::Pointer pImageSrcClone = ImageType3D::New();
    //获取文件信息
    bool bRet = ImageDataManager::getInstance()->getImageSerial(pImageSrcClone);
    LabelAnalysis::AllLabelInfo stAllLabelInfo;
    ImageDataManager::getInstance()->getAllLabelInfo(stAllLabelInfo);
    std::map<std::string, int> mapLabelIndex;
    ImageDataManager::getInstance()->getLabelPpMap(mapLabelIndex);

    itk::Size<3> sliceSize = pImageSrcClone->GetLargestPossibleRegion().GetSize();
    int nWidth = sliceSize[SagittalPlane];
    int nHeight = sliceSize[CoronalPlane];
    int nThickNess = sliceSize[TransversePlane];

    pImageNrrd->SetRegions(pImageSrcClone->GetBufferedRegion());
    pImageNrrd->Allocate();
    pImageNrrd->FillBuffer(itk::NumericTraits< ImageType3D::PixelType >::Zero);
    pImageNrrd->CopyInformation(pImageSrcClone);


    //bool bRet = ImageDataManager::getInstance()->getNiiImage(pImageNii);

    //将结构化信息转换为Nii文件信息
    for each (auto varFrame in stAllLabelInfo.mapSPTargets)
    {
        for each (auto varTarget in varFrame.second)
        {
            QPolygon poly;
            Vertex2Polygon(varTarget.second.lstVertex, nHeight, nThickNess, poly);
            int nColorIndex = mapLabelIndex.find(varTarget.second.strTargetName)->second;
            PolygonShader(SagittalPlane, varFrame.first, poly, nColorIndex, pImageNrrd, pImageSrcClone);
        }
    }
    for each (auto varFrame in stAllLabelInfo.mapCPTargets)
    {
        for each (auto varTarget in varFrame.second)
        {
            QPolygon poly;
            Vertex2Polygon(varTarget.second.lstVertex, nWidth, nThickNess, poly);
            int nColorIndex = mapLabelIndex.find(varTarget.second.strTargetName)->second;
            PolygonShader(CoronalPlane, varFrame.first, poly, nColorIndex, pImageNrrd, pImageSrcClone);
        }
    }

    for each (auto varFrame in stAllLabelInfo.mapTPTargets)
    {
        for each (auto varTarget in varFrame.second)
        {
            QPolygon poly;
            Vertex2Polygon(varTarget.second.lstVertex, nWidth, nHeight, poly);
            int nColorIndex = mapLabelIndex.find(varTarget.second.strTargetName)->second;
            PolygonShader(TransversePlane, varFrame.first, poly, nColorIndex, pImageNrrd, pImageSrcClone);
        }
    }

    if (!bRet)
    {
        return false;
    }
    typedef  itk::ImageFileWriter<ImageType3D> WriterType;
    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(strPath.toLocal8Bit().data());
    writer->SetInput(pImageNrrd);
    try
    {
        writer->Update();
    }
    catch (itk::ExceptionObject &ex)
    {
        std::string str(ex.what());
        return false;
    }

    return true;
}

/******************************************************** 
*  @function : Vertex2Polygon
*  @brief    : 把点集转换成多边形对象
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void SaveThread::Vertex2Polygon(std::list<LabelAnalysis::Vertex> lstVertex, int nWidth, int nHeight, QPolygon& poly)
{
    //int nColorIndex = mapLabelIndex.find(strLabelName)->second;
    poly.clear();
    for each (auto point in lstVertex)
    {
        poly.push_back(QPoint(round(point.fX*nWidth), round(point.fY*nHeight)));
    }
    poly.push_back(poly.first());
}

/******************************************************** 
*  @function : PolygonShader
*  @brief    : 多会只好的多边形“着色”
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void SaveThread::PolygonShader(VIEW_PLANE emPlane, int nIndex, QPolygon poly, int nColorIndex, ImageType3D::Pointer& pImage, const ImageType3D::Pointer &pSrcImage, bool bIsNiiImage /*= false*/)
{
    ImageType3D::RegionType inputRegion = pImage->GetLargestPossibleRegion();
    ImageType3D::SizeType size = inputRegion.GetSize();
    QRect rect = poly.boundingRect();

    switch (emPlane)
    {
    case SagittalPlane:
        for (int i = rect.left(); i <= rect.right(); i++)
        {
            for (int j = rect.top(); j <= rect.bottom(); j++)
            {
                if (poly.containsPoint(QPoint(i, j), Qt::OddEvenFill))
                {
                    if (bIsNiiImage)
                    {
                        pImage->SetPixel({ nIndex,i, j }, pSrcImage->GetPixel({ nIndex,i, j }));
                    }
                    else
                    {
                        pImage->SetPixel({ nIndex,i, j }, nColorIndex);
                    }
                }
            }
        }
        break;
    case CoronalPlane:
        for (int i = rect.left(); i <= rect.right(); i++)
        {
            for (int j = rect.top(); j <= rect.bottom(); j++)
            {
                if (poly.containsPoint(QPoint(i, j), Qt::OddEvenFill))
                {
                    if (bIsNiiImage)
                    {
                        pImage->SetPixel({ i,nIndex, j }, pSrcImage->GetPixel({ i,nIndex, j }));
                    }
                    else
                    {
                        pImage->SetPixel({ i,nIndex, j }, nColorIndex);
                    }
                }
            }
        }
        break;
    case TransversePlane:
        for (int i = rect.left(); i <= rect.right(); i++)
        {
            for (int j = rect.top(); j <= rect.bottom(); j++)
            {
                if (poly.containsPoint(QPoint(i, j), Qt::OddEvenFill))
                {
                    if (bIsNiiImage)
                    {
                        pImage->SetPixel({ i, j,nIndex }, pSrcImage->GetPixel({ i, j,nIndex }));
                    }
                    else 
                    {
                        pImage->SetPixel({ i, j,nIndex }, nColorIndex);
                    }
                }
            }
        }
        break;
    default:
        break;
    }


}
