#version 430

in Attribs {
	float facteurLatitude;
	vec4 couleurPoleRechauffer;
	vec4 couleur;
} AttribsIn;

out vec4 FragColor;

uniform float facteurRechauffement; 
uniform int coulProfondeur;
const float debAttenuation = 20.0;
const float finAttenuation = 40.0;
const vec4 couleurNoir = vec4(0.0,0.0,0.0,0.0);

float distanceCam;
vec4 couleurMixee;
vec4 couleurAttenuee;

void main( void )
{
	distanceCam = gl_FragCoord.z / gl_FragCoord.w;
	distanceCam -= debAttenuation;
	distanceCam /= debAttenuation;
	//la couleur du fragment est la couleur interpolée
	
	couleurMixee = mix(AttribsIn.couleur, AttribsIn.couleurPoleRechauffer, AttribsIn.facteurLatitude);
	couleurAttenuee = mix(couleurMixee, couleurNoir, clamp(distanceCam, 0.0, 1.0));
	FragColor = mix(couleurMixee, couleurAttenuee, coulProfondeur);
}
