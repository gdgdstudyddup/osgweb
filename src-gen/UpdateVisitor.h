
#ifndef UPDATE_VISITOR_H
#define UPDATE_VISITOR_H
#include <osg/NodeVisitor>
#include <osg/Geode>
#include <osg/MatrixTransform>
#include <osg/ShapeDrawable>
#include <map>

#include "BasicShader.h"
#include "ExtraShapes.h"
#include "VisualizerHelper.h"
using namespace std;

// MatrixdArray

//for (unsigned int i = 0; i < position->size(); ++i) {
//         (*position)[i].x() += 1.0; // x() returns value_type&
//     }
class InsertVisitor : public osg::NodeVisitor{
public:
  InsertVisitor():_mode(0) { setTraversalMode(NodeVisitor::TRAVERSE_ALL_CHILDREN);
  }
  virtual ~InsertVisitor() = default;
  InsertVisitor(const InsertVisitor &uv) = delete;
  InsertVisitor &operator=(const InsertVisitor &uv) = delete;
  virtual void apply(osg::Group &node) override
  {
    if(node.getName() == _searchName)
    {
      for (const auto &shapeName : _shapesIdentifiers)
      {
        printf("_searchName is %s createNodes: name = %s\n", _searchName.c_str(), shapeName.c_str());
        switch (_mode)
        {
          case 0:
            osg::ref_ptr<osg::MatrixTransform> transf = new osg::MatrixTransform();
            transf->setName(shapeName + "t");
            osg::ref_ptr<osg::ShapeDrawable> shapeDraw = new osg::ShapeDrawable();

            osg::ref_ptr<osg::Geode> geode = new osg::Geode();
            geode->addDrawable(shapeDraw.get());
            geode->setName(shapeName + "g");

            transf->addChild(geode.get());
            node.addChild(transf.get());
          // _root->addChild(transf.get());
          break;
        }
      }
      
    }
    traverse(node);
  }
  int _mode; // 0: transform+geod + drawable;
  std::string _searchName;
  std::vector<std::string> _shapesIdentifiers;
};
class UpdateVisitor : public osg::NodeVisitor {
public:
  UpdateVisitor():_vbo(){ 
    setTraversalMode(NodeVisitor::TRAVERSE_ALL_CHILDREN);
    
    _mode = 0; }
  virtual ~UpdateVisitor() = default;
  UpdateVisitor(const UpdateVisitor &uv) = delete;
  UpdateVisitor &operator=(const UpdateVisitor &uv) = delete;
  virtual void apply(osg::Geode &node) override 
  {
    std::string nodeName = node.getName();
    // printf("_searchName is %s UpdateVisitor::apply %s\n", _searchName.c_str(), nodeName.c_str());
    if(nodeName == _searchName+"g") 
    {
        osg::ref_ptr<osg::Drawable> draw = node.getDrawable(0);
        draw->dirtyBound();
        draw->dirtyDisplayList();
        std::map<std::string, int >::iterator iterator = _shapesTypes.find(_searchName);
        if (iterator != _shapesTypes.end())
        {
          using TransformData = global::TransformData;
          std::map<std::string, std::shared_ptr<TransformData> > &transformDataMap = global::transformDataMap;
          std::shared_ptr<TransformData> info = transformDataMap[_searchName];
          if(info)
          {
              const int len = int(info->basis.size() / 9) - 1;
              if(len >= 0)
              {
                int index = static_cast<int>(std::round(global::time * len)); // index of transformData
                if (iterator->second == 0 || iterator->second == 1)
                {
                  // node.setDrawable(0, new Pipecylinder(shape->_width.exp * shape->_extra.exp / 2, shape->_width.exp / 2, shape->_length.exp));
                  
                      osg::ref_ptr<Pipecylinder> shape = new Pipecylinder(info->width[index] * info->extra[index] * 0.5, info->width[index] * 0.5, info->length[index]);
                      node.setDrawable(0, shape.get());

                      // osg::ref_ptr<osg::Vec4Array> shared_colors = new osg::Vec4Array;
                      // shared_colors->push_back(osg::Vec4(1.0f,1.0f,1.0f,1.0f));
                      // shape->setColorArray(shared_colors.get(), osg::Array::BIND_OVERALL);
                      
                      osg::StateSet* stateset = shape->getOrCreateStateSet();
                      if(basicProgram)
                      {
                        stateset->setAttribute(basicProgram.get());
                      }
                      stateset->addUniform(
                          new osg::Uniform("specularStrength", info->specCoeff[index])
                      );
                      stateset->addUniform(
                                  new osg::Uniform("objectColor", osg::Vec3(info->color[index * 3 + 0], info->color[index * 3 + 1], info->color[index * 3 + 2]))
                              );
                      stateset->addUniform(
                                  new osg::Uniform("lightColor", osg::Vec3(1.0f, 1.0f, 1.0f))
                              );
                     
                }
                else if(iterator->second == 2)
                {
                    //node.setDrawable(0, new Spring(shape->_width.exp, shape->_height.exp, shape->_extra.exp, shape->_length.exp));
                    osg::ref_ptr<Spring> shape = new Spring(info->width[index], info->height[index], info->extra[index], info->length[index]);
                    node.setDrawable(0, shape.get());
                    osg::StateSet* stateset = shape->getOrCreateStateSet();
                      if(basicProgram)
                      {
                        stateset->setAttribute(basicProgram.get());
                      }
                      stateset->addUniform(
                          new osg::Uniform("specularStrength", info->specCoeff[index])
                      );
                      stateset->addUniform(
                                  new osg::Uniform("objectColor", osg::Vec3(info->color[index * 3 + 0], info->color[index * 3 + 1], info->color[index * 3 + 2]))
                              );
                      stateset->addUniform(
                                  new osg::Uniform("lightColor", osg::Vec3(1.0f, 1.0f, 1.0f))
                              );
                     
                }
                else if(iterator->second==3)
                {
                    osg::ref_ptr<osg::Cone> cone = new osg::Cone(osg::Vec3f(0, 0, 0), info->width[index] * 0.5, info->length[index]);
                    cone->setCenter(cone->getCenter() - osg::Vec3f(0, 0, cone->getBaseOffset())); // Cancel out undesired offset
                    draw->setShape(cone.get());
                    osg::StateSet* stateset = draw->getOrCreateStateSet();
                      if(basicProgram)
                      {
                        stateset->setAttribute(basicProgram.get());
                      }
                      stateset->addUniform(
                          new osg::Uniform("specularStrength", info->specCoeff[index])
                      );
                      stateset->addUniform(
                                  new osg::Uniform("objectColor", osg::Vec3(info->color[index * 3 + 0], info->color[index * 3 + 1], info->color[index * 3 + 2]))
                              );
                      stateset->addUniform(
                                  new osg::Uniform("lightColor", osg::Vec3(1.0f, 1.0f, 1.0f))
                              );
                     
                }
                else if(iterator->second==4)
                {
                    draw->setShape(new osg::Box(osg::Vec3f(0, 0, info->length[index] * 0.5), info->width[index], info->height[index], info->length[index]));
                    osg::StateSet* stateset = draw->getOrCreateStateSet();
                      if(basicProgram)
                      {
                        stateset->setAttribute(basicProgram.get());
                      }
                      stateset->addUniform(
                          new osg::Uniform("specularStrength", info->specCoeff[index])
                      );
                      stateset->addUniform(
                                  new osg::Uniform("objectColor", osg::Vec3(info->color[index * 3 + 0], info->color[index * 3 + 1], info->color[index * 3 + 2]))
                              );
                      stateset->addUniform(
                                  new osg::Uniform("lightColor", osg::Vec3(1.0f, 1.0f, 1.0f))
                              );
                     
                }
                else if(iterator->second==5)
                {
                    draw->setShape(new osg::Cylinder(osg::Vec3f(0, 0, info->length[index] * 0.5), info->width[index] * 0.5, info->length[index]));
                    osg::StateSet* stateset = draw->getOrCreateStateSet();
                      if(basicProgram)
                      {
                        stateset->setAttribute(basicProgram.get());
                      }
                      stateset->addUniform(
                          new osg::Uniform("specularStrength", info->specCoeff[index])
                      );
                      stateset->addUniform(
                                  new osg::Uniform("objectColor", osg::Vec3(info->color[index * 3 + 0], info->color[index * 3 + 1], info->color[index * 3 + 2]))
                              );
                      stateset->addUniform(
                                  new osg::Uniform("lightColor", osg::Vec3(1.0f, 1.0f, 1.0f))
                              );
                     
                }
                else if(iterator->second==6)
                {
                    draw->setShape(new osg::Sphere(osg::Vec3f(0, 0, info->length[index] * 0.5), info->length[index] * 0.5));
                    osg::StateSet* stateset = draw->getOrCreateStateSet();
                      if(basicProgram)
                      {
                        stateset->setAttribute(basicProgram.get());
                      }
                      stateset->addUniform(
                          new osg::Uniform("specularStrength", info->specCoeff[index])
                      );
                      stateset->addUniform(
                                  new osg::Uniform("objectColor", osg::Vec3(info->color[index * 3 + 0], info->color[index * 3 + 1], info->color[index * 3 + 2]))
                              );
                      stateset->addUniform(
                                  new osg::Uniform("lightColor", osg::Vec3(1.0f, 1.0f, 1.0f))
                              );
                     
                }
                else 
                {
                    draw->setShape(new osg::Capsule(osg::Vec3f(0, 0, 0), 0.1, 0.5));
                    osg::StateSet* stateset = draw->getOrCreateStateSet();
                      if(basicProgram)
                      {
                        stateset->setAttribute(basicProgram.get());
                      }
                      stateset->addUniform(
                          new osg::Uniform("specularStrength", info->specCoeff[index])
                      );
                      stateset->addUniform(
                                  new osg::Uniform("objectColor", osg::Vec3(info->color[index * 3 + 0], info->color[index * 3 + 1], info->color[index * 3 + 2]))
                              );
                      stateset->addUniform(
                                  new osg::Uniform("lightColor", osg::Vec3(1.0f, 1.0f, 1.0f))
                              );
                     
                }
                node.accept(_vbo);
              }
          }
          // std::cout << "UpdateVisitor::apply: found shape type for " << _searchName << " with type " << iterator->second << std::endl;
          
        }
    }
    traverse(node);
  }
  virtual void apply(osg::MatrixTransform &node) override {
    // node.setMatrix(_visualizer->_mat);
    if(node.getName() == _searchName + "t")
    {
      using TransformData = global::TransformData;
      std::map<std::string, std::shared_ptr<TransformData> > &transformDataMap = global::transformDataMap;
      std::shared_ptr<TransformData> info = transformDataMap[_searchName];
      const int len = int(info->basis.size() / 9) - 1;
      if(len >= 0)
      {
        int index = static_cast<int>(std::round(global::time * len)); // index of transformData
        rAndT rT = rotateModelica2OSG(
            osg::Matrix3(info->basis[index * 9 + 0], info->basis[index * 9 + 1], info->basis[index * 9 + 2],
                        info->basis[index * 9 + 3], info->basis[index * 9 + 4], info->basis[index * 9 + 5],
                        info->basis[index * 9 + 6], info->basis[index * 9 + 7], info->basis[index * 9 + 8]),
            osg::Vec3f(info->rotation[index *  3 + 0], info->rotation[index *  3 + 1], info->rotation[index *  3 + 2]),
            osg::Vec3f(info->rShape[index *  3 + 0], info->rShape[index *  3 + 1], info->rShape[index *  3 + 2]), // not array
            osg::Vec3f(info->lengthDir[index * 3 + 0], info->lengthDir[index * 3 + 1], info->lengthDir[index * 3 + 2]),
            osg::Vec3f(info->widthDir[index *  3 + 0], info->widthDir[index *  3 + 1], info->widthDir[index *  3 + 2]), // not array
            _shapesTypes[_searchName] );
        osg::Matrix mat = osg::Matrix(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);    
        assemblePokeMatrix(mat, rT._T, rT._r);
        node.setMatrix(mat);
      }
    }
    traverse(node);
  }
  int _mode;//0 add, 1 remove, 2 update
  int _childType;
  std::string _searchName;
  std::map<std::string, int> _shapesTypes; // searchname -> type
  VBOSetupVisitor _vbo;
};
#endif // UPDATE_VISITOR_H