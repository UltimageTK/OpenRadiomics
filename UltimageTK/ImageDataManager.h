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

    /*��ԭʼ���ݼ���֮��Ĳ���*/
    //��ȡ��װ��ͼƬ��Ϣ
    ImageHeaderInfo getImageHeaderInfo();
    void setHeaderInfo(ImageHeaderInfo stImageHeaderInfo);
    //���ô���λ
    void setWindowLevel(float fWindow, float fCenter);

    //װ��ͼƬ����
    void setImageSerial(ImageType3D* pItkImgSerial);
    bool getImageSerial(ImageType3D* pItkImgSerial);
    //void getImageOriginPos(float &x,float &y,float &z);

    //��ȡ��Ƭͼ
    bool getSliceImage(int nIndex, SliceType::Pointer pSlice, VIEW_PLANE emViewPlane);

    //��ȡ����ֵ
    int getPixelValue(int x, int y, int z);

    /*���ڱ�ǵ�һЩ�ӿڶ���*/
    //����Ĭ�ϵı�ǩ����
    void setDefaultLabelProperty();
    void updateLabelProperty(const std::map<std::string, int> &mapLabelProperty);
        //��ȡ��ǰ��ǩ���Լ���
    void getLabelProperty(std::map<std::string, int> &mapLabelProperty);
    void updateLabelPpIndex();
    int getLabelPpIndex(const std::string &strLabelName);
    void getLabelPpMap(std::map<std::string, int> &mapLabelIndex);

    //�������еı�ǩ���
    bool loadLabelInfo(QString strPath);
    //��ȡĳһ֡�ı�ǩ��Ϣ
    void PolygonShader(VIEW_PLANE emPlane, int nIndex, QPolygon poly,int nColorIndex);
    bool updateLabelVoxel(VIEW_PLANE emPlane, int nIndex, const std::map<int, ExPolygonF> &mapTargets);
    bool updateLabelInfo(VIEW_PLANE emPlane, int nIndex,const std::map<int, ExPolygonF> &mapTargets);
    
    //������ͼ��֡�Ż�ȡĿ����Ϣ
    bool getLabelInfo(VIEW_PLANE emPlane, int nIndex, std::map<int, LabelAnalysis::Target> &mapTargets);
    //��ȡ����Ŀ����Ϣ
    bool getAllLabelInfo(LabelAnalysis::AllLabelInfo &stAllLabelInfo);

    //��ȡ���������Ŀ���ߣ�Target����ֻ�������������߶�
    void getTargetWH(const std::map<int, std::map<int, LabelAnalysis::Target>> &mapTargets,
        QList<ExLineF> &lstWHTargets,int nTotalIndex, bool bWH, bool bNeedT = false);//W->true H ->false,�費��Ҫת��
    bool getOtherPlaneTargetWH(VIEW_PLANE emCurPlane, QList<ExLineF> &lstWTargets, QList<ExLineF> &lstHTargets);

    //������
    bool saveLabelInfo();

    //�����ļ�ͷ��Ϣ
    bool getImageInfo(ImageType3D::Pointer &pImage);

    //��ȡ��Щ����ת����ͼƬ���ѱ�����ʹ�ã���3Dģ�����ƺþͿ���ɾ��
    bool getNrrdImage(ImageType3D::Pointer &pNrrdImage);
    bool getNiiImage(ImageType3D::Pointer &pNiiImage);
signals:
    void sigColorMapChanged();

private:
    static ImageDataManager* m_pInstance;

    QMutex              m_muxSrcData;
    ImageHeaderInfo     m_stImageHeaderInfo;            //���ͼƬ�ͻ�����Ϣ
    ImageType3D::Pointer        m_pItkImgSerial = nullptr;     //���ͼƬ����
    ImageType3D::Pointer        m_pItkNrrdSerial = nullptr;    //��ע��Ϣ�����У�Ҳ�Ǻ�����Ⱦ3Dͼ�������
    ImageType3D::Pointer        m_pItkNiiSerial = nullptr;    //��ע��Ϣ�����У�Ҳ�Ǻ�����Ⱦ3Dͼ�������
    std::map<std::string, int>      m_mapLabelProperty;        //label����ɫ�Ͷ���   m_vecLabelProperty;             //��ǩ������Ϣ
    std::map<std::string, int>     m_mapLabelIndex; //label��Ӧ��index
    QMutex              m_muxLabel;             //�ṹ��������
    LabelAnalysis::AllLabelInfo m_stAllLabelInfo;         //�ṹ������
};
