#include <iostream>
#include <emscripten/emscripten.h>
#include<string>
// #include <GLES2/gl2.h>
#include <GLES3/gl32.h>
#include <EGL/egl.h>
#include<math.h>
#include <vector>
#include <GLFW/glfw3.h>
#include <emscripten/html5.h> // emscripten module
#include <GL/glut.h>

using std::vector;

#ifdef __cplusplus
extern "C" {
#endif

class RenderShapes {
    private: 
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
        "   gl_Position =  vec4(aPos.x * SMX, aPos.y * SMX + TMX, aPos.z, 1.);\n"
        "   gl_PointSize = 10.;\n"
        "}\n\0";

        const char* geometryShaderSrc = R"glsl(
            #version 200 es

            layout(points) in;
            layout(line_strip, max_vertices = 2) out;

            void main()
            {
                gl_Position = gl_in[0].gl_Position + vec4(-0.1, 0.0, 0.0, 0.0);
                EmitVertex();

                gl_Position = gl_in[0].gl_Position + vec4(0.1, 0.0, 0.0, 0.0);
                EmitVertex();

                EndPrimitive();
            }
        )glsl";

        int vertexShader = glCreateShader(GL_VERTEX_SHADER);
        int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        int geometryShader = glCreateShader( GL_GEOMETRY_SHADER );
    public:
        int shaderProgram = glCreateProgram();

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
        // "   vec2 r = gl_FragCoord.xy - vec2(1000., 1.);\n"
        // "   if( length( r ) > 1000. ) { discard; }"
        "gl_FragColor = vec4(color, 1.0);\n"
        "}\0";

        void initShader(  ) {
            // vertex shader
            glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
            glCompileShader(vertexShader);
            // check for shader compile errors

            // fragment shader
            glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
            glCompileShader(fragmentShader);

            // fragment shader
            // glShaderSource(geometryShader, 1, &geometryShaderSrc, NULL);
            // glCompileShader(geometryShader);

            // link shaders
            glAttachShader(shaderProgram, vertexShader);
            glAttachShader(shaderProgram, fragmentShader);
            // glAttachShader(shaderProgram, geometryShader);

            glLinkProgram( shaderProgram );
            clearShader();
            glUseProgram( shaderProgram );
            
        }

        void clearShader(  ) {
            
            // glDetachShader( r.shaderProgram, r.fragmentShader );
            // glDetachShader( r.shaderProgram, r.vertexShader );
            glDeleteShader( vertexShader );
            glDeleteShader( fragmentShader );
            // glDeleteShader( r.geometryShader );
        }
};

class Vert {
    public: 
        float* arr;
        // float* size = 199 * 8;
        void drawAttrib() {
            RenderShapes shaders;
            shaders.initShader(  );

            GLuint vertexBufferID;	// Creates a unsigned int to store the VBO address later
            glGenBuffers(1, &vertexBufferID);	// Generate the new buffer (1 of) and copy its address to the uint we made earlier 
            
            glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID); // Bind an array buffer to the VBO
            
