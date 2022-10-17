#include <iostream>
#include <emscripten/emscripten.h>

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
}

