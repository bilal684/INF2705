// Prénoms, noms et matricule des membres de l'équipe:
// - Prénom1 NOM1 (matricule1)
// - Prénom2 NOM2 (matricule2)
#warning "Écrire les prénoms, noms et matricule des membres de l'équipe dans le fichier et commenter cette ligne"

#include <stdlib.h>
#include <iostream>
#include "inf2705.h"
// 95, 164, 346 pour main
// variables pour l'utilisation des nuanceurs
ProgNuanceur progBase;  // le programme de nuanceurs de base
ProgNuanceur prog;      // votre programme de nuanceurs
GLint locVertex;
GLint loctempsDeVieRestant;
GLint locColor;
GLint locnombreCollision;
GLint locVitesse;
ProgNuanceur progNuanceurRetroaction;  // votre programme de nuanceurs pour la rétroaction
GLint locpositionRetroaction;
GLint locvitesseRetroaction;
GLint loctempsDeVieRestantRetroaction;
GLint locnombreCollisionRetroaction;

GLuint vao[3];
GLuint vbo[4];
GLuint tfo[1];
#define VBO_POS1       0
#define VBO_POS2       1
#define VBO_POSI_MURS  2
#define VBO_CONN_MURS  3

// matrices de du pipeline graphique
MatricePipeline matrModel;
MatricePipeline matrVisu;
MatricePipeline matrProj;

// variables pour définir le point de vue
double thetaCam = 0.0;        // angle de rotation de la caméra (coord. sphériques)
double phiCam = 0.0;          // angle de rotation de la caméra (coord. sphériques)
double distCam = 0.0;         // distance (coord. sphériques)

// variables d'état
bool enmouvement = false;     // le modèle est en mouvement/rotation automatique ou non
bool afficheAxes = true;      // indique si on affiche les axes
GLenum modePolygone = GL_FILL; // comment afficher les polygones

int texnumero = 0;             // numéro de la texture utilisée
GLuint textureETINCELLE = 0;
GLuint textureOISEAU = 0;
GLuint textureLEPRECHAUN = 0;

const unsigned int MAXNPARTICULES = 1000000;
struct Part
{
   GLfloat position[3];          // en unités
   GLfloat vitesse[3];           // en unités/seconde
   GLfloat tempsDeVieRestant;    // en secondes
   GLubyte couleur[4];           // couleur actuelle de la particule
   // (vous pouvez ajouter d'autres éléments, mais il faudra les prévoir dans les varyings)
   GLint nombreCollision;
};
Part part[MAXNPARTICULES];       // le tableau de particules

unsigned int nparticules = 300;  // nombre de particules utilisées (actuellement affichées)
float tempsVieMax = 5.0;         // temps de vie maximal (en secondes)
float temps = 0.0;               // le temps courant dans la simulation (en secondes)
const float dt = 1.0 / 60.0;     // intervalle entre chaque affichage (en secondes)
float gravite = 0.5;             // gravité utilisée dans le calcul de la position de la particule

glm::vec3 positionPuits = glm::vec3( 0.0, 0.0, 0.5 );    // position du puits de particules
const glm::vec3 bMin = glm::vec3( -1.75, -1.0, -0.5 );   // les valeurs minimales de la boite en x,y,z
const glm::vec3 bMax = glm::vec3(  1.75,  1.0,  2.0 );   // les valeurs maximales de la boite en x,y,z
const glm::vec3 bMilieu = glm::vec3(  0.0,  0.0,  0.5 ); // le point visé pour le point de vue

void verifierAngles()
{
   if ( thetaCam > 360.0 )
      thetaCam -= 360.0;
   else if ( thetaCam < 0.0 )
      thetaCam += 360.0;

   const GLdouble MINPHI = 0.01, MAXPHI = 180.0 - 0.01;
   if ( phiCam > MAXPHI )
      phiCam = MAXPHI;
   else if ( phiCam < MINPHI )
      phiCam = MINPHI;
}

// Valeur aléatoire entre 0.0 et 1.0
float myrandom()
{
   return float(rand()) / RAND_MAX;
}

