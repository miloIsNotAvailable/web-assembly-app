import { FC } from "react";
import PickShape from "../shapes/PickShape";
import { styles } from "./NavbarStyles";

const Navbar: FC = () => {

    return (
        <div className={ styles.navbar_wrap }>
            <PickShape/>
        </div>
    )
}

export default Navbar