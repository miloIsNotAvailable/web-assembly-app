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
        product = product * ( n + 1 - i ) / i;
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

float* computeBezier( float vx[], float vy[] ) {

    vector<float> curveX;
    vector<float> curveY;
    
    vector<float> curve;

    // convert array to vector
    vector<float> vx_vec( vx, vx + sizeof vx + 1 );
    vector<float> vy_vec( vy, vy + sizeof vy + 1 );

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
        // push twice: as end point then as beginning point 
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

    return curve_arr;
}

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

float vx[] = { -1., -1., 1., 1. };
float vy[] = { -.5, 1., 1., 1. };

EM_BOOL mouse_callback(int eventType, const EmscriptenMouseEvent *e, void *userData)
{
  printf("%s, screen: (%ld,%ld), client: (%ld,%ld),%s%s%s%s button: %hu, buttons: %hu, movement: (%ld,%ld), canvas: (%ld,%ld), timestamp: %lf\n",
    "", e->screenX, e->screenY, e->clientX, e->clientY,
    e->ctrlKey ? " CTRL" : "", e->shiftKey ? " SHIFT" : "", e->altKey ? " ALT" : "", e->metaKey ? " META" : "", 
    e->button, e->buttons, e->movementX, e->movementY, e->canvasX, e->canvasY,
    e->timestamp);

  return 0;
}

// float arr[200];
float* arr = computeBezier( vx, vy );

