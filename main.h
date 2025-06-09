
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

#ifndef OPENSCENEGRAPH_CROSS_PLATFORM_GUIDE_MAIN_H
#define OPENSCENEGRAPH_CROSS_PLATFORM_GUIDE_MAIN_H

#include "functions.h"

#include <osgDB/ReadFile>
#include <osgViewer/Viewer>

// BEGIN FEATURE VBO
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/NodeVisitor>
#include <osg/LightModel>
#include <osg/ShapeDrawable>
#include <osg/PolygonStipple>
#include <osg/PrimitiveSet>
#include <osg/Geode>
#include <osg/MatrixTransform>
#include <osg/AutoTransform>
#include <osg/Texture2D>

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
        Logger() { }
        virtual ~Logger() { }

        // Override NotifyHandler::notify() to receive OpenSceneGraph notifications.
        void notify(osg::NotifySeverity severity, const char *message)
        {
            std::string finalMessage =
                printfString(
                    "OSG/%s %s",
                    logLevelToString(severity).c_str(),
                    message);
            platformLog(finalMessage.c_str());
        }
};

// BEGIN FEATURE VBO
// This class forces the use of VBO.
class VBOSetupVisitor : public osg::NodeVisitor
{
    public:
        VBOSetupVisitor() :
            osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN) { }
        virtual void apply(osg::Geode &geode)
        {
            for (unsigned int i = 0; i < geode.getNumDrawables(); ++i)
            {
                osg::Geometry *geom =
                    dynamic_cast<osg::Geometry*>(geode.getDrawable(i));
                if (geom)
                {
                    geom->setUseVertexBufferObjects(true);
                    // geom->setUseVertexArrayObject(true);
                }
            }
            NodeVisitor::apply(geode);
        }
};
// END   FEATURE VBO
class UpdateVisitor : public osg::NodeVisitor
{
    public:
    UpdateVisitor() 
    { 
        setTraversalMode(NodeVisitor::TRAVERSE_ALL_CHILDREN);
    }
    virtual ~UpdateVisitor() = default;
    UpdateVisitor(const UpdateVisitor& uv) = delete;
    UpdateVisitor& operator=(const UpdateVisitor& uv) = delete;
    virtual void apply(osg::Node& node) override
    {
        traverse(node); // ? do i need this?
    }
    virtual void apply(osg::Group& node) override
    {
        traverse(node);
    }
    /**
     Transform
    */
    virtual void apply(osg::Transform& node) override
    {
        try {
            apply(dynamic_cast<osg::AutoTransform&>(node)); // Work-around for osg::NodeVisitor::apply(osg::AutoTransform&) (see OSG commit a4b0dc7)
        } catch (const std::bad_cast& exception) {
            NodeVisitor::apply(node);
        }
    }

    /**
     AutoTransform
    */
    virtual void apply(osg::AutoTransform& node) override
    {
        // std::cout<<"AT "<<node.className()<<"  "<<node.getName()<<std::endl;
        //   node.setPosition(_visualizer->_mat.getTrans());
        //   node.setRotation(_visualizer->_mat.getRotate());
        traverse(node);
    }

    /**
     MatrixTransform
    */
    virtual void apply(osg::MatrixTransform& node) override
    {
    // std::cout<<"MT "<<node.className()<<"  "<<node.getName()<<std::endl;
    //   node.setMatrix(_visualizer->_mat);
    traverse(node);
    }

    /**
     Geode
    */
    virtual void apply(osg::Geode& node) override
    {
        // std::cout<<"AT "<<node.className()<<"  "<<node.getName()<<std::endl;
            //   node.setPosition(_visualizer->_mat.getTrans());
        traverse(node);
    }

};
// end
class MyTransformCallback : public osg::NodeCallback 
{
public:

    // 构造函数，传入一个角度，这个角度为计算移动值的，移动的原理是，每一次时间变化乘以这个角度的量纲
    MyTransformCallback(float value) 
    {
        m_value = value;
    }

