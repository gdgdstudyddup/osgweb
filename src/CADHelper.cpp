#include "CADHelper.h"

/*!
 * \brief CADGroup constructor
 * \param osg::Node* subgraph
 */
CADGroup::CADGroup(osg::Node* subgraph)
  : osg::Group()
{
  addChild(subgraph);
  CADVisitor visitor(this);
  subgraph->accept(visitor);
}

/*!
 * \brief CADGroup::scaleVertices
 * \param osg::Geode& geode
 * \param bool scaling
 * \param float scaleX
 * \param float scaleY
 * \param float scaleZ
 */
void CADGroup::scaleVertices(osg::ref_ptr<osg::Geode> geode, bool scaling, float scaleX, float scaleY, float scaleZ)
{
  if (!scaling) {
    scaleX = scaleY = scaleZ = 1;
  }
  unsigned int num = geode->getNumDrawables();
  for (unsigned int i = 0; i < num; i++) {
    osg::Drawable* drawable = geode->getDrawable(i);
    if (drawable) {
      osg::Geometry* geometry = drawable->asGeometry();
      if (geometry) {
        osg::Vec3Array* vertices = dynamic_cast<osg::Vec3Array*>(geometry->getVertexArray());
        if (vertices) {
          osg::Vec3Array* unscaledVertices = unscaledGeometryVertices[geometry];
          if (unscaledVertices) {
            osg::Vec3Array::size_type size = unscaledVertices->size();
            for (osg::Vec3Array::size_type j = 0; j < size; j++) {
              osg::Vec3& unscaledVertex = unscaledVertices->at(j);
              osg::Vec3& vertex = vertices->at(j);
              vertex.x() = unscaledVertex.x() * scaleX;
              vertex.y() = unscaledVertex.y() * scaleY;
              vertex.z() = unscaledVertex.z() * scaleZ;
            }
            vertices->dirty();
            drawable->dirtyBound();
            drawable->dirtyDisplayList();
          }
        }
      }
    }
  }
}

/*!
 * \brief CADVisitor constructor
 * \param CADGroup* cadFile
 */
CADVisitor::CADVisitor(CADGroup* cadFile)
  : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
{
  this->cadFile = cadFile;
}

/*!
 * \brief CADVisitor::apply
 * \param osg::Geode& geode
 */
void CADVisitor::apply(osg::Geode& geode)
{
  unsigned int num = geode.getNumDrawables();
  for (unsigned int i = 0; i < num; i++) {
    osg::Drawable* drawable = geode.getDrawable(i);
    if (drawable) {
      osg::Geometry* geometry = drawable->asGeometry();
      if (geometry) {
        osg::Vec3Array* vertices = dynamic_cast<osg::Vec3Array*>(geometry->getVertexArray());
        if (vertices) {
          cadFile->unscaledGeometryVertices[geometry] = dynamic_cast<osg::Vec3Array*>(vertices->clone(osg::CopyOp::DEEP_COPY_ARRAYS));
        }
      }
    }
  }
}