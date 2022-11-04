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

    double* computeBezier( double vx[], double vy[] ) {

        vector<double> curveX;
        vector<double> curveY;

        // convert array to vector
        vector<double> vx_vec( vx, vx + sizeof vx + 1 );
        vector<double> vy_vec( vy, vy + sizeof vy + 1 );

        int n = vx_vec.size() - 1;
        int v = 0;

        for( double t = 0.; t < 1.; t += 0.01 ) {

            double curveXpoint{ 0 };
            double curveYpoint{ 0 };

            for( int i = 0; i <= n; i++ ) {

                curveXpoint = curveXpoint + binomial( n, i ) * pow( 1 - t, n - i ) * pow( t, i ) * vx_vec[i];
                curveYpoint = curveYpoint + binomial( n, i ) * pow( 1 - t, n - i ) * pow( t, i ) * vy_vec[i];
                
            }
            
            curveX.push_back( curveXpoint );
            curveY.push_back( curveYpoint );
        }

        for( auto i: curveX) {
            cout << "i ->" << i << '\n';
        }

        for( auto i: curveY) {
            cout << "j ->" << i << '\n';
        }
        
        double* curveX_arr = &curveX[0];
        double* curveY_arr = &curveY[0];

        return curveX_arr;
    }

    void Print( vector<double>& v ) {
        for( auto& i: v ) {
            cout << i << endl;
        }
    }

    // double* getTuple( double vx[], double vy[] ){
            
    //     std::vector<double> xX{0., 1., 2., 3.};
    //     std::vector<double> yY{0., 1., 2., 3.};

    //     // convert array to vector
    //     vector<double> outx( vx, vx + sizeof vx + 1 );
    //     vector<double> outy( vy, vy + sizeof vy + 1 );

    //     for( int i = 0; i < outx.size(); i++ ) {
    //         // out[i] = out[i] * 2.;
    //         // cout << out[i] << endl;
    //     }

    //     computeBezier( outx, outy );
    //     // convert vector to array
    //     double* out_arr = &outx[0];
        
    //     return out_arr;
    // }

    double* copy_array(double in_array[], int length) {
        double out_array[length];
        for (int i=0; i<length; i++) {
            in_array[i] = in_array[i] * 2;
            out_array[i] = in_array[i];
            cout << in_array[i] << "\n";
        }
    return out_array;
    }

    void reshape(int w, int h) {
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    // glMatrixMode(GL_PROJECTION);
    // glLoadIdentity();
    double M[] = {
        .2, 0., 0., 0., 
        0, .2, 0., 1., 
        0, 0., 2., 1., 
        0, 0., 0., 1., 
    };
    if (w <= h) {
        // glMultMatrixd( &M[0] );
          glOrtho(-5.0, 5.0, -5.0*(GLfloat)h/(GLfloat)w, 
                   5.0*(GLfloat)h/(GLfloat)w, -5.0, 5.0);
    }else{
          glOrtho(-5.0*(GLfloat)w/(GLfloat)h, 
                   5.0*(GLfloat)w/(GLfloat)h, -5.0, 5.0, -5.0, 5.0);
    }
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

GLfloat ctrlpoints[4][3] = {
        { -4.0, -4.0, 0.0}, { -2.0, 4.0, 0.0}, 
        {2.0, -4.0, 0.0}, {4.0, 4.0, 0.0}
};

void e(){
    glMap1f(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 4, &ctrlpoints[0][0]);
    glEnable(GL_MAP1_VERTEX_3);
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

EM_BOOL cb ( double time, void* userData ){
    // printf( "ye" );
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
    "vec4 plane_curve(float u)\n"
    "{\n"
    "   vec4 p = (2.*u*u - 3.*u + 1.) * vec4( aPos, 1. );\n"
    "   p += (-4.*u*u + 4.*u) * vec4( aPos, 1. );\n"
    "   p += (2.*u*u - u) * vec4( aPos, 1. ); \n"
    "\n"
    "   return p;\n"
    "}\n"
    "\n" 
    "void main() {\n"
    "   float u = aPos.x;\n"
    "   float v = aPos.y;\n"
    "\n"
    "   vec4 pos = plane_curve( 7. );\n"
    "    gl_Position =  pos * TM * SM;\n"
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

    // GLfloat verts[] = {
	// 	// Verticies				
	// 	 0.f,  0.2f, 0.0f,		1.0f, 1.0f, 1.0f,
	// 	 .5f,  .5f, 0.0f,		1.0f, 1.0f, 1.0f,
	// 	 -.7f,  -.5f, 0.0f,		1.0f, 1.0f, 1.0f,
	// 	 -.7f,  -.5f, 0.0f,		1.0f, 1.0f, 1.0f,
	// 	 -.8f,  -.8f, 0.0f,		1.0f, 1.0f, 1.0f,
	// };

    // GLfloat verts[] = {
	// 	// Verticies				
    //      -1.f,  1.f,
	// 	 1.f,  .5f,

    //      0.f, 0.f,
    //      1.f, .5f,
	// };

    GLfloat verts[] = {
    0.0f, 0.05f,
    0.0047479f, 0.05280725f,
    0.0047479f, 0.05280725f,
    0.009003025f, 0.05523825f,
    0.009003025f, 0.05523825f,
    0.01278245f, 0.05730625f,
    0.01278245f, 0.05730625f,
    0.0161029f, 0.059025f,
    0.0161029f, 0.059025f,
    0.018980925f, 0.06040675f,
    0.018980925f, 0.06040675f,
    0.0214328f, 0.06146525f,
    0.0214328f, 0.06146525f,
    0.0234745f, 0.06221225f,
    0.0234745f, 0.06221225f,
    0.02512175f, 0.06266075f,
    0.02512175f, 0.06266075f,
    0.02639025f, 0.06282275f,
    0.02639025f, 0.06282275f,
    0.027295f, 0.06271f,
    0.027295f, 0.06271f,
    0.02785125f, 0.0623345f,
    0.02785125f, 0.0623345f,
    0.0280735f, 0.06170775f,
    0.0280735f, 0.06170775f,
    0.0279765f, 0.060841f,
    0.0279765f, 0.060841f,
    0.02757425f, 0.0597455f,
    0.02757425f, 0.0597455f,
    0.026881f, 0.05843175f,
    0.026881f, 0.05843175f,
    0.02591025f, 0.056911f,
    0.02591025f, 0.056911f,
    0.024675725f, 0.05519325f,
    0.024675725f, 0.05519325f,
    0.02319055f, 0.05328875f,
    0.02319055f, 0.05328875f,
    0.021467725f, 0.05120775f,
    0.021467725f, 0.05120775f,
    0.01952f, 0.04896f,
    0.01952f, 0.04896f,
    0.01735985f, 0.046555f,
    0.01735985f, 0.046555f,
    0.0149995f, 0.044002f,
    0.0149995f, 0.044002f,
    0.012450975f, 0.0413105f,
    0.012450975f, 0.0413105f,
    0.00972595f, 0.03848925f,
    0.00972595f, 0.03848925f,
    0.006835925f, 0.035547f,
    0.006835925f, 0.035547f,
    0.00379215f, 0.032492f,
    0.00379215f, 0.032492f,
    0.00060557f, 0.029333f,
    0.00060557f, 0.029333f,
    -0.0027131f, 0.02607775f,
    -0.0027131f, 0.02607775f,
    -0.00615335f, 0.022734175f,
    -0.00615335f, 0.022734175f,
    -0.009705f, 0.01931f,
    -0.009705f, 0.01931f,
    -0.013358075f, 0.0158126f,
    -0.013358075f, 0.0158126f,
    -0.01710285f, 0.012249225f,
    -0.01710285f, 0.012249225f,
    -0.020929875f, 0.0086268f,
    -0.020929875f, 0.0086268f,
    -0.024829925f, 0.004952175f,
    -0.024829925f, 0.004952175f,
    -0.028794f, 0.001231875f,
    -0.028794f, 0.001231875f,
    -0.0328135f, -0.00252775f,
    -0.0328135f, -0.00252775f,
    -0.03688f, -0.00632055f,
    -0.03688f, -0.00632055f,
    -0.04098525f, -0.010140625f,
    -0.04098525f, -0.010140625f,
    -0.045121f, -0.013982275f,
    -0.045121f, -0.013982275f,
    -0.04928f, -0.01784f,
    -0.04928f, -0.01784f,
    -0.0534545f, -0.02170855f,
    -0.0534545f, -0.02170855f,
    -0.0576375f, -0.02558275f,
    -0.0576375f, -0.02558275f,
    -0.061822f, -0.029458f,
    -0.061822f, -0.029458f,
    -0.0660015f, -0.03332975f,
    -0.0660015f, -0.03332975f,
    -0.070169f, -0.03719325f,
    -0.070169f, -0.03719325f,
    -0.074319f, -0.04104425f,
    -0.074319f, -0.04104425f,
    -0.078445f, -0.04487925f,
    -0.078445f, -0.04487925f,
    -0.0825415f, -0.04869425f,
    -0.0825415f, -0.04869425f,
    -0.08660325f, -0.0524855f,
    -0.08660325f, -0.0524855f,
    -0.090625f, -0.05625f,
    -0.090625f, -0.05625f,
    -0.09460175f, -0.0599845f,
    -0.09460175f, -0.0599845f,
    -0.09852875f, -0.06368625f,
    -0.09852875f, -0.06368625f,
    -0.10240175f, -0.06735225f,
    -0.10240175f, -0.06735225f,
    -0.1062165f, -0.07098025f,
    -0.1062165f, -0.07098025f,
    -0.109969f, -0.07456825f,
    -0.109969f, -0.07456825f,
    -0.11365575f, -0.07811375f,
    -0.11365575f, -0.07811375f,
    -0.11727325f, -0.081615f,
    -0.11727325f, -0.081615f,
    -0.1208185f, -0.08507075f,
    -0.1208185f, -0.08507075f,
    -0.12428825f, -0.0884795f,
    -0.12428825f, -0.0884795f,
    -0.12768f, -0.09184f,
    -0.12768f, -0.09184f,
    -0.1309915f, -0.09515125f,
    -0.1309915f, -0.09515125f,
    -0.13422025f, -0.0984125f,
    -0.13422025f, -0.0984125f,
    -0.13736475f, -0.1016235f,
    -0.13736475f, -0.1016235f,
    -0.14042325f, -0.10478375f,
    -0.14042325f, -0.10478375f,
    -0.143394f, -0.10789325f,
    -0.143394f, -0.10789325f,
    -0.14627625f, -0.11095175f,
    -0.14627625f, -0.11095175f,
    -0.149069f, -0.11396025f,
    -0.149069f, -0.11396025f,
    -0.15177175f, -0.11691875f,
    -0.15177175f, -0.11691875f,
    -0.15438375f, -0.1198285f,
    -0.15438375f, -0.1198285f,
    -0.156905f, -0.12269f,
    -0.156905f, -0.12269f,
    -0.15933575f, -0.12550475f,
    -0.15933575f, -0.12550475f,
    -0.16167625f, -0.12827425f,
    -0.16167625f, -0.12827425f,
    -0.16392725f, -0.131f,
    -0.16392725f, -0.131f,
    -0.1660895f, -0.133684f,
    -0.1660895f, -0.133684f,
    -0.168164f, -0.13632825f,
    -0.168164f, -0.13632825f,
    -0.1701525f, -0.13893475f,
    -0.1701525f, -0.13893475f,
    -0.17205625f, -0.1415065f,
    -0.17205625f, -0.1415065f,
    -0.17387725f, -0.144046f,
    -0.17387725f, -0.144046f,
    -0.17561775f, -0.146556f,
    -0.17561775f, -0.146556f,
    -0.17728f, -0.14904f,
    -0.17728f, -0.14904f,
    -0.17886675f, -0.15150125f,
    -0.17886675f, -0.15150125f,
    -0.18038075f, -0.15394325f,
    -0.18038075f, -0.15394325f,
    -0.181825f, -0.15636975f,
    -0.181825f, -0.15636975f,
    -0.18320325f, -0.158785f,
    -0.18320325f, -0.158785f,
    -0.184519f, -0.16119325f,
    -0.184519f, -0.16119325f,
    -0.18577625f, -0.1635985f,
    -0.18577625f, -0.1635985f,
    -0.18697875f, -0.166006f,
    -0.18697875f, -0.166006f,
    -0.18813125f, -0.16842025f,
    -0.18813125f, -0.16842025f,
    -0.1892385f, -0.1708465f,
    -0.1892385f, -0.1708465f,
    -0.190305f, -0.17329f,
    -0.190305f, -0.17329f,
    -0.19133625f, -0.17575625f,
    -0.19133625f, -0.17575625f,
    -0.1923375f, -0.17825125f,
    -0.1923375f, -0.17825125f,
    -0.19331425f, -0.18078075f,
    -0.19331425f, -0.18078075f,
    -0.19427275f, -0.18335075f,
    -0.19427275f, -0.18335075f,
    -0.195219f, -0.18596825f,
    -0.195219f, -0.18596825f,
    -0.1961595f, -0.188639f,
    -0.1961595f, -0.188639f,
    -0.19710075f, -0.19137075f,
    -0.19710075f, -0.19137075f,
    -0.19804975f, -0.19416975f,
    -0.19804975f, -0.19416975f,
    -0.19901375f, -0.19704375f,
    -0.19901375f, -0.19704375f,
};

	GLuint vertexBufferID;	// Creates a unsigned int to store the VBO address later
	glGenBuffers(1, &vertexBufferID);	// Generate the new buffer (1 of) and copy its address to the uint we made earlier 
	
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID); // Bind an array buffer to the VBO
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Send the vert data to the buffer
	
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
	
    glClearColor(0.984, 0.4627, 0.502, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

    emscripten_request_animation_frame_loop( cb, userData );

    glMap1f(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 4, &ctrlpoints[0][0]);
    glEnable(GL_MAP1_VERTEX_3);

    glutReshapeFunc(reshape);

	return 0;
}

#ifdef __cplusplus
}
#endif