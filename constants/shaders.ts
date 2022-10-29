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
vec2 AB = normalize(gl_FragCoord.xy - vec2(1., 0.));

  gl_FragColor = vec4(vec3(AB, 0.),1.0);
}`

export const vert = `
#ifdef GL_ES
precision mediump float;
#endif

void main() {
    vec4 aPos = vec4( position, 1. );
    aPos.xy = aPos.xy * 2. - 1.;
    gl_Position = aPos;
    gl_PointSize = 10.;
}`