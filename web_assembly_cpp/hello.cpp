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

const float SCREEN_RATIO = 400./800.;

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

            for( float t = 0.; t < 1.; t += 0.05 ) {

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

            // for( auto i: curveX) {
            //     cout << "i ->" << i << '\n';
            // }

            // for( auto i: curveY) {
            //     cout << "j ->" << i << '\n';
            // }
            
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

    glutPostRedisplay();

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

class Vertex {
    public: 
        float* arr;
        // float* size = 199 * 8;
        void drawAttrib() {
            Render shaders;
            shaders.initShader( shaders );

            GLuint vertexBufferID;	// Creates a unsigned int to store the VBO address later
            glGenBuffers(1, &vertexBufferID);	// Generate the new buffer (1 of) and copy its address to the uint we made earlier 
            
            glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID); // Bind an array buffer to the VBO
            
            glEnableVertexAttribArray(0); // Tell opengl were using vertex data (with the id of 0)
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0); // How should it read the data
            glEnableVertexAttribArray(0); // Tell opengl were using vertex data (with the id of 0)
        }

        void draw( std::vector<float> color, int fetch, int MODE=GL_LINES ) {
            
            Render shaders;
            shaders.initShader( shaders );
            // sizeof is 199 * 8
            // since there are 199 elements in the arr itself
            // and each elements is a float
            // thus compiler assigns it 4 bytes + 3 for padding
            // sos in total sizeof is 199 * ( 1 + 3 + 4 )
            glBufferData(GL_ARRAY_BUFFER, 39 * 2 * sizeof( float* ), arr, GL_DYNAMIC_DRAW); // Send the vert data to the buffer

            vector<float> lineColor = { 1., 1., 1. };
            glUniform3fv(glGetUniformLocation(shaders.shaderProgram, "color"), 1, &color[0]);

            // GLfloat SM[] = {
            //     *p, 0., 0., 0.,
            //     0., *p, 0., 0.,
            //     0., 0., 1., 0.,
            //     0., 0., 0., 1.,
            // };

            // GLfloat TM[] = {
            //     1., 0., 0., 0.,
            //     0., 1., 0., *ty,
            //     0., 0., 1., 0.,
            //     0., 0., 0., 1.,
            // };

            glUniform1f( glGetUniformLocation(shaders.shaderProgram, "SMX"), *p );
            glUniform1f( glGetUniformLocation(shaders.shaderProgram, "TMX"), *ty );

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
            shaders.clearShader( shaders );
            glDeleteProgram( shaders.shaderProgram );
        }
};

class Draw {

    void drawQuarter( 
        float cx, 
        float cy, 
        float sx, 
        float sy,  
        std::vector<float> arr_c
    ) {
            
            float r_ = abs( sx );
            float scale = 0.5;

            Bezier b;
            Vertex vertex;

            float arr[] = { 

                cx, sy + cy,
                cx + sx * 0.552F/(1.77f), sy + cy,
                
                cx + sx * r_/(1.77f * r_), sy * .552f + cy,
                cx + sx * r_/(1.77f * r_), cy,
            };

            vector<float> col{ 0., 0.521, 1. };

            vertex.arr = arr;
            vertex.draw( col, 4 );

            vertex.arr = &arr_c[0];
            vertex.draw( color, 39 );
    }
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

        std::vector<float> calcQuarterBezier( 
            float cx, 
            float cy, 
            float sx, 
            float sy
        ){

            Bezier b;

            float r_ = abs( sx );
            float scale = 0.;

            float circle_x[] = { 
                cx, 
                cx + sx * .552F/(1.77f), 
                cx + sx * r_/(1.77f * r_), 
                cx + sx * r_/(1.77f * r_) 
            };
            float circle_y[] = { 
                sy + cy, 
                sy + cy, 
                sy * .552f + cy, 
                cy
            };
        
            vector<float> arr_c = b.computeBezier( 
                circle_x, 
                circle_y, 
                sizeof( circle_x ) / sizeof( float ),
                sizeof( circle_y ) / sizeof( float )
            );

            return arr_c;
        }

        void circle( 
            float cx, 
            float cy, 
            float sx, 
            float sy,
            std::vector<float> arr_c
         ) {

            drawQuarter( cx, cy, sx, sy, arr_c );
            // drawQuarter( cx, cy, -1 * sx, sy, arr_c );
            // drawQuarter( cx, cy, -1 * sx, -1 * sy, arr_c );
            // drawQuarter( cx, cy, sx, -1 * sy, arr_c );
        }
};

