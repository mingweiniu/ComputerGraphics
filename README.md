# ComputerGraphics  
  
## interactive ray tracing  
程式碼：(不公開)  
工具: CMake for C++, GLSL/ Boost, glew, glfw, glm, OpenCV, Tinyobjloader  
簡述：碩論為改良一篇real-time ray tracing的演算法  
  
## linear cosine transform  
![Alt text](https://github.com/mingweiniu/ComputerGraphics/blob/master/results/LTC.png)  
程式碼：https://github.com/mingweiniu/ComputerGraphics/tree/master/01CG/final  
工具：MSVC++, GLSL/ glew, glut  
簡述：實作有部分source的paper，此paper為使用matrix運算去近似GGX，實作對象為Real-Time Polygonal-Light Shading with Linearly Transformed Cosines https://eheitzresearch.wordpress.com/415-2/  
  
## Flat Gouraud Phong  
![Alt text](https://github.com/mingweiniu/ComputerGraphics/blob/master/results/Flat%20Gouraud%20Phong.jpg)  
程式碼：https://github.com/mingweiniu/ComputerGraphics/tree/master/02RTR/shadingModels  
工具：MSVC++, GLSL/ glew, glfw   
簡述：實作light shading model與parse obj格式  
  
## texturing  
![Alt text](https://github.com/mingweiniu/ComputerGraphics/blob/master/results/texturing.png)  
程式碼：https://github.com/mingweiniu/ComputerGraphics/tree/master/01CG/hw3  
工具：MSVC++, GLSL/ glut, glew, GLModel  
簡述：將GLModel讀到的texture貼到物件上  
  
## Stencil buffer  
![Alt text](https://github.com/mingweiniu/ComputerGraphics/blob/master/results/Stencil%20buffer.png)  
程式碼：https://github.com/mingweiniu/ComputerGraphics/tree/master/01CG/hw2  
工具：MSVC++/ glut, glew  
簡述：用stencil buffer做出鏡面的效果  
  
## brush model  
![Alt text](https://github.com/mingweiniu/ComputerGraphics/blob/master/results/brush%20model.png)  
程式碼：https://github.com/mingweiniu/ComputerGraphics/tree/master/04NPR/hw1  
工具：cmake for c++, GLSL/ Qt5   
簡述：用Qt寫出筆刷效果，可以橢圓狀筆和大小可調整  
  
## ink diffusion   
![Alt text](https://github.com/mingweiniu/ComputerGraphics/blob/master/results/ink%20diffusion.png)  
程式碼：https://github.com/mingweiniu/ComputerGraphics/tree/master/04NPR/hw2  
工具：cmake for c++, GLSL/ Qt5   
簡述：延續筆刷效果，畫布會吸水擴散  
  
## Bézier curve  
![Alt text](https://github.com/mingweiniu/ComputerGraphics/blob/master/results/B%C3%A9zier%20curve.png)  
程式碼：https://github.com/mingweiniu/ComputerGraphics/blob/master/00/bezier_curve.cpp  
工具：GNUg++/ glew, glfw  
簡述：實作貝茲曲線  
  
## non-rectangular clip  
![Alt text](https://github.com/mingweiniu/ComputerGraphics/blob/master/results/non-rectangular%20clip.png)  
程式碼：https://github.com/mingweiniu/ComputerGraphics/blob/master/00/nonrectangular_clip.py  
工具：python3/ numpy  
簡述：實作clipping  
  
## ComputerVision   
3D reconstruction from same viewport  
https://github.com/mingweiniu/ComputerGraphics/tree/master/05CV/hw1  
SIFT  
https://github.com/mingweiniu/ComputerGraphics/tree/master/05CV/hw2  
3D resonstruction from different viewport  
https://github.com/mingweiniu/ComputerGraphics/tree/master/05CV/hw3  