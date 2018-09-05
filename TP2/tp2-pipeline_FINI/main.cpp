// Prénoms, noms et matricule des membres de l'équipe:
// - Bilal ITANI (1743175)
// - Said HMANI (1228329)
//#warning "Écrire les prénoms, noms et matricule des membres de l'équipe dans le fichier et commenter cette ligne"

#include <iostream>
#include "inf2705.h"
#include "teapot_data.h"

// variables pour l'utilisation des nuanceurs
ProgNuanceur progBase;  // le programme de nuanceurs de base

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
GLenum modePolygone = GL_FILL; // comment afficher les polygones

// partie 1:
double angleElevation = 90.0;  // angle de rotation du bras à l'épaule (à la verticale)
double angleEpaule = 46.0;     // angle de rotation du bras à l'épaule (à l'horizontale)
double angleCoude = -90.0;      // angle de rotation du bras au coude (à l'horizontale)
double anglePoignet = 90.0;    // angle de rotation du bras au poignet (à l'horizontale)

// partie 2:
GLuint g_VBOsommets = 0;
GLuint g_VBOconnec = 0;

bool modeLookAt = true;

GLdouble thetaInit = 0., phiInit = 60., distInit = 10.;

// vérifier que les angles ne débordent pas les valeurs permises
void verifierAngles()
{
	//angle poignet : 45 a 135
	//angle coude : -160 a 0
	//angle elevation : 60 a 120
	if(angleElevation < 60.0)
		angleElevation = 60.0;
	else if (angleElevation > 120.0)
		angleElevation = 120.0;
	if(angleCoude < -160.0)
		angleCoude = -160.0;
	else if (angleCoude > 0.0)
		angleCoude = 0.0;
	if (anglePoignet < 45.0)
		anglePoignet = 45.0;
	else if (anglePoignet > 135.0)
		anglePoignet = 135.0;
	if(phiCam > 179.0)
		phiCam = 179.0;
	else if (phiCam < 1.0)
		phiCam = 1.0;
}

void chargerNuanceurs()
{
   // charger le nuanceur de base
   progBase.creer( );
   progBase.attacher( GL_VERTEX_SHADER, "nuanceurSommetsBase.glsl" );
   progBase.attacher( GL_FRAGMENT_SHADER, "nuanceurFragmentsBase.glsl" );
   progBase.lier( );
}

void initialiser()
{
   // positionnement de la caméra: angle et distance de la caméra à la base du bras
   thetaCam = thetaInit;
   phiCam = phiInit;
   distCam = distInit;

   // donner la couleur de fond
   glClearColor( 0.0, 0.0, 0.0, 1.0 );

   // activer les etats openGL
   glEnable( GL_DEPTH_TEST );

   // charger les nuanceurs
   chargerNuanceurs();

   // créer le VBO pour les sommets
   // ...
	glGenBuffers(1, &g_VBOsommets);
	glBindBuffer(GL_ARRAY_BUFFER, g_VBOsommets);
	glBufferData(GL_ARRAY_BUFFER, sizeof(gTeapotSommets), gTeapotSommets, GL_STATIC_DRAW);
	GLint loc = progBase.obtenirAttribLocation("Vertex");
	glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0,0);
	glEnableVertexAttribArray(loc);
   // créer le VBO la connectivité
   // ...
   	glGenBuffers(1, &g_VBOconnec);
   	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_VBOconnec);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(gTeapotConnec), gTeapotConnec, GL_STATIC_DRAW);

}

void conclure()
{
   glDeleteBuffers( 1, &g_VBOsommets );
   glDeleteBuffers( 1, &g_VBOconnec );
}

// (partie 1) Vous devez vous servir de ces deux fonctions (sans les modifier) pour tracer les quadriques.
void afficherCylindre( )
{
   // affiche un cylindre de rayon 1 et de hauteur 1
   const GLint slices = 16, stack = 2;
   glColor3f( 0, 0, 1 );
   shapesSolidCylinder( 1.0, 1.0, slices, stack );
}
void afficherSphere( )
{
   // affiche une sphere de rayon 1
   const GLint slices = 16, stack = 32;
   glColor3f( 1, 0, 0 );
   shapesSolidSphere( 1.0, slices, stack );
}

// (partie 2) Vous modifierez cette fonction pour utiliser les VBOs
void afficherTheiere()
{
   glColor3f( 1.0, 1.0, 0.5 );

   // voici pouvez utiliser temporairement cette fonction pour la première partie du TP, mais vous ferez mieux dans la seconde partie du TP
   //shapesSolidTeapot( );
	glBindBuffer(GL_ARRAY_BUFFER, g_VBOsommets);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_VBOconnec);
	glDrawElements(GL_TRIANGLES, 1024*3, GL_UNSIGNED_INT, 0);
	//defaire le lien
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
}

