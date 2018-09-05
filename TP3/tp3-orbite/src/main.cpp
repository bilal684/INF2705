// Prénoms, noms et matricule des membres de l'équipe:
// - Said Hmani (1228329)
// - Bilal Itani (1743175)
//#warning "Écrire les prénoms, noms et matricule des membres de l'équipe dans le fichier et commenter cette ligne"

#include <stdlib.h>
#include <iostream>
#include "inf2705.h"
#include <glm/gtx/io.hpp>
 
// variables pour l'utilisation des nuanceurs
ProgNuanceur prog;      // votre programme de nuanceurs
ProgNuanceur progBase;  // le programme de nuanceurs de base
GLint locVertex;
GLint locColor;
 
// matrices de du pipeline graphique
MatricePipeline matrModel;
MatricePipeline matrVisu;
MatricePipeline matrProj;
 
// variables pour définir le point de vue
double thetaCam = 0.0;        // angle de rotation de la caméra (coord. sphériques)
double phiCam = 0.0;          // angle de rotation de la caméra (coord. sphériques)
double distCam = 0.0;         // distance (coord. sphériques)
 
// variables d'état
bool enPerspective = false;   // indique si on est en mode Perspective (true) ou Ortho (false)
bool enmouvement = false;     // le modèle est en mouvement/rotation automatique ou non
bool afficheAxes = true;      // indique si on affiche les axes
bool coulProfondeur = false;  // indique si on veut colorer selon la profondeur
GLenum modePolygone = GL_FILL; // comment afficher les polygones
 
class CorpsCeleste
{
public:
   float rayon; // le rayon
   float distance; // la distance au soleil
   float rotation; // l'angle actuel de rotation en degrés
   float revolution; // l'angle actuel de révolution en degrés
   float incrRotation; // l'incrément à ajouter à chaque appel de la fonction calculerPhysique en degrés
   float incrRevolution; // l'incrément à ajouter à chaque appel de la fonction calculerPhysique en degrés
   CorpsCeleste( float r, float dist, float rot, float rev, float incrRot, float incrRev ) :
      rayon(r), distance(dist),
      rotation(rot), revolution(rev),
      incrRotation(incrRot), incrRevolution(incrRev)
   { }
};
 
CorpsCeleste Soleil( 4.0, 0.0, 5.0, 0.0, 0.01, 0.0 );
CorpsCeleste Terre( 0.5, 8.0, 20.0, 45.0, 0.5, 0.005 );
CorpsCeleste Lune( 0.2, 1.0, 20.0, 30.0, 0.5, 0.07 );
CorpsCeleste Jupiter( 1.2, 14.0, 10.0, 30.0, 0.03, 0.003 );
CorpsCeleste Europa( 0.2, 1.5, 5.0, 15.0, 0.7, 0.09 );
CorpsCeleste Callisto( 0.3, 2.0, 10.0, 2.0, 0.8, 0.7 );
 
int modele = 1; // le modèle à afficher (1-sphère, 2-cube, 3-théière). On change avec ';'.
 
int vueCourante = 0; // 0 pour la vue exoplanétaire; 1 pour la vue à partir du pôle Nord
 
// partie 1: utiliser un plan de coupe
glm::vec4 planCoupe( 0, 0, 1, 0 ); // équation du plan de coupe
 
// partie 3: utiliser un facteur de réchauffement
float facteurRechauffement = 0.2; // le facteur qui sert à calculer la couleur des pôles (0.0=froid, 1.0=chaud)
 
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
 
void calculerPhysique( )
{
   if ( enmouvement )
   {
      // incrémenter rotation[] et revolution[] pour faire tourner les planètes
      const float deltaTemps = 5.0;
      Soleil.rotation     += deltaTemps * Soleil.incrRotation;
      Soleil.revolution   += deltaTemps * Soleil.incrRevolution;
      Terre.rotation      += deltaTemps * Terre.incrRotation;
      Terre.revolution    += deltaTemps * Terre.incrRevolution;
      Lune.rotation       += deltaTemps * Lune.incrRotation;
      Lune.revolution     += deltaTemps * Lune.incrRevolution;
      Jupiter.rotation    += deltaTemps * Jupiter.incrRotation;
      Jupiter.revolution  += deltaTemps * Jupiter.incrRevolution;
      Europa.rotation     += deltaTemps * Europa.incrRotation;
      Europa.revolution   += deltaTemps * Europa.incrRevolution;
      Callisto.rotation   += deltaTemps * Callisto.incrRotation;
      Callisto.revolution += deltaTemps * Callisto.incrRevolution;
   }
}
 
