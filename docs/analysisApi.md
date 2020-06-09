# Introduction to LRS Data Structure

We defined a structure named LRS for UltImageTK to import and export the annotation data. Only two interfaces are provided, reading, and writing, for efficiency.


- ** The namespace of the LRS data structure is `LabelAnalysis`**

> ## Read File
```C++
    /******************************************************** 
    *  @function : ReadLabelFile
    *  @brief    : Read lrs file under pPath
    *  @input    : pPath             The path where the file is stored
    *  @output   : stAllLabelInfo    LRS data structure
    *  @return   : bool              Success or not
    *********************************************************/
    EXPORTS_LABEL bool ReadLabelFile(char* pPath, AllLabelInfo &stAllLabelInfo);
```

> ## Write File
```C++
    /********************************************************
    *  @function : WriteLabelFile
    *  @brief    : Write lrs file to pPath
    *  @input    : pPath            The path where the file should be stored
    *  @input    : stAllLabelInfo   LRS data structure
    *  @return   : bool             Success or not
    *********************************************************/
    EXPORTS_LABEL bool WriteLabelFile(char* pPath, AllLabelInfo stAllLabelInfo);
```

> ## Introduction of Internal Structure

> ### `AllLabelInfo`
```C++
    /******************************************************** 
    *  @struct  :  AllLabelInfo
    *  @brief   :  All information about a file or an image sequence
    *  @details :  Contains information about the source image,
    *              annotated tags, annotated ROI on each views
    *********************************************************/
    struct AllLabelInfo
    {
        int     nCurVersion;    // Current version No.
        FileInfo    stFileInfo; // Information loaded
        std::map<std::string,int>      mapLabelProperty;        //Color and definition of the label
        std::map<int, std::map<int, Target>>     mapSPTargets;  //Frame No. and target list, sagittal plane
        std::map<int, std::map<int,Target>>     mapCPTargets;  //Frame No. and target list, coronal plane
        std::map<int, std::map<int, Target>>     mapTPTargets;  //Frame No. and target list, transverse plane
    };
```

> ### `FileInfo`
```C++
    /********************************************************
    *  @struct  :  FileInfo
    *  @brief   :  Information about the source image
    *  @details :  Some important information in the source file
    *********************************************************/
    struct FileInfo
    {
        std::string strFilePath;    //The path of the source file or the folder
        std::string strPatientName; //Patient's name
        std::string strPatientAge;  //Patient's Age
        std::string strPatientSex;  //Patient's gender
        int         nFileType;      //Type of the file
        int         nWidth;         //Width
        int         nHeight;        //Height
        int         nThickness;     //Thickness
        float       fSpacing;       //Spacing
    };
```

> ### `Target`
```C++
    /********************************************************
    *  @struct  :  Target
    *  @brief   :  Information about each target
    *  @details :  Information about ROI of each target
    *********************************************************/
    struct Target
    {
        std::string strTargetName;  //Type of the target - tag's value
        std::string strTargetDisc;  //Description of the target - tag's description
        int         nTargetID;      //ID of the target in the current layer of the current view 
        int         nTargetType;    //Geometry type of the target
        std::list<Vertex> lstVertex;    //Vertex list of the target
    };
```

> ### `Vertex`
```C++
    /********************************************************
    *  @struct  :  Vertex
    *  @brief   :  The information about one point
    *  @details :  The position and CT value (HU) of one point
    *********************************************************/
    struct Vertex
    {
        float fX;   //x Coordinate in current view
        float fY;   //y Coordinate in current view
        int   nValue;   //CT value (HU)
    };
```
