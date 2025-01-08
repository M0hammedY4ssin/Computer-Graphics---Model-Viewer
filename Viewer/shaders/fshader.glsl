#version 330 core

struct Material
{
	sampler2D textureMap;
	// You can add more fields here...
	// Such as:
	//		1. diffuse/specular relections constants
	//		2. specular alpha constant
	//		3. Anything else you find relevant
		
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float alpha;
};

// We set this field's properties from the C++ code
uniform Material material;
uniform int drawlight;
uniform int modelcolor;
uniform int numOfBits;

uniform int sizeofarray;
uniform vec3 eye;

uniform vec3 ambients_[10];
uniform vec3 diffuses_[10];
uniform vec3 speculars_[10];
uniform vec3 positions_[10];
uniform vec3 directions_[10];
uniform mat4x4 transforms_[10];
uniform int modes_[10];
uniform int the_flag;

// Inputs from vertex shader (after interpolation was applied)
in vec3 fragPos;
in vec3 fragNormal;
in vec2 fragTexCoords;
in vec3 orig_fragPos;

// The final color of the fragment (pixel)
out vec4 frag_color;

//uniform sampler2D texture;


void main()
{
    vec3 textureColor = vec3(texture(material.textureMap, fragTexCoords));
    vec3 final_ambient=vec3(0.f,0.f,0.f);
	vec3 final_diffuse=vec3(0.f,0.f,0.f);
	vec3 final_specular=vec3(0.f,0.f,0.f);
    vec3 def;
    vec3 spec;
	// float in2;
	// vec3 diffuse2;
	 vec3 pos;
	
	if(modelcolor==1)
	{
		//frag_color = vec4(0,1,0,0);
		if(sizeofarray==0)
					frag_color=vec4(material.ambient,1.f);

		  else{
			  for(int i=0;i<sizeofarray;i++)
				{	
					vec3 I;
					// parallel
					if(modes_[i]==1) 
						I=normalize(directions_[i]);
					else if(modes_[i]==0){
						pos=vec3(transforms_[i]*vec4(positions_[i],1)); 
						I=normalize(fragPos - pos);
					}
					//in2=max(0.0, dot(I,-fragNormal));

					final_ambient+=vec3(ambients_[i].x*material.ambient.x,ambients_[i].y*material.ambient.y,ambients_[i].z*material.ambient.z);

					def=vec3(diffuses_[i].x*material.diffuse.x,diffuses_[i].y*material.diffuse.y,diffuses_[i].z*material.diffuse.z);
					final_diffuse+=def*(dot(-fragNormal,I));

					spec=vec3(speculars_[i].x*material.specular.x,speculars_[i].y*material.specular.y,speculars_[i].z*material.specular.z);
					vec3 v=(2.f*dot(-fragNormal,I)*fragNormal -I);
					float v2=pow(max(0.f, dot(v,normalize(eye))), material.alpha);
					final_specular+=spec*v2;
				}
					//phong
					if(the_flag == 1)
						frag_color=vec4(final_ambient+final_diffuse+final_specular, 1.f);

					if(the_flag == 2)
						frag_color=vec4(textureColor,1.f);	
					
					if(the_flag == 3)
					{
						frag_color=vec4(final_ambient+final_diffuse+final_specular, 1.f);
						frag_color = (round(frag_color*255 / numOfBits) *  numOfBits) / 255 ;
					}
		   }


  }			
	if(drawlight==1)
		frag_color =vec4(0.f,1.f,0.f,0.f);

}

