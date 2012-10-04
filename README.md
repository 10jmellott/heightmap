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

###completeness###
```
Heightmap is completed fully, does not conform to triangles, however
Skybox is completed fully
Interaction is above what is asked, see below
Video is named example.mp4
```

###extra credit###
+ Camera movement is intuitive and controlled by angles
+ Camera is 'physically' moved
+ Camera uses spherical coordinates
+ Save Image feature is also possible by right clicking



##progress##
###Ver. 4###
Heightmap is very finished.
+ heightmap uses triangles now to acheive visual appearance
+ heightmap also uses ```DRAW_BOTTOM_TEXTURE``` to add a texture to the heightmap

###Ver. 3###
Heightmap is finished.
+ note: heightmap does not need be ground texture
+ note: heightmap uses ```GL_LINE_STRIP``` to render
+ this aids the appearance of the skybox

Code has had comments added to it.

```SCALE_SKYBOX``` is another compiler defined variable.
+ the default for this variable is 0 and will not scale
+ if skybox scaling is preferred, set to 1

###Ver. 2###
Interaction is complete.
+ Skybox no longer can be translated
+ Objects in scene move accordingly
+ Control will shift the camera
+ Shift will scale the scene
+ Left clicking will rotate the camera

Skybox has an updated texture with better resolution.

Heightmap still isn't complete.
+ Heightmap still only uses GL_LINE_STRIP for its values
+ Heightmap is not the texture of the ground yet

Code has been cleaned up and additional functions have been added.
+ ```drawSkybox()``` and ```drawHeightmap()```
+ ```#define GL_CLAMP_TO_EDGE 0x812F``` has been added to poor texture borders

###Ver. 1###
Skybox has been changed to be the appropriate size. Denoted as ```SKYBOX_DISTANCE```.

Skybox is temporarily disabled.
+ This is for the heightmap

Interaction is nearly complete.
+ The viewing angle works correctly
+ Pressing control will move the camera
+ The camera needs to be able to move along with the direction of view

Heightmap is partially complete.
+ heightmap only records using ```GL_LINE_STRIP``` as opposed to ```GL_TRIANGLE_STRIP```
+ heightmap will read any image in
+ heightmap currently reads only the image noted in the main function for debug purposes


###Ver. 0###
Skybox contains correct textures and will only shift with rotations.

Skybox still needs more depth, as the range is only 2 deep.
