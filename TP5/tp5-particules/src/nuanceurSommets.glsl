#version 410

uniform mat4 matrModel;
uniform mat4 matrVisu;
uniform mat4 matrProj;

layout(location=0) in vec4 Vertex;
layout(location=3) in vec4 Color;
in float tempsDeVieRestant;
in vec3 vitesse;
in int nombreCollision;
out Attribs {
   vec4 couleur;
   float tempsDeVieRestant;
   vec3 vitesse;
   flat int nombreCollision;
} AttribsOut;

void main( void )
{
   // transformation standard du sommet
   gl_Position = matrVisu * matrModel * Vertex;
   vec4 coul = Color;
   AttribsOut.nombreCollision = nombreCollision;
	
   gl_PointSize = 0.15; // en unités du monde
   AttribsOut.couleur = Color;
   AttribsOut.tempsDeVieRestant = tempsDeVieRestant;
   AttribsOut.vitesse = vitesse;
   // À SUPPRIMER: la ligne suivante sert seulement à forcer le compilateur à conserver cet attribut
   if ( tempsDeVieRestant < 0.0 ) AttribsOut.couleur.a += 0.00001;
}
