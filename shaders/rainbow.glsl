float Circle(vec2 uv, vec2 pos, float r){
    uv += pos;
    return  (length(uv)-r);
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    // Normalized pixel coordinates (from 0 to 1)
    //vec2 uv = fragCoord/iResolution.xy;
    //uv.x *= fragCoord.x/iResolution.y;
    //uv += 0.5;
    vec2 uv = (fragCoord-.5*iResolution.xy)/iResolution.y;

    // Time varying pixel color
    float c,j;
    vec3 col = vec3(0.);
    for(float i=0.;i<=0.2*5.;i+=0.2){
        c = Circle(uv,vec2(0.,1.4-i),0.9);
        col += mix(vec3(0.2+i*(-cos(iTime)),0.2*sin(float(iFrame)/100.)+i*sin(iTime),0.2),col,step(0.,c));
        j +=  i+10.;
    }
    //c = Circle(uv,vec2(0.,.9),0.9);
        //col = mix(vec3(1.,1.,0.),col,step(0.,c));
    
    
    
     //c = Circle(uv,vec2(0.,.5),0.9+0.4);
     //col += mix(vec3(1.,3.,1.),col,step(0.,c));
    
    
    //col = col +c;
    // Output to screen
    fragColor = vec4(col,1.0);
}
