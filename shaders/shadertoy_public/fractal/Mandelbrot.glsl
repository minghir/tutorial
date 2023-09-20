//#define z uv
void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 uv = (fragCoord  - .5 * iResolution.xy)/iResolution.y;
    vec3 col = vec3(1.);
    
    uv *=2.;
    
    vec2 c = uv;
    for(int i = 0;i < 300;i++){
        uv = vec2(uv.x * uv.x - uv.y * uv.y, 2. * uv.x * uv.y) + c;
       // z = vec2(z.x * z.x - z.y * z.y, z.x*z.y+z.y*z.x) + c;
    }

    fragColor = vec4(1.,1.-uv,1.0);
}
