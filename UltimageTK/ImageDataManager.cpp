/******************************************************** 
* @file    : ImageDataManager.cpp
* @brief   : 
* @details : 
* @author  : qh.zhang@atisz.ac.cn
* @date    : 2019-4-25
*********************************************************/
#include <QDir>
#include <QDebug>
#include "ImageDataManager.h"
#include "./BridgeOpenCV/include/itkOpenCVImageBridge.h"

#include "itkMeanImageFilter.h"
#include "itkImageFileWriter.h"
#include "itkRescaleIntensityImageFilter.h"

#define LABEL_SEQ_RESULT_SUFFIX ".lsr"
#define TARGET_W true
#define TARGET_H false


ImageDataManager* ImageDataManager::m_pInstance = nullptr;


/******************************************************** 
*  @function : setHeaderInfo
*  @brief    : 设置结构化头信息
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void ImageDataManager::setHeaderInfo(ImageHeaderInfo stImageHeaderInfo)
{
    QMutexLocker locker(&m_muxSrcData);
    m_stImageHeaderInfo = stImageHeaderInfo;
}

/******************************************************** 
*  @function : setWindowLevel
*  @brief    : 设置窗宽窗位
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void ImageDataManager::setWindowLevel(float fWindow, float fCenter)
{
    QMutexLocker locker(&m_muxSrcData);
    
    /*typedef itk::RescaleIntensityImageFilter< ImageType3D, ImageType3D > RescaleFilterType;
    RescaleFilterType::Pointer rescaleFilter = RescaleFilterType::New();
    rescaleFilter->SetInput(m_pItkImgSerial);
    float fWindowWidth = m_stImageHeaderInfo.nWinWidth;
    float fWindowCenter = m_stImageHeaderInfo.nWinCenter;
    float fMax = fWindowCenter + fWindowWidth / 2.0f;
    float fMin = fWindowCenter - fWindowWidth / 2.0f;
    rescaleFilter->SetOutputMinimum(fMin);
    rescaleFilter->SetOutputMaximum(fMax);
    rescaleFilter->Update();
    m_pItkImgSerial = rescaleFilter->GetOutput();*/

    m_stImageHeaderInfo.nWinWidth = fWindow;
    m_stImageHeaderInfo.nWinCenter = fCenter;
}

/******************************************************** 
*  @function : setImageSerial
*  @brief    : 装载图片序列
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void ImageDataManager::setImageSerial(ImageType3D* pItkImgSerial)
{
    if (m_pItkNrrdSerial == nullptr)
    {
        m_pItkNrrdSerial = ImageType3D::New();
    }
    if (m_pItkNiiSerial == nullptr)
    {
        m_pItkNiiSerial = ImageType3D::New();
    }

    ImageType3D::RegionType region(pItkImgSerial->GetBufferedRegion());
    m_pItkNrrdSerial->SetRegions(region);
    m_pItkNrrdSerial->Allocate();
    m_pItkNrrdSerial->FillBuffer(itk::NumericTraits< ImageType3D::PixelType >::Zero);
    m_pItkNrrdSerial->CopyInformation(pItkImgSerial);
    //m_pItkNrrdSerial->SetSpacing(pItkImgSerial->GetSpacing());
    //m_pItkNrrdSerial->SetOrigin(pItkImgSerial->GetOrigin());
    //m_pItkNrrdSerial->SetNumberOfComponentsPerPixel(pItkImgSerial->GetNumberOfComponentsPerPixel());

    m_pItkNiiSerial->SetRegions(region);
    m_pItkNiiSerial->Allocate();
    m_pItkNiiSerial->FillBuffer(itk::NumericTraits< ImageType3D::PixelType >::Zero);
    m_pItkNiiSerial->CopyInformation(pItkImgSerial);
    /*m_pItkVtkShowSerial->SetSpacing(pItkImgSerial->GetSpacing());
    m_pItkVtkShowSerial->SetOrigin(pItkImgSerial->GetOrigin());
    m_pItkVtkShowSerial->SetNumberOfComponentsPerPixel(pItkImgSerial->GetNumberOfComponentsPerPixel());*/


    QMutexLocker locker(&m_muxSrcData);
    m_pItkImgSerial = pItkImgSerial;
}

