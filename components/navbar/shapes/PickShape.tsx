import { FC, useState } from "react";
import Icon from "../../custom/icons/Icon";
import { default as ShapeIcons } from '../../../assets/rectangle.svg'
import { default as OpenShapes } from '../../../assets/openMenu.svg'
import { styles } from "../build/NavbarStyles";
import MapShapes from "./MapShapes";

const PickShape: FC = () => {

    const [ open, setOpen ] = useState<boolean>( false )

    const handleOpenMenu: () => void = () => {
        setOpen( !open )
    }

    return (
        <div className={ styles.pick_shapes } tabIndex={ 0 }>
            <Icon src={ ShapeIcons }/>
            <Icon 
                src={ OpenShapes }
                width=".7rem"
                height=".7rem"
                onClick={ handleOpenMenu }
            />
            <MapShapes/>
        </div>  
    )
}

export default PickShape