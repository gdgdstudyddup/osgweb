#ifndef SHAPE_MANAGER_H
#define SHAPE_MANAGER_H
#include <osg/Drawable>
class ShapeManager
{
    public:
        // Constructor
        ShapeManager(){};

        // Destructor
        ~ShapeManager();

        // Method to add a shape
        void addShape(osg::ref_ptr<osg::Drawable> shape);

        // Method to remove a shape
        void removeShape(osg::ref_ptr<osg::Drawable> shape);
        void removeShapeAt(int index);

        // Method to list all shapes
        // void listShapes() const;
    private:
        std::vector<osg::ref_ptr<osg::Drawable> > _shapes;
};
ShapeManager::~ShapeManager() {
    _shapes.clear();    
}
void ShapeManager::addShape(osg::ref_ptr<osg::Drawable> shape) {
    _shapes.push_back(shape);
}
void ShapeManager::removeShape(osg::ref_ptr<osg::Drawable> shape) {
    auto it = std::find_if(_shapes.begin(), _shapes.end(),
        [&shape](const osg::ref_ptr<osg::Drawable>& ptr) {
            return ptr == shape; 
        });
    if (it != _shapes.end()) {
        _shapes.erase(it);
    }
}
void ShapeManager::removeShapeAt(int index) {
    if (index >= 0 && index < _shapes.size()) {
        _shapes.erase(_shapes.begin() + index);
    }
}
#endif // SHAPE_MANAGER_H