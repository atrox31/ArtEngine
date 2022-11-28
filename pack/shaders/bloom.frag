#version 130
//varying vec4 gpu_Color;

uniform sampler2D image;
in vec2 texCoord;

uniform bool horizontal;
uniform float weight[5] = float[] (0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162);

void main(void)
{
	vec2 tex_offset = 1.0 / textureSize(image, 0); // gets size of single texel
     vec4 result = texture2D(image, texCoord).rgba * weight[0];
     if(horizontal)
     {
         for(int i = 1; i < 5; ++i)
         {
            result += texture2D(image, texCoord + vec2(tex_offset.x * i, 0.0)).rgba * weight[i];
            result += texture2D(image, texCoord - vec2(tex_offset.x * i, 0.0)).rgba * weight[i];
         }
     }
     else
     {
         for(int i = 1; i < 5; ++i)
         {
             result += texture2D(image, texCoord + vec2(0.0, tex_offset.y * i)).rgba * weight[i];
             result += texture2D(image, texCoord - vec2(0.0, tex_offset.y * i)).rgba * weight[i];
         }
     }
	 result[3] = result[3] * 0.4;
     gl_FragColor = result;
}