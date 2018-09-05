//#version 410


uniform mat4 matrModel;
uniform mat4 matrVisu;
uniform mat4 matrProj;


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

uniform sampler2D textureCoul;

//bool gl_FrontFacing;  // variable standard d'OpenGL: c'est la face avant qui est actuellement tracée?

in Attribs {
   vec3 normale, lumiDir[2], obsVec;
   vec2 texCoord;
} AttribsIn;

out vec4 FragColor;

bool utiliseBlinn = true;

void main( void )
{
   vec4 coul = FrontMaterial.emission + FrontMaterial.ambient * LightModel.ambient;

   // vecteur normal
   vec3 N = normalize( gl_FrontFacing ? -AttribsIn.normale : AttribsIn.normale );

   // calculer la contribution de chaque source lumineuse
   for ( int i = 0 ; i < 2 ; ++i )
   {
      // ajouter la contribution de la composante ambiante
      coul += FrontMaterial.ambient * LightSource[i].ambient;
	  float NdotL = max(0.0, dot(N, AttribsIn.lumiDir[i]));
      //...

      // calcul de l'éclairage seulement si le produit scalaire est positif
      if ( NdotL > 0.0 )
      {
         // ajouter la contribution de la composante diffuse
         #if ( INDICEDIFFUSE == 0 )
			 // la composante diffuse (kd) du matériel est utilisé
			 coul += FrontMaterial.diffuse * LightSource[i].diffuse * NdotL;
         #else
			 // la composante diffuse (kd) provient de la texture 'textureCoul'
			 coul += LightSource[i].diffuse * NdotL * texture(textureCoul,AttribsIn.texCoord);
         #endif
			 // ajouter la contribution de la composante spéculaire
			 float NdotHVorRdotO;
			 NdotHVorRdotO = (utiliseBlinn) ? max(0.0, dot(normalize(AttribsIn.lumiDir[i] + AttribsIn.obsVec), N)) : max(0.0, dot(reflect(-AttribsIn.lumiDir[i], N), AttribsIn.obsVec));
			 coul += FrontMaterial.specular * LightSource[i].specular * pow(NdotHVorRdotO, FrontMaterial.shininess);
      }
   }

   // assigner la couleur finale
   FragColor = clamp( coul, 0.0, 1.0 );

#if ( AFFICHENORMALES == 1 )
   // pour le débogage
   FragColor = vec4(N,1.0);
#endif
}
