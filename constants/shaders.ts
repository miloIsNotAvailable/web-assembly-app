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
    vec3 color = vec3(1.0);
    
    gl_FragColor = vec4(color,1.0);
}
`

export const vert = `
#ifdef GL_ES
precision mediump float;
#endif

uniform vec2 u_resolution;
uniform vec2 u_mouse;
uniform float u_time;

void main() {

    mat4 projection = mat4(1.);
    mat4 view = mat4(1.);
    mat4 model = mat4(1.);

    float thickness = .21;
    float aspect = u_resolution.x/u_resolution.y;

    vec2 aspectVec = vec2(1., 1.0);
    mat4 projViewModel = projection * view * model;
    vec4 previousProjected = projViewModel * vec4(1.0);
    vec4 currentProjected = projViewModel * vec4(vec3( 0. ), 1.0);
    vec4 nextProjected = projViewModel * vec4(vec3(1.), 1.0);
  
    //get 2D screen space with W divide and aspect correction
    vec2 currentScreen = currentProjected.xy / currentProjected.w * aspectVec;
    vec2 previousScreen = previousProjected.xy / previousProjected.w * aspectVec;
    vec2 nextScreen = nextProjected.xy / nextProjected.w * aspectVec;
  
    float len = 1.;
    float orientation = 1.;
    int miter = 0;

    //starting point uses (next - current)
    vec2 dir = vec2(0.0);
    if (currentScreen == previousScreen) {
      dir = normalize(nextScreen - currentScreen);
    } 
    //ending point uses (current - previous)
    else if (currentScreen == nextScreen) {
      dir = normalize(currentScreen - previousScreen);
    }
    //somewhere in middle, needs a join
    else {
      //get directions from (C - B) and (B - A)
      vec2 dirA = normalize((currentScreen - previousScreen));
      if (miter == 1) {
        vec2 dirB = normalize((nextScreen - currentScreen));
        //now compute the miter join normal and length
        vec2 tangent = normalize(dirA + dirB);
        vec2 perp = vec2(-dirA.y, dirA.x);
        vec2 miter = vec2(-tangent.y, tangent.x);
        dir = tangent;
        len = thickness / dot(miter, perp);
      } else {
        dir = dirA;
      }
    }
    vec2 normal = vec2(-dir.y, dir.x);
    normal *= len/2.0;
    normal.x /= aspect;
  
    vec4 offset = vec4(normal * orientation, 0.0, 1.0);
    gl_Position = currentProjected + offset;
    gl_PointSize = 1.0;
  }`