void calculerPhysique( )
{
   if ( enmouvement )
   {
	  // À MODIFIER
      // déplacer en utilisant le nuanceur de rétroaction
      // ...
      /*uniform vec3 bMin, bMax, positionPuits;
uniform float temps, dt, tempsVieMax, gravite;*/
	  progNuanceurRetroaction.utiliser( );
      progNuanceurRetroaction.assignerUniform3fv( "bMin", bMin );
      progNuanceurRetroaction.assignerUniform3fv( "bMax", bMax );
      progNuanceurRetroaction.assignerUniform3fv( "positionPuits", positionPuits );
      progNuanceurRetroaction.assignerUniform1f( "temps", temps );
      progNuanceurRetroaction.assignerUniform1f( "dt", dt );
      progNuanceurRetroaction.assignerUniform1f( "tempsVieMax", tempsVieMax );
      progNuanceurRetroaction.assignerUniform1f( "gravite", gravite );
      // faire les transformations de retour (rÃ©troaction)
      glBindBufferBase( GL_TRANSFORM_FEEDBACK_BUFFER, 0, vbo[VBO_POS2] );

      glBindVertexArray( vao[1] );         // sÃ©lectionner le second VAO
      // se prÃ©parer
      glBindBuffer( GL_ARRAY_BUFFER, vbo[VBO_POS1] );
      glVertexAttribPointer( locpositionRetroaction, 3, GL_FLOAT, GL_FALSE, sizeof(Part), reinterpret_cast<void*>( offsetof(Part,position) ) );
      glVertexAttribPointer( locvitesseRetroaction, 3, GL_FLOAT, GL_FALSE, sizeof(Part), reinterpret_cast<void*>( offsetof(Part,vitesse) ) );
      glVertexAttribPointer( loctempsDeVieRestantRetroaction, 1, GL_FLOAT, GL_FALSE, sizeof(Part), reinterpret_cast<void*>( offsetof(Part,tempsDeVieRestant) ) );
      glVertexAttribIPointer(locnombreCollisionRetroaction, 1, GL_INT, sizeof(Part), reinterpret_cast<void*>( offsetof(Part,nombreCollision)));
      glEnableVertexAttribArray(locpositionRetroaction);
      glEnableVertexAttribArray(locvitesseRetroaction);
      glEnableVertexAttribArray(loctempsDeVieRestantRetroaction);
      glEnableVertexAttribArray(locnombreCollisionRetroaction);

      // Â« dessiner Â»
      glEnable( GL_RASTERIZER_DISCARD );
      glBeginTransformFeedback( GL_POINTS );
      glDrawArrays( GL_POINTS, 0,  nparticules );
      glEndTransformFeedback();
      glDisable( GL_RASTERIZER_DISCARD );

      glBindVertexArray( 0 );              // dÃ©sÃ©lectionner le VAO

      // échanger les deux VBO
      std::swap( vbo[VBO_POS1], vbo[VBO_POS2] );

      // avancer le temps
      temps += dt;

      FenetreTP::VerifierErreurGL("calculerPhysique");
   }
}

void chargerToutesLesTextures()
{
   unsigned char *pixels;
   GLsizei largeur, hauteur;
   if ( ( pixels = ChargerImage( "textures/etincelle.bmp", largeur, hauteur ) ) != NULL )
   {
      glGenTextures( 1, &textureETINCELLE );
      glBindTexture( GL_TEXTURE_2D, textureETINCELLE );
      glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, largeur, hauteur, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
      glBindTexture( GL_TEXTURE_2D, 0 );
      delete[] pixels;
   }
   if ( ( pixels = ChargerImage( "textures/oiseau.bmp", largeur, hauteur ) ) != NULL )
   {
      glGenTextures( 1, &textureOISEAU );
      glBindTexture( GL_TEXTURE_2D, textureOISEAU );
      glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, largeur, hauteur, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
      glBindTexture( GL_TEXTURE_2D, 0 );
      delete[] pixels;
   }
   if ( ( pixels = ChargerImage( "textures/leprechaun.bmp", largeur, hauteur ) ) != NULL )
   {
      glGenTextures( 1, &textureLEPRECHAUN );
      glBindTexture( GL_TEXTURE_2D, textureLEPRECHAUN );
      glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, largeur, hauteur, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
      glBindTexture( GL_TEXTURE_2D, 0 );
      delete[] pixels;
   }
}

