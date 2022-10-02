#define PI 3.141592653589793238
void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    // Normalized pixel coordinates (from 0 to 1)
    vec2 uv = (fragCoord-.5*iResolution.xy)/iResolution.y;
    // Time varying pixel color
     vec3 col  = vec3(
        step(0.,sin(uv.y)/sin(uv.x)-cos(iTime)*sin(iTime)*(0.53-iResolution.x/iResolution.y))
        //step(0.,sin(uv.y)/sin(uv.x)*sin(uv.y)/sin(uv.x)-cos(iTime)*sin(iTime)*(0.555-iResolution.x/iResolution.y))
        
        );
    
    col = mix(vec3(255, 99, 0)/255.,col,col);
    vec3 grid = vec3(
    //step(0.,sin(uv.x)*sin(uv.y))
      //0.9999-step(.0111,cos(uv.x/0.003)+0.999))*step(.0111,vec3(cos(uv.y/0.003)+0.999)
     0.999- (step(.00999,cos(uv.x/0.0002)+0.999)*step(.00999,cos(uv.y/0.0002)+.999))
    // step(1.,cos(15.*PI*uv.x)*cos(15.*PI*uv.y)*(cos(15.*PI*uv.x)*cos(15.*PI*uv.y)) - 0.002)
    );
   
    
    //col = mix(vec3(1., 1., 0.),col,grid);
    //col = mix(vec3(255, 127, 0)/255.,col,col);
    
    col -= length(uv.xy*cos(iTime)*sin(iTime))-(sin(uv.x)*sin(uv.y));   
   
 
    fragColor = vec4(col-grid,1.0);
}
