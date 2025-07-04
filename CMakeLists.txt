CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

# Path to OpenSceneGraph
SET(EXT_PROJ_DIR "${CMAKE_SOURCE_DIR}/../..")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s ALLOW_MEMORY_GROWTH=1 -sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2 --js-library ${CMAKE_SOURCE_DIR}/src/test.js")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Specify critical OpenSceneGraph build variables.
SET(BUILD_OSG_APPLICATIONS NO CACHE BOOL "Do not build applications")
SET(EGL_LIBRARY "GL" CACHE STRING "Suppress linkage error")
SET(OSG_GL1_AVAILABLE OFF CACHE BOOL "Unavailable under Emscripten")
SET(OSG_GL2_AVAILABLE OFF CACHE BOOL "Unavailable under Emscripten")
SET(OSG_GLES2_AVAILABLE ON CACHE BOOL "GLES2 is what Emscripten uses")
SET(OSG_WINDOWING_SYSTEM "None" CACHE STRING "Since we use SDL2 for OpenGL context creation, we don't have a windowing system")
SET(DYNAMIC_OPENTHREADS OFF CACHE BOOL "Link OpenThreads statically")
SET(DYNAMIC_OPENSCENEGRAPH OFF CACHE BOOL "Link OpenSceneGraph statically")
# Prevent CMake error during configuration.
SET(_OPENTHREADS_ATOMIC_USE_GCC_BUILTINS_EXITCODE "0" CACHE STRING "Prevent cfg error")

# Reference SDL2 during build process.
# We use SDL2 to do the following:
# * OpenGL functions' address retrieval
# * OpenGL graphics context creation
SET(USE_SDL2 "-s USE_SDL=2")
SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${USE_SDL2}" CACHE STRING "Use SDL2" FORCE)
# Build and reference OpenSceneGraph.
SET(OSG_SOURCE_DIR "${EXT_PROJ_DIR}/OpenSceneGraph")
SET(OSG_BUILD_DIR "${EXT_PROJ_DIR}/OpenSceneGraph/build/Emscripten")
FILE(MAKE_DIRECTORY ${OSG_BUILD_DIR})
ADD_SUBDIRECTORY(${OSG_SOURCE_DIR} ${OSG_BUILD_DIR})
INCLUDE_DIRECTORIES(${OSG_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${OSG_BUILD_DIR}/include)
LINK_DIRECTORIES(${OSG_BUILD_DIR}/lib)

# FILE(GLOB SOURCES "src/*.cpp")
SET(SOURCES 
    src/main.cpp 
    src/Global.cpp 
    src/Application.cpp 
    src/UpdateVisitor.cpp
    src/ExtraShapes.cpp
    src/CADHelper.cpp 
)
# Build sample.
INCLUDE_DIRECTORIES(src-gen)
ADD_EXECUTABLE(sample-ems ${SOURCES})
# Make Emscripten generate ready-to-open HTML page.
SET(CMAKE_EXECUTABLE_SUFFIX ".html")
# Make Emscripten preload the resource.
# SET_TARGET_PROPERTIES(sample-ems PROPERTIES LINK_FLAGS "--preload-file box.obj")
# Copy the resource to the build directory.
# CONFIGURE_FILE(box.osgt box.osgt COPYONLY)
# Libraries must be linked in the specified order.
# Otherwise you may get unsatisified linker errors.
TARGET_LINK_LIBRARIES(
    sample-ems
    osgViewer
    osgDB
    # osgDB plugins start.
    osgdb_osg
    osgdb_serializers_osg
    # osgDB plugins end.
    osgGA
    osgText
    osgUtil
    osg
    OpenThreads
)
# target_link_libraries(sample-ems PRIVATE "-sEXPORTED_FUNCTIONS=_main,_foo,_malloc,_free")
TARGET_LINK_OPTIONS(sample-ems PRIVATE
    -sEXPORTED_RUNTIME_METHODS=ccall,cwrap
)