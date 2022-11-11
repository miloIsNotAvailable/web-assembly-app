import { FC } from "react";
import Icon from "../../custom/icons/Icon";

interface ShapeProps {
    src: string
}

const Shape: FC<ShapeProps> = ( { src } ) => {

    return (
        <div>
            <Icon src={ src }/>    
        </div>  
    )
}

export default Shape