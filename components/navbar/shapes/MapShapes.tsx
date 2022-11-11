import { FC } from "react";
import { default as Square } from '../../../assets/rectangle.svg'
import { default as Poly } from '../../../assets/polygon.svg'
import { default as Ellipse } from '../../../assets/ellipse.svg'
import Shape from "./Shape";
import { styles } from "../build/NavbarStyles";

const MapShapes: FC = () => {

    const shapes = [ 
        { src: Square, id: "square" }, 
        { src: Poly, id: "polygon" }, 
        { src: Ellipse, id: "ellipse" }, 
    ]

    return (
        <div className={ styles.navbar_shapes }>
            { shapes.map( ( { src, id } ) => (
                <div className={ styles.shape } key={ id }>
                    <Shape src={ src } />
                    <div>{ id }</div>
                </div>
            ) ) }
        </div>
    )
}

export default MapShapes