float bckgr(vec2 uv){
    return 
        step(.0,cos(uv.x/.03)+0.999)*
        step(.0,cos(uv.y/.03)+0.999);
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
   vec2 uv = (fragCoord-.5*iResolution.xy)/iResolution.y;
   float d2 = smoothstep(0.,.09*iResolution.x/iResolution.y,length(uv/5.));
   float d = smoothstep(0.,.09*iResolution.x/iResolution.y,length(uv/.5));
   d /= bckgr(uv);
   float r =  .025 * sin(cos(atan(uv.x, uv.y) * 25.));
   float f = smoothstep(0.,.06*iResolution.x/iResolution.y, length(r));

   fragColor = vec4(vec3(f+(1.-d))*d2*10., 1.0);
  //fragColor = vec4(vec3(f*d)*10., 1.0);
}
