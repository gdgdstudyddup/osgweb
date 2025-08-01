#ifndef BASIC_SHADER_H
#define BASIC_SHADER_H
#include <osg/Program>

// Fragment shader to display everything in red colour.
// 33 83  99-37 44 59-62  08-79
 
static const char shaderFragment[] =
    "#version 300 es\n"
    "precision highp float;\n"
    "#define EPSILON 1e-6\n"
    "#define PI 3.141592653589793\n"
    "#define saturate( a ) clamp( a, 0.0, 1.0 )\n"
    "#define RECIPROCAL_PI 0.3183098861837907\n"
    "in vec3 normal;\n"
    "in vec3 position;\n"
    "in vec2 texCoord; \n"
    "in vec4 vertexColor; \n"
    //"in vec3 lightPos; \n"
    "uniform float mixFactor; \n"
    "uniform vec3 lightColor; \n"
    "uniform vec3 objectColor; \n"
    "uniform float specularStrength; \n"
    "uniform sampler2D baseTexture; \n"
    "out vec4 glFragColor;\n"
    "//uniform sampler2D baseTexture1; \n"
    "//uniform vec3 u_TestColor; \n"
    "uniform mat4 osg_ViewMatrix;\n"
    "vec3 F_Schlick( const in vec3 f0, const in float f90, const in float dotVH ) {\n"
    "    float fresnel = exp2( ( - 5.55473 * dotVH - 6.98316 ) * dotVH );\n"
    "    return f0 * ( 1.0 - fresnel ) + ( f90 * fresnel );\n"
    "}\n" // validated
    "float G_BlinnPhong_Implicit( /* const in float dotNL, const in float dotNV */ ) {\n"
    "    return 0.25;\n"
    "}\n"

    "float D_BlinnPhong( const in float shininess, const in float dotNH ) {\n"
    "    return RECIPROCAL_PI * ( shininess * 0.5 + 1.0 ) * pow( dotNH, shininess );\n"
    "}\n"
    "vec3 BRDF_BlinnPhong( const in vec3 lightDir, const in vec3 viewDir, const in vec3 normal, const in vec3 specularColor, const in float shininess ) {\n"
    "    vec3 halfDir = normalize( lightDir + viewDir );\n"
    "    float dotNH = saturate( dot( normal, halfDir ) );\n"
    "    float dotVH = saturate( dot( viewDir, halfDir ) );\n"
    "    vec3 F = F_Schlick( specularColor, 1.0, dotVH );\n"
    "    float G = G_BlinnPhong_Implicit( /* dotNL, dotNV */ );\n"
    "    float D = D_BlinnPhong( shininess, dotNH );\n"
    "    return F * ( G * D );\n"
    "}\n" // validated
    "vec3 BRDF_Lambert( const in vec3 diffuseColor ) {\n"
    "    return RECIPROCAL_PI * diffuseColor;\n"
    "}\n" // validated
    "float pow2( const in float x ) { return x*x; }\n"
    "vec3 pow2( const in vec3 x ) { return x*x; }\n"
    "float V_GGX_SmithCorrelated_Anisotropic( const in float alphaT, const in float alphaB, const in float dotTV, const in float dotBV, const in float dotTL, const in float dotBL, const in float dotNV, const in float dotNL )\n"
    "{\n"

	"	float gv = dotNL * length( vec3( alphaT * dotTV, alphaB * dotBV, dotNV ) );\n"
	"	float gl = dotNV * length( vec3( alphaT * dotTL, alphaB * dotBL, dotNL ) );\n"
	"	float v = 0.5 / ( gv + gl );\n"

	"	return saturate(v);\n"

	"}\n"
    "float V_GGX_SmithCorrelated( const in float alpha, const in float dotNL, const in float dotNV ) {\n"

    "    float a2 = pow2( alpha );\n"

    "    float gv = dotNL * sqrt( a2 + ( 1.0 - a2 ) * pow2( dotNV ) );\n"
    "    float gl = dotNV * sqrt( a2 + ( 1.0 - a2 ) * pow2( dotNL ) );\n"

    "   return 0.5 / max( gv + gl, EPSILON );\n"

    "}\n"
    "float D_GGX( const in float alpha, const in float dotNH ) \n"
    "{\n"

    "    float a2 = pow2( alpha );\n"

    "    float denom = pow2( dotNH ) * ( a2 - 1.0 ) + 1.0; \n"// avoid alpha = 0 with dotNH = 1

    "    return RECIPROCAL_PI * a2 / pow2( denom );\n"

    "}\n"
    "vec3 BRDF_GGX( const in vec3 lightDir, const in vec3 viewDir, const in vec3 normal)\n"
    "{\n"

    "    vec3 f0 = vec3(1.5);\n" // material.specularColor;
    "    float f90 = 0.65;\n" // material.specularF90;
    "    float roughness = 0.5;\n"

    "    float alpha = pow2( roughness );\n" // UE4's roughness

    "    vec3 halfDir = normalize( lightDir + viewDir );\n"

    "    float dotNL = saturate( dot( normal, lightDir ) );\n"
    "    float dotNV = saturate( dot( normal, viewDir ) );\n"
    "    float dotNH = saturate( dot( normal, halfDir ) );\n"
    "    float dotVH = saturate( dot( viewDir, halfDir ) );\n"

    "    vec3 F = F_Schlick( f0, f90, dotVH );\n"
    "    float V = V_GGX_SmithCorrelated( alpha, dotNL, dotNV );\n"

	"	 float D = D_GGX( alpha, dotNH );\n"
    "    return F * ( V * D );\n"
    "}\n"
    "void main()\n"
    "{\n"       
        "vec4 rgba = texture(baseTexture, texCoord); \n"
        "vec3 viewPos = vec3(0.0); \n"
        "//vec3 fixNormal = gl_FrontFacing ? normal : -normal; \n"
        "//vec3 N = normalize(fixNormal); \n"
        "vec3 fdx = dFdx( position );\n"
	    "vec3 fdy = dFdy( position );\n"
	    "vec3 N = normalize( cross( fdx, fdy ) );\n"
        "vec3 V = normalize(viewPos - position); \n"
        "//vec3 worldNormal = normalize( ( vec4( norm, 0.0 ) * osg_ViewMatrix ).xyz ); \n"
        "vec3 lightDir = normalize(vec3(0, 0.01, 1.0));//normalize(lightPos - position); \n"
        "float dotNL = saturate( dot( N, lightDir ) );\n"
	    "float irradiance = dotNL;\n"
        "vec3 diffuseColor = rgba.rgb * mix(vertexColor.rgb, objectColor.rgb, mixFactor);\n"
        "vec3 directDiffuse  = irradiance * 2.5 * BRDF_Lambert( diffuseColor );\n"
        "vec3 directSpecular = irradiance * BRDF_GGX( lightDir, V, N) * specularStrength; \n"
        "vec3 Lo = directDiffuse + directSpecular;\n"  
        "vec3 ambient = max(dotNL, 0.5) * vec3(0.2) * diffuseColor;\n"
        "vec3 color = ambient + Lo;\n"
        "//color = color / (color + vec3(1.0));\n"
        "//color = pow(color, vec3(1.0/2.2));\n"
        "glFragColor = vec4(color, rgba.a); \n"
    "} \n";
    
