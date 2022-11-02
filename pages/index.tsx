import { FC, useEffect, useRef, useState } from "react";
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

    const blobRef = useRef<HTMLCanvasElement | null>( null )

    const arrToPtr = ( array: any ) => {
        const ptr: any = (Module as any)._malloc( array.length * 8 )
        const e = (Module as any).HEAPF64.set( array, ptr / 8 )

        return ptr
    }
        
    useEffect( () => {
        const e = document.getElementById( "canvas" ) as HTMLCanvasElement
        // const gl = e.getContext( "webgl2" )
        // console.log( e.getContext( "experimental-webgl" ) )
    })

    return (
        <div>
            <button onClick={ () => {

                const WasmArrX = new WasmFloat64Array( [ 0., .5, -.7, -.7, -.8 ] )
                const WasmArrY = new WasmFloat64Array( [ .2, .5, -.5, -.5, -.8 ] )

                const ptr = arrToPtr( new Float64Array( [ 2.4, 5.6, 1.1, 2.12, 4.12 ] ) )

                const result = Module.ccall(
                    "computeBezier",                // name of C function
                    [ "array" ],              // return type
                    [ "array", "array" ],          // argument types
                    [ WasmArrX.arr, WasmArrY.arr ]                  // arguments
                );   

                // for( let i = 0; i <= 16; i+=8 ) {
                //     console.log( ( Module as any ).getValue( result + i, 'double' ) )
                // }
                console.log( WasmArrX.convert( result, 100 ) )
            } }>
                call C func
            </button>
        </div>
    )
}

export default Home