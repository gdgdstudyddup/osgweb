
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

#ifndef MAIN_H
#define MAIN_H
#include "Global.h"
#include "BasicShader.h"
#include "FunctionUtils.h"
#include "UpdateVisitor.h"
#include "ShapeManager.h"
#include "ExtraShapes.h"

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

// This class prints OpenSceneGraph notifications to console.
class Logger : public osg::NotifyHandler
{
public:
	Logger() {}
	virtual ~Logger() {}

	// Override NotifyHandler::notify() to receive OpenSceneGraph notifications.
	void notify(osg::NotifySeverity severity, const char* message) {
		std::string finalMessage =
			PrintfString("OSG/%s %s", LogLevelToString(severity).c_str(), message);
		platformLog(finalMessage.c_str());
	}
};

// end

class Application {
public:
	Application() : _insertVisitor(),_updateVisitor() {
		_basicProgram = new osg::Program();
		basicProgram = _basicProgram;
		osg::ref_ptr<osg::Shader> vertShader =
		new osg::Shader( osg::Shader::VERTEX, shaderVertex );
		osg::ref_ptr<osg::Shader> fragShader =
		new osg::Shader( osg::Shader::FRAGMENT, shaderFragment );
		basicProgram->addShader( vertShader.get() );
		basicProgram->addShader( fragShader.get() );
		// setupLogging();
		setupRendering();
		// // osg::ref_ptr<Spring> shape = new Spring(1,0.1,2,1);
		// // osg::ref_ptr<osg::Drawable> shape = new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(0.0f, 0.0f, 0.0f), 0.1f));
		// // geode->addChild(shape.get());
		// // _shapeManager->addShape(shape);
		// VBOSetupVisitor vbo;
		// osg::ref_ptr<Pipecylinder> shape = new Pipecylinder(0.5,1.5, 0.5);
		// osg::ref_ptr<osg::Geode> geode = new osg::Geode();
		// // osg::Program *prog = CreateShaderProgram(shaderVertex, shaderFragment);
		// osg::StateSet* stateset = shape->getOrCreateStateSet();
		// stateset->setAttribute(basicProgram);
		// stateset->addUniform(
		// 			new osg::Uniform("lightColor", osg::Vec3(1.0f, 1.0f, 1.0f))
		// 		);
		// stateset->addUniform(
		// 			new osg::Uniform("objectColor", osg::Vec3(1.0f, 0.0f, 1.0f))
		// 		);
		// stateset->addUniform(
		// 			new osg::Uniform("specularStrength", 0.0f)
		// 		);		
		// stateset->addUniform(
		// 			new osg::Uniform("uLightPos", osg::Vec3(0.0f, -10.0f, 10.0f))
		// 		);
				
		// geode->addDrawable(shape.get());
		// // geode->setDrawable(0, shape.get());
		// geode->accept(vbo);
		// _root->addChild(geode.get());
	}
	~Application() {
		tearDownLogging();
		tearDownRendering();
	}
	osg::ref_ptr<osg::Group> _root;

