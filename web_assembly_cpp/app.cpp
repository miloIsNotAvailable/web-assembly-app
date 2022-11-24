#include <vector>
#include <iostream>
#include <emscripten/emscripten.h>
#include<string>
#include <GLES2/gl2.h>
#include <GLES3/gl3.h>
#include <EGL/egl.h>
#include<math.h>
#include <emscripten/html5.h> // emscripten module
#include <GL/glut.h>

using std::vector;

#ifdef __cplusplus
extern "C" {
#endif

class Render {
    private: 
        const char *fragmentShaderSource = "\n"
        "#ifdef GL_ES\n"
        "precision highp float;\n"
        "#endif\n"
        "\n"    
        "uniform vec2 u_resolution;\n"
        "uniform vec2 u_mouse;\n"
        "uniform float u_time;\n"
        "uniform vec3 color;\n"
        "void main(){\n"
        "vec2 st = gl_FragCoord.xy/u_resolution.xy;\n"
        "st.x *= u_resolution.x/u_resolution.y;\n"
        "vec2 coord = gl_PointCoord - vec2(0.5);  //from [0,1] to [-0.5,0.5]\n"
        "if(length(coord) > .5 )                  //outside of circle radius?\n"
        "    discard;\n"
        "\n"    
        "gl_FragColor = vec4(color, 1.0);\n"
        "}\0";
        
        const char *vertexShaderSource = "\n"
        "#ifdef GL_ES\n"
        "precision highp float;\n"
        "#endif\n"
        "attribute vec3 aPos;\n"        
        "uniform mat4 SM;\n"    
        "uniform float SMX;\n"    
        "uniform float TMX;\n"    
        "uniform mat4 TM;\n"    
        "\n" 
        "void main() {\n"
        "   float u = aPos.x;\n"
        "   float v = aPos.y;\n"
        "    gl_Position =  vec4(aPos.x * SMX, aPos.y * SMX + TMX, aPos.z, 1.);\n"
        "    gl_PointSize = 10.;\n"
        "}\n\0";

        int vertexShader = glCreateShader(GL_VERTEX_SHADER);
        int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    public:
        int shaderProgram = glCreateProgram();

        void initShader( Render r ) {
            // vertex shader
            glShaderSource(r.vertexShader, 1, &r.vertexShaderSource, NULL);
            glCompileShader(r.vertexShader);
            // check for shader compile errors

            // fragment shader
            glShaderSource(r.fragmentShader, 1, &r.fragmentShaderSource, NULL);
            glCompileShader(r.fragmentShader);

            // link shaders
            glAttachShader(r.shaderProgram, r.vertexShader);
            glAttachShader(r.shaderProgram, r.fragmentShader);
            
            glLinkProgram(r.shaderProgram);
            clearShader( r );
            glUseProgram( r.shaderProgram );
            
        }

        void clearShader( Render r ) {
            
            // glDetachShader( r.shaderProgram, r.fragmentShader );
            // glDetachShader( r.shaderProgram, r.vertexShader );
            glDeleteShader( r.vertexShader );
            glDeleteShader( r.fragmentShader );
        }
};

#ifdef __cplusplus
}
#endif

