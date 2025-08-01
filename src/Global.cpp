#include "Global.h"
namespace global
{
   float time = 0.0f;
   std::map<std::string, TransformData* > transformDataMap;
   osg::ref_ptr<osg::Program> basicProgram;
}