export const frag = `
#ifdef GL_ES
precision mediump float;
#endif

uniform vec2 u_resolution;
uniform vec2 u_mouse;
uniform float u_time;

void main(){
  vec2 st = gl_FragCoord.xy/u_resolution.xy;
  st.x *= u_resolution.x/u_resolution.y;

  gl_FragColor = vec4(1.0);
}`

export const vert = `
#ifdef GL_ES
precision mediump float;
#endif

attribute vec4 vPosition;

void main() {
    vec4 aPos = vPosition;
    aPos.xy = aPos.xy * 2. - 1.;
    gl_Position = vPosition;
    gl_PointSize = 10.;
}`