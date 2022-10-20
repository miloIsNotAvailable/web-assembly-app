#include <iostream>
#include <emscripten/emscripten.h>
#include<math.h>

using namespace std;

void print( const char* input ) {
    cout << input << "\n";
}

int main()
{
    print( "Hello world!" );

    return 0;
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

    // TODO: compute points for 2 dimensional vectors
}

