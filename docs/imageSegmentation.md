# Image Segmentation
<center>

![process_of_radiomics_image_acquisition](./imgs/pipeline_2.png)

</center>

- Image segmentation refers to split the regions of interest out from the whole image. The radiomics features can be calculated from the regions of interest which sketched out from the medical images. Image segmentation is an extremely part, because it is the prerequisite of "feature extract," "feature select," and "model building." In this part, UltImageTK provides a rich and detailed experience for users.

## Tag Configuration
-  Users can customize the color and name of the segmentation brush here. ![](./imgs/setting_color.png)

## Perform a Segmentation
- In this current version, UltImageTK provides two tools, **`polygon`** and **`brush`**, to divide the target area. (`*We will update more flexible annotation tools in later versions`).

> - Polygon Annotation: Users can mark out regular regions of interest (ROI) point by point.
> - Brush Annotation: For the irregular ROI, users can mark them out by drawing continuous curves with the rush.


- Once the annotation has done, users can adjust the transparency of the marked area ![](./imgs/transparancy.png) to compare the marked area with the original image. This helps adjust the marked area.

- Modify the Marked ROI
   > * For the ROI annotated by polygon or brush, left-click to select the graphic and drag it to another position.
   > * To adjust the shape of the ROI, left-click to select the graphic and drag the position of points that need to be adjusted.

- Check the Parameters of ROI
   > * After annotating the ROI, select the graphic, and parameters of the shape will be shown in the upper left corner of the current view. Users will find some useful information such as the width, height, and area of the ROI to help them understand the geometric property clearly.



## Save the Result of Segmentation
- After annotating the ROI, you need to save the result of segmentation by clicking "saving" button ![](./imgs/save_btn.png), the result will stored in your customized path as "\*.lsr" format. "\*.lsr" is a special format developed by us, users can confirm the structure of the "\*.lsr" file in [Introduction to LRS Data Structure](zh-cn/analysisApi).
- After saving, the annotation graphic will be loaded when you re-open this file (do not move the "\*.lsr" file to another path).
- After saving, you can choose to export the result of segmentation from UltImageTK in three formats, Dicom, Nifit, or LSR. You can customize the storage path as well.