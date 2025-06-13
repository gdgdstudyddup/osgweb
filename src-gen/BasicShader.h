#ifndef BASIC_SHADER_H
#define BASIC_SHADER_H
#include <osg/Program>

// Fragment shader to display everything in red colour.
static const char shaderFragment[] =
    "#version 300 es\n"
    "precision highp float;\n"
    "in vec3 normal;\n"
    "in vec3 position;\n"
    "in vec2 texCoord; \n"
    "in vec4 vertexColor; \n"
    //"in vec3 lightPos; \n"
    "uniform vec3 lightColor; \n"
    "uniform vec3 objectColor; \n"
    "uniform float specularStrength; \n"
    "uniform sampler2D baseTexture; \n"
    "out vec4 glFragColor;\n"
    "//uniform sampler2D baseTexture1; \n"
    "//uniform vec3 u_TestColor; \n"
    "uniform mat4 osg_ViewMatrix;\n"
    "void main() {                            \n"
    " vec4 rgba = texture(baseTexture, texCoord); \n"
    "vec3 viewPos = vec3(0.0); \n"
    "float ambientStrength = 0.1; \n"
    "vec3 ambient = ambientStrength * lightColor; \n"
  	
    "// diffuse \n"
    // "vec3 fdx = dFdx( position );\n"
	// "vec3 fdy = dFdy( position );\n"
	// "vec3 dNormal = normalize( cross( fdx, fdy ) );\n"
    "vec3 norm = normalize(normal); \n"
    "vec3 worldNormal = normalize( ( vec4( norm, 0.0 ) * osg_ViewMatrix ).xyz ); \n"
    "vec3 lightDir = vec3(0, 0, 1);//normalize(lightPos - position); \n"
    "float diff = max(dot(norm, lightDir), 0.0); \n"
    "vec3 diffuse = diff * lightColor; \n"
    "\n"
    "// specular\n"
    "//float specularStrength = 0.5; \n"
    "vec3 viewDir = normalize(viewPos - position); \n"
    "vec3 reflectDir = reflect(-lightDir, norm); \n"  
    "float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64.0); \n"
    "vec3 specular = specularStrength * spec * lightColor; \n"  
    " \n"
    "vec3 result = (ambient + diffuse + specular) * objectColor; // * objectColor; \n"
    
    "glFragColor = vec4(result, 1.0); \n"
    "}                                         \n";
// Geometry shader to pass geometry vertices to fragment shader.
static const char shaderVertex[] =
    // "attribute vec4 gl_MultiTexCoord0; \n"
    // "attribute vec4 gl_Color; \n"
    // "attribute vec3 gl_Normal; \n"
    "#version 300 es\n"
    "out vec3 normal;\n"
    "out vec3 position;\n"
    "out vec3 eye;\n"
    "out vec2 texCoord; \n"
    "out vec4 vertexColor; \n"
    //"out vec3 lightPos; \n"
    //"uniform vec3 uLightPos; \n"
    "void main() {                                              \n"
    
    "texCoord = gl_MultiTexCoord0.xy; \n"
    "vertexColor = gl_Color; \n"
    //"lightPos = (gl_ModelViewMatrix * vec4(uLightPos, 1.0)).xyz; \n"
    "normal = (gl_NormalMatrix * gl_Normal).rgb; \n"
    "position = (gl_ModelViewMatrix * gl_Vertex).xyz; \n"
    "gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;  \n"
    "}                                                          \n";

// osg::Program *CreateShaderProgram(
//     const std::string &vertexShader,
//     const std::string &fragmentShader)
// {
//     // Load shaders.
//     // osg::Shader *vs = new osg::Shader(osg::Shader::VERTEX, vertexShader);
//     // osg::Shader *fs = new osg::Shader(osg::Shader::FRAGMENT, fragmentShader);
    
//     // Compile shaders and compose shader program.
//     osg::Program *prog = new osg::Program;
//     prog->addShader(new osg::Shader(osg::Shader::VERTEX, vertexShader));
//     prog->addShader(new osg::Shader(osg::Shader::FRAGMENT, fragmentShader));
//     return prog;
// }
osg::ref_ptr<osg::Program> basicProgram;
#endif // BASIC_SHADER_H
