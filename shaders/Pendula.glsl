
void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    // Normalized pixel coordinates (from 0 to 1)
    vec2 uv = (fragCoord-.5*iResolution.xy)/iResolution.y;
    // Time varying pixel color
   
     vec3 col  = vec3(
      step(0.,uv.y/uv.x-cos(iTime)*sin(iTime))
    );
   
     col *= vec3(
    step(0.,uv.x/uv.y+cos(iTime)*sin(iTime))
    );
 
 /*
    col  = vec3(
     step(0.,(max(sin(uv.y)/sin(uv.x),sin(uv.x)/sin(uv.y)))
     -cos(iTime)*sin(iTime))
   
    );
   */ 
  
   
   //uv=((sin(uv))/cos(uv))+abs(cos(iTime)*sin(iTime))/3.;//distort 
   
   vec3 grid = vec3(
        step(1.-col.x,
            cos(60.*uv.x)*cos(60.*uv.y)
            //cos(60.*uv.x)*cos(60.*uv.y)/cos(50.*uv.y)*cos(50.*uv.x)
            
            )
     );
    
    /*
       vec3 grid = vec3(
        step(1.-col.x*(1.-col.y),
           // cos(79.*uv.x)*cos(79.*uv.y))
           step(0.,sin(fragCoord.x/0.01)+.999))*step(0.,vec3(sin(fragCoord.y/0.01)+.999)
           )
     );
    */
   
    col = mix(vec3(255, 50, 0)/255.,col,col);
    
    
    col -= length(uv-cos(iTime)*sin(iTime))-sin(uv.y)*sin(uv.x)-0.7;
   
 
    //fragColor = vec4(col-grid,1.0);
    fragColor = vec4(col-grid,1.0);
}
