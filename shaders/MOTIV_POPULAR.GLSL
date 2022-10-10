float light(vec2 uv){
    uv.x -= 1.;
    uv.y =sin(iTime/2.)*cos(iTime/2.)/2.;
    float l1= length(uv)-0.130;
    float l2 = -l1;
    return (l1-l2)/2.9;
}

const float PI = 3.14159265359;
mat2 rotationMatrix(float angle)
{
	angle *= PI / 180.0;
    float s=sin(angle), c=cos(angle);
    return mat2( c, -s, s, c );
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    // Normalized pixel coordinates (from 0 to 1)
    vec2 uv = fragCoord/iResolution.xy;
    uv += vec2(0.5,0.5);
    float r = iResolution.x/iResolution.y;
    uv.x *= r;
    float pixel = 1.5/iResolution.y;
   
    uv *= rotationMatrix(30.);
    float l= light(uv);
    uv *= 1.1*r*(((sin(iTime/4.)*cos(iTime/4.))/6.+1.8)-(sin(1.-uv.x*4.7))/9.5);
    uv.x -= iTime/3.+sin(iTime/1.9)*cos(iTime/1.9)/.6;
   
   float g1 = smoothstep(0.,0.+25.*pixel,cos(69.*uv.x)*cos(69.*uv.y));
   float g2 = smoothstep(0.,0.+50.*pixel,cos(89.*uv.x)/cos(89.*uv.y));
   float g3 = smoothstep(1.,0.+3.*pixel,cos(79.*uv.x)*cos(79.*uv.y));
    
    vec3 col =vec3(g1+g2+g3);
    col *=vec3(g1-g2-g3);
    col = mix(vec3(1., 0.19, 0.19),col,1.-g1*g2*g3);
    fragColor = vec4(col-l,1.0);
}