void chargerNuanceurs()
{
   // charger le nuanceur de base
   progBase.creer( );
   progBase.attacher( GL_VERTEX_SHADER, "nuanceurSommetsBase.glsl" );
   progBase.attacher( GL_FRAGMENT_SHADER, "nuanceurFragmentsBase.glsl" );
   progBase.lier( );

   // charger le nuanceur
   prog.creer( );
   prog.attacher( GL_VERTEX_SHADER, "nuanceurSommets.glsl" );
   prog.attacher( GL_GEOMETRY_SHADER, "nuanceurGeometrie.glsl" );
   prog.attacher( GL_FRAGMENT_SHADER, "nuanceurFragments.glsl" );
   prog.lier( );
   locVertex = prog.obtenirAttribLocation( "Vertex" );
   locColor = prog.obtenirAttribLocation( "Color" );
   locVitesse = prog.obtenirAttribLocation( "vitesse" );
   loctempsDeVieRestant = prog.obtenirAttribLocation( "tempsDeVieRestant" );
   locnombreCollision = prog.obtenirAttribLocation( "nombreCollision" );

   // charger le nuanceur de rétroaction
   progNuanceurRetroaction.creer( );
   progNuanceurRetroaction.attacher( GL_VERTEX_SHADER, "nuanceurRetroaction.glsl" );
   // À MODIFIER
   const GLchar* vars[] = { "positionMod", "vitesseMod", "tempsDeVieRestantMod", "gl_SkipComponents1", "nombreCollisionMod" };
   glTransformFeedbackVaryings( progNuanceurRetroaction.id(), sizeof(vars)/sizeof(vars[0]), vars, GL_INTERLEAVED_ATTRIBS );
   progNuanceurRetroaction.lier( );
   // À MODIFIER
   locpositionRetroaction = progNuanceurRetroaction.obtenirAttribLocation( "position" );
   locvitesseRetroaction = progNuanceurRetroaction.obtenirAttribLocation( "vitesse" );
   loctempsDeVieRestantRetroaction = progNuanceurRetroaction.obtenirAttribLocation( "tempsDeVieRestant" );
   locnombreCollisionRetroaction = progNuanceurRetroaction.obtenirAttribLocation( "nombreCollision" ); 
}

static GLint Nconnecq = 6*4;

