# AM-Engine
Slowly adding everything I have learned into one rendering engine, plus more!

<img src="https://github.com/AshMagorian/AM-Engine/blob/master/screenshots/Capture2.JPG" width="1000"> 
<img src="https://github.com/AshMagorian/AM-Engine/blob/master/screenshots/Capture.JPG" width="1000"> 

## Features:

### Deferred Rendering
The basis of deferred rendering is to render each object onto a number textures on a separate Framebuffer, one texture would hold position details, one would hold normals and one would hold albedo colour data.
These textures are then processed through a Lighting Pass shader where lighting calculations are applied. This process allows many objects to be rendered at once with more efficiency.

### Randomly Generated Terrain
I have already implemented something similar to this in a previous project. However, instead of the terrain reading height values from a height map, values are now randomly generated from a psuedo-perlin noise system.
Terrain tiles can be added, removed and edited from the GUI menu.
The terrain also supports the blending of up to 4 PBR materials 

### Water Rendering
A water texture is rendered onto a plane using scrolling textures to simulate ripples. The scene is rendered 2 extra times per frame to get reflections and refractions.
These 2 textures are then mixed and specular reflections are added.

### Physical based rendering, Assimp model loading, scene management and Level building GUI
As implemented by my previous project "Fun with PBR"
