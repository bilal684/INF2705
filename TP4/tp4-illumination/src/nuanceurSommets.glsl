#version 430

// Définition des paramètres des sources de lumière
struct LightSourceParameters
{
   vec4 ambient;
   vec4 diffuse;
   vec4 specular;
   vec4 position;
   vec3 spotDirection;
   float spotExponent;
   float spotCutoff;            // ([0.0,90.0] ou 180.0)
   float constantAttenuation;
   float linearAttenuation;
   float quadraticAttenuation;
};
uniform LightSourceParameters LightSource[3];

// Définition des paramètres des matériaux
struct MaterialParameters
{
   vec4 emission;
   vec4 ambient;
   vec4 diffuse;
   vec4 specular;
   float shininess;
};
uniform MaterialParameters FrontMaterial;
uniform MaterialParameters BackMaterial;

// Définition des paramètres globaux du modèle de lumière
struct LightModelParameters
{
   vec4 ambient;       // couleur ambiante
   bool localViewer;   // observateur local ou à l'infini?
   bool twoSide;       // éclairage sur les deux côtés ou un seul?
};
uniform LightModelParameters LightModel;

layout(location=0) uniform mat4 matrModel;
layout(location=4) uniform mat4 matrVisu;
layout(location=8) uniform mat4 matrProj;
layout(location=12) uniform mat3 matrNormale;

/////////////////////////////////////////////////////////////////

layout(location=0) in vec4 Vertex;
layout(location=2) in vec3 Normal;
layout(location=3) in vec4 Color;
layout(location=8) in vec4 MultiTexCoord0;
//layout(location=9) in vec4 MultiTexCoord1;

uniform int typeIllumination;
uniform int utiliseCouleur;
uniform int utiliseBlinn;

out Attribs {
   vec4 couleur;
   vec3 normale;
   vec3 lumiDir;
   vec3 O;
   vec2 texCoord;
} AttribsOut;

vec4 calculerReflexion( in vec3 L, in vec3 N, in vec3 O )
{
   vec4 coul = Color;
   if(typeIllumination == 1) //modele illumination Gouraud
   {
		coul = FrontMaterial.emission + FrontMaterial.ambient * LightModel.ambient;
		coul += LightSource[0].ambient * FrontMaterial.ambient;
		float NdotL = max(0.0, dot(N, L));
		coul += FrontMaterial.diffuse * LightSource[0].diffuse * NdotL;
		float NdotHVorRdotO = (utiliseBlinn == 1) ? max(0.0, dot(normalize(L + O), N)) : max(0.0, dot(reflect(-L, N), O));
		coul += FrontMaterial.specular * LightSource[0].specular * pow(NdotHVorRdotO, FrontMaterial.shininess);
   }
   return(coul);
}

void main( void )
{
	// transformation standard du sommet
   gl_Position = matrVisu * matrModel * Vertex;
   vec3 pos = vec3(matrVisu * matrModel * Vertex);
   mat3 notreMatriceNormale = mat3(transpose(inverse(matrModel)));
   vec3 laNormale = notreMatriceNormale * Normal;
   vec3 LumiDirection = vec3((matrVisu*LightSource[0].position).xyz - pos);
   vec3 obs = normalize(-pos);
   AttribsOut.normale = laNormale;
   AttribsOut.O = obs;
   AttribsOut.lumiDir = LumiDirection;
   AttribsOut.texCoord = MultiTexCoord0.st;
   vec3 L = normalize(LumiDirection);
   vec3 N = normalize(laNormale);
   vec3 O = normalize(obs);
   // couleur du sommet
   AttribsOut.couleur = calculerReflexion(L,N,O);
}
