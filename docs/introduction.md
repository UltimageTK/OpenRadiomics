# Introduction
- Here is a brief introduction to our medical imaging processing tool: UltImageTK. It is easy to read images via this software; besides, we also provide a large number of essential tools and some powerful radiomics modules. In this document, we'll indicate each step in the process of radiomics.
![Process of Radiomics](./imgs/pipeline.png)
- Feel free to use this tool, and even better, our basic version is open source. This will boost the productivity of healthcare product developers like yours. Please pull the source code on [UltimageTK](https://github.com/UltimageTK/OpenRadiomics).
- This software is suitable for healthcare practitioners, educators, and researchers.
## Installation

> - System requirements: Windows(*`x64 Windows 7 or Windows 10`*). 
> - The basic version of UltimageTK is available [here](https://obs-huay-website.obs.cn-north-4.myhuaweicloud.com/download/UltimageTK_1.0_Installer_x64_win.exe).
> - Double-click on the executable file (`UltimageTK-Basic-Installer.exe`), choose the installation directory and complete the installation procedure.

## Table of Contents

- There are four parts to the introduction: `Image Reading`、`Segmentation`、`Tools` and `Radiomics Modules`.
- The knowledge base part is at the end of TOC, and we will update elementary knowledge about radiomics modules in this part. The knowledge base is important for using and learning this software, notably for newbies.

## Module Function
- **Image Reading:**
 > Supported file formats: Dicom and Nifti.
- **Image Segmentation:**
 > Splitting the image by category with brush or polygon tool, and the result could be store to local disk.
- **Image Preprocessing:**
 > Including format conversion, histogram matching, and image registration, etc.
- **Radiomics Processing:**
 > Including feature extraction, feature selection, and so on.