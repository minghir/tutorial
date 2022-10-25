#define SIZE 79.
#define test 4

#if test == 3
void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    
    vec2 uv = fragCoord;
  
    vec3 col2;
    if(uv.x<=SIZE)
        col2 = vec3(0.,1.,0.);
        
    if(uv.x>=iResolution.x-SIZE)
        col2 = vec3(0.,1.,0.);
    
     if(uv.y <= SIZE)
        col2 = vec3(0.,1.,0.);
        
     if(uv.y >= iResolution.y - SIZE)
        col2 = vec3(0.,1.,0.);
  
    fragColor = vec4(col2,1.0);
}

//no a_ratio
#elif test == 1
void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    // Normalized pixel coordinates (from 0 to 1)
    vec2 uv = fragCoord/iResolution.xy;
  
    vec3 col2;
    if(uv.x<(SIZE-uv.x)/iResolution.x || uv.x>(iResolution.x-(uv.x+SIZE-1.))/iResolution.x)
        col2 = vec3(0.,1.,0.);
    
     if(uv.y < (SIZE-uv.y)/iResolution.y  || uv.y > (iResolution.y-(uv.y+SIZE-1.))/iResolution.y )
        col2 = vec3(0.,1.,0.);
  
    fragColor = vec4(col2,1.0);
}
#elif test == 2

// a_ratio
void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    // Normalized pixel coordinates (from 0 to 1)
    vec2 uv = fragCoord/iResolution.xy;
    float a_r = iResolution.x/iResolution.y;
    uv.x *= a_r;
    
    vec3 col2;
    if(uv.x<(SIZE*a_r-uv.x)/iResolution.x || uv.x>( (iResolution.x*a_r-(uv.x+(SIZE-1.)*a_r))/iResolution.x))
        col2 = vec3(0.,1.,0.);
    
     if(uv.y < (SIZE-uv.y)/iResolution.y  || uv.y > (iResolution.y-(uv.y+SIZE-1.))/iResolution.y )
        col2 = vec3(0.,1.,0.);
  
    fragColor = vec4(col2,1.0);
}

#elif test == 3

// a_ratio center origin
void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    // Normalized pixel coordinates (from 0 to 1)
    //vec2 uv = (fragCoord-.5*iResolution.xy)/iResolution.y;
    vec2 uv = fragCoord/iResolution.xy;
    uv -= 0.5;
    float a_r = iResolution.x/iResolution.y;
    uv.x *=a_r;

   vec3 col2;
            
    if(uv.x < -0.5*a_r+(SIZE*a_r-uv.x)/iResolution.x)
         col2 = vec3(0.,1.,0.);
    
    if(uv.x > 0.5*a_r-(uv.x+SIZE*a_r)/iResolution.x)
        col2 = vec3(0.,1.,0.);
  
    if(uv.y < -0.5 + (SIZE-uv.y)/iResolution.y)
         col2 = vec3(0.,1.,0.);
         
    if(uv.y > 0.5- (uv.y+SIZE)/iResolution.y)
        col2 = vec3(0.,1.,0.);
    
    fragColor = vec4(col2,1.0);
}

#elif test == 4

// a_ratio center origin
void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    // Normalized pixel coordinates (from 0 to 1)
    float a_r = iResolution.x/iResolution.y;
    vec2 uv = (2.*fragCoord-iResolution.xy ) / iResolution.y ;
    
   vec3 col2;
            
    if(uv.x < -a_r+(SIZE*a_r*2.-uv.x)/iResolution.x)
         col2 = vec3(0.,1.,0.);
    
    if(uv.x > a_r-(uv.x+SIZE*a_r*2.)/iResolution.x)
        col2 = vec3(0.,1.,0.);
  
    if(uv.y < (2.*SIZE-uv.y)/iResolution.y - 1.)
         col2 = vec3(0.,1.,0.);
         
    if(uv.y > 1.- (uv.y+2.*SIZE)/iResolution.y)
        col2 = vec3(0.,1.,0.);
    
    fragColor = vec4(col2,1.0);
}

#endif
