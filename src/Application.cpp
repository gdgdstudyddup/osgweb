#include "Application.h"
#include "ExtraShapes.h"

#include <osg/Notify>

// Convert NotifySeverity enum value to string.
std::string LogLevelToString(osg::NotifySeverity severity)
{
    switch (severity)
    {
        case osg::DEBUG_FP:
            // Verbose.
            return "V";
        case osg::DEBUG_INFO:
            // Debug.
            return "D";
        case osg::NOTICE:
        case osg::INFO:
            // Info.
            return "I";
        case osg::WARN:
            // Warning.
            return "W";
        case osg::FATAL:
        case osg::ALWAYS:
            // Error.
            return "E";
    }
}

#include <cstdarg>

// Construct a string using printf-like syntax.
std::string PrintfString(const char *fmt, ...)
{
    const int PRINTF_STRING_MAX_STRLEN = 1024;
    va_list args;
    char msg[PRINTF_STRING_MAX_STRLEN];
    va_start(args, fmt);
    vsnprintf(msg, PRINTF_STRING_MAX_STRLEN, fmt, args);
    va_end(args);
    return msg;
}

// BEGIN FEATURE LOGGING_DEFAULT
#include <iostream>
// END   FEATURE LOGGING_DEFAULT

// Platform specific logging.
void platformLog(const char *message)
{
// BEGIN FEATURE LOGGING_DEFAULT
    std::cout << message << std::endl;
// END   FEATURE LOGGING_DEFAULT
}
void Logger::notify(osg::NotifySeverity severity, const char* message) {
		std::string finalMessage =
			PrintfString("OSG/%s %s", LogLevelToString(severity).c_str(), message);
		platformLog(finalMessage.c_str());
	}
