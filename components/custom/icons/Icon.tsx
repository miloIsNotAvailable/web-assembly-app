import { DetailedHTMLProps, FC, ImgHTMLAttributes } from "react";

interface IconProps {
    src: string
    width?: string
    height?: string
}

type ImgProps = DetailedHTMLProps<ImgHTMLAttributes<HTMLImageElement>, HTMLImageElement>

/**
 * @param Icon
 * @description takes the same attributes as image
 * but has an already defined height and width
 * @returns image with fixed size between 1 to 2rem
 */
const Icon: FC<ImgProps> = ( v ) => {

    return (
        <div className="icon">
            <img 
                className="icon" 
                { ...v }
                style={ {
                    width: v.width,
                    height: v.height
                } }
            />
        </div>
    )
}

export default Icon