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