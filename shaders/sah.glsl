void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    // Normalized pixel coordinates (from 0 to 1)
//    vec2 uv = fragCoord/iResolution.xy;
vec2 uv = (fragCoord-.5*iResolution.xy)/iResolution.y;


   vec3 col = 0.5 + 0.5*cos(iTime+uv.xyx+vec3(0,2,4));

    // Time varying pixel color
     col += vec3(
           step(0.,cos(69.*uv.x)*cos(69.*uv.y))
    );
    
    col *= vec3(
         step(0.,sin(fragCoord.x/0.01)+.999))*step(0.,vec3(sin(fragCoord.y/0.01)+.999)
    );
   
    //col = mix(vec3(255, 99, 0)/255.,col,col);
    // Output to screen
    fragColor = vec4(col,1.0);
}