/******************************************************** 
*  @function : getImageSerial
*  @brief    : 外部获取图片序列
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
bool ImageDataManager::getImageSerial(ImageType3D* pItkImgSerial)
{
    if (pItkImgSerial == nullptr)
    {
        return false;
    }
    QMutexLocker locker(&m_muxSrcData);
    if (m_pItkImgSerial == nullptr)
    {
        return false;
    }
    ImageType3D::RegionType region(m_pItkImgSerial->GetBufferedRegion());
    pItkImgSerial->SetRegions(region);
    pItkImgSerial->Allocate();
    pItkImgSerial->CopyInformation(m_pItkImgSerial);
    pItkImgSerial->Graft(m_pItkImgSerial);
    return true;
}


/******************************************************** 
*  @function : loadLabelInfo
*  @brief    : 加载结构化信息
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
bool ImageDataManager::loadLabelInfo(QString strPath, IMAGE_TYPE emImageType)
{
    QDir dir(strPath);
    QString strLsrFileName = strPath + QDir::separator() + dir.dirName() + LABEL_SEQ_RESULT_SUFFIX;

    //加载的时候先清空
    m_stAllLabelInfo = LabelAnalysis::AllLabelInfo();
    QFileInfo fileInfo(strLsrFileName);
    if (!fileInfo.exists())
    {
        //如果没有标注结果,不算失败
        return true;
    }

    //存在这个路径,但是不是个文件
    if (!fileInfo.isFile())
    {
        return false;
    }

    bool bRet = LabelAnalysis::ReadLabelFile(strLsrFileName.toLocal8Bit().data(), m_stAllLabelInfo);
    if (bRet)
    {
        if(!m_stAllLabelInfo.mapLabelProperty.empty())
        {
            m_mapLabelProperty.clear();
            m_mapLabelProperty = m_stAllLabelInfo.mapLabelProperty;
        }
        else
        {
            if (m_mapLabelProperty.empty())
            {
                setDefaultLabelProperty();
            }
        }
        emit sigColorMapChanged();
    }
    if (m_stAllLabelInfo.stFileInfo.strFilePath.empty())
    {
        m_stAllLabelInfo.stFileInfo.strFilePath = m_stImageHeaderInfo.strFilePath.toLocal8Bit().data();
        m_stAllLabelInfo.stFileInfo.strPatientName = m_stImageHeaderInfo.strPatientName.toLocal8Bit().data();
        m_stAllLabelInfo.stFileInfo.strPatientAge = m_stImageHeaderInfo.strPatientAge.toLocal8Bit().data();
        m_stAllLabelInfo.stFileInfo.strPatientSex = m_stImageHeaderInfo.strPatientSex.toLocal8Bit().data();
        m_stAllLabelInfo.stFileInfo.nFileType = emImageType;
        m_stAllLabelInfo.stFileInfo.nWidth = m_stImageHeaderInfo.nWidth;
        m_stAllLabelInfo.stFileInfo.nHeight = m_stImageHeaderInfo.nHeight;
        m_stAllLabelInfo.stFileInfo.nThickness = m_stImageHeaderInfo.nThickNess;
        m_stAllLabelInfo.stFileInfo.fSpacing = m_stImageHeaderInfo.fPixelSpacingT;
    }
    return bRet;
}

/******************************************************** 
*  @function : PolygonShader
*  @brief    : 渲染多边形，这个已经被拿掉，在保存线程中做处理
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void ImageDataManager::PolygonShader(VIEW_PLANE emPlane, int nIndex, QPolygon poly, int nColorIndex)
{
    ImageType3D::RegionType inputRegion = m_pItkNrrdSerial->GetLargestPossibleRegion();
    ImageType3D::SizeType size = inputRegion.GetSize();
    QRect rect = poly.boundingRect();

    switch (emPlane)
    {
    case SagittalPlane:
        for (int i = rect.left(); i <= rect.right(); i++)
        {
            for (int j = rect.top(); j <= rect.bottom(); j++)
            {
                if (poly.containsPoint(QPoint(i, j),Qt::OddEvenFill))
                {
                    m_pItkNrrdSerial->SetPixel({ nIndex,i, j }, nColorIndex);
                    m_pItkNiiSerial->SetPixel({ nIndex,i, j }, m_pItkImgSerial->GetPixel({ nIndex,i, j }));
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
                    m_pItkNrrdSerial->SetPixel({ i,nIndex, j }, nColorIndex);
                    m_pItkNiiSerial->SetPixel({ i,nIndex, j }, m_pItkImgSerial->GetPixel({ i,nIndex, j }));
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
                    m_pItkNrrdSerial->SetPixel({ i, j,nIndex }, nColorIndex);
                    m_pItkNiiSerial->SetPixel({ i, j,nIndex }, m_pItkImgSerial->GetPixel({ i, j,nIndex }));
                }
            }
        }
        break;
    default:
        break;
    }
    

}

/******************************************************** 
*  @function : updateLabelVoxel
*  @brief    : 更新目标体绘制
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
bool ImageDataManager::updateLabelVoxel(VIEW_PLANE emPlane, int nIndex, const std::map<int, ExPolygonF> &mapTargets)
{
    if (m_pItkNrrdSerial==nullptr)
    {
        return false;
    }

    int nWidth = 0;
    int nHeight = 0;
    switch (emPlane)
    {
    case SagittalPlane:
        nWidth = m_stImageHeaderInfo.nHeight;
        nHeight = m_stImageHeaderInfo.nThickNess;
        break;
    case CoronalPlane:
        nWidth = m_stImageHeaderInfo.nWidth;
        nHeight = m_stImageHeaderInfo.nThickNess;
        break;
    case TransversePlane:
        nWidth = m_stImageHeaderInfo.nWidth;
        nHeight = m_stImageHeaderInfo.nHeight;
        break;
    default:
        break;
    }
    for each (auto var in mapTargets)
    {
        QPolygon poly;
        int nColorIndex = getLabelPpIndex(var.second.strTargetName);
        for each (auto point in var.second)
        {
            poly.push_back(QPoint(round(point.rx()*nWidth), round(point.ry()*nHeight)));
        }
        poly.push_back(poly.first());
        PolygonShader(emPlane, nIndex, poly, nColorIndex);
    }
    
    return true;
}

/******************************************************** 
*  @function : updateLabelInfo
*  @brief    : 根据视图和帧号更新目标信息
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
bool ImageDataManager::updateLabelInfo(VIEW_PLANE emPlane, int nIndex,const std::map<int, ExPolygonF> &mapPolygons)
{
    std::map<int, LabelAnalysis::Target> mapTargets;
    for each (auto var in mapPolygons)
    {
        LabelAnalysis::Target stTarget;
        stTarget.nTargetID = var.second.nTargetID;
        stTarget.nTargetType = var.second.nTargetType;
        stTarget.lstVertex.clear();
        stTarget.strTargetName = var.second.strTargetName;
        for (int i = 0; i < var.second.size(); i++)
        {
            LabelAnalysis::Vertex stVertex;
            stVertex.fX = var.second[i].rx();
            stVertex.fY = var.second[i].ry();
            stVertex.nValue = var.second.lstValue[i];
            stTarget.lstVertex.push_back(stVertex);
        }
        mapTargets[var.first] = stTarget;
    }
    QMutexLocker locker(&m_muxLabel);
    std::map<int, std::map<int, LabelAnalysis::Target>>::iterator it;
    switch (emPlane)
    {
    case SagittalPlane:

        it = m_stAllLabelInfo.mapSPTargets.find(nIndex);
        if (it == m_stAllLabelInfo.mapSPTargets.end())
        {
            m_stAllLabelInfo.mapSPTargets[nIndex] = mapTargets;
        }
        else
        {
            it->second = mapTargets;
        }

        break;
    case CoronalPlane:

        it = m_stAllLabelInfo.mapCPTargets.find(nIndex);
        if (it == m_stAllLabelInfo.mapCPTargets.end())
        {
            m_stAllLabelInfo.mapCPTargets[nIndex] = mapTargets;
        }
        else
        {
            it->second = mapTargets;
        }
        break;
    case TransversePlane:
        it = m_stAllLabelInfo.mapTPTargets.find(nIndex);
        if (it == m_stAllLabelInfo.mapTPTargets.end())
        {
            m_stAllLabelInfo.mapTPTargets[nIndex] = mapTargets;
        }
        else
        {
            it->second = mapTargets;
        }
        break;
    default:
        return false;
        break;
    }
    //updateLabelVoxel(emPlane, nIndex, mapPolygons);
    return true;
}

/******************************************************** 
*  @function : getLabelInfo
*  @brief    : 根据视图和帧号获取标签信息
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
bool ImageDataManager::getLabelInfo(VIEW_PLANE emPlane, int nIndex, std::map<int, LabelAnalysis::Target> &mapTargets)
{
    QMutexLocker locker(&m_muxLabel);

    switch (emPlane)
    {
    case SagittalPlane:
        if (m_stAllLabelInfo.mapSPTargets.end()!= m_stAllLabelInfo.mapSPTargets.find(nIndex))
        {
            mapTargets = m_stAllLabelInfo.mapSPTargets[nIndex];
        }
        else
        {
            return false;
        }
        break;
    case CoronalPlane:
        if (m_stAllLabelInfo.mapCPTargets.end() != m_stAllLabelInfo.mapCPTargets.find(nIndex))
        {
            mapTargets = m_stAllLabelInfo.mapCPTargets[nIndex];
        }
        else
        {
            return false;
        }
        break;
    case TransversePlane:
        if (m_stAllLabelInfo.mapTPTargets.end() != m_stAllLabelInfo.mapTPTargets.find(nIndex))
        {
            mapTargets = m_stAllLabelInfo.mapTPTargets[nIndex];
        }
        else
        {
            return false;
        }
        break;
    default:
        return false;
        break;
    }
    return true;
}

/******************************************************** 
*  @function : getAllLabelInfo
*  @brief    : 获取所有结构化信息
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
bool ImageDataManager::getAllLabelInfo(LabelAnalysis::AllLabelInfo &stAllLabelInfo)
{
    QMutexLocker locker(&m_muxLabel);
    m_stAllLabelInfo.mapLabelProperty = m_mapLabelProperty;
    stAllLabelInfo = m_stAllLabelInfo;
    return true;
}

/******************************************************** 
*  @function : getTargetWH
*  @brief    : 获取目标在别的页面上的投影出的情况
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void ImageDataManager::getTargetWH(const std::map<int, std::map<int, LabelAnalysis::Target>> &mapTargets
    , QList<ExLineF> &lstWHTargets, int nTotalIndex, bool bWH, bool bNeedT)
{
    for each (auto var in mapTargets)
    {
        for each (auto tgt in var.second)
        {
            if (tgt.second.lstVertex.size() > 0)
            {
                LabelAnalysis::Vertex stVertex = tgt.second.lstVertex.front();
                QRectF rectF(stVertex.fX, stVertex.fY, 0.0f, 0.0f);
                for each (auto pt in tgt.second.lstVertex)
                {
                    if (pt.fX < rectF.left())
                    {
                        rectF.setLeft(pt.fX);
                    }
                    if (pt.fX > rectF.right())
                    {
                        rectF.setRight(pt.fX);
                    }
                    if (pt.fY < rectF.top())
                    {
                        rectF.setTop(pt.fY);
                    }
                    if (pt.fY > rectF.bottom())
                    {
                        rectF.setBottom(pt.fY);
                    }
                }
                ExLineF stExLineF;
                stExLineF.strTargetName = tgt.second.strTargetName;
                if (bWH)//宽
                {
                    if (bNeedT)
                    {
                        stExLineF.setP1(QPointF((float)var.first / nTotalIndex, rectF.left()));
                        stExLineF.setP2(QPointF((float)var.first / nTotalIndex, rectF.right()));
                    }
                    else
                    {
                        stExLineF.setP1(QPointF(rectF.left(), (float)var.first / nTotalIndex));
                        stExLineF.setP2(QPointF(rectF.right(), (float)var.first / nTotalIndex));
                    }
                }
                else//高
                {
                    if (bNeedT)
                    {
                        stExLineF.setP1(QPointF(rectF.top(), (float)var.first / nTotalIndex));
                        stExLineF.setP2(QPointF(rectF.bottom(), (float)var.first / nTotalIndex));
                    }
                    else
                    {
                        stExLineF.setP1(QPointF((float)var.first / nTotalIndex, rectF.top()));
                        stExLineF.setP2(QPointF((float)var.first / nTotalIndex, rectF.bottom()));
                    }

                }
                lstWHTargets.push_back(stExLineF);
            }
        }
    }
}

/******************************************************** 
*  @function : getOtherPlaneTargetWH
*  @brief    : 获取目标在别的页面上的投影出的情况
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
bool ImageDataManager::getOtherPlaneTargetWH(VIEW_PLANE emCurPlane, QList<ExLineF> &mapWTargets, QList<ExLineF> &mapHTargets)
{
    QMutexLocker locker(&m_muxLabel);

    switch (emCurPlane)
    {
    case SagittalPlane:
        getTargetWH(m_stAllLabelInfo.mapCPTargets, mapHTargets, m_stImageHeaderInfo.nHeight, TARGET_H);//OK
        getTargetWH(m_stAllLabelInfo.mapTPTargets, mapWTargets, m_stImageHeaderInfo.nThickNess, TARGET_H, true);//OK
        //m_stAllLabelInfo.mapSPTargets;
        break;
    case CoronalPlane:
        getTargetWH(m_stAllLabelInfo.mapSPTargets, mapHTargets, m_stImageHeaderInfo.nWidth, TARGET_H);
        getTargetWH(m_stAllLabelInfo.mapTPTargets, mapWTargets, m_stImageHeaderInfo.nThickNess, TARGET_W);//OK
        break;
    case TransversePlane:
        getTargetWH(m_stAllLabelInfo.mapSPTargets, mapHTargets, m_stImageHeaderInfo.nWidth, TARGET_W, true);
        getTargetWH(m_stAllLabelInfo.mapCPTargets, mapWTargets, m_stImageHeaderInfo.nHeight, TARGET_W);
        break;
    default:
        return false;
        break;
    }
    return true;
}

/******************************************************** 
*  @function : saveLabelInfo
*  @brief    : 保存结构化数据
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
bool ImageDataManager::saveLabelInfo()
{
    QString strLsrFileName;
    QFileInfo fileInfo(m_stImageHeaderInfo.strFilePath);
    if (fileInfo.isFile())
    {
        strLsrFileName = fileInfo.dir().path() + QDir::separator() + fileInfo.completeBaseName() + LABEL_SEQ_RESULT_SUFFIX;
    }
    else if (fileInfo.isDir())
    {
        QDir dir(m_stImageHeaderInfo.strFilePath);
        strLsrFileName = m_stImageHeaderInfo.strFilePath + QDir::separator() + dir.dirName() + LABEL_SEQ_RESULT_SUFFIX;
    }
    m_stAllLabelInfo.mapLabelProperty = m_mapLabelProperty;

    
    bool bRet = LabelAnalysis::WriteLabelFile(strLsrFileName.toLocal8Bit().data(), m_stAllLabelInfo);
    /*typedef  itk::ImageFileWriter<ImageType3D> WriterType;
    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(".\\a.nrrd");
    writer->SetInput(m_pItkNrrdSerial);
    WriterType::Pointer writer2 = WriterType::New();
    writer2->SetFileName(".\\a.nii");
    writer2->SetInput(m_pItkNiiSerial);
    try
    {
        writer->Update();
        writer2->Update();
    }
    catch (itk::ExceptionObject &ex)
    {
        std::string str(ex.what());
        std::cout << ex;
        return false;
    }*/
    return bRet;
    
}