void afficherBras()
{
   const GLfloat cylLongueur = 2.0;
   const GLfloat sphereRayon = 0.25;

   // toutes les modifications ici ...

   // ajouter une ou des transformations afin de tracer un cylindre de rayon "sphereRayon" et de hauteur "cylLongueur"
	matrModel.Scale( sphereRayon, sphereRayon, cylLongueur );
	progBase.assignerUniformMatrix4fv( "matrModel", matrModel );
	afficherCylindre();
   
   // ajouter une ou des transformations afin de tracer une sphère de rayon "sphereRayon"
    matrModel.LoadIdentity();
    matrModel.Translate(0.0,0.0,cylLongueur);
    matrModel.PushMatrix();
    matrModel.Scale(sphereRayon,sphereRayon,sphereRayon);
    progBase.assignerUniformMatrix4fv( "matrModel", matrModel );
	afficherSphere();

   //2em cylindre
	matrModel.PopMatrix();
	matrModel.Rotate(angleEpaule,0.0,0.0,1.0);
	matrModel.Rotate(angleElevation,0.0,1.0,0.0);
	matrModel.PushMatrix();
	matrModel.Scale( sphereRayon, sphereRayon, cylLongueur );
	progBase.assignerUniformMatrix4fv( "matrModel", matrModel );
	afficherCylindre();
	
	//2em sphere
	matrModel.PopMatrix();
	matrModel.Translate(0.0,0.0,cylLongueur);
	matrModel.PushMatrix();
	matrModel.Scale(sphereRayon,sphereRayon,sphereRayon);
	progBase.assignerUniformMatrix4fv( "matrModel", matrModel );
	afficherSphere();
	
	//3em cylindre
	matrModel.PopMatrix();
	matrModel.Rotate(angleCoude,1.0,0.0,0.0);
	matrModel.PushMatrix();
	matrModel.Scale( sphereRayon,sphereRayon, cylLongueur);
	progBase.assignerUniformMatrix4fv( "matrModel", matrModel );
	afficherCylindre();
	
	//3em sphere
	matrModel.PopMatrix();
	matrModel.Translate(0.0,0.0,cylLongueur);
	matrModel.Rotate(anglePoignet,0.0,1.0,0.0);
	matrModel.PushMatrix();
	matrModel.Scale(sphereRayon,sphereRayon,sphereRayon);
	progBase.assignerUniformMatrix4fv( "matrModel", matrModel );
	afficherSphere();

	// afficher la théière
	matrModel.PopMatrix();
	matrModel.Rotate( 180, 0.0, 1.0, 0.0 );
	matrModel.Rotate( 90, 1.0, 0.0, 0.0 );
	matrModel.Translate( 1.1, -0.6, 0.0 );
	matrModel.Scale(sphereRayon,sphereRayon,sphereRayon);
	progBase.assignerUniformMatrix4fv( "matrModel", matrModel );
	afficherTheiere();
}

void definirCamera()
{
   if ( modeLookAt )
   {
	   /*Rappel coordonnee spherique : 
	    * x = p * sin(phi) * cos(theta)
	    * y = p * sin(phi) * sin(theta)
	    * z = p * cos(phi)
	    * ou p represente la distance.
	    * **attention conversion vers radians est necessaire
	    */
      //matrVisu.LookAt( 0, 15, 5,  0, 0, 0,  0, 0, 1 );
      matrVisu.LookAt(distCam * sin(glm::radians(phiCam)) * cos(glm::radians(thetaCam)), 
      distCam * sin(glm::radians(phiCam)) * sin(glm::radians(thetaCam)), distCam * cos(glm::radians(phiCam)), 0,0,0,0,0,1);
   }
   else
   {
		matrVisu.LoadIdentity( );
		matrVisu.Translate(0.0, 0.0, -distCam);
		matrVisu.Rotate(-phiCam, 1.0, 0.0, 0.0);
		matrVisu.Rotate(270 - thetaCam ,0.0,0.0,1.0);
   }
}

