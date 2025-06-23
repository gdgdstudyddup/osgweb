
/*
This file is part of OpenSceneGraph cross-platform guide:
  https://github.com/OGStudio/openscenegraph-cross-platform-guide

Copyright (C) 2017 Opensource Game Studio

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

#ifndef FUNCTION_UTILS_H
#define FUNCTION_UTILS_H



// #include <osg/Camera>

// // Configure camera with common defaults.
// void setupCamera(
//     osg::Camera *cam,
//     osg::GraphicsContext *gc,
//     double fovy,
//     int width,
//     int height)
// {
//     // Provide GC.
//     cam->setGraphicsContext(gc);
//     // Viewport must have the same size.
//     cam->setViewport(new osg::Viewport(0, 0, width, height));
//     // Clear depth and color buffers each frame.
//     cam->setClearMask(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
//     cam->setClearColor(osg::Vec4(0.95, 0.95, 0.95, 1.0));
//     // Aspect ratio.
//     float aspect = static_cast<float>(width) / static_cast<float>(height);
//     // Configure projection.
//     cam->setProjectionMatrixAsPerspective(30, aspect, 1, 10000);
// }

#endif // FUNCTION_UTILS_H

