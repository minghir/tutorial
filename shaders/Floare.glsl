float flower(
        vec2 uv, 
        float amplitude, 
        float frequency 
        ) {
    
    vec3 color = vec3(1.0);
    vec2 figure = uv - vec2(0.5, 0.5);

    float size = 0.3;
    float r =  amplitude * cos(atan(figure.y, figure.x) * frequency);
    
    float fl = smoothstep(r,r+0.01, (length(figure)-size));

    return fl;
}


void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
     vec2 uv = fragCoord / iResolution.y - vec2(0.5, 0.0);
    float fl     = flower(uv,   0.05,  10.0);
    fragColor = vec4(vec3(fl), 1.0);
}
