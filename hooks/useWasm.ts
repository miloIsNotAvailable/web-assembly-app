import { Loose } from "../interfaces/custom"

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

type cppTypes = "double" | "float" | "int" | "char" | "wchar_t" | "void" | "bool"
type OmitTypes = Loose<cppTypes, string>

type wasmType<V=any> = {
    name: string, 
    returnType: OmitTypes, 
    argTypes: OmitTypes[], 
    args: V 
}
/**
 * 
 * @param args is an object that takes function 
 * name, arg types, return type, and args
 * @interface V is the type of args 
 * @returns callback from wasm function
 * @example
 *     const res = useWasm<number[]>( {
        args: [ 4., 3 ],
        argTypes: [ "double", "int" ],
        name: "bezier",
        returnType: 'double'
    } )
 */
export default function useWasm<V>( args: wasmType<V> ): {
    ccall: ( 
        name: string, 
        returnType: any, 
        argTypes: any, 
        args: any 
    ) => any
} {

    var Module: any;

    // const result = Module.ccall(
    //     args.name,                // name of C function
    //     [args.returnType],              // return type
    //     args.argTypes,          // argument types
    //     args.args                   // arguments
    // );   
    return Module.ccall(
        args.name,                // name of C function
        [args.returnType],              // return type
        args.argTypes,          // argument types
        args.args  
    )
}