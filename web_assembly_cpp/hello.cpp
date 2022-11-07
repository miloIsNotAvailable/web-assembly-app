#include <iostream>
#include <emscripten/emscripten.h>
#include<string>
#include <GLES2/gl2.h>
#include <GLES3/gl3.h>
#include <EGL/egl.h>
#include<math.h>
#include <vector>

// #ifndef __gles2_gl3_h_
// #define __gles2_gl3_h_ 1

#ifdef __cplusplus
extern "C" {
#endif
// #define GLM_FORCE_PURE

using std::vector;
using namespace std;
// typedef double float double;

void print( const char* input ) {
    cout << input << "\n";
}

#include <emscripten/html5.h> // emscripten module
#include <GL/glut.h>

void myFunction() {
    print("called function in cpp");
}

// calculates n take k
double binomial( int n, int k ) {

    double product = 1.;

    for( int i = 1; i <= k; i ++ ) {
        product = product * (( n + 1 - i ) / i);
    }

    if (n == k){
        product = 1.;
    }

    return product;
}

// calculates bezier from 
// this sexy equation: 
// https://miro.medium.com/max/640/1*c8Rg7_G5n-3Vwmzr4pMzXA.png
double bezier( double t, int n ) {
    
    double sum = 0;
    
    for( int i = 0; i < n; i ++ ) {
        sum = sum + binomial( n, i ) * pow( 1 - t, n - i ) * pow( t, i );
    }

    return sum;
}

class Bezier {
    public: 
        vector<float> computeBezier( float vx[], float vy[], int size_x, int size_y ) {

            vector<float> curveX;
            vector<float> curveY;
            
            vector<float> curve;

            // convert array to vector
            vector<float> vx_vec( vx, vx + size_x );
            vector<float> vy_vec( vy, vy + size_y );

            // cout <<  sizeof(vx) / sizeof(*vx) << endl;

            int n = vx_vec.size() - 1;
            int v = 0;

            for( float t = 0.; t < 1.; t += 0.01 ) {

                float curveXpoint{ 0 };
                float curveYpoint{ 0 };

                for( int i = 0; i <= n; i++ ) {

                    curveXpoint = curveXpoint + binomial( n, i ) * pow( 1 - t, n - i ) * pow( t, i ) * vx_vec[i];
                    curveYpoint = curveYpoint + binomial( n, i ) * pow( 1 - t, n - i ) * pow( t, i ) * vy_vec[i];
                    
                }
                // probably not the best way to do it but
                // 
                // push twice: as end point then as starting point 
                curve.push_back( curveXpoint );
                curve.push_back( curveYpoint );

                // leave index 0 as is
                if( t * 100. > 0. ) {
                    curve.push_back( curveXpoint );
                    curve.push_back( curveYpoint );
                }
                
                curveX.push_back( curveXpoint );
                curveY.push_back( curveXpoint );
            }

            for( auto i: curveX) {
                cout << "i ->" << i << '\n';
            }

            for( auto i: curveY) {
                cout << "j ->" << i << '\n';
            }
            
            float* curveX_arr = &curveX[0];
            float* curveY_arr = &curveY[0];
            
            float* curve_arr = &curve[0];

            return curve;
        }
};

void Print( vector<double>& v ) {
    for( auto& i: v ) {
        cout << i << endl;
    }
}

double* copy_array(double in_array[], int length) {
    double out_array[length];
    for (int i=0; i<length; i++) {
        in_array[i] = in_array[i] * 2;
        out_array[i] = in_array[i];
        cout << in_array[i] << "\n";
    }
return out_array;
}

static float i = 1.;
static float *p = &i;

static float trans_y = 0.;
static float *ty = &trans_y;

EM_BOOL wheel_callback(int eventType, const EmscriptenWheelEvent *e, void *userData)
{

    if( !e->mouse.ctrlKey ) {

        (*ty) = *ty + .001 * e->deltaY;    
        return 1;
    };

    if( *p < 0. ) *p = 0.;

    (*p) = *p + .001 * e->deltaY * -1;

    printf("delta:(%g,%g,%g)\n",
        (float)e->deltaX, *p, (float)e->deltaY);

    // return 1 to preventDefault
    return 1;
}

EM_BOOL mouse_callback(int eventType, const EmscriptenMouseEvent *e, void *userData)
{
  printf("%s, screen: (%ld,%ld), client: (%ld,%ld),%s%s%s%s button: %hu, buttons: %hu, movement: (%ld,%ld), canvas: (%ld,%ld), timestamp: %lf\n",
    "", e->screenX, e->screenY, e->clientX, e->clientY,
    e->ctrlKey ? " CTRL" : "", e->shiftKey ? " SHIFT" : "", e->altKey ? " ALT" : "", e->metaKey ? " META" : "", 
    e->button, e->buttons, e->movementX, e->movementY, e->canvasX, e->canvasY,
    e->timestamp);

  return 0;
}

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
        "uniform mat4 TM;\n"    
        "\n" 
        "void main() {\n"
        "   float u = aPos.x;\n"
        "   float v = aPos.y;\n"
        "    gl_Position =  vec4(aPos, 1.) * TM * SM;\n"
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

            // glDeleteShader(vertexShader);
            // glDeleteShader(fragmentShader);

            // link shaders
            glAttachShader(r.shaderProgram, r.vertexShader);
            glAttachShader(r.shaderProgram, r.fragmentShader);
            glLinkProgram(r.shaderProgram);
            glUseProgram( r.shaderProgram );
        }

        void clearShader( Render r ) {
            
            glDetachShader( r.shaderProgram, r.fragmentShader );
            glDetachShader( r.shaderProgram, r.vertexShader );
        }
};

