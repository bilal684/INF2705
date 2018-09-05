#version 430

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;
in Attribs {
	float facteurLatitude;
	vec4 couleurPoleRechauffer;
	vec4 couleur;
} AttribsIn[];

out Attribs {
	float facteurLatitude;
	vec4 couleurPoleRechauffer;
	vec4 couleur;
} AttribsOut;

void main()
{
	for(int i = 0; i < AttribsIn.length(); i++)
	{
		gl_Position = gl_in[i].gl_Position;
		AttribsOut.facteurLatitude = AttribsIn[i].facteurLatitude;
		AttribsOut.couleurPoleRechauffer = AttribsIn[i].couleurPoleRechauffer;
		AttribsOut.couleur = AttribsIn[i].couleur;
		gl_ClipDistance[0] = gl_in[i].gl_ClipDistance[0];
		EmitVertex();
	}
}
