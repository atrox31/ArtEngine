#version 130
varying vec2 texCoord;
uniform sampler2D image;

out vec4 out_Color;

uniform int Quality;
uniform float Distance;
uniform int Directions;
const float Pi = 3.14159265359;
const float Tau = Pi*2.0;

void main()
{
	vec4 Color;

	for(float dir = 0.0;dir<1.0;dir+=1.0/float(Directions)) {
	float xvar = (Distance*2.0+Distance)*sin(dir*Tau);
	float yvar = (Distance*2.0+Distance)*cos(dir*Tau);

	for(float i = 0.0;i < 1.0;i+=1.0/float(Quality)) {
	float dis = i*Distance;
	Color += texture2D(image,vec2(texCoord.x+xvar*dis,texCoord.y+yvar*dis));
	}

	}
	Color /= float(Quality)*float(Directions); 

	out_Color = Color;
	
}