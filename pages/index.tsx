import { FC, useEffect } from "react";
import useWasm from "../hooks/useWasm";

declare global {
    var Module: {
        ccall: ( 
            name: string, 
            returnType: any, 
            argTypes: any, 
            args: any 
        ) => any
    }
}

const Home: FC = () => {

    // Takes an Int32Array, copies it to the heap and returns a pointer
    function arrayToPtr( array: any[] ) {
        const v = (Module as any)._malloc( array.length * 8 )
        const e = (Module as any).HEAPF64.set(new Float64Array( array ), v / 8 )

        return v
        }

        // Takes a pointer and  array length, and returns a Int32Array from the heap
        function ptrToArray(ptr: any, length: number) {
        var array = new Float64Array(length)
        var pos = ptr / 8
        array.set((Module as any).HEAPF64.subarray(pos, pos + length))
        return array
        }

    return (
        <div>
            <button onClick={ () => {

                // console.log( arrayToPtr( [ 2.5, 1.5, 6, 10 ] ) )
                const inArr: any = [ 2.5, 1.5, 6, 10 ]
                const v = arrayToPtr( inArr )
                // const v1 = arrayToPtr( [ 0.5, 5, 5, 0.5 ] )
                
                let x = (Module as any).setValue(v, 2.5, 'double')
                x = (Module as any).setValue(v+8, 1.5, 'double')
                x = (Module as any).setValue(v+16, 6, 'double')

                var doublePtr = (Module as any).HEAPF64.subarray(v/8, v/8 + 3);
                doublePtr[0] = 1.;
                doublePtr[1] = 2.;
                doublePtr[2] = 3.;

                const result = Module.ccall(
                    "getTuple",                // name of C function
                    [ "array" ],              // return type
                    [ 'array' ],          // argument types
                    [ doublePtr ]                  // arguments
                );   
                // console.log( (Module as any).setValue(v + 8, 2.5, 'double') )

                const arr = [];
                arr[0] = (Module as any).getValue(v,'double'); //98
                arr[1] = (Module as any).getValue(v+8,'double') //99
                arr[2] = (Module as any).getValue(v+16,'double') //100
                console.log( arr, ptrToArray( result, 3 ) )

            } }>
                call C func
            </button>
        </div>
    )
}

export default Home