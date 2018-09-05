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

/////////////////////////////////////////////////////////////////

uniform int typeIllumination;
uniform int utiliseBlinn;
uniform int utiliseDirect;

uniform sampler2D laTexture;
uniform int utiliseCouleur;
uniform int texnumero;
uniform int noirTransparent;
uniform int afficheNormales;

in Attribs {
   vec4 couleur;
   vec3 normale;
   vec3 lumiDir;
   vec3 O;
   vec2 texCoord;
} AttribsIn;

out vec4 FragColor;

float calculerSpot( in vec3 spotDir, in vec3 L )
{
	float cosDelta = cos(radians(LightSource[0].spotCutoff));
	float cosGamma = dot(L, spotDir);
	float c = LightSource[0].spotExponent;
	float cosTetaOuter = pow(cosDelta,(1.01 + c / 2));
	float cosTetaInner = cosDelta;
	float spot; 
	if(utiliseDirect == 1)
	{
		if(cosGamma < cosTetaOuter)
		{
			spot = 0.0;
		}
		else if(cosGamma > cosTetaInner)
		{
			spot = 1.0;
		}
		else
		{
			spot = clamp(((cosGamma - cosTetaOuter) / (cosTetaInner - cosTetaOuter)), 0.0, 1.0);
		}
	}
	else
	{
		spot = (cosGamma > cosDelta) ? clamp(pow(cosGamma, c), 0.0, 1.0) : 0;
	}
	return spot;
}

vec4 calculerReflexion( in vec3 L, in vec3 N, in vec3 O )
{
	vec4 coul = AttribsIn.couleur;
	if(typeIllumination != 1) //modele illumination Phong & lambert
	{
		coul = FrontMaterial.emission + FrontMaterial.ambient * LightModel.ambient;
		coul += LightSource[0].ambient * FrontMaterial.ambient;
		float NdotL = max(0.0, dot(N, L));
		coul += FrontMaterial.diffuse * LightSource[0].diffuse * NdotL;
		float NdotHVorRdotO;
		NdotHVorRdotO = (utiliseBlinn == 1) ? max(0.0, dot(normalize(L + O), N)) : max(0.0, dot(reflect(-L, N), O));
		coul += FrontMaterial.specular * LightSource[0].specular * pow(NdotHVorRdotO, FrontMaterial.shininess);
	}
	return(coul);
}

//fonction qui calcul la moyenne des couleurs afin de faire la nuance de gris.
vec4 nuanceDeGris(in vec4 couleur)
{
  float moyenne = (couleur.r + couleur.g + couleur.b) / 3.0;
  return vec4(moyenne, moyenne, moyenne, 1.0);
}

void main( void )
{
	//FragColor = AttribsIn.couleur;
	vec3 L = normalize(AttribsIn.lumiDir);
	vec3 N = normalize(AttribsIn.normale);
	vec3 O = normalize(AttribsIn.O);
	vec3 Ln = normalize(-1 * LightSource[0].spotDirection);
	
	vec4 coul = calculerReflexion(L, N, O);
	coul *= calculerSpot(Ln, L);
	if(utiliseCouleur != 1) coul = nuanceDeGris(coul);
	vec4 couleur0 = texture(laTexture,AttribsIn.texCoord);
	if( noirTransparent == 1 && couleur0.r < 0.1 && couleur0.g < 0.1 && couleur0.b < 0.1 && texnumero != 0) discard;
	if(afficheNormales == 1)
	{
		FragColor = vec4(N, 1.0);
	}
	else if (texnumero != 0)
	{
		FragColor = clamp(coul * couleur0, 0.0, 1.0);
	}
	else
		FragColor = clamp(coul, 0.0, 1.0);
}