Vertex vertex;
Draw d;

std::vector<float> col1 = { 1., 1., 1. };
std::vector<float> col2 = { 0., 0.521, 1. };
std::vector<float> col3 = { 1., 0., 0.258 };

// std::vector<float> c_vec_1 = d.calcQuarterBezier( 0.5, 0., .5, .5 );
// std::vector<float> c_vec_2 = d.calcQuarterBezier( 0.5, 0., -.5, .5 );
// std::vector<float> c_vec_3 = d.calcQuarterBezier( 0.5, 0., -.5, -.5 );
// std::vector<float> c_vec_4 = d.calcQuarterBezier( 0.5, 0., .5, -.5 );

int shape = 0;
void choose_shape( int shape_ ) {

    shape = shape_;
}

std::vector<float> col1_1 = { 1., 1., 1. };
std::vector<float> col2_1 = { 0., 0.521, 1. };
std::vector<float> col3_1 = { 1., 0., 0.258 };

// std::vector<float> c_vec_1_1 = d.calcQuarterBezier( 0., 0., .5, .5 );
// std::vector<float> c_vec_2_1 = d.calcQuarterBezier( 0., 0., -.5, .5 );
// std::vector<float> c_vec_3_1 = d.calcQuarterBezier( 0., 0., -.5, -.5 );
// std::vector<float> c_vec_4_1 = d.calcQuarterBezier( 0., 0., .5, -.5 );

struct Position {
    float x = 0.;
    float y = 0.;
    float dx = 1.; 
    float dy = 1.; 
};

struct Dot {
    float x = 0.;
    float y = 0.;
    // float r_ = 0.;
    // glPointSize( r_ );    
};

Position position;
Dot dot{ 0., 0. };

float mid = 1920. * .5;
float mid_y = 1080. * .5;
bool mousedown = false;

// Draw d;

struct Ellipse {

    float x0 = 0.;
    float y0 = 0.;

    float x_1 = 0.;
    float y_1 = 0.;
    
    float x_2 = -x_1;
    float y_2 = y_1;
    
    float x_3 = -x_1;
    float y_3 = -y_1;
    
    float x_4 = x_1;
    float y_4 = -y_1;

    std::vector<float> c_vec_1 = d.calcQuarterBezier( x0, y0, x_1, y_1 );
    std::vector<float> c_vec_2 = d.calcQuarterBezier( x0, y0, x_2, y_2 );
    std::vector<float> c_vec_3 = d.calcQuarterBezier( x0, y0, x_3, y_3 );
    std::vector<float> c_vec_4 = d.calcQuarterBezier( x0, y0, x_4, y_4 );
    
    void drawEllipse( vector<float> color ) {
        d.color = col1;
        d.circle( x0, y0, x_1, y_1, c_vec_1 );
        d.circle( x0, y0, x_2, y_2, c_vec_2 );
        d.circle( x0, y0, x_3, y_3, c_vec_3 );
        d.circle( x0, y0, x_4, y_4, c_vec_4 );        
    }
};

EM_BOOL mouse_callback(int eventType, const EmscriptenMouseEvent *e, void *userData)
{
  printf("%s, screen: (%ld,%ld), client: (%ld,%ld),%s%s%s%s button: %hu, buttons: %hu, movement: (%ld,%ld), canvas: (%ld,%ld), timestamp: %lf\n",
    "", e->screenX, e->screenY, e->clientX, e->clientY,
    e->ctrlKey ? " CTRL" : "", e->shiftKey ? " SHIFT" : "", e->altKey ? " ALT" : "", e->metaKey ? " META" : "", 
    e->button, e->buttons, e->movementX, e->movementY, e->canvasX, e->canvasY,
    e->timestamp);

    mousedown = true;


    position.x = (e->screenX - mid)/mid;
    
    position.y = (mid_y - e->screenY)/mid_y;

    cout << "x ->" << position.x << endl;
    // cout << position.y << endl;

    cout << shape << endl;
    glutPostRedisplay();

  return 0;
}