Application::Application(): _insertVisitor(),_updateVisitor() {
		_basicProgram = new osg::Program();
		global::basicProgram = _basicProgram;
		osg::ref_ptr<osg::Shader> vertShader =
		new osg::Shader( osg::Shader::VERTEX, shaderVertex );
		osg::ref_ptr<osg::Shader> fragShader =
		new osg::Shader( osg::Shader::FRAGMENT, shaderFragment );
		global::basicProgram->addShader( vertShader.get() );
		global::basicProgram->addShader( fragShader.get() );
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
Application::~Application()
{
	tearDownLogging();
}
// BEGIN FEATURE INPUT_EMSCRIPTEN
bool Application::handleEvent(SDL_Event& e) {
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
void Application::setupWindow(int width, int height) {
		_viewer->setUpViewerAsEmbeddedInWindow(0, 0, width, height);
	}
void Application::resizeWindow(int width, int height) {
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
void Application::frame() {
		_viewer->frame();
}
void Application::home() {
		if (_viewer) {
			
			osg::BoundingSphere boundingSphere = _root->getBound();
			double radius = osg::maximum(double(boundingSphere.radius()), 1e-6);
			double dist = 3.5f * radius;
			// try to compute dist from frustum
            double left,right,bottom,top,zNear,zFar;
            if (_viewer->getCamera()->getProjectionMatrixAsFrustum(left,right,bottom,top,zNear,zFar))
            {
                double vertical2 = fabs(right - left) / zNear / 2.;
                double horizontal2 = fabs(top - bottom) / zNear / 2.;
                double dim = horizontal2 < vertical2 ? horizontal2 : vertical2;
                double viewAngle = atan2(dim,1.);
                dist = radius / sin(viewAngle);
            }
			else {
				// try to compute dist from ortho
                if (_viewer->getCamera()->getProjectionMatrixAsOrtho(left,right,bottom,top,zNear,zFar))
                {
                    dist = fabs(zFar - zNear) / 2.;
                }
			}
			osg::Vec3d center, eye, up;
    		_tracballManipulator->getTransformation( eye, center, up );

			osg::Vec3d newEye = boundingSphere.center() + osg::Vec3d(0.0, -dist, 0.0f);
			_tracballManipulator->setTransformation(newEye, center, up);

			//osg::Vec3d temp = boundingSphere.center() + osg::Vec3d(0.0, -dist, 0.0f);
			//std::cout<<"dist "<<radius<<" radius "<<dist<<std::endl;
			//std::cout<< temp.x() <<" "<<temp.y()<<" "<<temp.z()<< std::endl;
			cameraPositionSide();
		}
}
void Application::clearNodes() {
		if(_root)
		{
			_root->removeChildren(0, _root->getNumChildren());
		}
		_insertVisitor._shapesIdentifiers.clear();
		_insertVisitor._shapesTypes.clear();
		_updateVisitor._shapesTypes.clear();
	}
void Application::addModelData(
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
	)
	{
		std::string modelName(namefromJs);
		_insertVisitor._modelDataMap[modelName] = new ModelData(
			modelName,
			position,
			normal,
			color,
			lengthOfVertex,
			indices,
			lengthOfIndices,
			imageData,
			width,
			height,
			channel
		);
		// std::cout<<"append new data and now size is "<< _insertVisitor._modelDataMap.size() << std::endl;
		
	}
void Application::test()
	{
		for (auto it : _insertVisitor._modelDataMap) 
		{ 
					std::cout << it.first << " " << it.second -> _position[0] << it.second -> _position[1]<< it.second -> _position[2]<< std::endl;
		}
		// _insertVisitor._modelDataMap.erase(modelName);
		// std::cout<< _insertVisitor._modelDataMap.size() << std::endl;
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
void Application::createNodes(const char* namesfromJs, int* sizeArray, int* shapeTypeArray, int lengthOfSizeArray, const char* parentNamefromJs, int op)
	{

		std::vector<std::string> &_shapesIdentifiers = _insertVisitor._shapesIdentifiers;
		std::map<std::string, int>& _shapesTypesI = _updateVisitor._shapesTypes;
		std::map<std::string, int>& _shapesTypes = _updateVisitor._shapesTypes;
		if(op == 0)
		{
			_shapesTypesI.clear();
			_shapesTypes.clear();
			_shapesIdentifiers.clear();
		}
		size_t len = strlen(parentNamefromJs);
		char* temp = (char*)malloc(len + 1);
		strcpy(temp, parentNamefromJs);
		_insertVisitor._searchName = std::string(temp);

		std::string allString(namesfromJs);

		int from = 0;
		for(int i = 0; i < lengthOfSizeArray; i++)
		{
			int size = sizeArray[i];
			int shapeType = shapeTypeArray[i];
			std::string name = allString.substr(from, size);
			_shapesTypesI[name] = shapeType;
			_shapesTypes[name] = shapeType;
			_shapesIdentifiers.push_back(name);
			from = from + size;
		}

		if(_root)
		{
			_root->accept(_insertVisitor);
		}
		/*
			_insertVisitor._mode = 1;
			_root->accept(_insertVisitor); // do it for vector
		*/
		free((void*) temp);
		// app->_root->accept(insertVisitor);
	}
void Application::updateNodes(const char* namefromJs)
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
void Application::setBackgroundColor(float r, float g, float b, float a) 
	{
		if (_viewer && _viewer->getCamera()) 
		{
		_viewer->getCamera()->setClearColor(osg::Vec4(r, g, b, a));
		}
	}
void Application::setupLogging() {
		// Create custom logger.
		_logger = new Logger;
		// Provide the logger to OpenSceneGraph.
		osg::setNotifyHandler(_logger.get());
		// Only accept notifications of Info level or higher
		// like warnings and errors.
		osg::setNotifyLevel(osg::INFO);
	}
void Application::setupRendering() 
	{
		// Create OpenSceneGraph viewer.
		_viewer = new osgViewer::Viewer;
		_root = new osg::Group();
   		_root->setName("root");
		// BEGIN FEATURE RENDERING_DEFAULT
		// Use single thread: CRITICAL for mobile and web.
		_viewer->setThreadingModel(osgViewer::ViewerBase::SingleThreaded);
		// Create manipulator: CRITICAL for mobile and web.
		_tracballManipulator = new osgGA::TrackballManipulator();
		_cameraPose = 0;
    	_tracballManipulator->setHomePosition(
                    osg::Vec3d(0.0, -2, 0.0f),
                    osg::Vec3d(0.0f,0.0f,0.0f),
                    osg::Vec3d(0.0f,0.0f,1.0f)
                );
		_viewer->setCameraManipulator(_tracballManipulator.get());
		_viewer->setSceneData(_root.get());
		// END   FEATURE RENDERING_DEFAULT
	}
double Application::computeDistanceToOrigin()
{
  osg::ref_ptr<osgGA::CameraManipulator> manipulator = _tracballManipulator;
  osg::Matrixd mat = manipulator->getMatrix();
  //assemble

  //Compute distance to center using pythagoras theorem
  double d = sqrt(abs(mat(3,0))*abs(mat(3,0))+
                  abs(mat(3,1))*abs(mat(3,1))+
                  abs(mat(3,2))*abs(mat(3,2)));

  //If d is very small (~0), set it to 1 as default
  if(d < 1e-10) {
    d=1;
  }

  return d;
}
void Application::cameraPositionIsometric()
{
  double d = computeDistanceToOrigin();
  osg::Matrixd mat = osg::Matrixd(0.7071, 0, -0.7071, 0,
                                  -0.409, 0.816, -0.409, 0,
                                  0.57735,  0.57735, 0.57735, 0,
                                  0.57735*d, 0.57735*d, 0.57735*d, 1);
  _tracballManipulator->setByMatrix(mat);
  _cameraPose = 3; // isometric
}

/*!
 * sets the camera position to Side
 */
void Application::cameraPositionSide()
{
  double d = computeDistanceToOrigin();
  std::cout<<"computeDistanceToOrigin "<<d<<std::endl;
  osg::Matrixd mat = osg::Matrixd(1, 0, 0, 0,
                                  0, 1, 0, 0,
                                  0, 0, 1, 0,
                                  0, 0, d, 1);
  _tracballManipulator->setByMatrix(mat);
  _cameraPose = 0; // side
}

/*!
 * sets the camera position to Front
 */
void Application::cameraPositionFront()
{
  double d = computeDistanceToOrigin();
  osg::Matrixd mat = osg::Matrixd( 0, 0,-1, 0,
                                   0, 1, 0, 0,
                                   1, 0, 0, 0,
                                   d, 0, 0, 1);
  _tracballManipulator->setByMatrix(mat);
  _cameraPose = 1; // front
}

/*!
 * sets the camera position to Top
 */
void Application::cameraPositionTop()
{
  double d = computeDistanceToOrigin();
  osg::Matrixd mat = osg::Matrixd( 0, 0, 1, 0,
                                   1, 0, 0, 0,
                                   0, 1, 0, 0,
                                   0, d, 0, 1);
  _tracballManipulator->setByMatrix(mat);
  _cameraPose = 2; // top
}
void Application::rotateCameraLeft()
{
  osg::Matrixd mat = _tracballManipulator->getMatrix();
  osg::Camera *pCamera = _viewer->getCamera();

  osg::Vec3d eye, center, up;
  pCamera->getViewMatrixAsLookAt(eye, center, up);
  osg::Vec3d rotationAxis = center-eye;

  osg::Matrixd rotMatrix;
  rotMatrix.makeRotate(3.1415/2.0, rotationAxis);

 _tracballManipulator->setByMatrix(mat*rotMatrix);
}
void Application::rotateCameraRight()
{
  osg::Matrixd mat = _tracballManipulator->getMatrix();
  osg::Camera *pCamera = _viewer->getCamera();

  osg::Vec3d eye, center, up;
  pCamera->getViewMatrixAsLookAt(eye, center, up);
  osg::Vec3d rotationAxis = center-eye;

  osg::Matrixd rotMatrix;

  rotMatrix.makeRotate(-3.1415/2.0, rotationAxis);

 _tracballManipulator->setByMatrix(mat*rotMatrix);
}
void Application::rotateFixed(int isLeft)
{
	if(isLeft)
	{
		rotateCameraLeft();	
	}
	else
	{
		rotateCameraRight();
	}
}
void Application::tearDownLogging() 
	{
		// Remove the logger from OpenSceneGraph.
		// This also destroys the logger: no need to deallocate it manually.
		osg::setNotifyHandler(0);
		// if (_logger)
		// {
		// 	std::cout << "Deleting logger." << std::endl;
		// 	delete _logger;
		// 	_logger = nullptr;
		// 	std::cout << "Logger deleted." << std::endl;
		// }
	}