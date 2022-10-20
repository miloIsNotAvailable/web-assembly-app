#include <vector>
using std::vector;
typedef long int i32;
extern "C" {
  i32 myFunction(i32 myNumber) {
    vector<i32> myVector{ 666, 1337 }; 
    return myVector[0] + myVector[1] + myNumber;
  }
}