EM_BOOL mouse_move ( int eventType, const EmscriptenMouseEvent *e, void *userData ) {

    if( !mousedown ) return 0;

    glClearColor(0.188, 0.188, 0.188, .0f);
    glClear(GL_COLOR_BUFFER_BIT);

    float arr[] = {
        position.x, position.y,
        position.x, position.dy,
        position.dx, position.y,
        
        position.dx, position.dy,
        position.dx, position.y,
        position.x, position.dy,
    };

    // Draw d;

    position.dx = (e->screenX - mid)/mid;
    position.dy = position.y + (mid_y - e->screenY)/mid_y;


    if( shape == 0 ) {
        
        cout << (e->screenX - mid)/mid << endl;
        
        if( (e->screenX - mid)/mid <= dot.x + 10 && (e->screenX - mid)/mid <= dot.y + 10 ) {

            dot.x = (e->screenX - mid)/mid;
            dot.y = position.y + (mid_y - e->screenY)/mid_y;
        }

        float dot_arr[] = {
            dot.x, dot.y
        };

        vertex.arr = dot_arr;
        vertex.draw( col1, 1, GL_POINTS );
    }
    // glClear(GL_COLOR_BUF/FER_BIT);
    
    if( shape == 1 ) {
        
        d.color = col1;
        d.rect( arr, 6 );
    }

    if( shape == 3 ) {

        Ellipse el { position.x, position.y, position.dx, position.dy };
        el.drawEllipse( col1 );
    }

    return 0;
}

static vector<float> rects; 
static vector<float> ellipses; 

static int ind = 0;
static int ind_e = 0;

EM_BOOL mouse_up_callback( int eventType, const EmscriptenMouseEvent *e, void *userData ) {

    mousedown = false;

    float arr[] = {
        position.x, position.y,
        position.x, position.dy,
        position.dx, position.y,
        
        position.dx, position.dy,
        position.dx, position.y,
        position.x, position.dy,
    };

    float arr_el[] = {
        position.x, position.y,
        position.dx, position.dy
    };

    if( shape == 1 ) {
        ind += 6;
        for( int i = 0; i < 12; i++ ) {
            // cout << arr[i] << endl;
            rects.push_back( arr[i] );
        }
    }

    if( shape == 3 ) {
        ind_e += 4;
        for( int i = 0; i < 4; i++ ) {
            // cout << arr[i] << endl;
            ellipses.push_back( arr[i] );
        }
    }

    shape = 0;
    glutPostRedisplay();

    return 0;
}

// Ellipse e { 0., 0., .5, .5 };

void c() {
    glClearColor(0.188, 0.188, 0.188, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    float dot_arr[] = {
        dot.x, dot.y
    };

    vertex.arr = dot_arr;
    vertex.draw( col1, 1, GL_POINTS );

    // e.drawEllipse( col1 );

    // cout << shape << endl;

    switch( shape ) {

        case 1:
            // glClear(GL_COLOR_BUFFER_BIT);

            // glutPostRedisplay();
            break;
        
        case 3:

            // glClear(GL_COLOR_BUFFER_BIT);

            // Ellipse ellipse { .5, .5, 1., 1. };
            // ellipse.drawEllipse( col1 );

            // glutPostRedisplay();
            break;
    };

    for( auto i: rects ) {
        cout << i << endl;
    }

    // d.vertex( &ellipses[0], ind_e );
    for( int i = 0; i < ind_e; i+=4 ) {
        
        Ellipse ellipse { ellipses[i], ellipses[i+1], ellipses[i+2], ellipses[i+3] };
        // ellipse.x0 = ellipses[i]; 
        // ellipse.y0 = ellipses[i+1];
        // ellipse.x_1 = ellipses[i+2];
        // ellipse.y_1 = ellipses[i+3];

        ellipse.drawEllipse( col1 );
    }

    d.color = col1_1;
    d.rect( &rects[0], ind );

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
    emscripten_set_mouseup_callback("#canvas", 0, 1, mouse_up_callback);	
    emscripten_set_mousemove_callback("#canvas", 0, 1, mouse_move);

    glClearColor(0.984, 0.4627, 0.502, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

    vertex.drawAttrib();

    glutDisplayFunc( c );
    glutPostRedisplay();
    // glutMouseFunc(mousescroll);
    // glutMainLoop();
    // emscripten_request_animation_frame_loop( cb, userData );
    // glutReshapeFunc(reshape);

	return 0;
}

#ifdef __cplusplus
}
#endif