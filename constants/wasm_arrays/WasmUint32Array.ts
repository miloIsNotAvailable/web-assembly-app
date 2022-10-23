export const WasmUint32Array = class {

    arr: Uint8Array
    private arr_: number[]
    constructor( arr: number[] ) {
        this.arr_ = arr
        this.arr = new Uint8Array( new Uint32Array( this.arr_ ).buffer )
    }

    convert( result: any ) {
        return this.ptrToArray( result, this.arr_.length )
    }

    // Takes a pointer and  array length, and returns a Int32Array from the heap
    private ptrToArray( ptr: any, length: number ) {

        /**
         * @params array 
         * @description is the array, 
         * with defined byte size, the 
         * byte size has to be the same as the 
         * inner array:
         * @example 
         * const arr = new Uint8Array( new Uint32Array( [3, 8, 2] ).buffer )  
         */
        var array = new Uint32Array( length )
    
        // to get position divide by 8 
        // ex. 32 int array -> 32 / 8 = 4
        var pos = ptr / 4

        // move he pointers around and get all the values 
        // in the array
        array
        .set((Module as any)
        .HEAPU32
        .subarray(pos, pos + length))

    return array
    }
    
}