// static const char shaderFragment[] =
//     "#version 300 es\n"
//     "precision highp float;\n"
//     "in vec3 normal;\n"
//     "in vec3 position;\n"
//     "in vec2 texCoord; \n"
//     "in vec4 vertexColor; \n"
//     //"in vec3 lightPos; \n"
//     "uniform float mixFactor; \n"
//     "uniform vec3 lightColor; \n"
//     "uniform vec3 objectColor; \n"
//     "uniform float specularStrength; \n"
//     "uniform sampler2D baseTexture; \n"
//     "out vec4 glFragColor;\n"
//     "//uniform sampler2D baseTexture1; \n"
//     "//uniform vec3 u_TestColor; \n"
//     "uniform mat4 osg_ViewMatrix;\n"
//     "void main() {                            \n"
//     " vec4 rgba = texture(baseTexture, texCoord); \n"
//     "vec3 viewPos = vec3(0.0); \n"
//     "float ambientStrength = 0.15; \n"
//     "vec3 ambient = ambientStrength * lightColor; \n"
  	
//     "// diffuse \n"
//     // "vec3 fdx = dFdx( position );\n"
// 	// "vec3 fdy = dFdy( position );\n"
// 	// "vec3 dNormal = normalize( cross( fdx, fdy ) );\n"
//     "vec3 fixNormal = gl_FrontFacing ? normal : -normal; \n"
//     "vec3 norm = normalize(fixNormal); \n"
//     "vec3 worldNormal = normalize( ( vec4( norm, 0.0 ) * osg_ViewMatrix ).xyz ); \n"
//     "vec3 lightDir = vec3(0, 0, 1);//normalize(lightPos - position); \n"
//     "float diff = max(dot(norm, lightDir), 0.0); \n"
//     "vec3 diffuse = diff * lightColor; \n"
//     "\n"
//     "// specular\n"
//     "//float specularStrength = 0.5; \n"
//     "vec3 viewDir = normalize(viewPos - position); \n"
//     "vec3 reflectDir = reflect(-lightDir, norm); \n"  
//     "float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0); \n"
//     "vec3 specular = specularStrength * spec * lightColor; \n"  
//     "vec3 extraColor = mix(vertexColor.rgb, objectColor.rgb, mixFactor);\n"
//     "vec3 result = (ambient + diffuse + specular) * extraColor * rgba.rgb; // * objectColor; \n"
    
//     "glFragColor = vec4(result, rgba.a); \n"
//     "}                                         \n";
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

#endif // BASIC_SHADER_H