void initialiser()
{
   enmouvement = true;

   // donner la position de la caméra
   thetaCam = 270.0;
   phiCam = 80.0;
   distCam = 5.0;

   // donner la couleur de fond
   glClearColor( 0.3, 0.3, 0.3, 1.0 );

   // activer les états openGL
   glEnable( GL_DEPTH_TEST );
   glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
   glPointSize( 5. );

   // Initialisation des particules
   for ( unsigned int i = 0 ; i < MAXNPARTICULES ; i++ )
   {
      part[i].tempsDeVieRestant = 0.0; // la particules sera initialisée par le nuanceur lors de la rétroaction
      // ... autres valeurs à initialiser ...
      part[i].nombreCollision = 0;
      const GLubyte COULMIN =  50; // valeur minimale d'une composante de couleur lorsque la particule (re)naît
      const GLubyte COULMAX = 250; // valeur maximale d'une composante de couleur lorsque la particule (re)naît
      // interpolation linéaire entre COULMIN et COULMAX
      part[i].couleur[0] = COULMIN + ( COULMAX - COULMIN ) * myrandom();
      part[i].couleur[1] = COULMIN + ( COULMAX - COULMIN ) * myrandom();
      part[i].couleur[2] = COULMIN + ( COULMAX - COULMIN ) * myrandom();
      part[i].couleur[3] = 255;
   }

   // charger les textures
   chargerToutesLesTextures();

   // charger les nuanceurs
   chargerNuanceurs();

   glGenVertexArrays( 3, vao ); // générer deux VAOs
   glGenBuffers( 4, vbo );      // générer les VBOs
   glGenTransformFeedbacks( 1, tfo );

   // Initialiser le vao pour les murs
   progBase.utiliser( );
   glBindVertexArray( vao[2] );
   const GLfloat coo[] = { bMin[0], bMax[1], bMin[2],  bMax[0], bMax[1], bMin[2],  bMax[0], bMin[1], bMin[2],  bMin[0], bMin[1], bMin[2],
                           bMax[0], bMax[1], bMax[2],  bMin[0], bMax[1], bMax[2],  bMin[0], bMax[1], bMin[2],  bMax[0], bMax[1], bMin[2],
                           bMin[0], bMax[1], bMax[2],  bMin[0], bMin[1], bMax[2],  bMin[0], bMin[1], bMin[2],  bMin[0], bMax[1], bMin[2],
                           bMax[0], bMax[1], bMax[2],  bMax[0], bMin[1], bMax[2],  bMax[0], bMin[1], bMin[2],  bMax[0], bMax[1], bMin[2] };
   const GLubyte couleur[] = { 255, 255, 255,  255, 255, 255,  255, 255, 255,  255, 255, 255,
                               255, 255,   0,  255, 255,   0,  255, 255,   0,  255, 255,   0,
                               255,   0, 255,  255,   0, 255,  255,   0, 255,  255,   0, 255,
                                 0, 255, 255,    0, 255, 255,    0, 255, 255,    0, 255, 255 };
   const GLuint connecq[] = { 0+0, 1+0, 2+0, 2+0, 3+0, 0+0,
                              0+4, 1+4, 2+4, 2+4, 3+4, 0+4,
                              0+8, 1+8, 2+8, 2+8, 3+8, 0+8,
                              0+12, 1+12, 2+12, 2+12, 3+12, 0+12 };

   // remplir le VBO
   glBindBuffer( GL_ARRAY_BUFFER, vbo[VBO_POSI_MURS] );
   glBufferData( GL_ARRAY_BUFFER, sizeof(coo)+sizeof(couleur), NULL, GL_STATIC_DRAW );
   glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(coo), coo );
   glBufferSubData( GL_ARRAY_BUFFER, sizeof(coo), sizeof(couleur), couleur );
   // faire le lien avec les attributs du nuanceur de sommets
   GLint locVertexBase = progBase.obtenirAttribLocation( "Vertex" );
   glVertexAttribPointer( locVertexBase, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0 );
   glEnableVertexAttribArray(locVertexBase);
   GLint locColorBase = progBase.obtenirAttribLocation( "Color" );
   glVertexAttribPointer( locColorBase, 3, GL_UNSIGNED_BYTE, GL_TRUE, 0, (GLvoid *)sizeof(coo) );
   glEnableVertexAttribArray(locColorBase);
   glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vbo[VBO_CONN_MURS] );
   glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(connecq), connecq, GL_STATIC_DRAW );
   glBindVertexArray( 0 );

   // Initialiser le vao pour les particules
   // charger le VBO pour les valeurs modifiés
   glBindBuffer( GL_ARRAY_BUFFER, 0 );
   glBindTransformFeedback( GL_TRANSFORM_FEEDBACK, tfo[0] );
   glBindBuffer( GL_ARRAY_BUFFER, vbo[VBO_POS1] );
   glBufferData( GL_ARRAY_BUFFER, sizeof(part), NULL, GL_STREAM_DRAW ); // on ne donne rien sinon la taille

   prog.utiliser( );
   // remplir les VBO et faire le lien avec les attributs du nuanceur de sommets
   glBindVertexArray( vao[0] );
   glBindBuffer( GL_ARRAY_BUFFER, vbo[VBO_POS1] );
   glBufferData( GL_ARRAY_BUFFER, sizeof(part), part, GL_STREAM_DRAW );
   glVertexAttribPointer( locVertex, 3, GL_FLOAT, GL_FALSE, sizeof(Part), reinterpret_cast<void*>( offsetof(Part,position) ) );
   glEnableVertexAttribArray(locVertex);
   glVertexAttribPointer( locColor, 3, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Part), reinterpret_cast<void*>( offsetof(Part,couleur) ) );
   glEnableVertexAttribArray(locColor);
   glVertexAttribPointer( loctempsDeVieRestant, 1, GL_FLOAT, GL_FALSE, sizeof(Part), reinterpret_cast<void*>( offsetof(Part,tempsDeVieRestant) ) );
   glEnableVertexAttribArray(loctempsDeVieRestant);
   glVertexAttribIPointer( locnombreCollision, 1, GL_INT, sizeof(Part), reinterpret_cast<void*>( offsetof(Part,nombreCollision) ) );
   glEnableVertexAttribArray(locnombreCollision);
   glVertexAttribPointer( locVitesse, 3, GL_FLOAT, GL_FALSE, sizeof(Part), reinterpret_cast<void*>( offsetof(Part,vitesse) ) );
   glEnableVertexAttribArray(locVitesse);
   glBindVertexArray( 0 );

   // remplir les VBO pour les valeurs modifiées
   glBindVertexArray( vao[1] );
   glBindBuffer( GL_ARRAY_BUFFER, vbo[VBO_POS2] );
   glBufferData( GL_ARRAY_BUFFER, sizeof(part), part, GL_STREAM_DRAW ); // déjà fait ci-dessus
   glVertexAttribPointer( locVertex, 3, GL_FLOAT, GL_FALSE, sizeof(Part), reinterpret_cast<void*>( offsetof(Part,position) ) );
   glEnableVertexAttribArray(locVertex);
   glVertexAttribPointer( locColor, 3, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Part), reinterpret_cast<void*>( offsetof(Part,couleur) ) );
   glEnableVertexAttribArray(locColor);
   glVertexAttribPointer( loctempsDeVieRestant, 1, GL_FLOAT, GL_FALSE, sizeof(Part), reinterpret_cast<void*>( offsetof(Part,tempsDeVieRestant) ) );
   glEnableVertexAttribArray(loctempsDeVieRestant);
   glVertexAttribIPointer( locnombreCollision, 1, GL_INT, sizeof(Part), reinterpret_cast<void*>( offsetof(Part,nombreCollision) ) );
   glEnableVertexAttribArray(locnombreCollision);
   glVertexAttribPointer( locVitesse, 3, GL_FLOAT, GL_FALSE, sizeof(Part), reinterpret_cast<void*>( offsetof(Part,vitesse) ) );
   glEnableVertexAttribArray(locVitesse);
   glBindVertexArray( 0 );

   // Défaire tous les liens
   glBindBuffer( GL_ARRAY_BUFFER, 0 );

   FenetreTP::VerifierErreurGL("fin de initialiser");
}

