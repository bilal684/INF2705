// Prénoms, noms et matricule des membres de l'équipe:
// - Prénom1 NOM1 (matricule1)
// - Prénom2 NOM2 (matricule2)
#warning "Écrire les prénoms, noms et matricule des membres de l'équipe dans le fichier et commenter cette ligne"

#include <iostream>
#include "inf2705.h"

// variables pour l'utilisation des nuanceurs
ProgNuanceur progNuanceurBase;  // le programme de nuanceurs de base

// matrices de du pipeline graphique
MatricePipeline matrModel;
MatricePipeline matrVisu;
MatricePipeline matrProj;

bool afficheAxes = false;      // indique si on affiche les axes
bool changer = false;
#if 1
// pour un W
float p1[3] = { -4.0,  2.0,  0.0 };
float p2[3] = { -3.0, -3.0,  0.0 };
float p3[3] = { -1.0, -3.0,  0.0 };
float p4[3] = {  0.0,  0.0,  0.0 };
float p5[3] = {  1.0, -3.0,  0.0 };
float p6[3] = {  3.0, -3.0,  0.0 };
float p7[3] = {  4.0,  2.0,  0.0 };
#else
// pour une flèche (Voir apprentissage supplémentaire)
float p1[3] = { -3.0,  1.0,  0.0 };
float p2[3] = { -3.0, -1.0,  0.0 };
float p3[3] = {  0.0, -1.0,  0.0 };
float p4[3] = { -0.5, -2.5,  0.0 };
float p5[3] = {  3.0,  0.0,  0.0 };
float p6[3] = { -0.5,  2.5,  0.0 };
float p7[3] = {  0.0,  1.0,  0.0 };
#endif

void chargerNuanceurs()
{
   // charger le nuanceur de base
   progNuanceurBase.creer( );
   progNuanceurBase.attacherSourceSommets( "nuanceurSommetsBase.glsl" );
   progNuanceurBase.attacherSourceFragments( "nuanceurFragmentsBase.glsl" );
   progNuanceurBase.lier( );
}

void initialiser()
{
   // donner la couleur de fond
   glClearColor( 0.0, 0.0, 0.0, 1.0 );

   // activer le mélange de couleur pour bien voir les possibles plis à l'affichage
   glEnable( GL_BLEND );
   glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

   // charger les nuanceurs
   chargerNuanceurs();

   glPointSize( 3.0 );
}

void conclure()
{
}

