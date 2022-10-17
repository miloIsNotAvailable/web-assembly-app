import { Request, Response } from 'express'

export default async function handler( res: Response, req: Request ) {
//   const { exports } = (await WebAssembly.instantiate(wasm)) as any

//   console.log( exports )
  res.json( { name: "john Doe" } )
}