void conclure()
{
   glUseProgram( 0 );
   glDeleteVertexArrays( 3, vao );
   glDeleteBuffers( 4, vbo );
}

void FenetreTP::afficherScene()
{
   // effacer l'écran et le tampon de profondeur
   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

   progBase.utiliser( );

   // définir le pipeline graphique
   matrProj.Perspective( 45.0, (GLdouble) largeur_ / (GLdouble) hauteur_, 0.1, 10.0 );
   progBase.assignerUniformMatrix4fv( "matrProj", matrProj );

   matrVisu.LookAt( bMilieu[0]+distCam*cos(glm::radians(thetaCam))*sin(glm::radians(phiCam)),
                    bMilieu[1]+distCam*sin(glm::radians(thetaCam))*sin(glm::radians(phiCam)),
                    bMilieu[2]+distCam*cos(glm::radians(phiCam)),
                    bMilieu[0], bMilieu[1], bMilieu[2],
                    0, 0, 1 );
   progBase.assignerUniformMatrix4fv( "matrVisu", matrVisu );

   matrModel.LoadIdentity();
   progBase.assignerUniformMatrix4fv( "matrModel", matrModel );

   // afficher les axes
   if ( afficheAxes ) FenetreTP::afficherAxes( 0.2 );

   // afficher la boîte
   glBindVertexArray( vao[2] );
   glDrawElements( GL_TRIANGLES, Nconnecq, GL_UNSIGNED_INT, 0 );

   // afficher les particules
   //glActiveTexture( GL_TEXTURE0 ); // activer la texture '0' (valeur de défaut)
   prog.utiliser( );
   prog.assignerUniformMatrix4fv( "matrProj", matrProj );
   prog.assignerUniformMatrix4fv( "matrVisu", matrVisu );
   prog.assignerUniformMatrix4fv( "matrModel", matrModel );
   prog.assignerUniform1i( "laTexture", 0 ); // '0' => utilisation de GL_TEXTURE0
   prog.assignerUniform1i( "texnumero", texnumero );

   glBindVertexArray( vao[0] );
   // refaire le lien avec les attributs du nuanceur de sommets pour le vbo actuellement utilisé
   glBindBuffer( GL_ARRAY_BUFFER, vbo[VBO_POS1] );
   glVertexAttribPointer( locVertex, 3, GL_FLOAT, GL_FALSE, sizeof(Part), reinterpret_cast<void*>( offsetof(Part,position) ) );
   glVertexAttribPointer( loctempsDeVieRestant, 1, GL_FLOAT, GL_FALSE, sizeof(Part), reinterpret_cast<void*>( offsetof(Part,tempsDeVieRestant) ) );
   glVertexAttribPointer( locColor, 3, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Part), reinterpret_cast<void*>( offsetof(Part,couleur) ) );
   glVertexAttribPointer( locnombreCollision, 1, GL_INT, GL_TRUE, sizeof(Part), reinterpret_cast<void*>( offsetof(Part,nombreCollision) ) );
   glVertexAttribPointer( locVitesse, 3, GL_FLOAT, GL_FALSE, sizeof(Part), reinterpret_cast<void*>( offsetof(Part,vitesse) ) );
   switch ( texnumero )
   {
   default: glBindTexture( GL_TEXTURE_2D, textureETINCELLE ); break;
   case 1:  glBindTexture( GL_TEXTURE_2D, textureOISEAU ); break;
   case 2:  glBindTexture( GL_TEXTURE_2D, textureLEPRECHAUN ); break;
   }

   // tracer le résultat de la rétroaction
   // À MODIFIER
   glDrawTransformFeedback( GL_POINTS, tfo[0] );
   glDrawArrays( GL_POINTS, 0, nparticules );

   glBindTexture( GL_TEXTURE_2D, 0 );
   glBindVertexArray( 0 );

   VerifierErreurGL("fin de afficherScene");
}

