import { FC } from "react";
import { default as Square } from '../../../assets/rectangle.svg'
import { default as Poly } from '../../../assets/polygon.svg'
import { default as Ellipse } from '../../../assets/ellipse.svg'
import Shape from "./Shape";
import { styles } from "../build/NavbarStyles";

const MapShapes: FC = () => {

    const shapes = [ 
        { src: Square, id: 1, name: "rectangle" }, 
        { src: Poly, id: 2, name: "polygon" }, 
        { src: Ellipse, id: 3, name: "ellipse" }, 
    ]

    const handleChooseShape: ( key: number ) => void 
    = ( key ) => {
            
        const result = Module.ccall(
            "choose_shape",                // name of C function
            [ "void" ],              // return type
            [ "int" ],          // argument types
            [ key ]                  // arguments
        );   
    }

    return (
        <div className={ styles.navbar_shapes }>
            { shapes.map( ( { src, id, name } ) => (
                <div 
                    className={ styles.shape } 
                    key={ id }
                    onClick={ () => handleChooseShape( id ) }
                >
                    <Shape src={ src } />
                    <div>{ name }</div>
                </div>
            ) ) }
        </div>
    )
}

export default MapShapes