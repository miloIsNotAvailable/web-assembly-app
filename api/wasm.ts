import { Request, Response } from 'express'

export default async function handler( req: Request, res: Response ) {
//   const { exports } = (await WebAssembly.instantiate( fetch( "hello.wasm" ) )) as any

//   console.log( exports )
  res.send( { name: "john Doe" } )
}