void FenetreTP::redimensionner( GLsizei w, GLsizei h )
{
   glViewport( 0, 0, w, h );
}

void FenetreTP::clavier( TP_touche touche )
{
   switch ( touche )
   {
   case TP_ESCAPE:
   case TP_q: // Quitter l'application
      quit();
      break;

   case TP_x: // Activer/désactiver l'affichage des axes
      afficheAxes = !afficheAxes;
      std::cout << "// Affichage des axes ? " << ( afficheAxes ? "OUI" : "NON" ) << std::endl;
      break;

   case TP_v: // Recharger les fichiers des nuanceurs et recréer le programme
      chargerNuanceurs();
      std::cout << "// Recharger nuanceurs" << std::endl;
      break;

   case TP_j: // Incrémenter le nombre de particules
   case TP_RIGHTBRACKET:
      {
         unsigned int nparticulesPrec = nparticules;
         nparticules *= 1.2;
         if ( nparticules > MAXNPARTICULES ) nparticules = MAXNPARTICULES;
         std::cout << " nparticules=" << nparticules << std::endl;
         // on met les nouvelles particules au puits
         // (glBindBuffer n'est pas très efficace, mais on ne fait pas ça souvent)
         glBindBuffer( GL_ARRAY_BUFFER, vbo[VBO_POS1] );
         Part *ptr = (Part*) glMapBuffer( GL_ARRAY_BUFFER, GL_WRITE_ONLY );
         for ( unsigned int i = nparticulesPrec ; i < nparticules ; ++i )
            ptr[i].tempsDeVieRestant = 0.0; // la particules sera initialisée par le nuanceur
         glUnmapBuffer( GL_ARRAY_BUFFER );
      }
      break;
   case TP_u: // Décrémenter le nombre de particules
   case TP_LEFTBRACKET:
      nparticules /= 1.2;
      if ( nparticules < 5 ) nparticules = 5;
      std::cout << " nparticules=" << nparticules << std::endl;
      break;

   case TP_UNDERSCORE:
   case TP_MINUS: // Incrémenter la distance de la caméra
      distCam += 0.2;
      std::cout << " distCam=" << distCam << std::endl;
      break;

   case TP_PLUS: // Décrémenter la distance de la caméra
   case TP_EQUALS:
      distCam -= 0.2;
      if ( distCam < 0.4 ) distCam = 0.4;
      std::cout << " distCam=" << distCam << std::endl;
      break;

   case TP_b: // Incrémenter la gravité
      gravite += 0.05;
      std::cout << " gravite=" << gravite << std::endl;
      break;
   case TP_h: // Décrémenter la gravité
      gravite -= 0.05;
      if ( gravite < 0.0 ) gravite = 0.0;
      std::cout << " gravite=" << gravite << std::endl;
      break;

   case TP_l: // Incrémenter la durée de vie maximale
      tempsVieMax += 0.2;
      std::cout << " tempsVieMax=" << tempsVieMax << std::endl;
      break;
   case TP_k: // Décrémenter la durée de vie maximale
      tempsVieMax -= 0.2;
      if ( tempsVieMax < 1.0 ) tempsVieMax = 1.0;
      std::cout << " tempsVieMax=" << tempsVieMax << std::endl;
      break;

   case TP_t: // Changer la texture utilisée: étincelle, oiseau, leprechaun
      if ( ++texnumero > 2 ) texnumero = 0;
      std::cout << " texnumero=" << texnumero << std::endl;
      break;

   case TP_g: // Permuter l'affichage en fil de fer ou plein
      modePolygone = ( modePolygone == GL_FILL ) ? GL_LINE : GL_FILL;
      glPolygonMode( GL_FRONT_AND_BACK, modePolygone );
      break;

   case TP_SPACE: // Mettre en pause ou reprendre l'animation
      enmouvement = !enmouvement;
      break;

   default:
      std::cout << " touche inconnue : " << (char) touche << std::endl;
      imprimerTouches();
      break;
   }
}

