# SlimeCraft
A voxel engine written in C++ using OpenGL.

![Flying Around](assets/Flying.gif)

![Placing Blocks](assets/PlacingBlocks.gif)

![Breaking Blocks](assets/BreakingBlocks.gif)

![Some Shrubs](assets/Shrubs.gif)

![Going for a swim](assets/Swimming.gif)

![Some calming waves](assets/CalmingWaves.gif)

## Further Information:

### Features:

**Tech used:** OpenGL, C++

- Flying around a procedurally generated world
- Placing a wide selection of blocks
- Breaking blocks
- Swimming underwater (with a nice wave effect!)
- Enjoying nature (with a nice gentle breeze effect!)
- Having a great time

### Terrain Generation:

My world is generated in three phases: 

First, my world is generated in 32x32x256 chunks, with each chunk being stored in a 1D array with a 3D lookup function. The chunks are then all stored together in a hashmap.
The terrain is entirely procedurally generated using fractal perlin noise, which is generated using the fastnoiselite library. Shoutout this library because it made my life a whole lot easier. 

Then after this first pass of just the blocks based on the noise, I then rendered the water in a second pass to maintain transparency. 

Finally, the trees and shrubs are rendered in a third pass, as they are also transparent and required a different shader to get the little breeze effect.
The position of these are just determined by as standard distribution.

### Rendering:

There is one draw call for each mesh group in each chunk (all the blocks, then all the water, then all the shrubs and trees), which comes up to 3 draw calls per chunk. 
Only the visible faces of each block are rendered, unless the block is transparent, in which case that face is also rendered, and back faces are culled as well. 
There are also a few nice shader effects, such as the moving of the water and shrubs, as well as the linear fog in the distance and the blue fog underwater.
The last notable thing is the block selector, which is implemented with a simple DDA traversal algorithm and a shader that draws the outline of the block. 

### Optimizations:

There are a decent few optimizations that I have implemented in this project:
- Multithreaded chunk generation
- Face Culling
- Mesh Batching
- Billboarding
- Texture Atlas

## Lessons Learned:

In coding this project, I learned a lot about the basics of OpenGL. I went into this project almost blindly, with my initial attempt taking over 8 seconds to generate a single chunk. I now realize that that is generally frowned upon.
I've loved delving into the world of voxel engines and learning about the techniques used to generate millions of tiny cubes, as well as how to convince users that you are rendering much than what you really are.
The most important lesson I learned is that you can never be too optimized, and that there are always ways to improve. I look forward to continuing to improve and optimize this project as I learn more!

