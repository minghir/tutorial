/*
void mainImage( out vec4 fragColor, in vec2 fragCoord ){
    fragCoord = (fragCoord-0.5*iResolution.xy)/iResolution.y+cos(fragCoord.yx)*sin(fragCoord.yx);
     vec3 col = vec3(step(sin(iTime/10.)*cos(iTime*10.),fragCoord.x/fragCoord.y));
     col *= vec3(step(sin(iTime/10.)*cos(iTime*10.),fragCoord.y/fragCoord.x));
     //col = smoothstep(0.212,0.21,col);
    fragColor = vec4(vec3(fragCoord.xy,2.)+col,1.);
   // fragColor = vec4(vec3(col.gr,0.),1.);
}
*/

void mainImage( out vec4 fragColor, in vec2 fragCoord ){
    fragColor = vec4((fragCoord-.5*iResolution.xy)/iResolution.y+sin(fragCoord.xy)*cos(fragCoord.yx),0.,1.);
    /*
     vec3 col  = vec3(
        step(0.,sin(fragCoord.x/6.)+.99))*step(0.,vec3(sin(fragCoord.y/6.)+.992)
        );
        fragColor = vec4(col,1.);
    */
}
/*
void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    // Normalized pixel coordinates (from 0 to 1)
    vec2 uv = (fragCoord-.5*iResolution.xy)/iResolution.y;
    // Time varying pixel color
  //  vec3 col = 0.5 + 0.5*cos(iTime+uv.xyx+vec3(0,2,4));
  vec3 col  = vec3(
        step(0.,sin(fragCoord.x/25.)))*step(0.,vec3(sin(fragCoord.y/25.))
        );
    // Output to screen
    fragColor = vec4(col,1.0);
}
*/