void FenetreTP::afficherScene()
{
   // effacer l'ecran et le tampon de profondeur
   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

#if 0
   // définir le pipeline graphique
   // VERSION OpenGL 2.1
   glMatrixMode( GL_PROJECTION );
   glLoadIdentity();
   glOrtho( -12, 12, -8, 8, -10, 10 ); // la taille de la fenêtre

   glMatrixMode( GL_MODELVIEW );
   glLoadIdentity();

#else
   // définir le pipeline graphique
   // VERSION OpenGL 4.x
   progNuanceurBase.utiliser( );

   // définir le pipeline graphique
   matrProj.Ortho( -12, 12, -8, 8, -10, 10 ); // la taille de la fenêtre
   progNuanceurBase.assignerUniformMatrix4fv( "matrProj", matrProj );

   matrVisu.LoadIdentity();
   progNuanceurBase.assignerUniformMatrix4fv( "matrVisu", matrVisu );

   matrModel.LoadIdentity();
   progNuanceurBase.assignerUniformMatrix4fv( "matrModel", matrModel );

   // Attention: Redéfinir "glTranslatef()" ici afin de ne rien changer ensuite
#define glTranslatef( X, Y, Z )                                         \
   {                                                                    \
      matrModel.Translate( (X), (Y), (Z) );                             \
      progNuanceurBase.assignerUniformMatrix4fv( "matrModel", matrModel ); \
   }

#endif

   // afficher les axes
   if ( afficheAxes ) FenetreTP::afficherAxes();

   // ...

   // exemple: utiliser du rouge opaque
   glColor3f( 1, 0, 0 );
   // mieux encore: utiliser du rouge, mais avec un alpha de 0.8 plutôt que 1.0 pour bien voir les possibles plis à l'affichage
   glColor4f( 1, 0, 0, 0.8 );

   // la fenêtre varie en X de -12 à 12 et en Y de -8 à 8
   glColor3f( 1, 1, 1 );

   // à modifier  ...
	glBegin( GL_LINES );
	{
		glVertex3f(-12.0,0.0,0.0);
		glVertex3f(12.0,0.0,0.0);
		glVertex3f(-4.0,8.0,0.0);
		glVertex3f(-4.0,-8.0,0.0);
		glVertex3f(4.0,8.0,0.0);
		glVertex3f(4.0,-8.0,0.0);
	}
	glEnd();
#if 1	

	glTranslatef( -8.0, 4.0, 0.0 );
	glColor4f( 1, 0, 0, 0.5 );
	glBegin(GL_QUADS);
	{
		glVertex3fv(p1);
		glVertex3fv(p2);
		glVertex3fv(p3);
		glVertex3fv(p4);
		glVertex3fv(p4);
		glVertex3fv(p5);
		glVertex3fv(p6);
		glVertex3fv(p7);
		glVertex3fv(p1);
		glVertex3fv(p4);
		glVertex3fv(p7);
		glVertex3fv(p1);		
	}
	glEnd();

	glTranslatef( 8.0, 0.0, 0.0 );
	glColor4f( 0, 1, 0, 0.5 );
	glBegin(GL_QUAD_STRIP);
	{
		glVertex3fv(p2);
		glVertex3fv(p3);
		glVertex3fv(p1);
		glVertex3fv(p4);
		glVertex3fv(p7);
		glVertex3fv(p4);
		glVertex3fv(p6);
		glVertex3fv(p5);
	}
	glEnd();

	glTranslatef( 8.0, 0.0, 0.0 );
	glColor4f( 0, 0, 1, 0.5 );
	glBegin(GL_TRIANGLES);
	{
		glVertex3fv(p1);
		glVertex3fv(p2);
		glVertex3fv(p3);
		glVertex3fv(p1);
		glVertex3fv(p3);
		glVertex3fv(p4);
		glVertex3fv(p1);
		glVertex3fv(p4);
		glVertex3fv(p7);
		glVertex3fv(p4);
		glVertex3fv(p5);
		glVertex3fv(p7);
		glVertex3fv(p5);
		glVertex3fv(p6);
		glVertex3fv(p7);
	}
	glEnd();

	glTranslatef( -16.0, -8.0, 0.0 );
	glColor4f( 0, 1, 1, 0.5 );
	glBegin(GL_TRIANGLE_STRIP);
	{
		glVertex3fv(p2);
		glVertex3fv(p3);
		glVertex3fv(p1);
		glVertex3fv(p4);
		glVertex3fv(p7);
		glVertex3fv(p5);
		glVertex3fv(p6);
	}
	glEnd();

	glTranslatef(8.0, 0.0, 0.0 );
	glColor4f( 1, 0, 1, 0.5 );
	glBegin(GL_POLYGON);
	{
		glVertex3fv(p4);
		glVertex3fv(p5);
		glVertex3fv(p6);
		glVertex3fv(p7);
		glVertex3fv(p1);
		glVertex3fv(p2);
		glVertex3fv(p3);
		glVertex3fv(p4);
	}
	glEnd();

	glTranslatef(8.0, 0.0, 0.0 );
	glColor4f( 1, 1, 0, 0.5 );
	glBegin(GL_TRIANGLE_FAN);
	{
		glVertex3fv(p4);
		glVertex3fv(p5);
		glVertex3fv(p6);
		glVertex3fv(p7);
		glVertex3fv(p1);
		glVertex3fv(p2);
		glVertex3fv(p3);
	}
	glEnd();
#else

	glTranslatef( -8.0, 4.0, 0.0 );
	glColor4f( 1, 0, 0, 0.5 );
	glBegin(GL_QUADS);
	{
		glVertex3fv(p5);
		glVertex3fv(p6);
		glVertex3fv(p7);
		glVertex3fv(p1);
		glVertex3fv(p5);
		glVertex3fv(p1);
		glVertex3fv(p2);
		glVertex3fv(p3);
		glVertex3fv(p5);
		glVertex3fv(p3);
		glVertex3fv(p4);
		glVertex3fv(p5);		
	}
	glEnd();

	glTranslatef( 8.0, 0.0, 0.0 );
	glColor4f( 0, 1, 0, 0.5 );
	glBegin(GL_QUAD_STRIP);
	{
		glVertex3fv(p4);
		glVertex3fv(p5);
		glVertex3fv(p3);
		glVertex3fv(p5);
		glVertex3fv(p2);
		glVertex3fv(p5);
		glVertex3fv(p1);
		glVertex3fv(p5);
		glVertex3fv(p7);
		glVertex3fv(p5);
		glVertex3fv(p6);
		glVertex3fv(p5);
	}
	glEnd();

	glTranslatef( 8.0, 0.0, 0.0 );
	glColor4f( 0, 0, 1, 0.5 );
	glBegin(GL_TRIANGLES);
	{
		glVertex3fv(p1);
		glVertex3fv(p2);
		glVertex3fv(p7);
		glVertex3fv(p2);
		glVertex3fv(p3);
		glVertex3fv(p7);
		glVertex3fv(p7);
		glVertex3fv(p5);
		glVertex3fv(p6);
		glVertex3fv(p3);
		glVertex3fv(p5);
		glVertex3fv(p7);
		glVertex3fv(p4);
		glVertex3fv(p5);
		glVertex3fv(p3);
	}
	glEnd();

	glTranslatef( -16.0, -8.0, 0.0 );
	glColor4f( 0, 1, 1, 0.5 );
	glBegin(GL_TRIANGLE_STRIP);
	{
		glVertex3fv(p6);
		glVertex3fv(p7);
		glVertex3fv(p5);
		glVertex3fv(p1);
		glVertex3fv(p2);
		glVertex3fv(p2);
		glVertex3fv(p3);
		glVertex3fv(p5);
		glVertex3fv(p4);
	}
	glEnd();

	glTranslatef(8.0, 0.0, 0.0 );
	glColor4f( 1, 0, 1, 0.5 );
	glBegin(GL_POLYGON);
	{
		glVertex3fv(p5);
		glVertex3fv(p6);
		glVertex3fv(p7);
		glVertex3fv(p1);
		glVertex3fv(p2);
		glVertex3fv(p3);
		glVertex3fv(p4);
		glVertex3fv(p5);
	}
	glEnd();

	glTranslatef(8.0, 0.0, 0.0 );
	glColor4f( 1, 1, 0, 0.5 );
	glBegin(GL_TRIANGLE_FAN);
	{
		glVertex3fv(p5);
		glVertex3fv(p6);
		glVertex3fv(p7);
		glVertex3fv(p1);
		glVertex3fv(p2);
		glVertex3fv(p3);
		glVertex3fv(p4);
	}
	glEnd();






#endif


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
   case TP_g :
	if (changer)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		changer = false;
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);   
		changer = true; 
	}
   default:
      std::cout << " touche inconnue : " << (char) touche << std::endl;
      break;
   }
}

void FenetreTP::sourisClic( int button, int state, int x, int y )
{
}

void FenetreTP::sourisWheel( int x, int y )
{
}

void FenetreTP::sourisMouvement( int x, int y )
{
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
