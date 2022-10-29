#include <iostream>
#include <emscripten/emscripten.h>
#include<string>
#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include<math.h>
#include <vector>

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

    // this goes after you have activated the webgl context
	glClearColor(0.984, 0.4627, 0.502, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	return 0;
}