void chargerNuanceurs()
{
   // charger le nuanceur de base
   progBase.creer( );
   progBase.attacher( GL_VERTEX_SHADER, "nuanceurSommetsBase.glsl" );
   progBase.attacher( GL_FRAGMENT_SHADER, "nuanceurFragmentsBase.glsl" );
   progBase.lier( );
 
   // charger le nuanceur de ce TP
   prog.creer( );
   prog.attacher( GL_VERTEX_SHADER, "nuanceurSommets.glsl" );
   prog.attacher( GL_GEOMETRY_SHADER, "nuanceurGeometrie.glsl" );
   prog.attacher( GL_FRAGMENT_SHADER, "nuanceurFragments.glsl" );
   prog.lier( );
 
   locVertex = prog.obtenirAttribLocation( "Vertex" );
   locColor = prog.obtenirAttribLocation( "Color" );
}
 
void initialiser()
{
   enmouvement = true;
   thetaCam = 60.0;
   phiCam = 75.0;
   distCam = 26.0;
 
   // donner la couleur de fond
   glClearColor( 0.1, 0.1, 0.1, 1.0 );
 
   // activer les etats openGL
   glEnable( GL_DEPTH_TEST );
 
   // charger les nuanceurs
   chargerNuanceurs();
}
 
void conclure()
{
}
 
void observerDeLaTerre( )
{
   // placer le point de vue pour voir de la Terre
   // partie 2: modifs ici ...
	
	matrModel.PushMatrix();
    matrModel.Rotate( Terre.revolution, 0, 0, 1 );
    matrModel.Translate( Terre.distance, 0, 0 );
    matrModel.Rotate(Terre.rotation,0,0,1);
    matrModel.Rotate(90,1,0,0);
    matrModel.Translate(0,Terre.rayon * 1.1, 0);	
    matrVisu.setMatr(glm::inverse(matrModel.getMatr()));
    matrModel.PopMatrix();
}
 
void definirCamera()
{
   if ( vueCourante == 0 )
   {
      // La souris influence le point de vue
      matrVisu.LookAt( distCam*cos(glm::radians(thetaCam))*sin(glm::radians(phiCam)),
                       distCam*sin(glm::radians(thetaCam))*sin(glm::radians(phiCam)),
                       distCam*cos(glm::radians(phiCam)),
                       0, 0, 0,
                       0, 0, 1 );
   }
   else
   {
      // La caméra est sur la Terre et voir passer les autres objets célestes en utilisant l'inverse de la matrice mm
      // partie 2:
      observerDeLaTerre( );
   }
}
 
void afficherCorpsCeleste( GLfloat rayon )
{
   switch ( modele )
   {
   default:
   case 1:
      shapesSolidSphere( rayon, 16, 16 );
      break;
   case 2:
      shapesSolidCube( 2*rayon );
      break;
   // case 3:
   //    matrModel.PushMatrix(); {
   //       matrModel.Scale( 0.4*rayon, 0.4*rayon, 0.4*rayon );
   //       prog.assignerUniformMatrix4fv( "matrModel", matrModel );
   //       shapesSolidTeapot( );
   //    } matrModel.PopMatrix(); prog.assignerUniformMatrix4fv( "matrModel", matrModel );
   //    break;
   }
}
 
void afficherOrbite( GLfloat distance, GLfloat r, GLfloat g, GLfloat b )
{
   glVertexAttrib3f( locColor, r, g, b );
   shapesSolidTorus( 0.1, distance, 4, 64 );
}
 
void afficherQuad( GLfloat alpha ) // le plan qui ferme les solides
{
   glVertexAttrib4f( locColor, 0.0, 1.0, 0.0, alpha );
   // partie 1: modifs ici ...
   glEnable(GL_BLEND);
   glDepthMask(GL_FALSE);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   GLfloat coo[4*3] = {-16.0, 16.0 , -planCoupe[3], -16.0, -16.0, -planCoupe[3], 16.0, -16.0, -planCoupe[3], 16.0, 16.0, -planCoupe[3]};
   GLuint cooConnec[1*4] = {0,3,2,1};
   glVertexAttribPointer( locVertex, 3, GL_FLOAT, GL_FALSE, 0, coo );
   glEnableVertexAttribArray(0);
   glDrawElements(GL_QUADS,sizeof(cooConnec)/sizeof(GLuint), GL_UNSIGNED_INT, cooConnec);
   glDisableVertexAttribArray(0);
   glDepthMask(GL_TRUE);
   glDisable(GL_BLEND);
}
 