class Vertex {
    public: 
        float* arr;
        // float* size = 199 * 8;
        void draw( std::vector<float> color, int fetch, int MODE=GL_LINES ) {
            
            Render shaders;
            shaders.initShader( shaders );

            GLuint vertexBufferID;	// Creates a unsigned int to store the VBO address later
            glGenBuffers(1, &vertexBufferID);	// Generate the new buffer (1 of) and copy its address to the uint we made earlier 
            
            glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID); // Bind an array buffer to the VBO
            
            // sizeof is 199 * 8
            // since there are 199 elements in the arr itself
            // and each elements is a float
            // thus compiler assigns it 4 bytes + 3 for padding
            // sos in total sizeof is 199 * ( 1 + 3 + 4 )
            glBufferData(GL_ARRAY_BUFFER, 199 * 2 * sizeof( float* ), arr, GL_STATIC_DRAW); // Send the vert data to the buffer
                
            glEnableVertexAttribArray(0); // Tell opengl were using vertex data (with the id of 0)
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0); // How should it read the data
            glEnableVertexAttribArray(0); // Tell opengl were using vertex data (with the id of 0)

            vector<float> lineColor = { 1., 1., 1. };
            glUniform3fv(glGetUniformLocation(shaders.shaderProgram, "color"), 1, &color[0]);

            GLfloat SM[] = {
                *p, 0., 0., 0.,
                0., *p, 0., 0.,
                0., 0., 1., 0.,
                0., 0., 0., 1.,
            };

            GLfloat TM[] = {
                1., 0., 0., 0.,
                0., 1., 0., *ty,
                0., 0., 1., 0.,
                0., 0., 0., 1.,
            };

            glUniformMatrix4fv(glGetUniformLocation(shaders.shaderProgram, "SM"), 1, GL_FALSE, &SM[0]);
            glUniformMatrix4fv(glGetUniformLocation(shaders.shaderProgram, "TM"), 1, GL_FALSE, &TM[0]);

            glLineWidth( 3 );
            // glDrawElements(GL_LINES, 6, GL_UNSIGNED_SHORT, nullptr); // Draw the data using the element array
            glDrawArrays(MODE, 0, fetch); // Draw the data using the element array
            
            glEnable(GL_POINT_SMOOTH);
            lineColor = { 0., 0.521, 1. };
            glUniform3fv(glGetUniformLocation(shaders.shaderProgram, "color"), 1, &color[0]);
            // glDrawElements(GL_POINTS, 6, GL_UNSIGNED_SHORT, nullptr); // Draw the data using the element array
            // glDrawArrays(GL_POINTS, 0, 199); // Draw the data using the element array
            shaders.clearShader( shaders );
        }
};

