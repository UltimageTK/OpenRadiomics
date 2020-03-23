#pragma once

#include <QObject>
#include <QImage>
#include <QMutex>
#include <vector>
#include "GlobalDef.h"
#include "LabelAnalysis.h"
#include "DrawBase.h"

class ImageDataManager : public QObject
{
    Q_OBJECT
private:
    ImageDataManager() {};
    ~ImageDataManager() {};
public:
    
    static ImageDataManager* getInstance()
    {
        if (nullptr == m_pInstance)
        {
            m_pInstance = new ImageDataManager;
        }
        return m_pInstance;
    }

    static void releaseInstance()
    {
        if (nullptr != m_pInstance)
        {
            delete m_pInstance;
            m_pInstance = nullptr;
        }
    }

    /*对原始数据加载之类的操作*/
    //获取和装载图片信息
    ImageHeaderInfo getImageHeaderInfo();
    void setHeaderInfo(ImageHeaderInfo stImageHeaderInfo);
    //设置窗宽窗位
    void setWindowLevel(float fWindow, float fCenter);

    //装载图片序列
    void setImageSerial(ImageType3D* pItkImgSerial);
    bool getImageSerial(ImageType3D* pItkImgSerial);
    //void getImageOriginPos(float &x,float &y,float &z);

    //获取切片图
    bool getSliceImage(int nIndex, SliceType::Pointer pSlice, VIEW_PLANE emViewPlane);

    //获取物理值
    int getPixelValue(int x, int y, int z);

    /*关于标记的一些接口定义*/
    //设置默认的标签属性
    void setDefaultLabelProperty();
    void updateLabelProperty(const std::map<std::string, int> &mapLabelProperty);
        //获取当前标签属性集合
    void getLabelProperty(std::map<std::string, int> &mapLabelProperty);
    void updateLabelPpIndex();
    int getLabelPpIndex(const std::string &strLabelName);
    void getLabelPpMap(std::map<std::string, int> &mapLabelIndex);

    //加载已有的标签结果
    bool loadLabelInfo(QString strPath);
    //获取某一帧的标签信息
    void PolygonShader(VIEW_PLANE emPlane, int nIndex, QPolygon poly,int nColorIndex);
    bool updateLabelVoxel(VIEW_PLANE emPlane, int nIndex, const std::map<int, ExPolygonF> &mapTargets);
    bool updateLabelInfo(VIEW_PLANE emPlane, int nIndex,const std::map<int, ExPolygonF> &mapTargets);
    
    //根据视图和帧号获取目标信息
    bool getLabelInfo(VIEW_PLANE emPlane, int nIndex, std::map<int, LabelAnalysis::Target> &mapTargets);
    //获取所有目标信息
    bool getAllLabelInfo(LabelAnalysis::AllLabelInfo &stAllLabelInfo);

    //获取其他界面的目标宽高，Target里面只存两个点连成线段
    void getTargetWH(const std::map<int, std::map<int, LabelAnalysis::Target>> &mapTargets,
        QList<ExLineF> &lstWHTargets,int nTotalIndex, bool bWH, bool bNeedT = false);//W->true H ->false,需不需要转置
    bool getOtherPlaneTargetWH(VIEW_PLANE emCurPlane, QList<ExLineF> &lstWTargets, QList<ExLineF> &lstHTargets);

    //保存结果
    bool saveLabelInfo();

    //拷贝文件头信息
    bool getImageInfo(ImageType3D::Pointer &pImage);

    //获取这些类型转换的图片，已被抛弃使用，等3D模块完善好就可以删掉
    bool getNrrdImage(ImageType3D::Pointer &pNrrdImage);
    bool getNiiImage(ImageType3D::Pointer &pNiiImage);
signals:
    void sigColorMapChanged();

private:
    static ImageDataManager* m_pInstance;

    QMutex              m_muxSrcData;
    ImageHeaderInfo     m_stImageHeaderInfo;            //存放图片和患者信息
    ImageType3D::Pointer        m_pItkImgSerial = nullptr;     //存放图片序列
    ImageType3D::Pointer        m_pItkNrrdSerial = nullptr;    //标注信息的序列，也是后来渲染3D图像的数据
    ImageType3D::Pointer        m_pItkNiiSerial = nullptr;    //标注信息的序列，也是后来渲染3D图像的数据
    std::map<std::string, int>      m_mapLabelProperty;        //label的颜色和定义   m_vecLabelProperty;             //标签属性信息
    std::map<std::string, int>     m_mapLabelIndex; //label对应的index
    QMutex              m_muxLabel;             //结构化数据锁
    LabelAnalysis::AllLabelInfo m_stAllLabelInfo;         //结构化数据
};