/******************************************************** 
*  @function : getImageInfo
*  @brief    : 获取源文件的头结构信息
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
bool ImageDataManager::getImageInfo(ImageType3D::Pointer &pImage)
{
    if (nullptr == m_pItkImgSerial)
    {
        return false;
    }
    ImageType3D::RegionType region(m_pItkImgSerial->GetBufferedRegion());
    pImage->SetRegions(region);
    pImage->Allocate();
    pImage->FillBuffer(itk::NumericTraits< ImageType3D::PixelType >::Zero);
    pImage->CopyInformation(m_pItkImgSerial);

    return true;
}

/******************************************************** 
*  @function : getNrrdImage
*  @brief    : 已抛弃
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
bool ImageDataManager::getNrrdImage(ImageType3D::Pointer &pNrrdImage)
{
    if (m_pItkNrrdSerial==nullptr)
    {
        return false;
    }
    else
    {
        if (pNrrdImage == nullptr)
        {
            pNrrdImage = ImageType3D::New();
        }
        ImageType3D::RegionType region(m_pItkNrrdSerial->GetBufferedRegion());
        pNrrdImage->SetRegions(region);
        pNrrdImage->Allocate();
        //pNrrdImage->FillBuffer(itk::NumericTraits< ImageType3D::PixelType >::Zero);
        pNrrdImage->Graft(m_pItkNrrdSerial);
    }
    return true;
}

/******************************************************** 
*  @function : getNiiImage
*  @brief    : 已抛弃
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
bool ImageDataManager::getNiiImage(ImageType3D::Pointer &pNiiImage)
{
    if (m_pItkNiiSerial == nullptr)
    {
        return false;
    }
    else
    {
        if (pNiiImage == nullptr)
        {
            pNiiImage = ImageType3D::New();
        }
        ImageType3D::RegionType region(m_pItkNiiSerial->GetBufferedRegion());
        pNiiImage->SetRegions(region);
        pNiiImage->Allocate();
        //pNiiImage->CopyInformation(m_pItkNiiSerial);
        //pNiiImage->FillBuffer(itk::NumericTraits< ImageType3D::PixelType >::Zero);
        pNiiImage->Graft(m_pItkNiiSerial);
        //pNiiImage = m_pItkNiiSerial;
        //pNiiImage->CopyInformation(m_pItkVtkShowSerial);
    }

    return true;
}

/******************************************************** 
*  @function :  getImageHeaderInfo
*  @brief    :  brief 
*  @input    : 
*  @output   : 
*  @return   : 
*********************************************************/
ImageHeaderInfo ImageDataManager::getImageHeaderInfo()
{
    return m_stImageHeaderInfo;
}

