# MedImgParser

+ Instruction:
   + This tool aims to parse different medical image formats to pure raw format (in float32), and also print the spatial information on the screen if there is. 
   + How to convert ***.raw**: additional tools to convert any common used image format: 
     > ***./MedImg2Raw filepath***. 
   + Supporting formats: 
     + **2D formats: JPG, PNG, BMP.** 
     + **3D formats: .nrrd, .nii, .nii.gz, .dcm**
+ Example usage: 
   + 2D image: 
     > **MedImg2Raw /home/ultrast-s1/testImage.png**
   + 3D image: 
     > **MedImg2Raw /home/ultrast-s1/testImage.nrrd**
     
    + Result (.raw) will be saved to the same path. Spatial information will be printed on the screen. 
