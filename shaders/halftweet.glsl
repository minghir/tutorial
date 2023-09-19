void mainImage( out vec4 o, in vec2 i )
{
//o*=.0;o[int(i)/2&int(i.y)/2]++;
//o*=.0;o[int(i)/3&int(i.y)/33]++;
//o*=.0;o[int(i)&int(i.y)/33]++;
//o*=.0;o[int(i)/33&int(i.y)*9]++;
//o*=.0;o[int(i)&int(i.y)]+=8.;
//o*=.7;o[int(i)&int(i.y)]+=9.;
o*=.0;o[(int(i)&int(i.y))/26]++;
o*=.0;o[(int(i)|int(i.y))/99]++;
o*=.9;o[int(i)/199]+=9.;
}