/******************************************************** 
*  @function :  getImage
*  @brief    :  brief 
*  @input    : 
*  @output   : 
*  @return   : 
*********************************************************/
/*
bool ImageDataManager::getImage(int nIndex, QImage& img, VIEW_PLANE emViewPlane)
{
    QMutexLocker locker(&m_muxData);
    try
    {
        SliceType::Pointer pSlice = SliceType::New();
        pSlice->SetDirectionCollapseToIdentity();
        pSlice->SetInput(m_pItkImgSerial); //reader的输出
        itk::Size<3> sliceSize = m_pItkImgSerial->GetLargestPossibleRegion().GetSize();
        itk::Index<3> sliceStart = m_pItkImgSerial->GetLargestPossibleRegion().GetIndex();

        int nCurPlaneSize = sliceSize[emViewPlane];
        sliceSize[emViewPlane] = 0;//设方向的大小为0
        sliceStart[emViewPlane] = nIndex;//切第n片

        if (nIndex < 0 || nIndex >= nCurPlaneSize)
        {
            return false;
        }

        ImageType3D::RegionType desiredRegion;
        desiredRegion.SetSize(sliceSize);
        desiredRegion.SetIndex(sliceStart);
        pSlice->SetExtractionRegion(desiredRegion);
        try
        {
            pSlice->Update();
        }
        catch (itk::ExceptionObject &ex)
        {
            qDebug() << ex.what();
            return false;
        }

        ImageType2D *img2D = pSlice->GetOutput();
        cv::Mat matImg = itk::OpenCVImageBridge::ITKImageToCVMat(img2D);
        matImg.convertTo(matImg, CV_8U);
        QImage img2Copy(matImg.data, matImg.cols, matImg.rows, matImg.step, QImage::Format_Grayscale8);
        img = img2Copy.copy();
        if (img.isNull())
        {
            return false;
        }
        return true;
    }
    catch (itk::ExceptionObject &ex)
    {
        qDebug() << ex.what();
        return false;
    }
    return true;
}
*/
bool ImageDataManager::getSliceImage(int nIndex, SliceType::Pointer pSlice, VIEW_PLANE emViewPlane)
{
    if (nullptr == pSlice || nullptr == m_pItkImgSerial)
    {
        return false;
    }
    QMutexLocker locker(&m_muxSrcData);
    try
    {
        pSlice->SetDirectionCollapseToIdentity();
        pSlice->SetInput(m_pItkImgSerial); //reader的输出
        itk::Size<3> sliceSize = m_pItkImgSerial->GetLargestPossibleRegion().GetSize();
        itk::Index<3> sliceStart = m_pItkImgSerial->GetLargestPossibleRegion().GetIndex();

        int nCurPlaneSize = sliceSize[emViewPlane];
        sliceSize[emViewPlane] = 0;//设方向的大小为0
        sliceStart[emViewPlane] = nIndex;//切第n片

        if (nIndex < 0 || nIndex >= nCurPlaneSize)
        {
            return false;
        }

        ImageType3D::RegionType desiredRegion;
        desiredRegion.SetSize(sliceSize);
        desiredRegion.SetIndex(sliceStart);
        pSlice->SetExtractionRegion(desiredRegion);
        pSlice->UpdateLargestPossibleRegion();
        try
        {
            pSlice->Update();
        }
        catch (itk::ExceptionObject &ex)
        {
            qDebug() << ex.what();
            return false;
        }
        return true;
    }
    catch (itk::ExceptionObject &ex)
    {
        qDebug() << ex.what();
        return false;
    }
}

