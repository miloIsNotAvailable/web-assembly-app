import { FC } from "react";
import App from "./App";
import ReactDOMserver from 'react-dom/server'
import { StaticRouter } from 'react-router-dom/server'
// import { Provider } from "react-redux";
// import { store } from "../redux/store";

export const render = ( url: string ) => ReactDOMserver.renderToString(
    // <Provider store={ store }>
        <StaticRouter location={ url }>
            <App/>
        </StaticRouter>
    // </Provider>
)   