void afficherModele()
{
   // afficher le système solaire
   matrModel.PushMatrix( ); {
 
      glVertexAttrib4f( locColor, 1.0, 1.0, 1.0, 1.0 );
      matrModel.PushMatrix( ); { // Terre
         matrModel.Rotate( Terre.revolution, 0, 0, 1 ); // révolution terre autour soleil
         matrModel.Translate( Terre.distance, 0, 0 );
         matrModel.PushMatrix( ); {
            matrModel.Rotate( Terre.rotation, 0, 0, 1 );
            prog.assignerUniformMatrix4fv( "matrModel", matrModel );
            glVertexAttrib3f( locColor, 0.5, 0.5, 1.0 );
            afficherCorpsCeleste( Terre.rayon );
         } matrModel.PopMatrix();
         matrModel.PushMatrix( ); { // Lune
            matrModel.Rotate( Lune.revolution, 0, 0, 1 ); // révolution lune autour terre
            matrModel.Translate( Lune.distance, 0, 0 );
            matrModel.Rotate( Lune.rotation, 0, 0, 1 );
            prog.assignerUniformMatrix4fv( "matrModel", matrModel );
            glVertexAttrib3f( locColor, 0.6, 0.6, 0.6 );
            afficherCorpsCeleste( Lune.rayon );
         } matrModel.PopMatrix();
      } matrModel.PopMatrix();
 
      matrModel.PushMatrix( ); { // Jupiter
         matrModel.Rotate( Jupiter.revolution, 0, 0, 1 ); // révolution jupiter autour soleil
         matrModel.Translate( Jupiter.distance, 0, 0 );
         matrModel.PushMatrix( ); {
            matrModel.Rotate( Jupiter.rotation, 0, 0, 1 );
            prog.assignerUniformMatrix4fv( "matrModel", matrModel );
            glVertexAttrib3f( locColor, 1.0, 0.5, 0.5 );
            afficherCorpsCeleste( Jupiter.rayon );
         } matrModel.PopMatrix();
         matrModel.PushMatrix( ); { // Europa
            matrModel.Rotate( Europa.revolution, 0, 0, 1 ); // révolution europa autour jupiter
            matrModel.Translate( Europa.distance, 0, 0 );
            matrModel.Rotate( Europa.rotation, 0, 0, 1 );
            prog.assignerUniformMatrix4fv( "matrModel", matrModel );
            glVertexAttrib3f( locColor, 0.4, 0.4, 0.8 );
            afficherCorpsCeleste( Europa.rayon );
         } matrModel.PopMatrix();
         matrModel.PushMatrix( ); { // Callisto
            matrModel.Rotate( Callisto.revolution, 0, 0, 1 ); // révolution callisto autour jupiter
            matrModel.Translate( Callisto.distance, 0, 0 );
            matrModel.Rotate( Callisto.rotation, 0, 0, 1 );
            prog.assignerUniformMatrix4fv( "matrModel", matrModel );
            glVertexAttrib3f( locColor, 0.5, 0.5, 0.1 );
            afficherCorpsCeleste( Callisto.rayon );
         } matrModel.PopMatrix();
      } matrModel.PopMatrix();
 
   } matrModel.PopMatrix(); prog.assignerUniformMatrix4fv( "matrModel", matrModel );
 
   // afficher les deux tores pour identifier les orbites de la Terre et de Jupiter
   afficherOrbite( Terre.distance, 0.0, 0.0, 1.0 );
   afficherOrbite( Jupiter.distance, 1.0, 0.0, 0.0 );
 
   // afficher le soleil, un peu transparent ... et sans réchauffement!
   // partie 1: modifs ici ...
   matrModel.PushMatrix( ); 
   { // Soleil
      matrModel.Rotate( Soleil.rotation, 0, 0, 1 );
      glEnable(GL_BLEND);
      glDepthMask(GL_FALSE);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      prog.assignerUniformMatrix4fv( "matrModel", matrModel );
      prog.assignerUniform1f("facteurRechauffement", 1.0);
      glVertexAttrib4f( locColor, 1.0, 1.0, 0.0, 0.5 );
      afficherCorpsCeleste( Soleil.rayon );
      glDepthMask(GL_TRUE);
      glDisable(GL_BLEND);
   } matrModel.PopMatrix(); 
   prog.assignerUniformMatrix4fv( "matrModel", matrModel );
}