	// BEGIN FEATURE INPUT_EMSCRIPTEN
	bool handleEvent(SDL_Event& e) {
		osgViewer::GraphicsWindow* gw = dynamic_cast<osgViewer::GraphicsWindow*>(
			_viewer->getCamera()->getGraphicsContext());
		if (!gw) {
			return false;
		}
		osgGA::EventQueue& queue = *(gw->getEventQueue());
		switch (e.type) {
		case SDL_MOUSEMOTION:
			queue.mouseMotion(e.motion.x, e.motion.y);
			return true;
		case SDL_MOUSEBUTTONDOWN:
			queue.mouseButtonPress(e.button.x, e.button.y, e.button.button);
			//printf("OSGWeb. Application. Mouse button down\n");
			return true;
		case SDL_MOUSEBUTTONUP:
			queue.mouseButtonRelease(e.button.x, e.button.y, e.button.button);
			//printf("OSGWeb. Application. Mouse button up\n");
			return true;
		case SDL_MOUSEWHEEL:
			queue.mouseScroll((e.button.x > 0)
				? osgGA::GUIEventAdapter::SCROLL_DOWN
				: osgGA::GUIEventAdapter::SCROLL_UP);
			return true;
		default:
			break;
		}
		return false;
	}
	// END   FEATURE INPUT_EMSCRIPTEN
	// BEGIN FEATURE RENDERING_EMBEDDED
	void setupWindow(int width, int height) {
		_viewer->setUpViewerAsEmbeddedInWindow(0, 0, width, height);
	}
	void resizeWindow(int width, int height) {
		osgViewer::GraphicsWindow* gw = dynamic_cast<osgViewer::GraphicsWindow*>(
			_viewer->getCamera()->getGraphicsContext());
		if (gw) {
			gw->resized(0, 0, width, height);
			// delete gw;
			// gw = nullptr;
		}
		// _viewer->setUpViewerAsEmbeddedInWindow(0, 0, width, height);
		_viewer->getCamera()->setViewport(new osg::Viewport(0,0,width,height));
    	_viewer->getCamera()->setProjectionMatrixAsPerspective(30.0f, static_cast<double>(width)/static_cast<double>(height), 1.0f, 10000.0f);
	}
	// END   FEATURE RENDERING_EMBEDDED
	// BEGIN FEATURE RENDERING_DEFAULT
	void frame() {
		_viewer->frame();
	}
	void clearNodes() {
		if(_root)
		{
			_root->removeChildren(0, _root->getNumChildren());
		}
		_insertVisitor._shapesIdentifiers.clear();
		_updateVisitor._shapesTypes.clear();
	}
	// END   FEATURE RENDERING_DEFAULT
	/**
	@brief Create nodes from JS data.
	@param namesfromJs - string with names of shapes.
	@param sizeArray - array with sizes of names.
	@param lengthOfSizeArray - length of sizeArray.
	@param parentNamefromJs - name of the parent node.
	@param op - operation type, todo.
	 */
	void createNodes(const char* namesfromJs, int* sizeArray, int* shapeTypeArray, int lengthOfSizeArray, const char* parentNamefromJs, int op)
	{

		std::vector<std::string> &_shapesIdentifiers = _insertVisitor._shapesIdentifiers;
		std::map<std::string, int>& _shapesTypes = _updateVisitor._shapesTypes;
		if(op == 0)
		{
			_shapesTypes.clear();
			_shapesIdentifiers.clear();
		}
		size_t len = strlen(parentNamefromJs);
		char* temp = (char*)malloc(len + 1);
		strcpy(temp, parentNamefromJs);
		_insertVisitor._searchName = std::string(temp);

		std::string allString(namesfromJs);

		for(int i = 0; i < lengthOfSizeArray; i++)
		{
			int size = sizeArray[i];
			int shapeType = shapeTypeArray[i];
			int from = i==0 ? 0 : sizeArray[i - 1];
			int to = from + size;
			std::string name = allString.substr(from, size);
			_shapesTypes[name] = shapeType;
			_shapesIdentifiers.push_back(name);
		}

		if(_root)
		{
			_root->accept(_insertVisitor);
		}
		free((void*) temp);
		// app->_root->accept(insertVisitor);
	}
	void updateNodes(const char* namefromJs)
	{
		size_t len = strlen(namefromJs);
		char* temp = (char*)malloc(len + 1);
		strcpy(temp, namefromJs);
		_updateVisitor._searchName = std::string(temp);
		if(_root)
		{
			_root->accept(_updateVisitor);
		}
	}
	void setBackgroundColor(float r, float g, float b, float a) 
	{
		if (_viewer && _viewer->getCamera()) 
		{
		_viewer->getCamera()->setClearColor(osg::Vec4(r, g, b, a));
		}
	}
	void setupLogging() {
		// Create custom logger.
		_logger = new Logger;
		// Provide the logger to OpenSceneGraph.
		osg::setNotifyHandler(_logger.get());
		// Only accept notifications of Info level or higher
		// like warnings and errors.
		osg::setNotifyLevel(osg::INFO);
	}
private:
	
	void setupRendering() 
	{
		printf("transformDataMap %d\n", global::transformDataMap.size());
		// Create OpenSceneGraph viewer.
		_viewer = new osgViewer::Viewer;
		_root = new osg::Group();
   		_root->setName("root");
		// BEGIN FEATURE RENDERING_DEFAULT
		// Use single thread: CRITICAL for mobile and web.
		_viewer->setThreadingModel(osgViewer::ViewerBase::SingleThreaded);
		// Create manipulator: CRITICAL for mobile and web.
		_tracballManipulator = new osgGA::TrackballManipulator();
    	_tracballManipulator->setHomePosition(
                    osg::Vec3d(0.0, -2, 0.0f),
                    osg::Vec3d(0.0f,0.0f,0.0f),
                    osg::Vec3d(0.0f,0.0f,1.0f)
                );
		_viewer->setCameraManipulator(_tracballManipulator.get());
		_viewer->setSceneData(_root.get());
		// END   FEATURE RENDERING_DEFAULT
	}
	
	void tearDownLogging() 
	{
		// Remove the logger from OpenSceneGraph.
		// This also destroys the logger: no need to deallocate it manually.
		std::cout << "Tearing down logging." << std::endl;
		osg::setNotifyHandler(0);
		// if (_logger)
		// {
		// 	std::cout << "Deleting logger." << std::endl;
		// 	delete _logger;
		// 	_logger = nullptr;
		// 	std::cout << "Logger deleted." << std::endl;
		// }
	}
	void tearDownRendering() { }
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

#endif // MAIN_H
