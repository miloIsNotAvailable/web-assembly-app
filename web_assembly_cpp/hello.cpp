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

    const char *fragmentShaderSource = "#ifdef GL_ES\n"
    "precision mediump float;\n"
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
    "\n"    
    "gl_FragColor = vec4(1.0);\n"
    "}\0";
    const char *vertexShaderSource = "#ifdef GL_ES\n"
    "precision mediump float;\n"
    "#endif\n"
    "\n" 
    "void main() {\n"
    "    vec4 aPos = vec4( 1. );\n"
    "    aPos.xy = aPos.xy * 2. - 1.;\n"
    "    gl_Position = mat4(1.) * aPos;\n"
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
    // check for shader compile errors

    // link shaders
    int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);


    vector<float> vertices = {
        10., 10., 0.,
        20., 20., 0.
    };

    vector<float> lineColor = { 0.984, 0.4627, 0.502 };

    GLuint VAO, VBO; 
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0); 

    glUseProgram(shaderProgram);

    glUniform3fv(glGetUniformLocation(shaderProgram, "color"), 1, &lineColor[0]);

    glBindVertexArray(VAO);
    glDrawArrays(GL_LINES, 0, 2);

	glClearColor(0.984, 0.4627, 0.502, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	return 0;
}