/******************************************************** 
*  @function : getPixelValue
*  @brief    : 获取像素值（物理值）
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
int ImageDataManager::getPixelValue(int x, int y, int z)
{
    QMutexLocker locker(&m_muxSrcData);
    return m_pItkImgSerial->GetPixel({x,y,z});
}

/******************************************************** 
*  @function : setDefaultLabelProperty
*  @brief    : 生成默认的标签颜色列表
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void ImageDataManager::setDefaultLabelProperty()
{
    m_mapLabelProperty.clear();
    int nColor;
    std::string strLabel;
    nColor = 0xffff0000;
    strLabel = std::string("Label0");
    m_mapLabelProperty[strLabel] = nColor;
    nColor = 0xff00ff00;
    strLabel = std::string("Label1");
    m_mapLabelProperty[strLabel] = nColor;
    nColor = 0xff0000ff;
    strLabel = std::string("Label2");
    m_mapLabelProperty[strLabel] = nColor;
    nColor = 0xffffff00;
    strLabel = std::string("Label3");
    m_mapLabelProperty[strLabel] = nColor;
}

/******************************************************** 
*  @function : updateLabelProperty
*  @brief    : 更新标签颜色列表
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void ImageDataManager::updateLabelProperty(const std::map<std::string, int> &mapLabelProperty)
{
    m_mapLabelProperty.clear();
    m_mapLabelProperty = mapLabelProperty;
}

/******************************************************** 
*  @function : getLabelProperty
*  @brief    : 获取标签颜色表
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void ImageDataManager::getLabelProperty(std::map<std::string, int> &mapLabelProperty)
{
    if (m_mapLabelProperty.empty())
    {
        setDefaultLabelProperty();
    }
    mapLabelProperty = m_mapLabelProperty;
}

/******************************************************** 
*  @function : updateLabelPpIndex
*  @brief    : 更新标签属性的序列顺序
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void ImageDataManager::updateLabelPpIndex()
{
    m_mapLabelIndex.clear();
    int i = 1;
    for each (auto var in m_mapLabelProperty)
    {
        m_mapLabelIndex[var.first] = i;
        ++i;
    }
}

/******************************************************** 
*  @function : getLabelPpIndex
*  @brief    : 根据标签名获取索引位置
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
int ImageDataManager::getLabelPpIndex(const std::string &strLabelName)
{
    auto it = m_mapLabelIndex.find(strLabelName);
    return it->second;
}

/******************************************************** 
*  @function : getLabelPpMap
*  @brief    : 获取标签索引表
*  @input    : 
*  @output   : 
*  @return   :
*********************************************************/
void ImageDataManager::getLabelPpMap(std::map<std::string, int> &mapLabelIndex)
{
    mapLabelIndex = m_mapLabelIndex;
}

