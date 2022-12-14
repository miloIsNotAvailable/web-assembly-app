import { FC, useEffect, useRef, useState } from "react";
import Navbar from "../components/navbar/build/Navbar";
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

    return (
        <div>
            <Navbar/>
        </div>
    )
}

export default Home