    // nodevisitor可以判断出哪个是需要的结点
    virtual void operator() (osg::Node *node, osg::NodeVisitor *nv) override
    {
        m_test += 0.1f;
        
        // 验证得到的结点是不是MatrixTransform
        osg::MatrixTransform *transform = dynamic_cast<osg::MatrixTransform *>(node);
        // 如果是的
        if (nv && transform && nv->getFrameStamp()) {
            
            //  得到参考时间与当前时间差，用来计算cos然后改变移动值
            double time = nv->getFrameStamp()->getReferenceTime();
            transform->setMatrix(osg::Matrix::translate(0.0f, 0.0f, cosf(time * m_value)));

            osg::Geode* geode = dynamic_cast<osg::Geode *>(transform->getChild(0));
            if(geode) {
                printf("!!!!!!!!!!!!?%f\n",time);
                if(m_test > 10.0f) 
                {
                    osg::ref_ptr<osg::Geometry> geom = dynamic_cast<osg::Geometry*>(geode->getDrawable(0));
                    if( geom != nullptr) 
                    {
                        //osg::Geometry* geom = dynamic_cast<osg::Geometry*>(drawable);
                        
                        osg::Vec3Array* vertices = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray() );
                        for (unsigned int i=0; i<vertices->size(); i++)
                        {
                            if(i<2)
                            {
                                (*vertices)[i].set( vertices->at(i).x() + 0.001,  vertices->at(i).y(), vertices->at(i).z() + 0.001 );
                            }
                        }
                        geom->dirtyDisplayList();
                        
                    }
                }
            }
        }
        
        // 向下遍历node，以便找到transform
        traverse(node, nv);
    }
protected:
    float m_value;
    float m_test = 0.0f;

};

class Application
{
    public:
        Application():mUpdateVisitor() 
        {
            setupLogging();
            setupRendering();
        }
        ~Application()
        {
            tearDownLogging();
            tearDownRendering();
        }

        void loadScene(const std::string &fileName)
        {
            // Load scene.
            mScene = osgDB::readNodeFile(fileName);
            if (!mScene)
            {
                osg::notify(osg::FATAL) << "Could not load scene";
                return;
            }
            if (osg::Geode* geometry = dynamic_cast<osg::Geode*>(mScene.get())) {
                std::cout << "ooooooooook " << std::endl;
            } else {
                std::cout << "ggggggggggggggggg " << std::endl;
            }
// BEGIN FEATURE VBO
            // Use VBO and EBO instead of display lists. CRITICAL for web (Emscripten)
            // to skip FULL_ES2 emulation flag.
            VBOSetupVisitor vbo;
            mScene->accept(vbo);
// END   FEATURE VBO
            // Load shaders.
            osg::Program *prog = createShaderProgram(shaderVertex, shaderFragment);
            // Apply shaders.
            osg::StateSet* stateset = mScene->getOrCreateStateSet();
            stateset->setAttribute(prog);
            // osg::ref_ptr<osg::Material> mat = new osg::Material();      
            // mat->setDiffuse(osg::Material::FRONT, osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f)); // 设置正面散射颜色      
            // mat->setSpecular(osg::Material::FRONT, osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f)); // 设置正面镜面颜色     
            // mat->setShininess(osg::Material::FRONT, 90.0f); // 设置正面指数 
            // mScene->getOrCreateStateSet()->setAttribute(mat.get*());
            // const osg::ref_ptr<osg::LightModel> lightModel = new osg::LightModel();
            // lightModel->setTwoSided(true);
            // const osg::ref_ptr<osg::StateSet> ss = mScene->getOrCreateStateSet();
            // ss->setAttributeAndModes(lightModel.get(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE | osg::StateAttribute::PROTECTED);

            osg::ref_ptr<osg::Image> image = new osg::Image;
            // unsigned char* ptr = image->data();
            int s, t; s=t=256;
            image->allocateImage(s, t, 1, GL_RGBA, GL_UNSIGNED_BYTE);
            for (int row = 0; row < t; ++row)
            {
                for (int col = 0; col < s; ++col)
                {
                unsigned char* newImageData = image->data(col, row);
                newImageData[0] = 255;
                newImageData[1] = col;
                newImageData[2] = (row+col)/2;
                newImageData[3] = 255;
                }
            }
            osg::ref_ptr<osg::Texture2D> texture =
                new osg::Texture2D( image.get() );
            texture->setWrap( osg::Texture::WRAP_S, osg::Texture::REPEAT );
            texture->setWrap( osg::Texture::WRAP_R, osg::Texture::REPEAT );
            // Set scene.
            stateset->addUniform(
                new osg::Uniform("u_TestColor", osg::Vec3(1.0f, 0.0f, 0.0f))
            );
            stateset->addUniform(
                new osg::Uniform("baseTexture1", 1) // texture.get()
            );
            stateset->setTextureAttributeAndModes( 1, texture.get() );
            mRoot->addChild(mScene.get());
            std::string className = std::string(typeid(mScene.get()).name());
            printf("mScene %s", className.c_str());
            // for(int i=0;i<mScene._vertexArray.length();i++) {
            //     printf("mScene vertex %d %d %d", i, mScene.get()._vertexArray[i].x,mScene.get()._vertexArray[i].y,mScene.get()._vertexArray[i].z);
            // }
            // std::string className = std::string(typeid(_a).name());
            // mViewer->setSceneData(mScene.get());

            geode = new osg::Geode();

            osg::Geometry* polyGeom = new osg::Geometry();

            // note, anticlockwise ordering.
            osg::Vec3 myCoords[] =
            {
                osg::Vec3(-1.12056, -2.15188e-09, -0.840418),
                osg::Vec3(-0.95165, -2.15188e-09, -0.840418),
                osg::Vec3(-1.11644, 9.18133e-09, -0.716827),

                // note in anticlockwise order.
                osg::Vec3(-0.840418, 9.18133e-09, -0.778623),
                osg::Vec3(-0.622074, 9.18133e-09, -0.613835),
                osg::Vec3(-1.067, 9.18133e-09, -0.609715),

            };


            int numCoords = sizeof(myCoords)/sizeof(osg::Vec3);

            osg::Vec3Array* vertices = new osg::Vec3Array(numCoords,myCoords);

            // pass the created vertex array to the points geometry object.
            polyGeom->setVertexArray(vertices);
            osg::ref_ptr<osg::DrawElementsUInt> facette = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 6);
            (*facette)[0] = 0;
            (*facette)[1] = 1;
            (*facette)[2] = 5;
            (*facette)[3] = 2;
            (*facette)[4] = 4;
            (*facette)[5] = 5;
            polyGeom->addPrimitiveSet(facette);
            //osg::ref_ptr<osg::Vec4Array> shared_colors = new osg::Vec4Array;
            //shared_colors->push_back(osg::Vec4(1.0f,1.0f,0.0f,1.0f));

