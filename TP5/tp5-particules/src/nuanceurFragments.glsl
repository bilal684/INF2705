#version 410

uniform sampler2D laTexture;

in Attribs {
   vec4 couleur;
   vec2 texCoord;
   flat int nombreCollision;
} AttribsIn;

out vec4 FragColor;

void main( void )
{
   // Mettre un test bidon afin que l'optimisation du compilateur n'élimine l'attribut "couleur".
   // Vous MODIFIEREZ ce test plutôt inutile!
   vec4 coul = AttribsIn.couleur;
   mod(float(AttribsIn.nombreCollision), 2.0) == 0.0 ? coul.rgb = 1 - coul.rgb : coul.rgb;
   vec4 tex = texture( laTexture, AttribsIn.texCoord );
   if ( (tex.r + tex.g + tex.b) >= 3.0 ) discard;
   FragColor =  tex;

  // FragColor = tex;
}
