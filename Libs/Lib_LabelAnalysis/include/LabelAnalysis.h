/******************************************************************************
*  Copyright (C), 2017-2019,Advanced Technology Institute of Suzhou. 
*  THIS IS AN UNPUBLISHED WORK CONTAINING CONFIDENTIAL AND PROPRIETARY 
*   
*******************************************************************************
*  File Name: LabelAnalysis.h
*  Author: qh.zhang@atisz.ac.cn
*  Version: 1.0.0
*  Date: 2020-3-23
*  Description: LRS�ṹ��UltImageTKΪ�˷��㱣��Ͷ�ȡ�ѱ�ע�ļ��������һ�����л������صĸ�ʽ
*  History: 
******************************************************************************/
#pragma once
#include <string.h>
#include <vector>
#include <list>
#include <map>

#ifdef LABELANALYSIS_EXPORTS
#define EXPORTS_LABEL _declspec(dllexport)
#else
#define EXPORTS_LABEL _declspec(dllimport)
#endif


#ifdef __cplusplus

extern "C"
{
    namespace LabelAnalysis
    {

        /********************************************************
        *  @struct  :  Vertex
        *  @brief   :  һ�������Ϣ
        *  @details :  һ�����λ�ú�����ֵ��HU��
        *********************************************************/
        struct Vertex
        {
            float fX;   //�ڵ�ǰ��ͼ�µ�x����
            float fY;   //�ڵ�ǰ��ͼ�µ�y����
            int   nValue;   //HUֵ
        };

        /********************************************************
        *  @struct  :  Target
        *  @brief   :  ÿһ��Ŀ�����Ϣ
        *  @details :  ��Ǻõ�ÿһ��Ŀ��ROI����Ϣ
        *********************************************************/
        struct Target
        {
            std::string strTargetName;  //Ŀ������-��ǩֵ
            std::string strTargetDisc;  //Ŀ������-��ǩ����
            int         nTargetID;      //Ŀ���ڵ�ǰ��ͼ��ǰ���ID
            int         nTargetType;    //Ŀ�꼸����״����
            std::list<Vertex> lstVertex;    //Ŀ�������㼯
        };

        /********************************************************
        *  @struct  :  FileInfo
        *  @brief   :  Դͼ���ļ�����Ϣ
        *  @details :  Դ�ļ���һЩ��ԱȽ���Ҫ����Ϣ
        *********************************************************/
        struct FileInfo
        {
            std::string strFilePath;    //Դ�ļ�·�������ļ���·��
            std::string strPatientName; //������
            std::string strPatientAge;  //��������
            std::string strPatientSex;  //�����Ա�
            int         nFileType;      //�ļ�����
            int         nWidth;         //��
            int         nHeight;        //��
            int         nThickness;     //����
            float       fSpacing;       //�������
        };

        /********************************************************
        *  @struct  :  AllLabelInfo
        *  @brief   :  һ���ļ���ͼ�����е�������Ϣ
        *  @details : ����Դͼ���ļ�����Ϣ���ѱ�ע�ı�ǩ��Ϣ���Լ���ע�ڸ�����ͼ���ϵ�ROI��Ϣ
        *********************************************************/
        struct AllLabelInfo
        {
            int     nCurVersion;    //����
            FileInfo    stFileInfo; //��ȡ���ļ���Ϣ
            std::map<std::string, int>      mapLabelProperty;        //label����ɫ�Ͷ���
            std::map<int, std::map<int, Target>>     mapSPTargets;  //ʸ״��,֡�ź�Ŀ���б�
            std::map<int, std::map<int, Target>>     mapCPTargets;  //��״��,֡�ź�Ŀ���б�
            std::map<int, std::map<int, Target>>     mapTPTargets;  //�����,֡�ź�Ŀ���б�
        };

        /********************************************************
        *  @function : ReadLabelFile
        *  @brief    : ��ȡpPath·���µ�lrs�ļ�
        *  @input    : pPath ��·��
        *  @output   : stAllLabelInfo LRS���ݽṹ
        *  @return   : bool �ɹ����
        *********************************************************/
        EXPORTS_LABEL bool ReadLabelFile(char* pPath, AllLabelInfo &stAllLabelInfo);

        /********************************************************
        *  @function : WriteLabelFile
        *  @brief    : ��pPath·��дlrs�ļ�
        *  @input    : pPath д·��
        *  @input    : stAllLabelInfo LRS���ݽṹ
        *  @return   : bool �ɹ����
        *********************************************************/
        EXPORTS_LABEL bool WriteLabelFile(char* pPath, AllLabelInfo stAllLabelInfo);
    }
}

#endif
