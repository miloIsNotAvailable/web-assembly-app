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

    // const result = useWasm<number[]>( {
    //     args: [ 4., 3 ],
    //     argTypes: [ "double", "int" ],
    //     returnType: "double",
    //     name: 'bezier'
    // } )

    return (
        <div>
            <button onClick={ () => {

                const result = Module.ccall(
                    "bezier",                // name of C function
                    [ "double" ],              // return type
                    [ 'double', 'int' ],          // argument types
                    [ 4.0, 3 ]                   // arguments
                );   
                console.log( result )
            } }>
                call C func
            </button>
        </div>
    )
}

export default Home