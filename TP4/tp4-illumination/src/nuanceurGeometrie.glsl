#version 430

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

layout(location=0) uniform mat4 matrModel;
layout(location=4) uniform mat4 matrVisu;
layout(location=8) uniform mat4 matrProj;
uniform int modele; //Ajouter pour regler la theiere (bonus selon notre charger de lab).
uniform int typeIllumination;

in Attribs {
   vec4 couleur;
   vec3 normale;
   vec3 lumiDir;
   vec3 O;
   vec2 texCoord;
} AttribsIn[];

out Attribs {
   vec4 couleur;
   vec3 normale;
   vec3 lumiDir;
   vec3 O;
   vec2 texCoord;
} AttribsOut;

void main()
{
   // émettre les sommet
	vec3 nouvelleNormal = normalize(cross(gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz, gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz));
	if(modele == 2) nouvelleNormal *= -1.0; //Ajouter pour regler le probleme de la theiere (bonus selon notre charger de laboratoire).
	for ( int i = 0 ; i < gl_in.length() ; ++i )
	{
		gl_Position = matrProj * gl_in[i].gl_Position;
		AttribsOut.couleur = AttribsIn[i].couleur;
		AttribsOut.normale = (typeIllumination == 0) ? nouvelleNormal : AttribsIn[i].normale;
		AttribsOut.O = AttribsIn[i].O;
		AttribsOut.lumiDir = AttribsIn[i].lumiDir;
		AttribsOut.texCoord = AttribsIn[i].texCoord;
		EmitVertex();
	}
}
