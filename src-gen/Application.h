
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

#ifndef APPLICATION_H
#define APPLICATION_H
#include "Global.h"
#include "BasicShader.h"
#include "FunctionUtils.h"
#include "UpdateVisitor.h"

#include <osgDB/ReadFile>
#include <osgViewer/Viewer>

// BEGIN FEATURE VBO
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/LightModel>
#include <osg/MatrixTransform>
#include <osg/NodeVisitor>
#include <osg/PolygonStipple>
#include <osg/PrimitiveSet>
#include <osg/ShapeDrawable>
#include <osg/Texture2D>
#include <memory>
#include <SDL2/SDL.h>

// END   FEATURE VBO

// BEGIN FEATURE RENDERING_DEFAULT
#include <osgGA/TrackballManipulator>
// END   FEATURE RENDERING_DEFAULT

// BEGIN FEATURE PLUGINS_STATIC
// Initialize OSG plugins when OpenSceneGraph is built
// as a static library.
USE_OSGPLUGIN(osg2)
USE_SERIALIZER_WRAPPER_LIBRARY(osg)
// END   FEATURE PLUGINS_STATIC

class ModelData;
// This class prints OpenSceneGraph notifications to console.
class Logger : public osg::NotifyHandler
{
public:
	Logger() {}
	virtual ~Logger() {}

	// Override NotifyHandler::notify() to receive OpenSceneGraph notifications.
	void notify(osg::NotifySeverity severity, const char* message);
};

// end

class Application {
public:
	Application();
	~Application();
	osg::ref_ptr<osg::Group> _root;

	// BEGIN FEATURE INPUT_EMSCRIPTEN
	bool handleEvent(SDL_Event& e);
	// END   FEATURE INPUT_EMSCRIPTEN
	// BEGIN FEATURE RENDERING_EMBEDDED
	void setupWindow(int width, int height);
	void resizeWindow(int width, int height);
	// END   FEATURE RENDERING_EMBEDDED
	// BEGIN FEATURE RENDERING_DEFAULT
	void frame();
	void clearNodes();
	void addModelData(
		const char* namefromJs,
		float* position,
		float* normal,
		float* color,
		int lengthOfVertex,
		int* indices,
		int lengthOfIndices,
		unsigned char* imageData,
		int width,
		int height,
		int channel
	);
	void test();
	// END   FEATURE RENDERING_DEFAULT
	/**
	@brief Create nodes from JS data.
	@param namesfromJs - string with names of shapes.
	@param sizeArray - array with sizes of names.
	@param lengthOfSizeArray - length of sizeArray.
	@param parentNamefromJs - name of the parent node.
	@param op - operation type, todo.
	 */
	void createNodes(const char* namesfromJs, int* sizeArray, int* shapeTypeArray, int lengthOfSizeArray, const char* parentNamefromJs, int op);
	void updateNodes(const char* namefromJs);
	void setBackgroundColor(float r, float g, float b, float a);
	void setupLogging();
private:
	
	void setupRendering();
	void tearDownLogging();
	// osg::ref_ptr<osg::Node> mScene;
	// osg::ref_ptr<osg::Geode> geode;
	// osg::ref_ptr<osg::MatrixTransform> mt;
	osg::ref_ptr<osgGA::TrackballManipulator> _tracballManipulator;
	osg::ref_ptr<osgViewer::Viewer> _viewer;
	osg::ref_ptr<osg::Program> _basicProgram;
    InsertVisitor _insertVisitor;
	UpdateVisitor _updateVisitor;
	osg::ref_ptr<Logger> _logger;
};

#endif // APPLICATION_H
