#ifndef CAD_HELPER_H
#define CAD_HELPER_H
#include <osg/Node>
#include <osg/Group>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Shape>
#include <osg/NodeVisitor>

#include <unordered_map>
#include <cmath>

#include <osg/StateAttribute>
#include <osg/StateSet>

#include <iostream>
template<typename T>
struct std::hash<const osg::ref_ptr<T>> {
  std::size_t operator()(const osg::ref_ptr<T>& ref) const {
    return reinterpret_cast<std::uintptr_t>(ref.get());
  }
};

// class CADVisitor;

class CADGroup : public osg::Group
{
public:
  CADGroup(osg::Node* subgraph);
  ~CADGroup() = default;
  void scaleVertices(osg::ref_ptr<osg::Geode> geode, bool scaling, float scaleX, float scaleY, float scaleZ);


  std::unordered_map<const osg::ref_ptr<osg::Geometry>, osg::ref_ptr<osg::Vec3Array>> unscaledGeometryVertices;

//   friend class CADVisitor;
};

class CADVisitor : public osg::NodeVisitor
{
public:
  CADVisitor(CADGroup* cadFile);
  ~CADVisitor() {cadFile.release();}
  void apply(osg::Geode& geode) override;


  osg::ref_ptr<CADGroup> cadFile;
};

#endif