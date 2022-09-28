
void mainImage( out vec4 fragColor, in vec2 fragCoord ){
    fragCoord = (fragCoord-0.5*iResolution.xy)/iResolution.y+cos(fragCoord.xy)*sin(fragCoord.yx);
    //fragCoord *=10.1;
    fragColor = vec4(vec3(fragCoord.xy,1.),1.);

}
