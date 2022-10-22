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
        var array = new Uint32Array( length )
        var pos = ptr / 8
        array.set((Module as any).HEAPU32.subarray(pos, pos + length))
        return array
        }

    return (
        <div>
            <button onClick={ () => {

                // console.log( arrayToPtr( [ 2.5, 1.5, 6, 10 ] ) )
                const inArr: any = [ 2.5, 1.5, 6, 10 ]
                const v = arrayToPtr( inArr )
                const arr = new Uint8Array( new Uint32Array( [3, 8, 2] ).buffer )

                const result = Module.ccall(
                    "copy_array",                // name of C function
                    [ "array" ],              // return type
                    [ 'array', 'number' ],          // argument types
                    [ arr, 3 ]                  // arguments
                );   

                for( let i = 0; i <= 8; i+=4 ) {

                    console.log( ( Module as any ).getValue( result + i ) )
                }
            } }>
                call C func
            </button>
        </div>
    )
}

export default Home