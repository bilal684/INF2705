#version 410

uniform vec3 bMin, bMax, positionPuits;
uniform float temps, dt, tempsVieMax, gravite;

in vec3 position;
in vec3 vitesse;
in float tempsDeVieRestant;
in int nombreCollision;

out vec3 positionMod;
out vec3 vitesseMod;
out float tempsDeVieRestantMod;
out int nombreCollisionMod;

uint randhash( uint seed ) // entre  0 et UINT_MAX
{
    uint i=(seed^12345391u)*2654435769u;
    i ^= (i<<6u)^(i>>26u);
    i *= 2654435769u;
    i += (i<<5u)^(i>>12u);
    return i;
}

float myrandom( uint seed ) // entre  0 et 1
{
   const float UINT_MAX = 4294967295.0;
   return float(randhash(seed)) / UINT_MAX;
}

void main( void )
{
   if ( tempsDeVieRestant <= 0.0 )
   {
      // se préparer à produire une valeur un peu aléatoire
      uint seed = uint(temps * 1000.0) + uint(gl_VertexID);
      // faire renaitre la particule au puits
      positionMod = positionPuits;
      tempsDeVieRestantMod = myrandom(seed++) * tempsVieMax; // entre 0 et tempsVieMax secondes
      vitesseMod.x = myrandom(seed++) - 0.5;   // entre -0.5 et 0.5
      vitesseMod.y = myrandom(seed++) - 0.5;  // entre -0.5 et 0.5
      vitesseMod.z = myrandom(seed++);   // entre  0 et 1
   }	
   else
   {
      // avancer la particule
      vitesseMod = vitesse;
      vitesseMod.z -= gravite * dt;
      positionMod = position + vitesseMod * dt;
      tempsDeVieRestantMod = tempsDeVieRestant - dt;
      nombreCollisionMod = nombreCollision;
      
      if(positionMod.x > bMax.x)
      {
		  positionMod.x -= 2 * (positionMod.x - bMax.x);
		  vitesseMod.x *= -1.0;
		  nombreCollisionMod++;
	  }
	  if(positionMod.y > bMax.y)
      {
		  positionMod.y -= 2 * (positionMod.y - bMax.y);
		  vitesseMod.y *= -1.0;
		  nombreCollisionMod++;
	  }
	  if(positionMod.z > bMax.z)
      {
		  positionMod.z -= 2 * (positionMod.z - bMax.z);
		  vitesseMod.z *= 1.0;
		  nombreCollisionMod++;
	  }
	  if(positionMod.x <= bMin.x)
      {
		  positionMod.x += 2 * (bMin.x - positionMod.x);
		  vitesseMod.x *= -1.0;
		  nombreCollisionMod++;
	  }
	  if(positionMod.y <= bMin.y)
      {
		  positionMod.y += 2 * (bMin.y - positionMod.y);
		  vitesseMod.y *= -1.0;
		  nombreCollisionMod++;
	  }
	  if(positionMod.z <= bMin.z)
	  {
		  positionMod.z += 2* (bMin.z - positionMod.z);
		  vitesseMod.z *= -1.0;
		  nombreCollisionMod++;
	  }
   }
}