            glEnableVertexAttribArray(0); // Tell opengl were using vertex data (with the id of 0)
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0); // How should it read the data
            glEnableVertexAttribArray(0); // Tell opengl were using vertex data (with the id of 0)
        }

        void draw( std::vector<float> color, int fetch, float p, float ty, int MODE=GL_LINES ) {
            
            RenderShapes shaders;

            const char *fragmentShaderSource = "\n"
            "#ifdef GL_ES\n"
            "precision highp float;\n"
            "#endif\n"
            "\n"    
            // "uniform vec2 u_resolution;\n"
            "uniform mat4 SM;\n"    
            "uniform vec2 u_mouse;\n"
            "uniform float u_time;\n"
            "uniform vec3 color;\n"
            "void main(){\n"
            "vec2 u_resolution = vec2( 1920., 1080. );\n"
            "vec2 st = gl_FragCoord.xy/u_resolution.xy;\n"
            "st.x *= u_resolution.x/u_resolution.y;\n"
            "vec2 coord = gl_PointCoord - vec2(0.5);  //from [0,1] to [-0.5,0.5]\n"
            "if(length(coord) > .5 ) {                  //outside of circle radius?\n"
            "    discard;\n"
            "}\n"    
            "   vec2 r = -1. + 2. * st;\n"
            "   if( length( r ) < .5 ) {\n"
            "       gl_FragColor = vec4( color, 1. );\n"  
            "       return;\n"  
            "}\n"
            "gl_FragColor = vec4(0.188, 0.188, 0.188, 1.0);\n"
            "}\0";

            shaders.fragmentShaderSource = fragmentShaderSource;

            shaders.initShader(  );
            // sizeof is 199 * 8
            // since there are 199 elements in the arr itself
            // and each elements is a float
            // thus compiler assigns it 4 bytes + 3 for padding
            // sos in total sizeof is 199 * ( 1 + 3 + 4 )
            glBufferData(GL_ARRAY_BUFFER, 39 * 2 * sizeof( float* ), arr, GL_DYNAMIC_DRAW); // Send the vert data to the buffer

            vector<float> lineColor = { 1., 1., 1. };
            glUniform3fv(glGetUniformLocation(shaders.shaderProgram, "color"), 1, &color[0]);

            glUniform1f( glGetUniformLocation(shaders.shaderProgram, "SMX"), p );
            glUniform1f( glGetUniformLocation(shaders.shaderProgram, "TMX"), ty );
            glUniform1f( glGetUniformLocation(shaders.shaderProgram, "TMX"), ty );

            // glUniformMatrix4fv(glGetUniformLocation(shaders.shaderProgram, "SM"), 1, GL_FALSE, &SM[0]);
            // glUniformMatrix4fv(glGetUniformLocation(shaders.shaderProgram, "TM"), 1, GL_FALSE, &TM[0]);

            glLineWidth( 3 );
            // glDrawElements(GL_LINES, 6, GL_UNSIGNED_SHORT, nullptr); // Draw the data using the element array
            glDrawArrays(MODE, 0, fetch); // Draw the data using the element array
            
            glEnable(GL_POINT_SMOOTH);
            lineColor = { 0., 0.521, 1. };
            glUniform3fv(glGetUniformLocation(shaders.shaderProgram, "color"), 1, &color[0]);
            // glDrawElements(GL_POINTS, 6, GL_UNSIGNED_SHORT, nullptr); // Draw the data using the element array
            // glDrawArrays(GL_POINTS, 0, 199); // Draw the data using the element array
            shaders.clearShader(  );
            glDeleteProgram( shaders.shaderProgram );

            glFlush();
        }
        
        void drawLine( std::vector<float> color, int fetch, float p, float ty, int MODE=GL_LINES ) {
            
            RenderShapes shaders;

            const char *fragmentShaderSource = R"glsl(
            #ifdef GL_ES
            precision mediump float;
            #endif

            #define PI 3.14159265359
            #define TWO_PI 6.28318530718

            // uniform vec2 u_resolution;
            uniform vec2 u_mouse;
            uniform float u_time;

            vec2 u_resolution = vec2( 1920., 1080. );

            float quadraticBezier (float x, vec2 a){
            // adapted from BEZMATH.PS (1993)
            // by Don Lancaster, SYNERGETICS Inc. 
            // http://www.tinaja.com/text/bezmath.html

            float epsilon = 0.00001;
            a.x = clamp(a.x,0.0,1.0); 
            a.y = clamp(a.y,0.0,1.0); 
            if (a.x == 0.5){
                a += epsilon;
            }
            
            // solve t from x (an inverse operation)
            float om2a = 1.0 - 2.0 * a.x;
            float t = (sqrt(a.x*a.x + om2a*x) - a.x)/om2a;
            float y = (1.0-2.0*a.y)*(t*t) + (2.0*a.y)*t;
            return y;
            }

            float lineSegment(vec2 p, vec2 a, vec2 b) {
                
                float t = 0.006;
                
                vec2 pa = p - a; 
                vec2 ba = b - a;
                
                float h = clamp( dot(pa,ba)/dot(ba,ba), 0., 1. );
                return smoothstep(t-1./u_resolution.y, t, length(pa - ba*h));
            }

            void main(){
            vec2 st = gl_FragCoord.xy/u_resolution.xy;
            st.x *= u_resolution.x/u_resolution.y;
                
                vec3 col = vec3( 1. ); 
                vec2 p = -1. + 2. * st;
                
                // vec2 cp = vec2(u_mouse.x/u_resolution.x,u_mouse.y/u_resolution.y);
                vec2 cp2 = vec2(0.530,0.680);
                
                col = mix(vec3(0.), col, lineSegment(st, vec2(0.650,0.160), cp2));
                // col = mix(vec3(0.), col, lineSegment(st, cp, cp2));
                // col = mix(vec3(0.), col, lineSegment(st, cp2, vec2(0.170,0.820)));
                
                col = mix(vec3(0.), col, lineSegment(st, cp2, vec2(0.170,0.820)));
                

                gl_FragColor = vec4( col, 1. );
            })glsl";

            shaders.fragmentShaderSource = fragmentShaderSource;

            shaders.initShader(  );
            // sizeof is 199 * 8
            // since there are 199 elements in the arr itself
            // and each elements is a float
            // thus compiler assigns it 4 bytes + 3 for padding
            // sos in total sizeof is 199 * ( 1 + 3 + 4 )
            glBufferData(GL_ARRAY_BUFFER, 39 * 2 * sizeof( float* ), arr, GL_DYNAMIC_DRAW); // Send the vert data to the buffer

            vector<float> lineColor = { 1., 1., 1. };
            glUniform3fv(glGetUniformLocation(shaders.shaderProgram, "color"), 1, &color[0]);

            glUniform1f( glGetUniformLocation(shaders.shaderProgram, "SMX"), p );
            glUniform1f( glGetUniformLocation(shaders.shaderProgram, "TMX"), ty );
            glUniform1f( glGetUniformLocation(shaders.shaderProgram, "TMX"), ty );

            // glUniformMatrix4fv(glGetUniformLocation(shaders.shaderProgram, "SM"), 1, GL_FALSE, &SM[0]);
            // glUniformMatrix4fv(glGetUniformLocation(shaders.shaderProgram, "TM"), 1, GL_FALSE, &TM[0]);

            glLineWidth( 3 );
            // glDrawElements(GL_LINES, 6, GL_UNSIGNED_SHORT, nullptr); // Draw the data using the element array
            glDrawArrays(MODE, 0, fetch); // Draw the data using the element array
            
            glEnable(GL_POINT_SMOOTH);
            lineColor = { 0., 0.521, 1. };
            glUniform3fv(glGetUniformLocation(shaders.shaderProgram, "color"), 1, &color[0]);
            // glDrawElements(GL_POINTS, 6, GL_UNSIGNED_SHORT, nullptr); // Draw the data using the element array
            // glDrawArrays(GL_POINTS, 0, 199); // Draw the data using the element array
            shaders.clearShader(  );
            glDeleteProgram( shaders.shaderProgram );

            glFlush();
        }
};

#ifdef __cplusplus
}
#endif

