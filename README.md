#Readme on heightmap#
###https://github.com/kinikibu/heightmap.git###

##information##
```
Programmer: Joshua Mellott-Lillie
      Date: 9/15/2012
   Program: Heightmap
```

##description##
```
This program is intended to use basic OpenGL utilities and libraries to achieve
a surface mesh in which height is determined by pixel values. OpenGL is to be
used to render this mesh within a skybox.
```

##progress##
###Ver. 1###
Skybox has been changed to be the appropriate size. Denoted as ```SKYBOX_DISTANCE```.

Skybox is temporarily disabled.
+ This is for the heightmap

Interaction is nearly complete.
+ The viewing angle works correctly
+ Pressing control will move the camera
+ The camera needs to be able to move along with the direction of view

Heightmap is partially complete
+ heightmap only records using ```GL_LINE_STRIP``` as opposed to ```GL_TRIANGLE_STRIP```
+ heightmap will read any image in
+ heightmap currently reads only the image noted in the main function for debug purposes



###Ver. 0###
Skybox contains correct textures and will only shift with rotations.

Skybox still needs more depth, as the range is only 2 deep.
