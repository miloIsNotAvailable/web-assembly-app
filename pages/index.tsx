import { FC, useEffect } from "react";
import { WasmFloat64Array } from "../constants/wasm_arrays/WasmFloat64Array";

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

    const arrToPtr = ( array: any ) => {
        const ptr: any = (Module as any)._malloc( array.length * 8 )
        const e = (Module as any).HEAPF64.set( array, ptr / 8 )

        return ptr
    }

    return (
        <div>
            <button onClick={ () => {

                const WasmArr = new WasmFloat64Array( [ 2.4, 5.6, 1.1, 2.12, 4.12 ] )

                const ptr = arrToPtr( new Float64Array( [ 2.4, 5.6, 1.1, 2.12, 4.12 ] ) )

                const result = Module.ccall(
                    "getTuple",                // name of C function
                    [ "number" ],              // return type
                    [ "array" ],          // argument types
                    [ WasmArr.arr ]                  // arguments
                );   

                // for( let i = 0; i <= 16; i+=8 ) {
                //     console.log( ( Module as any ).getValue( result + i, 'double' ) )
                // }
                console.log( WasmArr.convert( result ) )
            } }>
                call C func
            </button>
        </div>
    )
}

export default Home