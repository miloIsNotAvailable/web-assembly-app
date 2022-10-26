export const frag = `
// Author:
// Title:
#ifdef GL_ES
precision mediump float;
#endif
uniform vec2 u_resolution;
uniform float u_mouse;
uniform float u_time;
// uniform float u_random;
const int MAX_MARCHING_STEPS = 255;
const float MAX_HIT_DIST = 256.;
const float EPSILON = 0.003;
float map(float value, float inMin, float inMax, float outMin, float outMax) {
  return outMin + (outMax - outMin) * (value - inMin) / (inMax - inMin);
}
float SDFShape(in vec3 p, in vec3 c, float r){ return length(p - c) - r;}
float sdBox( vec3 p, vec3 b )
{
  vec3 q = abs(p) - b;
  return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
}
vec2 hash( vec2 p ) // replace this by something better
{
	p = vec2( dot(p,vec2(127.1,311.7)), dot(p,vec2(269.5,183.3)) );
	return -1.0 + 2.0*fract(sin(p)*43758.5453123);
}
float noise( in vec2 p )
{
    const float K1 = 0.366025404; // (sqrt(3)-1)/2;
    const float K2 = 0.211324865; // (3-sqrt(3))/6;
	vec2  i = floor( p + (p.x+p.y)*K1 );
    vec2  a = p - i + (i.x+i.y)*K2;
    float m = step(a.y,a.x); 
    vec2  o = vec2(m,1.0-m);
    vec2  b = a - o + K2;
	vec2  c = a - 1.0 + 2.0*K2;
    vec3  h = max( 0.5-vec3(dot(a,a), dot(b,b), dot(c,c) ), 0.0 );
	vec3  n = h*h*h*h*vec3( dot(a,hash(i+0.0)), dot(b,hash(i+o)), dot(c,hash(i+1.0)));
    return dot( n, vec3(70.0) );
}
mat4 rotateZ(float theta) {
    float c = cos(theta);
    float s = sin(theta);
    return mat4(
        vec4(c, -s, 0, 0),
        vec4(s, c, 0, 0),
        vec4(0, 0, 1, 0),
        vec4(0, 0, 0, 1)
    );
}
mat4 rotateX(float theta) {
    float c = cos(theta);
    float s = sin(theta);
    return mat4(
        vec4(1, 0, 0, 0),
        vec4(0, c, -s, 0),
        vec4(0, s, c, 0),
        vec4(0, 0, 0, 1)
    );
}
mat4 rotateY(float theta) {
    float c = cos(theta);
    float s = sin(theta);
    return mat4(
        vec4(c, 0, s, 0),
        vec4(0, 1, 0, 0),
        vec4(-s, 0, c, 0),
        vec4(0, 0, 0, 1)
    );
}
vec3 z ;
float sceneSDF(in vec3 p){
    
    float displacement = 0.;
    float rot = -.0;
    vec3 bbox = vec4(vec4(vec3(p.x, p.y , p.z ) , 0.) * rotateY(rot)).xyz;
    vec3 bbox2 = vec4(vec4(vec3(p.x, p.y - .0, p.z + .3 ) , 0.) * rotateY(rot)).xyz;
    vec3 bbox3 = vec4(vec4(vec3(p.x, p.y - .0, p.z + .5 ) , 0.) * rotateY(rot)).xyz;
    vec3 bbox4 = vec4(vec4(vec3(p.x, p.y - .0, p.z + .7 ) , 0.) * rotateY(rot)).xyz;
    vec3 bbox5 = vec4(vec4(vec3(p.x, p.y - .0, p.z + .9 ) , 0.) * rotateY(rot)).xyz;
    vec3 bbox6 = vec4(vec4(vec3(p.x, p.y - .0, p.z + 1.1 ) , 0.) * rotateY(rot)).xyz;
    
  // displacement = (bbox.x + 1.5) * cos(5.0 * (p.x )) * sin(5.0 * p.x) * cos(5.0 * p.x) * 0.25 * (bbox.x - 1.5) ;
    displacement =  (bbox.x + 1.5) * .4 * (bbox.x) * sin(noise(vec2(p.y, p.x) + u_time * -0.7) * -.4) * (bbox.x - 1.5);
   
  if( bbox.x >= -1.5 && bbox.x <= 1.5){
        bbox.y += displacement ;
        bbox2.y += displacement * 1.6;
        bbox3.y += displacement * 1.2;
        bbox4.y += displacement * 1.8;
        bbox5.y += displacement * 2.6;
        bbox6.y += displacement * 2.;
    }
    
    float res = sdBox(bbox, vec3(6.5, 1., .1) );
    float res2 = sdBox(bbox2, vec3(6.5, 1., .1) );
    float res3 = sdBox(bbox3, vec3(6.5, 1., .1) );
    float res4 = sdBox(bbox4, vec3(6.5, 1., .1) );
    float res5 = sdBox(bbox5, vec3(6.5, 1., .1) );
    float res6 = sdBox(bbox6, vec3(6.5, 1., .1) );
    
    float d = 1e10;
    {
        vec3 q = p - vec3(0., 0., 0.);
        d = min(d, res);
        z = vec3(1., .88, .88);
    }
    
    {
        vec3 q = p - vec3(0., 1., 0.);
        d = min(d, res2);
        z = vec3(1., 0.88, 0.88);
    }
    
    {
        vec3 q = p - vec3(0., 1., 0.);
        d = min(d, res3);
        z= vec3(1., .88, 0.88);
    }
    
    {
        vec3 q = p - vec3(0., 1., 0.);
        d = min(d, res4);
        z= vec3(1., .88, 0.88);
    }
    
    {
        vec3 q = p - vec3(0., 1., 0.);
        d = min(d, res5);
        z= vec3(1., .88, 0.88);
    }
    
    {
        vec3 q = p - vec3(0., 1., 0.);
        d = min(d, res6);
        z= vec3(1., .88, 0.88);
    }
        
    return d;
}
vec3 rayDirection(float fieldOfView, vec2 size, vec2 fragCoord) {
    vec2 xy = fragCoord - size / 2.0;
    float z = size.y / tan(radians(fieldOfView) / 2.0);
    return normalize(vec3(xy, -z));
}
mat4 viewWorldMatrix(vec3 eye, vec3 center, vec3 up) {
    // Based on gluLookAt man page
    vec3 f = normalize(center - eye);
    vec3 s = normalize(cross(f, up));
    vec3 u = cross(s, f);
    return mat4(
        vec4(s, 0.0),
        vec4(u, 0.0),
        vec4(-f, 0.0),
        vec4(0.0, 0.0, 0.0, 1)
    );
}
float ray_march(vec3 eye, vec3 marchingDirection, float start, float end) {
    float depth = start;
    for (int i = 0; i < MAX_MARCHING_STEPS; i++) {
        float dist = sceneSDF(eye + depth * marchingDirection);
        // depth += dist;
        if (dist < EPSILON) {
			return depth;
        }
        depth += dist;
        if (depth >= end) {
            return end;
        }
    }
    return end;
}
   
vec3 estimateNormal(vec3 p) {
    return normalize(vec3(
        sceneSDF(vec3(p.x + EPSILON, p.y, p.z)) - sceneSDF(vec3(p.x - EPSILON, p.y, p.z)),
        sceneSDF(vec3(p.x, p.y + EPSILON, p.z)) - sceneSDF(vec3(p.x, p.y - EPSILON, p.z)),
        sceneSDF(vec3(p.x, p.y, p.z  + EPSILON)) - sceneSDF(vec3(p.x, p.y, p.z - EPSILON))
    ));
}
vec3 phongContribForLight(vec3 k_d, vec3 k_s, float alpha, vec3 p, vec3 eye,
                          vec3 lightPos, vec3 lightIntensity) {
    vec3 N = estimateNormal(p);
    vec3 L = normalize(lightPos - p);
    vec3 V = normalize(eye - p);
    vec3 R = normalize(reflect(-L, N));
    
    float dotLN = dot(L, N);
    float dotRV = dot(R, V);
    
    if(dotLN < 0.){
        return vec3(0.);
    }
    if (dotRV < 0.0) {
        return lightIntensity * (k_d * dotLN);
    }
    return lightIntensity * (k_d * dotLN + k_s * pow(dotRV, alpha));
}
vec3 phongIllumination(vec3 k_a, vec3 k_d, vec3 k_s, float alpha, vec3 p, vec3 eye) {
    const vec3 ambientLight = 0. * vec3(.0, .0, .0);
    vec3 color = vec3(0.910,0.228,0.228) + k_a;
    
    float ang = 3.;
    
    vec3 light1Pos = vec3(-6.0 * sin(0.37 * ang),
                          10.0,
                          2.0 * cos(0.37 * ang));
    vec3 light1Intensity = vec3(.7);
    
    color += phongContribForLight(k_d, k_s, alpha, p, eye,
                                  light1Pos,
                                  light1Intensity);
    
    float theta = 2.448;
    
    vec3 light2Pos = vec3(-7.0 * sin(0.37 * theta),
                          -5. * cos(0.37 * theta),
                          2.0);
    vec3 light2Intensity = vec3(.6);
    
    color += phongContribForLight(k_d, k_s, alpha, p, eye,
                                  light2Pos,
                                  light2Intensity);    
    // return color;   
    return color ;
}
void main() {
    vec2 st = gl_FragCoord.xy/u_resolution.xy;
    st.x *= u_resolution.x/u_resolution.y;
    
    vec3 color = vec3(1.);

    gl_FragColor = vec4(color, 1.0);
}`

export const vert = `
#ifdef GL_ES
precision mediump float;
#endif
void main() {
    vec4 aPos = vec4( position, 1. );
    aPos.xy = aPos.xy * 2. - 1.;
    gl_Position = aPos;
}`