            // Same trick for shared normal.
            //osg::ref_ptr<osg::Vec3Array> shared_normals = new osg::Vec3Array;
            //shared_normals->push_back(osg::Vec3(0.0f,-1.0f,0.0f));
            
            // use the shared color array.
            //polyGeom->setColorArray(shared_colors.get(), osg::Array::BIND_OVERALL);


            // use the shared normal array.
            //polyGeom->setNormalArray(shared_normals.get(), osg::Array::BIND_OVERALL);


            // This time we simply use primitive, and hardwire the number of coords to use
            // since we know up front,
            //polyGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS,0,numCoords));
            osg::StateSet* stateSet = new osg::StateSet();
            // stateSet->setMode(GL_LIGHTING, osg::StateAttribute::ON | osg::StateAttribute::PROTECTED);
            // stateSet->setMode(GL_CULL_FACE, osg::StateAttribute::OFF);
            polyGeom->setStateSet(stateSet);
            // osg::ref_ptr<osg::CullFace> cullface = new osg::CullFace(osg::CullFace::BACK);  
	        // stateset->setAttribute(cullface.get());  
	        // stateset->setMode(GL_CULL_FACE, osg::StateAttribute::ON);  

            // polyGeom->getOrCreateStateSet()->setAttribute(prog);

            polyGeom->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::TRIANGLES,0,6 ) );

            //osg::PolygonStipple* polygonStipple = new osg::PolygonStipple;
            //stateSet->setAttributeAndModes(polygonStipple,osg::StateAttribute::OVERRIDE|osg::StateAttribute::ON);
            /** use the ShapeDrawable*/
            geode->addDrawable(polyGeom);
           
            geode->accept(vbo);
            mt = new osg::MatrixTransform;
            // mt->setUpdateCallback(new MyTransformCallback(1.0f));
            //osg::Matrix _m;
            //_m.makeTranslate(osg::Vec3(0.0f, 0.0f, 0.5f));  //X方向平移10个单位

            //mt->setMatrix(_m);
            mt->addChild(geode.get());
            mRoot->addChild(mt.get());

            // osg::ref_ptr<osg::Node> CreateNode()
            // { 
            //     osg::ref_ptr<osg::Group> _root = new osg::Group;
            //     osg::ref_ptr<osg::MatrixTransform> _mt = new osg::MatrixTransform;
            //     root->addChild(_mt);
            //     return _root.get(); // ?????????????
            // }
            // mRoot->addChild(geode.get());

            if(mTrackballManipulator){
                // const osg::BoundingSphere& boundingSphere=mScene->getBound();
                // mTrackballManipulator->setHomePosition(
                //     boundingSphere.center() + osg::Vec3d(0.0, -2, 0.0f),
                //     boundingSphere.center(),
                //     osg::Vec3d(0.0f,0.0f,1.0f)
                // );
                // mTrackballManipulator->setDistance(5 * 2);

                mTrackballManipulator->setHomePosition(
                    osg::Vec3d(0.0, -3, 0.0f),
                    osg::Vec3d(0.0f,0.0f,0.0f),
                    osg::Vec3d(0.0f,0.0f,1.0f)
                );
                mTrackballManipulator->setDistance(5 );
                mTrackballManipulator->setWheelZoomFactor(0.1);
            }
            geode->getOrCreateStateSet()->setMode(GL_LIGHTING,
                    osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);

                    // osg::ref_ptr<osg::Uniform> uniform =
            // osg::ref_ptr<osg::Group> root = new osg::Group;
            // mViewer->setSceneData(root);
            // root->addChild(scene.get());
        }
