# FastAndFurious

This is an attempt to show the different ways to run real time image processing computation.

## Problem

- To run image processing you would assume to use the GPU.
- What is the best way to push the data to the GPU.
- Is it not just faster to have the CPU run it threads
  - This is not considering now the CPU can't handle other operations
  
## The Screen Buffer

- The Android NDK lets you have control of a ANativeBuffer* that is attached to a ANativeWindow. This allows you to set the buffer and then push to screen
- The idea of having this all in OpenGL Texture Creates a new 

## The Computation

- Android does not support OpenCL and getting it to work can be annoying and almost impossible for a production standpoint
- Google has their own RenderScript which is not cross platform and universal as OpenCL is
- The best workaround I see is to use Vulkan as it has both Android native support (Android 7.0+) AND Compute capabilites
  - The computation would be done in SPIR-V shaders which is still more universal than RenderScript
  - Also OpenCL is planning to one day merge into Vulkan for reason such as this
