vec3 colors[8]= vec3[](
vec3(148, 0, 211),//		Violet
vec3(75, 0, 130),//		Indigo
vec3(0, 0, 255),//		Blue
vec3(0, 255, 0),//		Green
vec3(255, 255, 0),//	Yellow
vec3(255, 127, 0),//	Orange
vec3(255, 0 , 0),//		Red
vec3(0, 0 , 0)//		
);


//https://www.shadertoy.com/view/3lVGWt
const float PI = 3.14;
mat2 rotationMatrix(float angle)
{
	angle *= PI / 180.0;
    float s=sin(angle), c=cos(angle);
    return mat2( c, -s, s, c );
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 uv = (fragCoord-.5*iResolution.xy)/iResolution.y;

    uv *= rotationMatrix(iTime*20.);//rotate
    uv=((sin(uv)-.5)/cos(uv-0.2))+abs(cos(iTime)*sin(iTime))/3.;//distort
    uv.x +=0.4;
   
    vec3 col = vec3(0.);
    int j=0;
    for(float i=0.;i<=0.16*7.;i+=0.16)
    {
        //float r = 1.5-i;//circle size
        float r = 1.16-i*.8+abs(sin(iTime)*cos(iTime)*1.3);//circle size
        float c = length(uv+vec2(0.,0.5))-(r/2.5-i/8.);//circle
        col = mix(vec3(j<=2?1.:0.),col,smoothstep(.021,.028,(c)));//white or black circle line
        col = mix(colors[j++]/255.,col,smoothstep(.02,.025,c));//color circle
    }
   
    
    fragColor = vec4(col,1.0);
}
