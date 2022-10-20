import { FC, useEffect } from "react";

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
      
    useEffect( () => {
        const path ='/public/hello.wasm';
        ( async() => {
            
            // const importObject: WebAssembly.Imports = {
            //     env: {
            //         "abort": function(){},
            //         "emscripten_memcpy_big": function(){},
            //         "emscripten_resize_heap": function(){},
            //         "environ_get": function(){},
            //         "environ_sizes_get": function(){},
            //         "fd_close": function(){},
            //         "fd_read": function(){},
            //         "fd_seek": function(){},
            //         "fd_write": function(){},
            //         "strftime_l": function(){},
            //         "wasi_snapshot_preview1": () => ""
            //     }
            //   };              
            
            const data = await fetch( path )
            const arr = await data.arrayBuffer()

            const module = new WebAssembly.Module( arr ) as any
            const instance = await WebAssembly.instantiate( arr );

            console.log( instance )
        } )()
        
    }, [] )

    return (
        <div>
            <button onClick={ () => {

                const result = Module.ccall(
                    "myFunction",  // name of C function
                    null,          // return type
                    null,          // argument types
                    null           // arguments
                );  
            } }>
                call C func
            </button>
        </div>
    )
}

export default Home