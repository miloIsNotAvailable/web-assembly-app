import fs from 'fs'
import path from 'path'
import { fileURLToPath } from 'url'
import express, { application } from 'express'
import { createServer as createViteServer } from 'vite'
import cookies from 'cookie-parser'
import glob from 'glob'
import bodyParser from 'body-parser'

async function createServer() {
  const app = express()

  app.use(bodyParser.urlencoded({ extended: false, limit: '50mb' }))

  // parse application/json
  app.use(bodyParser.json( { limit: '50mb' } ))

  // Create Vite server in middleware mode and configure the app type as
  // 'custom', disabling Vite's own HTML serving logic so parent server
  // can take control
  const vite = await createViteServer({
    server: { middlewareMode: true },
    appType: 'custom'
  })

  // use vite's connect instance as middleware
  // if you use your own express router (express.Router()), you should use router.use
  app.use(vite.middlewares)
  app.use( cookies() )

  let e = glob.sync( "./api/**/*.ts" )

  const imports = e.map( async n => await import( n.replace( ".ts", "" ) ) )
  imports.forEach( async ( n, ind ) => {
    
    const api_name = e[ind]
    .replace( ".ts", "" )
    .replace( "./api/", "/api/" )
    
    app.use( api_name, async( req, res, next ) => {
      const func = await n
      func.default( req, res, next )
    } )
  } )

  app.use('*', async (req, res, next) => {
    const url = req.originalUrl
  
    try {
      // 1. Read index.html
      let template = fs.readFileSync(
        path.resolve(process.cwd(), 'index.html'),
        'utf-8'
      )
  
      // 2. Apply Vite HTML transforms. This injects the Vite HMR client, and
      //    also applies HTML transforms from Vite plugins, e.g. global preambles
      //    from @vitejs/plugin-react
      template = await vite.transformIndexHtml(url, template)

      // 3. Load the server entry. vite.ssrLoadModule automatically transforms
      //    your ESM source code to be usable in Node.js! There is no bundling
      //    required, and provides efficient invalidation similar to HMR.
      const { render } = await vite.ssrLoadModule('src/render.tsx')
  
      // 4. render the app HTML. This assumes entry-server.js's exported `render`
      //    function calls appropriate framework SSR APIs,
      //    e.g. ReactDOMServer.renderToString()
      const appHtml = await render(url)
        
      const preload_css = glob.sync( "./styles/**/*.css" )
      .map( file => `<link rel="stylesheet preload prefetch" href="${ file.replace( "./", "/" ) }" as="style"/>` )
      .join( "\n" )

      // 5. Inject the app-rendered HTML into the template.
      const html = template
      .replace(`<!--ssr-outlet-->`, appHtml)
      .replace( '<!-- preload-css -->', preload_css )
  
      // 6. Send the rendered HTML back.
      res.status(200).set({ 'Content-Type': 'text/html' }).end(html)
    } catch (e) {
      // If an error is caught, let Vite fix the stack trace so it maps back to
      // your actual source code.
      vite.ssrFixStacktrace(e as any)
      next(e)
    }
  })

  const PORT = 6173 || process.env.PORT
  app.listen(PORT, () => console.log( `✨ app is running on http://localhost:${ PORT }` ))
}

createServer()