int dernierX = 0; // la dernière valeur en X de position de la souris
int dernierY = 0; // la derniere valeur en Y de position de la souris
static enum { deplaceCam, deplacePuits } deplace = deplaceCam;
static bool pressed = false;
void FenetreTP::sourisClic( int button, int state, int x, int y )
{
   pressed = ( state == TP_PRESSED );
   if ( pressed )
   {
      // on vient de presser la souris
      dernierX = x;
      dernierY = y;
      switch ( button )
      {
      case TP_BUTTON_LEFT: // Manipuler la caméra
      case TP_BUTTON_MIDDLE:
         deplace = deplaceCam;
         break;
      case TP_BUTTON_RIGHT: // Déplacer le puits
         deplace = deplacePuits;
         break;
      }
   }
   else
   {
      // on vient de relacher la souris
   }
}

void FenetreTP::sourisWheel( int x, int y )
{
   const int sens = +1;
   distCam -= 0.2 * sens*y;
   if ( distCam < 0.4 ) distCam = 0.4;
}

void FenetreTP::sourisMouvement( int x, int y )
{
   if ( pressed )
   {
      int dx = x - dernierX;
      int dy = y - dernierY;
      switch ( deplace )
      {
      case deplaceCam:
         thetaCam -= dx / 3.0;
         phiCam   -= dy / 3.0;
         break;
      case deplacePuits:
         positionPuits[0] += 0.005 * dx;
         positionPuits[1] -= 0.005 * dy;
         if ( positionPuits[0] < bMin[0] ) positionPuits[0] = bMin[0];
         else if ( positionPuits[0] > bMax[0] ) positionPuits[0] = bMax[0];
         if ( positionPuits[1] < bMin[1] ) positionPuits[1] = bMin[1];
         else if ( positionPuits[1] > bMax[1] ) positionPuits[1] = bMax[1];
         break;
      }

      dernierX = x;
      dernierY = y;

      verifierAngles();
   }
}

int main( int argc, char *argv[] )
{
   // créer une fenêtre
   FenetreTP fenetre( "INF2705 TP" );

   // allouer des ressources et définir le contexte OpenGL
   initialiser();

   bool boucler = true;
   while ( boucler )
   {
      // mettre à jour la physique
      calculerPhysique( );

      // affichage
      fenetre.afficherScene();
      fenetre.swap();

      // récupérer les événements et appeler la fonction de rappel
      boucler = fenetre.gererEvenement();
   }

   // détruire les ressources OpenGL allouées
   conclure();

   return 0;
}
