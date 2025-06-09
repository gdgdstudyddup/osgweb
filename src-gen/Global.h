#include<map>
using namespace std;

namespace global{
    struct TransformData
    {
        std::vector<float> basis; // 9 array
        std::vector<float> rotation; // 3 array
        std::vector<float> lengthDir; // 3 array
        std::vector<float> length; // 1 array
        std::vector<float> width; // 1 array
        std::vector<float> height; // 1 array
        std::vector<float> color; // 3 array
        std::vector<float> specCoeff; // 1 array

        std::vector<float> rShape; // 3 array
        std::vector<float> widthDir; // 3 array
        std::vector<float> extra; // 1 array
        
        TransformData(){};
        ~TransformData(){
            basis.clear();
            rotation.clear();
            rShape.clear();
            lengthDir.clear();
            widthDir.clear();
            length.clear();
            width.clear();
            height.clear();
            color.clear();
            specCoeff.clear();
        };
    };

    // struct TransformData
    // {
    //   osg::Vec3Array basis;
    //   osg::MatrixfArray rotation;
    //   osg::Vec3 rShape;
    //   osg::Vec3Array lengthDir;
    //   osg::Vec4 widthDir;
    //   osg::FloatArray length;
    //   osg::FloatArray width;
    //   osg::FloatArray height;
    //   osg::Vec3Array color;
    //   osg::FloatArray specCoeff;
    
    // };
	std::map<std::string, std::shared_ptr<TransformData> > transformDataMap;
    float time = 0.0f;
}