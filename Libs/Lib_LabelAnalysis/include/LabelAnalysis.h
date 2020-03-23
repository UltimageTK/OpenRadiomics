/******************************************************************************
*  Copyright (C), 2017-2019,Advanced Technology Institute of Suzhou. 
*  THIS IS AN UNPUBLISHED WORK CONTAINING CONFIDENTIAL AND PROPRIETARY 
*   
*******************************************************************************
*  File Name: LabelAnalysis.h
*  Author: qh.zhang@atisz.ac.cn
*  Version: 1.0.0
*  Date: 2020-3-23
*  Description: LRS结构是UltImageTK为了方便保存和读取已标注文件而定义的一种序列化到本地的格式
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
        *  @brief   :  一个点的信息
        *  @details :  一个点的位置和物理值（HU）
        *********************************************************/
        struct Vertex
        {
            float fX;   //在当前视图下的x坐标
            float fY;   //在当前视图下的y坐标
            int   nValue;   //HU值
        };

        /********************************************************
        *  @struct  :  Target
        *  @brief   :  每一个目标的信息
        *  @details :  标记好的每一个目标ROI的信息
        *********************************************************/
        struct Target
        {
            std::string strTargetName;  //目标类型-标签值
            std::string strTargetDisc;  //目标描述-标签描述
            int         nTargetID;      //目标在当前视图当前层的ID
            int         nTargetType;    //目标几何形状类型
            std::list<Vertex> lstVertex;    //目标轮廓点集
        };

        /********************************************************
        *  @struct  :  FileInfo
        *  @brief   :  源图像文件的信息
        *  @details :  源文件中一些相对比较重要的信息
        *********************************************************/
        struct FileInfo
        {
            std::string strFilePath;    //源文件路径，或文件夹路径
            std::string strPatientName; //患者名
            std::string strPatientAge;  //患者年龄
            std::string strPatientSex;  //患者性别
            int         nFileType;      //文件类型
            int         nWidth;         //宽
            int         nHeight;        //高
            int         nThickness;     //层数
            float       fSpacing;       //间隔比例
        };

        /********************************************************
        *  @struct  :  AllLabelInfo
        *  @brief   :  一个文件或图像序列的所有信息
        *  @details : 包含源图像文件的信息，已标注的标签信息，以及标注在各个视图面上的ROI信息
        *********************************************************/
        struct AllLabelInfo
        {
            int     nCurVersion;    //本版
            FileInfo    stFileInfo; //读取的文件信息
            std::map<std::string, int>      mapLabelProperty;        //label的颜色和定义
            std::map<int, std::map<int, Target>>     mapSPTargets;  //矢状面,帧号和目标列表
            std::map<int, std::map<int, Target>>     mapCPTargets;  //冠状面,帧号和目标列表
            std::map<int, std::map<int, Target>>     mapTPTargets;  //横断面,帧号和目标列表
        };

        /********************************************************
        *  @function : ReadLabelFile
        *  @brief    : 读取pPath路径下的lrs文件
        *  @input    : pPath 读路径
        *  @output   : stAllLabelInfo LRS数据结构
        *  @return   : bool 成功与否
        *********************************************************/
        EXPORTS_LABEL bool ReadLabelFile(char* pPath, AllLabelInfo &stAllLabelInfo);

        /********************************************************
        *  @function : WriteLabelFile
        *  @brief    : 往pPath路径写lrs文件
        *  @input    : pPath 写路径
        *  @input    : stAllLabelInfo LRS数据结构
        *  @return   : bool 成功与否
        *********************************************************/
        EXPORTS_LABEL bool WriteLabelFile(char* pPath, AllLabelInfo stAllLabelInfo);
    }
}

#endif