void FenetreTP::afficherScene()
{
   // effacer l'ecran et le tampon de profondeur
   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
   glPolygonMode( GL_FRONT_AND_BACK, modePolygone );

   progBase.utiliser( );

   // définir le pipeline graphique
   matrProj.Perspective( 45.0, (GLdouble) largeur_ / (GLdouble) hauteur_, 0.1, 300.0 );
   progBase.assignerUniformMatrix4fv( "matrProj", matrProj );

   definirCamera();
   progBase.assignerUniformMatrix4fv( "matrVisu", matrVisu );

   matrModel.LoadIdentity();
   progBase.assignerUniformMatrix4fv( "matrModel", matrModel );

   // afficher les axes
   if ( afficheAxes ) FenetreTP::afficherAxes();

   glColor3f( 1., .5, .5 );
   glBegin( GL_QUADS );
   glVertex3f( -4.0,  4.0, 0.0 );
   glVertex3f(  4.0,  4.0, 0.0 );
   glVertex3f(  4.0, -4.0, 0.0 );
   glVertex3f( -4.0, -4.0, 0.0 );
   glEnd();

   afficherBras();
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

   case TP_MINUS:
   case TP_UNDERSCORE:
      distCam += 0.1;
      break;
   case TP_PLUS:
   case TP_EQUALS:
      if ( distCam > 1.0 )
         distCam -= 0.1;
      break;
   case TP_i:
   case TP_r:
      phiCam = phiInit; thetaCam = thetaInit; distCam = distInit;
      break;
   case TP_l:
      modeLookAt = !modeLookAt;
      std::cout << " modeLookAt=" << modeLookAt << std::endl;
      break;
   case TP_g:
      modePolygone = ( modePolygone == GL_FILL ) ? GL_LINE : GL_FILL;
      break;
   case TP_PAGEDOWN:
      angleElevation -= 1.0;
      verifierAngles();
      std::cerr << " angleElevation=" << angleElevation << " angleEpaule=" << angleEpaule << " angleCoude=" << angleCoude << " anglePoignet=" << anglePoignet << std::endl;
      break;
   case TP_PAGEUP:
      angleElevation += 1.0;
      verifierAngles();
      std::cerr << " angleElevation=" << angleElevation << " angleEpaule=" << angleEpaule << " angleCoude=" << angleCoude << " anglePoignet=" << anglePoignet << std::endl;
      break;
   case TP_LEFT:
      angleEpaule -= 2.0;
      verifierAngles();
      std::cerr << " angleElevation=" << angleElevation << " angleEpaule=" << angleEpaule << " angleCoude=" << angleCoude << " anglePoignet=" << anglePoignet << std::endl;
      break;
   case TP_RIGHT:
      angleEpaule += 2.0;
      verifierAngles();
      std::cerr << " angleElevation=" << angleElevation << " angleEpaule=" << angleEpaule << " angleCoude=" << angleCoude << " anglePoignet=" << anglePoignet << std::endl;
      break;
   case TP_DOWN:
      angleCoude -= 2.0;
      verifierAngles();
      std::cerr << " angleElevation=" << angleElevation << " angleEpaule=" << angleEpaule << " angleCoude=" << angleCoude << " anglePoignet=" << anglePoignet << std::endl;
      break;
   case TP_UP:
      angleCoude += 2.0;
      verifierAngles();
      std::cerr << " angleElevation=" << angleElevation << " angleEpaule=" << angleEpaule << " angleCoude=" << angleCoude << " anglePoignet=" << anglePoignet << std::endl;
      break;
   case TP_LEFTBRACKET:
      anglePoignet -= 1.0;
      verifierAngles();
      std::cerr << " angleElevation=" << angleElevation << " angleEpaule=" << angleEpaule << " angleCoude=" << angleCoude << " anglePoignet=" << anglePoignet << std::endl;
      break;
   case TP_RIGHTBRACKET:
      anglePoignet += 1.0;
      verifierAngles();
      std::cerr << " angleElevation=" << angleElevation << " angleEpaule=" << angleEpaule << " angleCoude=" << angleCoude << " anglePoignet=" << anglePoignet << std::endl;
      break;

   default:
      std::cout << " touche inconnue : " << (char) touche << std::endl;
      break;
   }
}

int dernierX, dernierY;
static bool pressed = false;
void FenetreTP::sourisClic( int button, int state, int x, int y )
{
   // button est un parmi { TP_BUTTON_LEFT, TP_BUTTON_MIDDLE, TP_BUTTON_RIGHT }
   // state  est un parmi { TP_PRESSED, DL_RELEASED }
   pressed = ( state == TP_PRESSED );
   if ( pressed )
   {
      dernierX = x;
      dernierY = y;
   }
}

void FenetreTP::sourisWheel( int x, int y )
{
	if(distCam < 1.5)
		distCam = 1.5;
	distCam -= y;
}

void FenetreTP::sourisMouvement( int x, int y )
{
	//Click --> sourisClic est appeler .: dernierX et dernierY sont updater. ici thetaCam et phicam se deplacer de dx et de dy.
	if(pressed)
	{
		thetaCam -= x - dernierX;
		phiCam += y - dernierY;
	}
	verifierAngles();
	dernierX = x;
	dernierY = y;
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
