#ifndef VISUALIZER_HELPER_H
#define VISUALIZER_HELPER_H

#include <osg/Array>
#include <osg/Drawable>
#include <osg/Geometry>
#include <osg/Shape>
#include <osg/ShapeDrawable>
#include <osg/Vec3f>
#include <osg/Matrix>

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <exception>
#include <functional>
#include <limits>
#include <map>
#include <typeinfo>
#include <unordered_map>
#include <vector>

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
osg::Vec3f Mat3mulV3(osg::Matrix3 M, osg::Vec3f V)
{
  return osg::Vec3f(M[0] * V[0] + M[1] * V[1] + M[2] * V[2],
                    M[3] * V[0] + M[4] * V[1] + M[5] * V[2],
                    M[6] * V[0] + M[7] * V[1] + M[8] * V[2]);
}

osg::Vec3f V3mulMat3(osg::Vec3f V, osg::Matrix3 M)
{
  return osg::Vec3f(M[0] * V[0] + M[3] * V[1] + M[6] * V[2],
                    M[1] * V[0] + M[4] * V[1] + M[7] * V[2],
                    M[2] * V[0] + M[5] * V[1] + M[8] * V[2]);
}

osg::Matrix3 Mat3mulMat3(osg::Matrix3 M1, osg::Matrix3 M2)
{
  osg::Matrix3 M3;
  for (int i = 0; i < 3; ++i)
  {
    for (int j = 0; j < 3; ++j)
    {
      //cout<<" i and j "<<i<<" "<<j<<endl;
      float x = 0.0;
      for (int k = 0; k < 3; ++k)
      {
        //cout<<M1[i*3+k]<<" * "<<M2[k*3+j]<<" = "<<M1[i*3+k]*M2[k*3+j]<<endl;
        x = M1[i * 3 + k] * M2[k * 3 + j] + x;
      }
      M3[i * 3 + j] = x;
    }
  }

  return M3;
}

osg::Vec3f normalize(osg::Vec3f vec)
{
  osg::Vec3f vecOut;
  if (vec.length() >= 100 * 1.e-15)
    vecOut = vec / vec.length();
  else
    vecOut = vec / (100 * 1.e-15);
  return vecOut;
}

osg::Vec3f cross(osg::Vec3f vec1, osg::Vec3f vec2)
{
  return osg::Vec3f(vec1[1] * vec2[2] - vec1[2] * vec2[1],
                    vec1[2] * vec2[0] - vec1[0] * vec2[2],
                    vec1[0] * vec2[1] - vec1[1] * vec2[0]);
}

Directions fixDirections(osg::Vec3f lDir, osg::Vec3f wDir)
{
  Directions dirs;
  osg::Vec3f e_x;
  osg::Vec3f e_y;

  //lengthDirection
  double abs_n_x = lDir.length();
  if (abs_n_x < 1e-10)
    e_x = osg::Vec3f(1, 0, 0);
  else
    e_x = lDir / abs_n_x;

  //widthDirection
  osg::Vec3f n_z_aux = cross(e_x, wDir);
  osg::Vec3f e_y_aux;
  if (n_z_aux * n_z_aux > 1e-6)
    e_y_aux = wDir;
  else
  {
    if (fabs(e_x[0]) > 1e-6)
      e_y_aux = osg::Vec3f(0, 1, 0);
    else
      e_y_aux = osg::Vec3f(1, 0, 0);
  }
  e_y = cross(normalize(cross(e_x, e_y_aux)), e_x);

  dirs._lDir = e_x;
  dirs._wDir = e_y;
  return dirs;
}

void assemblePokeMatrix(osg::Matrix& M, const osg::Matrix3& T, const osg::Vec3f& r)
{
  M(3, 3) = 1.0;
  for (int row = 0; row < 3; ++row)
  {
    M(3, row) = r[row];
    M(row, 3) = 0.0;
    for (int col = 0; col < 3; ++col)
      M(row, col) = T[row * 3 + col];
  }
}

rAndT rotateModelica2OSG(osg::Matrix3 T, osg::Vec3f r, osg::Vec3f r_shape, osg::Vec3f lDir, osg::Vec3f wDir, int type)
{
  rAndT res;

  Directions dirs = fixDirections(lDir, wDir);
  osg::Vec3f hDir = dirs._lDir ^ dirs._wDir;
  //std::cout << "lDir " << dirs._lDir[0] << ", " << dirs._lDir[1] << ", " << dirs._lDir[2] << std::endl;
  //std::cout << "wDir " << dirs._wDir[0] << ", " << dirs._wDir[1] << ", " << dirs._wDir[2] << std::endl;
  //std::cout << "hDir " <<       hDir[0] << ", " <<       hDir[1] << ", " <<       hDir[2] << std::endl;

  osg::Matrix3 T0;
//   if (isCADType(type))
//   {
//     T0 = osg::Matrix3(dirs._lDir[0], dirs._lDir[1], dirs._lDir[2],
//                       dirs._wDir[0], dirs._wDir[1], dirs._wDir[2],
//                             hDir[0],       hDir[1],       hDir[2]);
//   } else 
//   {
    T0 = osg::Matrix3(dirs._wDir[0], dirs._wDir[1], dirs._wDir[2],
                            hDir[0],       hDir[1],       hDir[2],
                      dirs._lDir[0], dirs._lDir[1], dirs._lDir[2]);
//   }
  //std::cout << "T0 " << T0[0] << ", " << T0[1] << ", " << T0[2] << std::endl;
  //std::cout << "   " << T0[3] << ", " << T0[4] << ", " << T0[5] << std::endl;
  //std::cout << "   " << T0[6] << ", " << T0[7] << ", " << T0[8] << std::endl;

  res._r = V3mulMat3(r_shape, T) + r;
  res._T = Mat3mulMat3(T0, T);

  return res;
}

rAndT rotateModelica2OSG(osg::Matrix3 T, osg::Vec3f r, osg::Vec3f dir)
{
  rAndT res;

  // See https://math.stackexchange.com/a/413235
  int i = dir[0] ? 0 : dir[1] ? 1 : 2;
  int j = (i + 1) % 3;

  osg::Vec3f lDir = dir;
  osg::Vec3f wDir = osg::Vec3f();
  wDir[i] = -lDir[j];
  wDir[j] = +lDir[i];

  Directions dirs = fixDirections(lDir, wDir);
  osg::Vec3f hDir = dirs._lDir ^ dirs._wDir;
  //std::cout << "lDir " << dirs._lDir[0] << ", " << dirs._lDir[1] << ", " << dirs._lDir[2] << std::endl;
  //std::cout << "wDir " << dirs._wDir[0] << ", " << dirs._wDir[1] << ", " << dirs._wDir[2] << std::endl;
  //std::cout << "hDir " <<       hDir[0] << ", " <<       hDir[1] << ", " <<       hDir[2] << std::endl;

  osg::Matrix3 T0 = osg::Matrix3(dirs._wDir[0], dirs._wDir[1], dirs._wDir[2],
                                       hDir[0],       hDir[1],       hDir[2],
                                 dirs._lDir[0], dirs._lDir[1], dirs._lDir[2]);
  //std::cout << "T0 " << T0[0] << ", " << T0[1] << ", " << T0[2] << std::endl;
  //std::cout << "   " << T0[3] << ", " << T0[4] << ", " << T0[5] << std::endl;
  //std::cout << "   " << T0[6] << ", " << T0[7] << ", " << T0[8] << std::endl;

  res._r = r;
  res._T = Mat3mulMat3(T0, T);

  return res;
}
#endif // VISUALIZER_HELPER_H