// BEGIN FEATURE INPUT_EMSCRIPTEN
        bool handleEvent(SDL_Event &e)
        {
            osgViewer::GraphicsWindow *gw =
                dynamic_cast<osgViewer::GraphicsWindow *>(
                    mViewer->getCamera()->getGraphicsContext());
            if (!gw)
            {
                return false;
            }
            osgGA::EventQueue &queue = *(gw->getEventQueue());
            switch (e.type)
            {
                case SDL_MOUSEMOTION:
                    queue.mouseMotion(e.motion.x, e.motion.y);
                    return true;
                case SDL_MOUSEBUTTONDOWN:
                    queue.mouseButtonPress(e.button.x, e.button.y, e.button.button);
                    printf("OSGWeb. Application. Mouse button down\n");
                    return true;
                case SDL_MOUSEBUTTONUP:
                    queue.mouseButtonRelease(e.button.x, e.button.y, e.button.button);
                    printf("OSGWeb. Application. Mouse button up\n");
                    return true;
                case SDL_MOUSEWHEEL:
                    //queue.mouseButtonWheel(e.button.x, e.button.y, e.button.button);
                    queue.mouseScroll(  (e.button.x > 0) ? osgGA::GUIEventAdapter::SCROLL_DOWN :osgGA::GUIEventAdapter::SCROLL_UP); // 
                    //printf("OSGWeb. Application. Mouse Wheel\n");
                    return true; 
                default:
                    break;
            }
            return false;
        }
// END   FEATURE INPUT_EMSCRIPTEN
// BEGIN FEATURE RENDERING_EMBEDDED
        void setupWindow(int width, int height)
        {
            mViewer->setUpViewerAsEmbeddedInWindow(0, 0, width, height);
        }
// END   FEATURE RENDERING_EMBEDDED
// BEGIN FEATURE RENDERING_DEFAULT
        void frame()
        {
            if(!mt)
            {
                mt = new osg::MatrixTransform;
                
            }
            osg::Matrix _m;
            _m.makeTranslate(osg::Vec3(0.0f, 0.0f, 0.5f));  //X方向平移10个单位
            mt->setMatrix(_m);
            
            mRoot->accept(mUpdateVisitor);
            mViewer->frame();
        }
// END   FEATURE RENDERING_DEFAULT

    private:
        void setupLogging()
        {
            // Create custom logger.
            mLogger = new Logger;
            // Provide the logger to OpenSceneGraph.
            osg::setNotifyHandler(mLogger);
            // Only accept notifications of Info level or higher
            // like warnings and errors.
            osg::setNotifyLevel(osg::INFO);
        }
        void setupRendering()
        {
            // Create OpenSceneGraph viewer.
            mViewer = new osgViewer::Viewer;
            
// BEGIN FEATURE RENDERING_DEFAULT
            // Use single thread: CRITICAL for mobile and web.
            mViewer->setThreadingModel(osgViewer::ViewerBase::SingleThreaded);
            // Create manipulator: CRITICAL for mobile and web.
            mTrackballManipulator = new osgGA::TrackballManipulator();
            mViewer->setCameraManipulator(mTrackballManipulator.get());
            mRoot = new osg::Group();
            mViewer->setSceneData(mRoot.get());
// END   FEATURE RENDERING_DEFAULT
        }
        void tearDownLogging()
        {
            // Remove the logger from OpenSceneGraph.
            // This also destroys the logger: no need to deallocate it manually.
            osg::setNotifyHandler(0);
        }
        void tearDownRendering()
        {
            delete mViewer;
        }

    private:
        osg::ref_ptr<osg::Node> mScene;
        osg::ref_ptr<osg::Group> mRoot;
        osg::ref_ptr<osg::Geode> geode;
        osg::ref_ptr<osgGA::TrackballManipulator> mTrackballManipulator;
        Logger *mLogger;
        osg::ref_ptr<osgViewer::Viewer> mViewer;
        osg::ref_ptr<osg::MatrixTransform> mt;
        UpdateVisitor mUpdateVisitor;
};

#endif // OPENSCENEGRAPH_CROSS_PLATFORM_GUIDE_MAIN_H

