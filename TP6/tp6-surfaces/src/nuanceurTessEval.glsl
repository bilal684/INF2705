//#version 410

layout(quads) in;

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

// Définition des paramètres globaux du modèle de lumière
struct LightModelParameters
{
   vec4 ambient;       // couleur ambiante
   bool localViewer;   // observateur local ou à l'infini?
   bool twoSide;       // éclairage sur les deux côtés ou un seul?
};
uniform LightModelParameters LightModel;
uniform sampler2D textureCoul;

// in vec3 gl_TessCoord;
// in int gl_PatchVerticesIn;
// in int gl_PrimitiveID;
// patch in float gl_TessLevelOuter[4];
// patch in float gl_TessLevelInner[2];
// in gl_PerVertex
// {
//   vec4 gl_Position;
//   float gl_PointSize;
//   float gl_ClipDistance[];
// } gl_in[gl_MaxPatchVertices];

// out gl_PerVertex {
//   vec4 gl_Position;
//   float gl_PointSize;
//   float gl_ClipDistance[];
// };

uniform mat4 matrModel;
uniform mat4 matrVisu;
uniform mat4 matrProj;
uniform mat3 matrNormale;

/////////////////////////////////////////////////////////////////

uniform float facteurZ;
uniform sampler2D textureDepl;

out Attribs {
   vec3 normale, lumiDir[2], obsVec;
   vec2 texCoord;
} AttribsOut;

float interpole( float v0, float v1, float v2, float v3 )
{
   float r = gl_TessCoord.x;
   float s = gl_TessCoord.y;
   return ( v0 * (1.0-r-s+r*s) + v1 * (1.0+r-s-r*s) + v2 * (1.0+r+s+r*s) + v3 * (1.0-r+s-r*s) )/4.0;
}
vec2 interpole( vec2 v0, vec2 v1, vec2 v2, vec2 v3 )
{
   float r = gl_TessCoord.x;
   float s = gl_TessCoord.y;
   return ( v0 * (1.0-r-s+r*s) + v1 * (1.0+r-s-r*s) + v2 * (1.0+r+s+r*s) + v3 * (1.0-r+s-r*s) )/4.0;
}
vec3 interpole( vec3 v0, vec3 v1, vec3 v2, vec3 v3 )
{
   float r = gl_TessCoord.x;
   float s = gl_TessCoord.y;
   return ( v0 * (1.0-r-s+r*s) + v1 * (1.0+r-s-r*s) + v2 * (1.0+r+s+r*s) + v3 * (1.0-r+s-r*s) )/4.0;
}
vec4 interpole( vec4 v0, vec4 v1, vec4 v2, vec4 v3 )
{
   float r = gl_TessCoord.x;
   float s = gl_TessCoord.y;
   return ( v0 * 4.*(1.0-r-s+r*s) + v1 * (1.0+(2.*r-1.)-(2.*s-1.)-(2.*r-1.)*(2.*s-1.)) + v2 * (1.0+(2.*r-1.)+(2.*s-1.)+(2.*r-1.)*(2.*s-1.)) + v3 * (1.0-(2.*r-1.)+(2.*s-1.)-(2.*r-1.)*(2.*s-1.)) )/4.0;
}

float FctMath( float x, float y )
{
   // déplacement selon la fonction mathématique
   float z = 0.0;
#if ( INDICEFONCTION == 1 )
   z = 0.5 * ( x*x - y*y );
#elif ( INDICEFONCTION == 2 )
   z = ( x*y * cos(x)*sin(y) );
#elif ( INDICEFONCTION == 3 )
   z = 4.0 * x*y / exp(x*x + y*y);
#elif ( INDICEFONCTION == 4 )
   z = 0.4 * sin(3.0*x)*cos(6.0*y);
#elif ( INDICEFONCTION == 5 )
   z = 0.2 * exp(sin(5.0*y)*sin(2.0*x)) * exp(sin(1.0*y)*sin(5.0*x));
#elif ( INDICEFONCTION == 6 )
   { float t = x*x+y*y; z = (1.0-5.0*t) * exp(-t); }
#endif
   return facteurZ * z;
}

float FctText( vec2 texCoord )
{
   // déplacement selon la texture
   vec4 texel = texture(textureDepl, texCoord);
   return facteurZ * ( texel.x + texel.y + texel.z ) / 30.0;
}

vec4 planCoupeHaut = vec4( 0.0, 0.0, -1.0, 2.0 );
vec4 planCoupeBas = vec4( 0.0, 0.0, 1.0, 2.0 );

void main( void )
{
   // interpoler la position selon la tessellation
   vec4 sommet = interpole( gl_in[0].gl_Position, gl_in[1].gl_Position, gl_in[2].gl_Position, gl_in[3].gl_Position );

   // générer (en utilisant sommet.xy) les coordonnées de texture plutôt que les interpoler
   vec2 coordTexture;
   coordTexture.x = 0.5 * (sommet.x - 1);
   coordTexture.y = 0.5 * (sommet.y - 1);
   AttribsOut.texCoord = coordTexture;
   sommet = matrModel * sommet;
  
#if ( APPLIQUERDEPL != 0 )
    // déplacer le sommet selon la fonction ou la texture
	# if ( INDICEFONCTION != 0 )
	   // déplacement selon la fonction mathématique
	   sommet.z = FctMath(sommet.x, sommet.y);
	# else
	   // déplacement selon la texture
	   sommet.z = FctText( sommet.xy );
	#endif
#endif

	gl_Position =  matrProj * matrVisu * sommet;
   // initialiser la variable gl_ClipDistance[] pour que le découpage soit fait par OpenGL
   vec4 pos = sommet;
   gl_ClipDistance[0] = dot(planCoupeHaut, pos);
   gl_ClipDistance[1] = dot(planCoupeBas, pos);
   
   float eps = 0.01;
   // calculer la normale
   vec3 N;
#if ( INDICEFONCTION != 0 )
   float x = sommet.x;
   float y = sommet.y;
   N.x = (FctMath(x + eps, y) - FctMath(x - eps, y)) / (2 * eps);
   N.y = (FctMath(x, y + eps) - FctMath(x, y - eps)) / (2 * eps);
   N.z = -1;
   normalize(N);
   
#else
   float x = coordTexture.x;
   float y = coordTexture.y;
   N.x = (FctText(vec2(x + eps, y)) - FctText(vec2(x - eps, y))) / (2 * eps);
   N.y = (FctText(vec2(x, y + eps)) - FctText(vec2(x, y - eps))) / (2 * eps);
   N.z = -1;
#endif
   AttribsOut.normale = matrNormale * N;
   
   //vec3 obs = normalize(-posi);
      
   // vecteur de la direction vers la lumière
   vec3 posi = vec3(matrVisu * matrModel * sommet);
   AttribsOut.lumiDir[0] = normalize(vec3((matrVisu*LightSource[0].position).xyz - posi));
   AttribsOut.lumiDir[1] = normalize(vec3((matrVisu*LightSource[1].position).xyz - posi));
   // vecteur de la direction vers l'observateur
   AttribsOut.obsVec = (LightModel.localViewer) ? normalize(-posi) : vec3(0.0,0.0,1.0);
}
