import { FC, useEffect, useRef } from "react";
import { WasmFloat64Array } from "../constants/wasm_arrays/WasmFloat64Array";
import { 
    Scene, 
    PerspectiveCamera, 
    WebGLRenderer, 
    BoxGeometry, 
    MeshBasicMaterial, 
    Mesh, 
    PlaneGeometry, 
    ShaderMaterial, 
    Vector2 
} from 'three'
import { frag, vert } from "../constants/shaders";

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

    const blobRef = useRef<HTMLDivElement | null>( null )

    const arrToPtr = ( array: any ) => {
        const ptr: any = (Module as any)._malloc( array.length * 8 )
        const e = (Module as any).HEAPF64.set( array, ptr / 8 )

        return ptr
    }
        
    useEffect( () => {
        const e = document.getElementById( "canvas" ) as HTMLCanvasElement
        const gl = e.getContext( "webgl2" )

        if (!gl) return
        
          // Compile the vertex shader
          const vertexShaderSource = `#version 300 es
          void main() {
            gl_Position = vec4(0, 0, 0, 1);
            gl_PointSize = 10.0;
          }`
          const vs: any = gl.createShader(gl.VERTEX_SHADER)
          gl.shaderSource(vs, vertexShaderSource)
          gl.compileShader(vs)
        
          // Compile the fragment shader
          const fragmentShaderSource = `#version 300 es
          precision highp float;
          out vec4 color;
          void main() {
            color = vec4(1);
          }`
          const fs: any = gl.createShader(gl.FRAGMENT_SHADER)
          gl.shaderSource(fs, fragmentShaderSource)
          gl.compileShader(fs)
          
          // Link the program
          const prog: any = gl.createProgram()
          gl.attachShader(prog, vs)
          gl.attachShader(prog, fs)
          gl.linkProgram(prog)
          if (!gl.getProgramParameter(prog, gl.LINK_STATUS)) {
            console.error('prog info-log:', gl.getProgramInfoLog(prog))
            console.error('vert info-log: ', gl.getShaderInfoLog(vs))
            console.error('frag info-log: ', gl.getShaderInfoLog(fs))
          }
          
          // Use the program
          gl.useProgram(prog)
          
          // Draw the point
          gl.clearColor(0, 0, 0, 1) // set clear color to black
          gl.lineWidth( 2. )
          gl.clear(gl.COLOR_BUFFER_BIT) // clear the screen
          gl.drawArrays(gl.POINTS, 0, 3) // draw 1 point
          gl.drawArrays(gl.POINTS, 0, 1) // draw 1 point
    }, [])

    useEffect( () => {
        if( !blobRef.current || window.innerWidth < 600 ) return

        // const width = blobRef.current.offsetWidth;
        // const height = blobRef.current.offsetHeight;

        const width = window.innerWidth
        const height = window.innerHeight/2

        const scene = new Scene()
        const camera = new PerspectiveCamera( 
            75, 
            width/height, 
            .1, 
            100 
        )

        const renderer = new WebGLRenderer()
        renderer.setSize( width, height )
        blobRef.current.appendChild( renderer.domElement )

        const geometry = new PlaneGeometry( width, height );
        // const material = new MeshBasicMaterial( { color: 'tomato' } );
        const material = new ShaderMaterial( {
            uniforms: {
                u_time: { value: 0. },
                u_resolution: { value: new Vector2( width, height ) },
                u_mouse: { value: 1. },
            },
            fragmentShader: frag,
            vertexShader: vert,
        } )

        const cube = new Mesh( geometry, material );
        scene.add( cube );

        camera.position.z = 1;

        const render = () => {
            renderer.render( scene, camera );
        }

        let delta = 0;
        let reqId: number | undefined = undefined;
        
        function animate() {
            reqId = undefined
            delta += .02;
            material.uniforms.u_time.value = delta
            
            reqId = requestAnimationFrame( animate );
            render()

            if( window.location.pathname !== "/" && reqId ) {
                cancelAnimationFrame( reqId! )
                // reqId = undefined
                delta = 0;
            }
        }
        animate()

    }, [ blobRef.current  ] )

    return (
        <div>
            <button onClick={ () => {

                const WasmArrX = new WasmFloat64Array( [ 2.4, 5.6, 1.1, 2.12, 4.12 ] )
                const WasmArrY = new WasmFloat64Array( [ 1.5, 3.2, 5.6, 3.3, 3. ] )

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
            <canvas id="canvas"></canvas>
            <div id="cnv"></div>
            {/* <div ref={ blobRef }></div> */}
        </div>
    )
}

export default Home