void FenetreTP::afficherScene( )
{
   // effacer l'ecran et le tampon de profondeur et le stencil
   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
 
   progBase.utiliser( );
 
   // définir le pipeline graphique
   matrProj.Perspective( 70.0, (GLdouble) largeur_ / (GLdouble) hauteur_, 0.1, 100.0 );
   progBase.assignerUniformMatrix4fv( "matrProj", matrProj );
 
   definirCamera( );
   progBase.assignerUniformMatrix4fv( "matrVisu", matrVisu );
 
   matrModel.LoadIdentity();
   progBase.assignerUniformMatrix4fv( "matrModel", matrModel );
 
   // afficher les axes
   if ( afficheAxes ) FenetreTP::afficherAxes();
 
   // dessiner la scène
   prog.utiliser( );
   prog.assignerUniformMatrix4fv( "matrProj", matrProj );
   prog.assignerUniformMatrix4fv( "matrVisu", matrVisu );
   prog.assignerUniformMatrix4fv( "matrModel", matrModel );
   prog.assignerUniform1f( "facteurRechauffement", facteurRechauffement );
   prog.assignerUniform4fv( "planCoupe", planCoupe );
   prog.assignerUniform1i( "coulProfondeur", coulProfondeur );
 
   // afficher le modèle et tenir compte du stencil et du plan de coupe
   // partie 1: modifs ici ...
   glEnable(GL_CLIP_PLANE0);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS,0,1);
    glStencilOp(GL_INCR,GL_INCR,GL_INCR);
    afficherModele();
    glStencilFunc(GL_EQUAL,1,1);
    glStencilOp(GL_KEEP,GL_KEEP,GL_KEEP);
   // en plus, dessiner le plan en transparence pour bien voir son étendue
   afficherQuad( 0.5 );
   glDisable(GL_CLIP_PLANE0);
   afficherQuad( 0.5 );
   glDisable(GL_STENCIL_TEST);
   afficherQuad( 0.2 );
        
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
   case TP_q:
      quit();
      break;
 
   case TP_x: // permutation de l'affichage des axes
      afficheAxes = !afficheAxes;
      std::cout << "// Affichage des axes ? " << ( afficheAxes ? "OUI" : "NON" ) << std::endl;
      break;
 
   case TP_v: // Recharger les nuanceurs
      chargerNuanceurs();
      std::cout << "// Recharger nuanceurs" << std::endl;
      break;
 
   case TP_SPACE:
      enmouvement = !enmouvement;
      break;
 
   case TP_c:
      facteurRechauffement += 0.05; if ( facteurRechauffement > 1.0 ) facteurRechauffement = 1.0;
      std::cout << " facteurRechauffement=" << facteurRechauffement << " " << std::endl;
      break;
   case TP_f:
      facteurRechauffement -= 0.05; if ( facteurRechauffement < 0.0 ) facteurRechauffement = 0.0;
      std::cout << " facteurRechauffement=" << facteurRechauffement << " " << std::endl;
      break;
 
   case TP_0: // point de vue globale/externe
   case TP_1: // point de vue situé au pôle Nord
   case TP_2: // ... autres point de vue
   case TP_3:
   case TP_4:
   case TP_5:
   case TP_6:
   case TP_7:
   case TP_8:
   case TP_9:
      vueCourante = touche - TP_0;
      break;
 
   case TP_g:
      modePolygone = ( modePolygone == GL_FILL ) ? GL_LINE : GL_FILL;
      glPolygonMode( GL_FRONT_AND_BACK, modePolygone );
      break;
 
   case TP_PLUS:
   case TP_EQUALS:
      distCam--;
      std::cout << " distCam=" << distCam << std::endl;
      break;
 
   case TP_UNDERSCORE:
   case TP_MINUS:
      distCam++;
      std::cout << " distCam=" << distCam << std::endl;
      break;
 
   case TP_RIGHTBRACKET:
   case TP_GREATER:
      planCoupe[3] += 0.1;
      std::cout << " planCoupe[3]=" << planCoupe[3] << std::endl;
      break;
   case TP_LEFTBRACKET:
   case TP_LESS:
      planCoupe[3] -= 0.1;
      std::cout << " planCoupe[3]=" << planCoupe[3] << std::endl;
      break;
 
   case TP_SEMICOLON:
      if ( ++modele > 3 ) modele = 1;
      std::cout << " modele=" << modele << std::endl;
      break;
 
   case TP_p:
      coulProfondeur = !coulProfondeur;
      break;
 
   default:
      std::cout << " touche inconnue : " << (char) touche << std::endl;
      break;
   }
}
 
int dernierX = 0, dernierY = 0;
static bool pressed = false;
void FenetreTP::sourisClic( int button, int state, int x, int y )
{
   pressed = ( state == TP_PRESSED );
   if ( pressed )
      if ( button == TP_BUTTON_LEFT )
      {
         dernierX = x;
         dernierY = y;
      }
}
 
void FenetreTP::sourisWheel( int x, int y )
{
   const int sens = +1;
   planCoupe[3] += 0.02 * sens * y;
   std::cout << " planCoupe[3]=" << planCoupe[3] << std::endl;
}
 
void FenetreTP::sourisMouvement( int x, int y )
{
   if ( pressed )
   {
      int dx = x - dernierX;
      int dy = y - dernierY;
      thetaCam -= dx / 3.0;
      phiCam   -= dy / 3.0;
 
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
