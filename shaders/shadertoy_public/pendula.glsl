float bckgr(vec2 uv){
    return step(step(0.,cos(81.*uv.x)*sin(81.*uv.y)),cos(80.*uv.x)*sin(80.*uv.y))/6.;
}

float pend(vec2 uv,float speed,float size){
   uv.y = .8+sin(uv.y*2.6)/2.*size;
   float p1 = step(-.2,uv.x/uv.y+cos(iTime*speed)*sin(iTime*speed));
   p1 -= step(.2,uv.x/uv.y+cos(iTime*speed)*sin(iTime*speed));
   float p2 = step(.55,uv.x/uv.y+cos(iTime*speed)*sin(iTime*speed));
   p2 -= step(-.55,uv.x/uv.y+cos(iTime*speed)*sin(iTime*speed));
   p1 -=p2/2.;
  return (1.-p1+.5); 
}

float light(vec2 uv){
    uv.y += .6;
    uv.x +=.7*sin(iTime);
    return length(uv)-.795;
}

void mainImage( out vec4 fragColor, in vec2 fragCoord ){
vec2 uv = (fragCoord-.5*iResolution.xy)/iResolution.y;
vec3 col = vec3(bckgr(uv));
float p1 = pend(uv,.7,1.5);
float p2 = pend(uv,.75,1.5);
float p3 = pend(uv,.80,1.5);
vec3 c1 = mix(vec3(255, 50, 50)/255.,col,p1);
vec3 c2 = mix(vec3(50, 255, 50)/255.,col,p2);
vec3 c3 = mix(vec3(50, 50, 255)/255.,col,p3);
col += (c1+c2+c3)-light(uv);
 fragColor = vec4(col,1.);
}