EM_BOOL cb ( double time, void* userData ){
    // printf( "ye" );
    // cout << sizeof(arr)/sizeof(float) << endl;
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

    // vertex shader
    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors

    // fragment shader
    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // glDeleteShader(vertexShader);
    // glDeleteShader(fragmentShader);

    // link shaders
    int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glUseProgram( shaderProgram );

    // cout << arr[199] << endl;

    GLfloat verts[] = {
    0.f, 0.2f,
0.0189916f, 0.211229f,
0.0189916f, 0.211229f,
0.0360121f, 0.220953f,
0.0360121f, 0.220953f,
0.0511298f, 0.229225f,
0.0511298f, 0.229225f,
0.0644116f, 0.2361f,
0.0644116f, 0.2361f,
0.0759237f, 0.241627f,
0.0759237f, 0.241627f,
0.0857312f, 0.245861f,
0.0857312f, 0.245861f,
0.093898f, 0.248849f,
0.093898f, 0.248849f,
0.100487f, 0.250643f,
0.100487f, 0.250643f,
0.105561f, 0.251291f,
0.105561f, 0.251291f,
0.10918f, 0.25084f,
0.10918f, 0.25084f,
0.111405f, 0.249338f,
0.111405f, 0.249338f,
0.112294f, 0.246831f,
0.112294f, 0.246831f,
0.111906f, 0.243364f,
0.111906f, 0.243364f,
0.110297f, 0.238982f,
0.110297f, 0.238982f,
0.107524f, 0.233727f,
0.107524f, 0.233727f,
0.103641f, 0.227644f,
0.103641f, 0.227644f,
0.0987029f, 0.220773f,
0.0987029f, 0.220773f,
0.0927622f, 0.213155f,
0.0927622f, 0.213155f,
0.0858709f, 0.204831f,
0.0858709f, 0.204831f,
0.07808f, 0.19584f,
0.07808f, 0.19584f,
0.0694394f, 0.18622f,
0.0694394f, 0.18622f,
0.059998f, 0.176008f,
0.059998f, 0.176008f,
0.0498039f, 0.165242f,
0.0498039f, 0.165242f,
0.0389038f, 0.153957f,
0.0389038f, 0.153957f,
0.0273437f, 0.142188f,
0.0273437f, 0.142188f,
0.0151686f, 0.129968f,
0.0151686f, 0.129968f,
0.00242228f, 0.117332f,
0.00242228f, 0.117332f,
-0.0108524f, 0.104311f,
-0.0108524f, 0.104311f,
-0.0246134f, 0.0909367f,
-0.0246134f, 0.0909367f,
-0.03882f, 0.07724f,
-0.03882f, 0.07724f,
-0.0534323f, 0.0632504f,
-0.0534323f, 0.0632504f,
-0.0684114f, 0.0489969f,
-0.0684114f, 0.0489969f,
-0.0837195f, 0.0345072f,
-0.0837195f, 0.0345072f,
-0.0993197f, 0.0198087f,
-0.0993197f, 0.0198087f,
-0.115176f, 0.0049275f,
-0.115176f, 0.0049275f,
-0.131254f, -0.010111f,
-0.131254f, -0.010111f,
-0.14752f, -0.0252822f,
-0.14752f, -0.0252822f,
-0.163941f, -0.0405625f,
-0.163941f, -0.0405625f,
-0.180484f, -0.0559291f,
-0.180484f, -0.0559291f,
-0.19712f, -0.07136f,
-0.19712f, -0.07136f,
-0.213818f, -0.0868342f,
-0.213818f, -0.0868342f,
-0.23055f, -0.102331f,
-0.23055f, -0.102331f,
-0.247288f, -0.117832f,
-0.247288f, -0.117832f,
-0.264006f, -0.133319f,
-0.264006f, -0.133319f,
-0.280676f, -0.148773f,
-0.280676f, -0.148773f,
-0.297276f, -0.164177f,
-0.297276f, -0.164177f,
-0.31378f, -0.179517f,
-0.31378f, -0.179517f,
-0.330166f, -0.194777f,
-0.330166f, -0.194777f,
-0.346413f, -0.209942f,
-0.346413f, -0.209942f,
-0.3625f, -0.225f,
-0.3625f, -0.225f,
-0.378407f, -0.239938f,
-0.378407f, -0.239938f,
-0.394115f, -0.254745f,
-0.394115f, -0.254745f,
-0.409607f, -0.269409f,
-0.409607f, -0.269409f,
-0.424866f, -0.283921f,
-0.424866f, -0.283921f,
-0.439876f, -0.298273f,
-0.439876f, -0.298273f,
-0.454623f, -0.312455f,
-0.454623f, -0.312455f,
-0.469093f, -0.32646f,
-0.469093f, -0.32646f,
-0.483274f, -0.340283f,
-0.483274f, -0.340283f,
-0.497153f, -0.353918f,
-0.497153f, -0.353918f,
-0.51072f, -0.36736f,
-0.51072f, -0.36736f,
-0.523966f, -0.380605f,
-0.523966f, -0.380605f,
-0.536881f, -0.39365f,
-0.536881f, -0.39365f,
-0.549459f, -0.406494f,
-0.549459f, -0.406494f,
-0.561693f, -0.419135f,
-0.561693f, -0.419135f,
-0.573576f, -0.431573f,
-0.573576f, -0.431573f,
-0.585105f, -0.443807f,
-0.585105f, -0.443807f,
-0.596276f, -0.455841f,
-0.596276f, -0.455841f,
-0.607087f, -0.467675f,
-0.607087f, -0.467675f,
-0.617535f, -0.479314f,
-0.617535f, -0.479314f,
-0.62762f, -0.49076f,
-0.62762f, -0.49076f,
-0.637343f, -0.502019f,
-0.637343f, -0.502019f,
-0.646705f, -0.513097f,
-0.646705f, -0.513097f,
-0.655709f, -0.524f,
-0.655709f, -0.524f,
-0.664358f, -0.534736f,
-0.664358f, -0.534736f,
-0.672656f, -0.545313f,
-0.672656f, -0.545313f,
-0.68061f, -0.555739f,
-0.68061f, -0.555739f,
-0.688225f, -0.566026f,
-0.688225f, -0.566026f,
-0.695509f, -0.576184f,
-0.695509f, -0.576184f,
-0.702471f, -0.586224f,
-0.702471f, -0.586224f,
-0.70912f, -0.59616f,
-0.70912f, -0.59616f,
-0.715467f, -0.606005f,
-0.715467f, -0.606005f,
-0.721523f, -0.615773f,
-0.721523f, -0.615773f,
-0.7273f, -0.625479f,
-0.7273f, -0.625479f,
-0.732813f, -0.63514f,
-0.732813f, -0.63514f,
-0.738076f, -0.644773f,
-0.738076f, -0.644773f,
-0.743105f, -0.654394f,
-0.743105f, -0.654394f,
-0.747915f, -0.664024f,
-0.747915f, -0.664024f,
-0.752525f, -0.673681f,
-0.752525f, -0.673681f,
-0.756954f, -0.683386f,
-0.756954f, -0.683386f,
-0.76122f, -0.69316f,
-0.76122f, -0.69316f,
-0.765345f, -0.703025f,
-0.765345f, -0.703025f,
-0.76935f, -0.713005f,
-0.76935f, -0.713005f,
-0.773257f, -0.723123f,
-0.773257f, -0.723123f,
-0.777091f, -0.733403f,
-0.777091f, -0.733403f,
-0.780876f, -0.743873f,
-0.780876f, -0.743873f,
-0.784638f, -0.754556f,
-0.784638f, -0.754556f,
-0.788403f, -0.765483f,
-0.788403f, -0.765483f,
-0.792199f, -0.776679f,
-0.792199f, -0.776679f,
-0.796055f, -0.788175f,
-0.796055f, -0.788175f
};

    // cout << sizeof( arr ) << endl;

	GLuint vertexBufferID;	// Creates a unsigned int to store the VBO address later
	glGenBuffers(1, &vertexBufferID);	// Generate the new buffer (1 of) and copy its address to the uint we made earlier 
	
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID); // Bind an array buffer to the VBO
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), arr, GL_STATIC_DRAW); // Send the vert data to the buffer
	
    glEnableVertexAttribArray(0); // Tell opengl were using vertex data (with the id of 0)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0); // How should it read the data
	glEnableVertexAttribArray(0); // Tell opengl were using vertex data (with the id of 0)

	// The colors
	// glEnableVertexAttribArray(1); // Use the second lot of data (colors)
	// glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(GL_FLOAT), (char*)(sizeof(GL_FLOAT)*2)); // Select the second lot of vertex data

	// GLushort indicies[] = { 0, 1, 2, 0, 3, 4 };

	// GLuint indexBufferID;
	// glGenBuffers(1, &indexBufferID); // new buffer
	// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID); // assign to element array
	// glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), indicies, GL_STATIC_DRAW); // pass in the element array data

    // glfwPollEvents();

    glClearColor(0.188, 0.188, 0.188, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    vector<float> lineColor = { 1., 1., 1. };
    glUniform3fv(glGetUniformLocation(shaderProgram, "color"), 1, &lineColor[0]);

    // Draw the triangle here!
    //glDrawArrays(GL_TRIANGLES, 0, 3); // Draw the verticies to the screen
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

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "SM"), 1, GL_FALSE, &SM[0]);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "TM"), 1, GL_FALSE, &TM[0]);

    glLineWidth( 3 );
    // glDrawElements(GL_LINES, 6, GL_UNSIGNED_SHORT, nullptr); // Draw the data using the element array
    glDrawArrays(GL_LINES, 0, 199); // Draw the data using the element array
    
    glEnable(GL_POINT_SMOOTH);
    lineColor = { 0., 0.521, 1. };
    glUniform3fv(glGetUniformLocation(shaderProgram, "color"), 1, &lineColor[0]);
    // glDrawElements(GL_POINTS, 6, GL_UNSIGNED_SHORT, nullptr); // Draw the data using the element array
    // glDrawArrays(GL_POINTS, 0, 199); // Draw the data using the element array

    glDetachShader( shaderProgram, fragmentShader );
    glDetachShader( shaderProgram, vertexShader );

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