#version 130
in vec2 texCoord;
uniform sampler2D image;

void main(void)
{
	vec4 c = texture2D(image, texCoord);

	if(c[0] < 0.75) c[3] = 0;
	if(c[1] < 0.75) c[3] = 0;
	if(c[2] < 0.75) c[3] = 0;
	
    gl_FragColor = c;
}