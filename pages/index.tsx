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

        // Takes a pointer and  array length, and returns a Int32Array from the heap
        function ptrToArray(ptr: any, length: number) {
        
            /**
             * @params array 
             * @description is the array, 
             * with defined byte size, the 
             * byte size has to be the same as the 
             * inner array:
             * @example 
             * const arr = new Uint8Array( new Uint32Array( [3, 8, 2] ).buffer )  
             */
            var array = new Float64Array( length )
        
            // to get position divide by 8 
            // ex. 32 int array -> 32 / 8 = 4
            var pos = ptr / 8

            // move he pointers around and get all the values 
            // in the array
            array
            .set((Module as any)
            .HEAPF64
            .subarray(pos, pos + length))

        return array
        }

    return (
        <div>
            <button onClick={ () => {

                var data = new Float64Array([0.1, 0.1, 0.1]);

                var buffer = new ArrayBuffer(data.byteLength);
                var floatView = new Float64Array(buffer).set(data);
                var byteView = new Uint8Array(buffer);

                
                const arr = new Uint8Array( new Float64Array( [3.1, 8.1, 2.2] ).buffer )
                console.log( byteView, arr )

                const result = Module.ccall(
                    "copy_array",                // name of C function
                    [ "array" ],              // return type
                    [ 'array', 'number' ],          // argument types
                    [ arr, 3 ]                  // arguments
                );   

                for( let i = 0; i <= 16; i+=8 ) {

                    console.log( ( Module as any ).getValue( result + i, 'double' ) )
                }
                console.log( ptrToArray( result, 3 ) )
            } }>
                call C func
            </button>
        </div>
    )
}

export default Home