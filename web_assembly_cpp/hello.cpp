#include <iostream>
#include <emscripten/emscripten.h>
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

    tuple<vector<double>, vector<double>> computeBezier( vector<double> vx, vector<double> vy ) {

        vector<double> curveX;
        vector<double> curveY;

        int n = vx.size() - 1;
        int v = 0;

        for( double t = 0.; t < 1.; t += 0.01 ) {

            double curveXpoint{ 0 };
            double curveYpoint{ 0 };

            for( int i = 0; i <= n; i++ ) {

                curveXpoint = curveXpoint + binomial( n, i ) * pow( 1 - t, n - i ) * pow( t, i ) * vx[i];
                curveYpoint = curveYpoint + binomial( n, i ) * pow( 1 - t, n - i ) * pow( t, i ) * vy[i];
                
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
        
        return make_tuple( curveX, curveY );
    }

    vector<double>* getTuple( vector<double>* v ){
            
        std::vector<double> xX{2.5, 1.5, 6, 10, 7, 3};
        std::vector<double> yY{0.5, 5.0, 5.0, 0.5, 1.0 , 2.0};

        vector<double> &out = *v;

        // (*v)[0] = 10.;
        // (*v)[1] = 3.3;
        // (*v)[2] = 1.5;

        for( int i = 0; i < 3; i ++ ) {
            // out[i] = out[i] * 2.;
            (*v)[i] *= 2.;
            cout << (*v)[i] << "\n";
        }

        return v;
    }

    int* copy_array(int in_array[], int length) {
        int out_array[length];
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

    return 0;
}