#version 410

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

in Attribs {
   vec4 couleur;
   float tempsDeVieRestant;
   vec3 vitesse;
   flat int nombreCollision;
} AttribsIn[];

out Attribs {
   vec4 couleur;
   vec2 texCoord;
   flat int nombreCollision;
} AttribsOut;

uniform mat4 matrProj;
uniform int texnumero;

void main()
{
   vec2 coins[4];
   vec2 coinsModifies[4];
   coins[0] = vec2( -0.5,  0.5 );
   coins[1] = vec2( -0.5, -0.5 );
   coins[2] = vec2(  0.5,  0.5 );
   coins[3] = vec2(  0.5, -0.5 );
   mat2 rotationEtincelle = mat2(cos(4.0 * AttribsIn[0].tempsDeVieRestant), -sin(4.0 * AttribsIn[0].tempsDeVieRestant), 
   sin(4.0 * AttribsIn[0].tempsDeVieRestant), cos(4.0 * AttribsIn[0].tempsDeVieRestant));
   
   for ( int i = 0 ; i < 4 ; ++i )
   {
	  coinsModifies[i] = (AttribsIn[0].vitesse.x <= 0) ? coins[int(mod(i+2,4))] : coins[i];
      float fact = gl_in[0].gl_PointSize; // (gl_PointSize n'est pas initialisÃ© si on ne produit pas de point)
      vec2 decal = fact * coinsModifies[i]; // on positionne successivement aux quatre coins
      decal = (texnumero == 0) ? decal * rotationEtincelle : decal;
      vec4 pos = vec4( gl_in[0].gl_Position.xy + decal, gl_in[0].gl_Position.zw );
      gl_Position = matrProj * pos;    // on termine la transformation dÃ©butÃ©e dans le nuanceur de sommets
      
      
      vec2 textureCoord = coins[i] + vec2( 0.5, 0.5 ); //etincelle a la base.
      if(texnumero != 0) //si lutin ou oiseau
      {
		int coordXLutinOiseau = int(mod(20.0 * AttribsIn[0].tempsDeVieRestant, 16.0));
		textureCoord.x += coordXLutinOiseau;
		textureCoord.x /= 16.0;
	  }
	  
	  AttribsOut.couleur = AttribsIn[0].couleur;
      AttribsOut.texCoord = textureCoord;  
      AttribsOut.nombreCollision = AttribsIn[0].nombreCollision;
      
      EmitVertex();
  }
}
