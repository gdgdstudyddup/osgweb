
#ifndef UPDATE_VISITOR_H
#define UPDATE_VISITOR_H
#include <osg/NodeVisitor>
#include <osg/Geode>
#include <osg/MatrixTransform>
#include <osg/AutoTransform>
#include <osg/ShapeDrawable>
#include <map>

using namespace std;

class ModelData;
class DXFFile;
// This class forces the use of VBO.
class VBOSetupVisitor : public osg::NodeVisitor
{
public:
	VBOSetupVisitor()
		: osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN) {}
	virtual void apply(osg::Geode& geode) {
		for (unsigned int i = 0; i < geode.getNumDrawables(); ++i) {
			osg::Geometry* geom = dynamic_cast<osg::Geometry*>(geode.getDrawable(i));
			if (geom) {
				geom->setUseVertexBufferObjects(true);
				// geom->setUseVertexArrayObject(true);
			}
		}
		NodeVisitor::apply(geode);
	}
};
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
  virtual void apply(osg::Group &node) override;
  int _mode; // 0: transform+geod + drawable;
  std::string _searchName;
  std::vector<std::string> _shapesIdentifiers;
  std::map<std::string, int> _shapesTypes; // searchname -> type
  std::map<std::string, ModelData*> _modelDataMap;
};
class UpdateVisitor : public osg::NodeVisitor {
public:
  UpdateVisitor():_vbo(){ 
    setTraversalMode(NodeVisitor::TRAVERSE_ALL_CHILDREN);
    _mode = 0; }
  virtual ~UpdateVisitor() = default;
  UpdateVisitor(const UpdateVisitor &uv) = delete;
  UpdateVisitor &operator=(const UpdateVisitor &uv) = delete;
  virtual void apply(osg::Geode &node) override;
  virtual void apply(osg::MatrixTransform &node) override;
  int _mode;//0 add, 1 remove, 2 update
  int _childType;
  std::string _searchName;
  std::map<std::string, int> _shapesTypes; // searchname -> type
  osg::Matrix _mat;
  VBOSetupVisitor _vbo;
};
#endif // UPDATE_VISITOR_H