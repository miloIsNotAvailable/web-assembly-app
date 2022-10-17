// import { AnimatePresence } from 'framer-motion'
import { FC } from 'react'
import { BrowserRouter, Route, Routes, useLocation } from 'react-router-dom'

const AppRoutes: FC = () => {

    // @ts-ignore
    const __routes__: any = import.meta.globEager( '/pages/**/[a-z[]*.tsx' )
    
    const routes = Object.keys(__routes__).map((route) => {
        const path = route
          .replace(/\/pages|index|\.tsx$/g, '')
          .replace(/\[\.{3}.+\]/, '*')
          .replace(/\[(.+)\]/, ':$1')
      
        return { path, component: __routes__[route].default }
      })
    
    const location = useLocation()

    return (
        // <AnimatePresence mode="wait">
            <Routes location={ location } key={ location.pathname }>
                { routes.map( ( { component: Component, path } ) =>(
                    <Route 
                        path={ path } 
                        element={ <Component/> }
                        key={ path }
                    />
                ) ) }
            </Routes>
        // </AnimatePresence>
    )
}

export default AppRoutes