#include <iostream>
#include <emscripten/emscripten.h>
#include<string>
#include <GLES2/gl2.h>
#include <GLES3/gl3.h>
#include <EGL/egl.h>
#include<math.h>
#include <vector>

#define GLM_FORCE_PURE

using std::vector;
using namespace std;
// typedef double float double;

void print( const char* input ) {
    cout << input << "\n";
}

// gets converted to _[function name]
extern "C" {

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
}

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

	glClearColor(0.984, 0.4627, 0.502, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

    const char *fragmentShaderSource = "\n"
    "#ifdef GL_ES\n"
    "precision highp float;\n"
    "#endif\n"
    "\n"    
    "uniform vec2 u_resolution;\n"
    "uniform vec2 u_mouse;\n"
    "uniform float u_time;\n"
    "uniform vec3 color;\n"
    "\n"
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
    "\n" 
    "void main() {\n"
    "    mat4 T = mat4(\n"
    "    1., 0., 0., 0.,\n"
    "    0., 1., 0., 0.,\n"
    "    0., 0., 1., 0.,\n"
    "    0., 0., 0., 1. );\n"
    "    vec4 pos = vec4( aPos, 1. );\n"
    "    gl_Position =  pos * T;\n"
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
    GLfloat verts[] = {
		// Verticies				
		 0.f,  0.0f, 0.0f,		1.0f, 1.0f, 1.0f,
		 1.f,  1.f, 0.0f,		1.0f, 1.0f, 1.0f,
		 1.0f, -1.f, 0.f,		1.0f, 1.0f, 1.0f,
	};

	GLuint vertexBufferID;	// Creates a unsigned int to store the VBO address later
	glGenBuffers(1, &vertexBufferID);	// Generate the new buffer (1 of) and copy its address to the uint we made earlier 
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID); // Bind an array buffer to the VBO
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Send the vert data to the buffer
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GL_FLOAT), 0); // How should it read the data
	glEnableVertexAttribArray(0); // Tell opengl were using vertex data (with the id of 0)

	// The colors
	glEnableVertexAttribArray(1); // Use the second lot of data (colors)
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (char*)(sizeof(GL_FLOAT)*2)); // Select the second lot of vertex data

	GLushort indicies[] = { 0, 1, 2, 0, 3, 4 };

	GLuint indexBufferID;
	glGenBuffers(1, &indexBufferID); // new buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID); // assign to element array
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), indicies, GL_STATIC_DRAW); // pass in the element array data

    // glfwPollEvents();

    glClearColor(0.188, 0.188, 0.188, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    vector<float> lineColor = { 1., 1., 1. };
    glUniform3fv(glGetUniformLocation(shaderProgram, "color"), 1, &lineColor[0]);

    // Draw the triangle here!
    //glDrawArrays(GL_TRIANGLES, 0, 3); // Draw the verticies to the screen
    glDrawElements(GL_LINES, 6, GL_UNSIGNED_SHORT, nullptr); // Draw the data using the element array
    glEnable(GL_POINT_SMOOTH);
    lineColor = { 0., 0.521, 1. };
    glUniform3fv(glGetUniformLocation(shaderProgram, "color"), 1, &lineColor[0]);
    glDrawElements(GL_POINTS, 6, GL_UNSIGNED_SHORT, nullptr); // Draw the data using the element array
    // glfwSwapBuffers(window);

	return 0;
}