class Draw {
    public:
        std::vector<float> color;
        void vertex( float* arr, int size ) {
            Vertex vertex;
            
            vertex.arr = arr;

            vertex.draw( color, size );
        }
    
        void rect( float* arr, int size ) {
            Vertex vertex;
            
            vertex.arr = arr;

            vertex.draw( color, size, GL_TRIANGLES );
        }
};

float vx[] = { -1., -.5, .5, 1. };
float vy[] = { -1., 1., -.5, 1. };

float ux[] = { -1., -.5, .5, 1. };
float uy[] = { -1., 1., .5, 1. };

Bezier b;

// manually pass in lengths of arrays 
// since length of dynamic array is 
// just sizeof( type )
vector<float> arr = b.computeBezier( 
    vx, 
    vy, 
    sizeof( vx ) / sizeof( float ),
    sizeof( vy ) / sizeof( float )
);

vector<float> arr_2 = b.computeBezier( 
    ux, 
    uy, 
    sizeof( ux ) / sizeof( float ),
    sizeof( uy ) / sizeof( float )
);

float arr2[] = { 
    -1., -1.,
    -0.5, 1.,
    
    -0.5, 1.,
    .5, -.5,

    .5, -.5,
    1., 1.
 };

float arr3[] = { 

    0., 1.,
    0.552, 1.,
    
    0.552, 1.,
    1., .552,
    
    1., .552,
    1., -.552,


 };

float cx[] = { 0., 0.552, 1., 1. };
float cy[] = { 1., 1., .552, -.552 };

vector<float> arr_c = b.computeBezier( 
    cx, 
    cy, 
    sizeof( cx ) / sizeof( float ),
    sizeof( cy ) / sizeof( float )
);

Vertex vertex;
Draw d;

std::vector<float> col1 = { 1., 1., 1. };
std::vector<float> col2 = { 0., 0.521, 1. };
std::vector<float> col3 = { 1., 0., 0.258 };

EM_BOOL cb ( double time, void* userData ){

    glClearColor(0.188, 0.188, 0.188, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // convert vector arr, to array
    d.color = col1;
    d.vertex( &arr[0], 199 );

    d.color = col2;
    d.vertex( &arr2[0], 6 );
    
    d.color = col3;
    d.vertex( &arr3[0], 6 );

    d.color = col1;  
    d.vertex( &arr_c[0], 199 );

    return 1;
}

void* userData;

int main()
{
    print( "Hello world!" );

	// setting up EmscriptenWebGLContextAttributes
	EmscriptenWebGLContextAttributes attr;
	emscripten_webgl_init_context_attributes(&attr);
	attr.alpha = 0;

	// target the canvas selector
	EMSCRIPTEN_WEBGL_CONTEXT_HANDLE ctx = emscripten_webgl_create_context("#canvas", &attr);
	emscripten_webgl_make_context_current(ctx);

    EM_BOOL ret = emscripten_set_wheel_callback("#canvas", 0, 1, wheel_callback);
    emscripten_set_mousedown_callback("#canvas", 0, 1, mouse_callback);	
    
    glClearColor(0.984, 0.4627, 0.502, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

    emscripten_request_animation_frame_loop( cb, userData );

    // glutReshapeFunc(reshape);

	return 0;
}

#ifdef __cplusplus
}
#endif