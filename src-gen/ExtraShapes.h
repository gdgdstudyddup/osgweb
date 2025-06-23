#ifndef EXTRASHAPES_H
#define EXTRASHAPES_H

#include <osg/Node>
#include <osg/Group>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Shape>

#include <unordered_map>
#include <cmath>

#include <osg/StateAttribute>
#include <osg/StateSet>

#include <iostream>


// TODO: Support is missing for the following shape types:
//  - beam,
//  - gearwheel.
// They are currently replaced by a capsule.
// In addition, the extra parameter is not always considered, in particular for cone and cylinder shapes.
// See documentation of Modelica.Mechanics.MultiBody.Visualizers.Advanced.Shape model.
// Also, the spring shape is implemented but has an undesired torsion near the end of each winding,
// and it should be drawn with more facets for a nicer animation
// (moreover, it misses normals and texture coordinates).

struct ModelData
{
  std::string _namefromJs;
	float* _position;
	float* _normal;
	float* _color;
	int _lengthOfVertex;
	int* _indices;
	int _lengthOfIndices;
	unsigned char* _imageData;
	int _width;
	int _height;
	int _channel;
  ModelData(
    std::string namefromJs,
    float* position,
    float* normal,
    float* color,
    int lengthOfVertex,
    int* indices,
    int lengthOfIndices,
    unsigned char* imageData,
    int width,
    int height,
    int channel)
  {
    _namefromJs = namefromJs;
    if(lengthOfVertex > 0)
    {
      _lengthOfVertex = lengthOfVertex;
      int sizeOfFloat = sizeof(float);
      _position = (float*)malloc(sizeOfFloat * lengthOfVertex * 3);
      _normal = (float*)malloc(sizeOfFloat * lengthOfVertex * 3);
      _color = (float*)malloc(sizeOfFloat * lengthOfVertex * 4);

      memcpy(_position, position, sizeOfFloat * lengthOfVertex * 3);
      memcpy(_normal, normal, sizeOfFloat * lengthOfVertex * 3);
      memcpy(_color, color, sizeOfFloat * lengthOfVertex * 4);

      if(lengthOfIndices > 0)
      {
        _lengthOfIndices = lengthOfIndices;
        _indices = (int*)malloc(sizeof(int) * lengthOfIndices * 3);
        memcpy(_indices, indices, sizeof(int) * lengthOfIndices);
      }

      if(width > 0 && height > 0)
      {
        _imageData = (unsigned char*)malloc(sizeof(unsigned char) * width * height * channel);
        memcpy(_imageData, imageData, sizeof(unsigned char) * width * height * channel);
      }
    }
  }
  ~ModelData()
  {
    if(_position)
    {
     free(_position);
     _position = nullptr;
    }
    if(_normal)
    {
     free(_normal);
     _normal = nullptr;
    }
    if(_color)
    {
     free(_color);
     _color = nullptr;
    }
    if(_indices)
    {
     free(_indices);
     _indices = nullptr;
    }
    if(_imageData)
    {
     free(_imageData);
     _imageData = nullptr;
    }
  }
};

struct rAndT
{
  rAndT()
      : _r(osg::Vec3f()),
        _T(osg::Matrix3())
  {
  }
  osg::Vec3f _r;
  osg::Matrix3 _T;
};

struct Directions
{
  Directions()
      : _lDir(osg::Vec3f()),
        _wDir(osg::Vec3f())
  {
  }
  osg::Vec3f _lDir;
  osg::Vec3f _wDir;
};

// BEGIN FEATURE VBO

// END   FEATURE VBO
class DXFFile: public osg::Geometry
{
    public:
        DXFFile(ModelData* data);
        ~DXFFile() {};
        
};

class Pipecylinder : public osg::Geometry
{
public:
  Pipecylinder(float rI, float rO, float l);
  ~Pipecylinder() {};
};

class Spring : public osg::Geometry
{
public:
  Spring(float r, float rCoil, float nWindings,  float l);
  ~Spring() {};
private:
  osg::Vec3f getNormal(osg::Vec3f vec, float length = 1);
  osg::Vec3f rotateX(osg::Vec3f vec, float phi);
  osg::Vec3f rotateY(osg::Vec3f vec, float phi);
  osg::Vec3f rotateZ(osg::Vec3f vec, float phi);
  osg::Vec3f rotateArbitraryAxis_expensive(osg::Vec3f vec, osg::Vec3f axis, float phi);
  osg::Vec3f rotateArbitraryAxis(osg::Vec3f vec, osg::Vec3f axis, float phi);
  float angleBetweenVectors(osg::Vec3f vec1, osg::Vec3f vec2);

  osg::Vec3Array* mpOuterVertices;
  osg::Vec3Array* mpSplineVertices;
};


osg::Vec3f Mat3mulV3(osg::Matrix3 M, osg::Vec3f V);
osg::Vec3f V3mulMat3(osg::Vec3f V, osg::Matrix3 M);
osg::Matrix3 Mat3mulMat3(osg::Matrix3 M1, osg::Matrix3 M2);
osg::Vec3f normalize(osg::Vec3f vec);
osg::Vec3f cross(osg::Vec3f vec1, osg::Vec3f vec2);
Directions fixDirections(osg::Vec3f lDir, osg::Vec3f wDir);
void assemblePokeMatrix(osg::Matrix& M, const osg::Matrix3& T, const osg::Vec3f& r);
rAndT rotateModelica2OSG(osg::Matrix3 T, osg::Vec3f r, osg::Vec3f r_shape, osg::Vec3f lDir, osg::Vec3f wDir, int type);
rAndT rotateModelica2OSG(osg::Matrix3 T, osg::Vec3f r, osg::Vec3f dir);

#endif //end EXTRASHAPES_H