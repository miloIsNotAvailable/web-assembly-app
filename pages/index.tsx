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
                    "computeBezier",                // name of C function
                    [ "tuple<vector<double>, vector<double>>" ],              // return type
                    [ 'vector<double>', 'vector<double>' ],          // argument types
                    [ [ 2.5, 1.5, 6, 10 ], [ 0.5, 5, 5, 0.5 ] ]                   // arguments
                );   
                console.log( result )
            } }>
                call C func
            </button>
        </div>
    )
}

export default Home