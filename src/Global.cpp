#include "Global.h"
namespace global
{
   float time = 0.0f;
   std::map<std::string, std::shared_ptr<TransformData> > transformDataMap;
   osg::ref_ptr<osg::Program> basicProgram;
}