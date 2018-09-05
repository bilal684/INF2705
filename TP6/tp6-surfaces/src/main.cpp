// Prénoms, noms et matricule des membres de l'équipe:
// - Said HMANI (1228329)
// - Bilal ITANI (1743175)
//#warning "Écrire les prénoms, noms et matricule des membres de l'équipe dans le fichier et commenter cette ligne"

#include <iostream>
#include "inf2705.h"

// variables pour l'utilisation des nuanceurs
ProgNuanceur prog;      // votre programme de nuanceurs
ProgNuanceur progBase;  // le programme de nuanceurs de base
GLint locVertex;
GLint locVertexBase;
GLint locColorBase;

GLuint vao[2];
GLuint vbo[2];

// matrices de du pipeline graphique
MatricePipeline matrModel;
MatricePipeline matrVisu;
MatricePipeline matrProj;

// variables pour définir le point de vue
double thetaCam = -90.0;        // angle de rotation de la caméra (coord. sphériques)
double phiCam = 80.0;         // angle de rotation de la caméra (coord. sphériques)
double distCam = 10.0;        // distance (coord. sphériques)

// variables d'état
bool enPerspective = false;   // indique si on est en mode Perspective (true) ou Ortho (false)
bool enmouvement = false;     // le modèle est en mouvement/rotation automatique ou non
bool afficheAxes = true;      // indique si on affiche les axes
GLenum modePolygone = GL_FILL; // comment afficher les polygones

GLfloat TessLevelInner = 40;
GLfloat TessLevelOuter = 40;

glm::vec3 posLumi[2] = { glm::vec3( -0.7, -1.0, 1.5 ),
                         glm::vec3(  0.7, -1.0, 1.5 ) }; // position des sources lumineuses
int curLumi = 0;                 // la source lumineuse courante (celle qui peut être déplacée)
bool positionnelle = true;       // la lumière est de type positionnelle?
bool localViewer = false;        // doit-on prendre en compte la position de l'observateur?
bool afficheNormales = false;    // indique si on utilise les normales comme couleurs (utile pour le débogage)

int affichageStereo = 0;         // type d'affichage: mono, stéréo anaglyphe, stéréo double

GLuint textures[9];              // les textures chargées
int indiceTexture = 0;              // indice de la texture à utiliser pour le déplacement
int indiceDiffuse = 0;              // indice de la texture à utiliser pour la couleur
int indiceFonction = 1;              // indice de la fonction à afficher
bool appliquerDepl = true;       // doit-on appliquer le déplacement en Z ?
float facteurZ = 1.0;            // facteur de déplacement initial en Z

// nombre de pixels par pouce
const GLdouble resolution = 100.0; 

struct VueStereo
{
   GLdouble dip;       // la distance interpupillaire
   GLdouble factzoom;  // le facteur de zoom
   GLdouble zavant;    // la position du plan avant du volume de visualisation
   GLdouble zarriere;  // la position du plan arrière du volume de visualisation
   GLdouble zecran;    // la position du plan de l'écran: les objets affichés en avant de ce plan «sortiront» de l'écran
   GLint modele;       // le modèle à afficher
};
struct VueStereo vue;

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
      static int sensZ = +1;
      facteurZ += 0.01 * sensZ;
      if ( facteurZ < -1.0 ) sensZ = +1.0;
      else if ( facteurZ > 1.0 ) sensZ = -1.0;

#if 0
      static int sensPhi = 1;
      thetaCam += 0.4;
      phiCam += 0.7 * sensPhi;
      if ( phiCam < 0.0 || phiCam > 180.0 ) sensPhi = -sensPhi;
      verifierAngles();
#endif
   }
}

void chargerToutesLesTextures()
{
   ChargerTexture( "textures/texture1.bmp", textures[0] );
   ChargerTexture( "textures/texture2.bmp", textures[1] );
   ChargerTexture( "textures/texture3.bmp", textures[2] );
   ChargerTexture( "textures/texture4.bmp", textures[3] );
   ChargerTexture( "textures/texture5.bmp", textures[4] );
   ChargerTexture( "textures/texture6.bmp", textures[5] );
   ChargerTexture( "textures/texture7.bmp", textures[6] );
   ChargerTexture( "textures/texture8.bmp", textures[7] );
   ChargerTexture( "textures/texture9.bmp", textures[8] );
}

void chargerNuanceurs()
{
   // charger le nuanceur de base
   progBase.creer( "base" );
   progBase.attacher( GL_VERTEX_SHADER, "nuanceurSommetsBase.glsl" );
   progBase.attacher( GL_FRAGMENT_SHADER, "nuanceurFragmentsBase.glsl" );
   progBase.lier( );
   locVertexBase = progBase.obtenirAttribLocation( "Vertex" );
   locColorBase = progBase.obtenirAttribLocation( "Color" );

   // charger le nuanceur de ce TP
   std::ostringstream preambule; // ce préambule sera ajouté avant le contenu du fichier du nuanceur
   preambule << "#version 410" << std::endl
             << "#define INDICEFONCTION " << indiceFonction << std::endl
             << "#define INDICETEXTURE " << indiceTexture << std::endl
             << "#define INDICEDIFFUSE " << indiceDiffuse << std::endl
             << "#define APPLIQUERDEPL " << appliquerDepl << std::endl
             << "#define AFFICHENORMALES " << afficheNormales << std::endl;
   prog.creer( "prog" );
   prog.attacher( GL_VERTEX_SHADER, preambule.str(), "nuanceurSommets.glsl" );
   prog.attacher( GL_TESS_CONTROL_SHADER, preambule.str(), "nuanceurTessCtrl.glsl" ); // Partie 1: ACTIVER ces nuanceurs
   prog.attacher( GL_TESS_EVALUATION_SHADER, preambule.str(), "nuanceurTessEval.glsl" ); // Partie 1: ACTIVER ces nuanceurs
   prog.attacher( GL_GEOMETRY_SHADER, preambule.str(), "nuanceurGeometrie.glsl" );
   prog.attacher( GL_FRAGMENT_SHADER, preambule.str(), "nuanceurFragments.glsl" );
   prog.lier( );
   locVertex = prog.obtenirAttribLocation( "Vertex" );
}

void initialiser()
{
   // donner la couleur de fond
   glClearColor( 0.2, 0.2, 0.2, 1.0 );

   // activer les etats openGL
   glEnable( GL_DEPTH_TEST );

   vue.dip = 0.80;
   vue.factzoom = 1.0;
   vue.zavant = 4.0;
   vue.zarriere = 20.0;
   vue.zecran = 10.0;
   vue.modele = 1;

   // charger les nuanceurs
   chargerNuanceurs();

   // charger les textures
   chargerToutesLesTextures();

   // créer le VAO pour conserver les informations
   glGenVertexArrays( 2, vao ); // les VAOs: un pour la surface, l'autre pour les décorations
   glGenBuffers( 2, vbo ); // les VBO pour différents tableaux

   // la surface
   glBindVertexArray( vao[0] );
   {
      // les tableaux
      const GLfloat sommets[] = { -1.0, -1.0,
                                   1.0, -1.0,
                                   1.0,  1.0,
                                  -1.0,  1.0 };
      // créer le VBO pour les sommets
      glBindBuffer( GL_ARRAY_BUFFER, vbo[0] );
      glBufferData( GL_ARRAY_BUFFER, sizeof(sommets), sommets, GL_STATIC_DRAW );
      // faire le lien avec l'attribut du nuanceur de sommets
      glVertexAttribPointer( locVertex, 2, GL_FLOAT, GL_FALSE, 0, (void*)0 );
      glEnableVertexAttribArray(locVertex);
   }

   // les décorations
   glBindVertexArray( vao[1] );
   {
      // créer le VBO pour les sommets
      glBindBuffer( GL_ARRAY_BUFFER, vbo[1] );

      GLfloat coords[] = { 1., 0., 0., 0., 0., 0. };
      glBufferData( GL_ARRAY_BUFFER, sizeof(coords), coords, GL_STATIC_DRAW );

      // faire le lien avec l'attribut du nuanceur de sommets
      glVertexAttribPointer( locVertexBase, 3, GL_FLOAT, GL_FALSE, 0, (void*)0 );
      glEnableVertexAttribArray(locVertexBase);
   }

   glBindVertexArray( 0 );
}

void conclure()
{
   glDeleteBuffers( 2, vbo );
   glDeleteVertexArrays( 2, vao );
}

void definirProjection( int OeilMult, int w, int h ) // 0: mono, -1: oeil gauche, +1: oeil droit
{
   // partie 2: utiliser plutôt Frustum() pour le stéréo
   //matrProj.Perspective( 35.0, (GLdouble) w / (GLdouble) h, vue.zavant, vue.zarriere );
   
   GLdouble decalage = 0.5 * OeilMult * vue.dip;
   GLdouble profondeur = vue.zavant / vue.zecran;

   matrProj.Frustum( (-0.5 * w / resolution - decalage) * profondeur,
                     (0.5 * w / resolution - decalage) * profondeur,
                     (-0.5 * h / resolution) * profondeur,
                     (0.5 * h / resolution) * profondeur,
                     vue.zavant, vue.zarriere );
   matrProj.Translate( -decalage, 0.0, 0.0 );
   prog.assignerUniformMatrix4fv( "matrProj", matrProj );
}

void afficherDecoration()
{
   // remettre le programme de base pour le reste des décorations
   progBase.utiliser( );
   progBase.assignerUniformMatrix4fv( "matrProj", matrProj ); // donner la projection courante

   // dessiner le cube englobant
   glVertexAttrib3f( locColorBase, 1.0, 1.0, 1.0 ); // blanc
   shapesCubeFildefer( 4.0 );

   for ( int i = 0 ; i < 2 ; ++i )
   {
      // dessiner une ligne vers la source lumineuse
      glVertexAttrib3f( locColorBase, 1.0, 1.0, 0.5 ); // jaune
      if ( !positionnelle )
      {
         matrModel.PushMatrix();{
            glm::mat4 a;
            a[0] = glm::vec4( 2*posLumi[i][0],  2*posLumi[i][1],  2*posLumi[i][2], 0.0 );
            a[1] = glm::vec4( 2*posLumi[i][0], -2*posLumi[i][1],  2*posLumi[i][2], 0.0 );
            a[2] = glm::vec4( 2*posLumi[i][0],  2*posLumi[i][1], -2*posLumi[i][2], 0.0 );
            matrModel.setMatr( matrModel.getMatr() * a );
            progBase.assignerUniformMatrix4fv( "matrModel", matrModel );
            glBindVertexArray( vao[1] );
            glDrawArrays( GL_LINES, 0, 2 );
            glBindVertexArray( 0 );
         }matrModel.PopMatrix();
         progBase.assignerUniformMatrix4fv( "matrModel", matrModel );
      }

      // dessiner une sphère à la position de la lumière
      matrModel.PushMatrix();{
         matrModel.Translate( posLumi[i][0], posLumi[i][1], posLumi[i][2] );
         progBase.assignerUniformMatrix4fv( "matrModel", matrModel );
      shapesSphere( 0.1, 10, 10 );
      }matrModel.PopMatrix();
      progBase.assignerUniformMatrix4fv( "matrModel", matrModel );
   }
}

void afficherModele()
{
   prog.utiliser();

   // s'il y a lieu, assigner les textures aux unités de texture
   glActiveTexture( GL_TEXTURE0 );
   glBindTexture( GL_TEXTURE_2D, indiceTexture ? textures[indiceTexture-1] : 0 );
   glActiveTexture( GL_TEXTURE1 );
   glBindTexture( GL_TEXTURE_2D, indiceDiffuse ? textures[indiceDiffuse-1] : 0 );

   // partie 1: définir la position des sources lumineuses
   GLfloat posH = positionnelle ? 1.0 : 0.0;
   prog.assignerUniform4fv( "LightSource[0].position", glm::vec4( posLumi[0][0], posLumi[0][1], posLumi[0][2], posH ) );
   prog.assignerUniform4fv( "LightSource[1].position", glm::vec4( posLumi[1][0], posLumi[1][1], posLumi[1][2], posH ) );

   // partie 1: activer les deux glClipPane limitant le z vers le haut et vers le bas
   glEnable( GL_CLIP_PLANE0 );
   glEnable( GL_CLIP_PLANE1 );

   // afficher la surface (plane)
   glBindVertexArray( vao[0] );
   glPatchParameteri( GL_PATCH_VERTICES, 4 );
   matrModel.PushMatrix();{
      matrModel.Scale( 2.0, 2.0, 2.0 );
      prog.assignerUniformMatrix4fv( "matrModel", matrModel );
      const GLuint connec[] = { 0, 1, 2, 2, 3, 0 };
      glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, connec );
      glDrawArrays( GL_PATCHES, 0, 4 ); // Partie 1: UTILISER des GL_PATCHES plutôt que des GL_TRIANGLES
   }matrModel.PopMatrix();
   prog.assignerUniformMatrix4fv( "matrModel", matrModel );
   glBindVertexArray( 0 );

   glDisable( GL_CLIP_PLANE0 );
   glDisable( GL_CLIP_PLANE1 );

   // afficher les décorations
   afficherDecoration();
}

void FenetreTP::afficherScene()
{
   // effacer l'ecran et le tampon de profondeur
   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

   progBase.utiliser( );

   // définir le pipeline graphique
   definirProjection( 0, largeur_, hauteur_ );
   progBase.assignerUniformMatrix4fv( "matrProj", matrProj );

   matrVisu.LookAt( distCam*cos(glm::radians(thetaCam))*sin(glm::radians(phiCam)),
                    distCam*sin(glm::radians(thetaCam))*sin(glm::radians(phiCam)),
                    distCam*cos(glm::radians(phiCam)),
                    0, 0, 0,
                    0, 0, 1 );
   progBase.assignerUniformMatrix4fv( "matrVisu", matrVisu );

   matrModel.LoadIdentity();
   progBase.assignerUniformMatrix4fv( "matrModel", matrModel );

   // afficher les axes
   if ( afficheAxes ) FenetreTP::afficherAxes( );

   prog.utiliser( );
   prog.assignerUniform1f( "facteurZ", facteurZ );
   prog.assignerUniform1i( "textureDepl", 0 );    // '0' => utilisation de GL_TEXTURE0
   prog.assignerUniform1i( "textureCoul", 1 );    // '1' => utilisation de GL_TEXTURE1
   prog.assignerUniform1f( "TessLevelInner", TessLevelInner ); // Partie 1: ACTIVER ces variables
   prog.assignerUniform1f( "TessLevelOuter", TessLevelOuter ); // Partie 1: ACTIVER ces variables
   prog.assignerUniform1f( "appliquerDepl", appliquerDepl );
   
   // Donner les propriétés du matériau
   prog.assignerUniform4fv( "FrontMaterial.ambient", glm::vec4( 0.2, 0.2, 0.2, 1.0 ) );
   prog.assignerUniform4fv( "FrontMaterial.diffuse", glm::vec4( 1.0, 1.0, 1.0, 1.0 ) );
   prog.assignerUniform4fv( "FrontMaterial.specular", glm::vec4( 1.0, 1.0, 1.0, 1.0 ) );
   prog.assignerUniform4fv( "FrontMaterial.emission", glm::vec4( 0.0, 0.0, 0.0, 1.0 ) );
   prog.assignerUniform1f( "FrontMaterial.shininess", 500.0 );

   // Donner les propriétés des sources lumineuses
   prog.assignerUniform1i( "LightModel.localViewer", localViewer );
   prog.assignerUniform4fv( "LightSource[0].ambient", glm::vec4( 0.2, 0.2, 0.2, 1.0 ) );
   prog.assignerUniform4fv( "LightSource[1].ambient", glm::vec4( 0.2, 0.2, 0.2, 1.0 ) );
   prog.assignerUniform4fv( "LightSource[0].diffuse", glm::vec4( 0.4, 0.4, 0.4, 1.0 ) );
   prog.assignerUniform4fv( "LightSource[1].diffuse", glm::vec4( 0.4, 0.4, 0.4, 1.0 ) );
   prog.assignerUniform4fv( "LightSource[0].specular", glm::vec4( 0.6, 0.6, 0.6, 1.0 ) );
   prog.assignerUniform4fv( "LightSource[1].specular", glm::vec4( 0.6, 0.6, 0.6, 1.0 ) );

   //prog.assignerUniformMatrix4fv( "matrProj", matrProj ); // inutile car on modifie ensuite la projection
   prog.assignerUniformMatrix4fv( "matrVisu", matrVisu );
   prog.assignerUniformMatrix4fv( "matrModel", matrModel );
   glm::mat3 matrNormale = glm::transpose( glm::inverse( glm::mat3( matrVisu.getMatr() * matrModel.getMatr() ) ) );
   prog.assignerUniformMatrix3fv( "matrNormale", matrNormale );

   // partie 2: afficher la surface en mono ou en stéréo

   switch ( affichageStereo )
   {
   case 0: // mono
	  redimensionner(largeur_, hauteur_);
      definirProjection(0, largeur_, hauteur_);
      afficherModele();
      break;

   case 1: // stéréo anaglyphe
      // partie 2: à modifier pour afficher en anaglyphe
      redimensionner(largeur_, hauteur_);
	  definirProjection(-1, largeur_, hauteur_);
      glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE);
      afficherModele();
	  glClear(GL_DEPTH_BUFFER_BIT);
	  prog.utiliser();
	  definirProjection(1, largeur_, hauteur_);
	  glColorMask(GL_FALSE, GL_TRUE, GL_TRUE, GL_TRUE);
	  afficherModele();
	  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
      break;

   case 2: // stéréo double
      // partie 2: à modifier pour afficher en stéréo double
      glViewport(0, 0, 0.5 * largeur_, hauteur_);
	  definirProjection(-1, largeur_, hauteur_);      
      afficherModele();
	  glViewport(0.5 * largeur_, 0, 0.5 * largeur_, hauteur_);
      definirProjection(1, largeur_, hauteur_);
	  afficherModele();
	  break;
   }
}

// fonction de redimensionnement de la fenêtre graphique
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

   case TP_i: // Augmenter le niveau de tessellation interne
      if ( ++TessLevelInner > 64 ) TessLevelInner = 64;
      std::cout << " TessLevelInner=" << TessLevelInner << " TessLevelOuter=" << TessLevelOuter << std::endl;
      glPatchParameteri( GL_PATCH_DEFAULT_INNER_LEVEL, TessLevelInner );
      break;
   case TP_k: // Diminuer le niveau de tessellation interne
      if ( --TessLevelInner < 1 ) TessLevelInner = 1;
      std::cout << " TessLevelInner=" << TessLevelInner << " TessLevelOuter=" << TessLevelOuter << std::endl;
      glPatchParameteri( GL_PATCH_DEFAULT_INNER_LEVEL, TessLevelInner );
      break;

   case TP_o: // Augmenter le niveau de tessellation externe
      if ( ++TessLevelOuter > 64 ) TessLevelOuter = 64;
      std::cout << " TessLevelInner=" << TessLevelInner << " TessLevelOuter=" << TessLevelOuter << std::endl;
      glPatchParameteri( GL_PATCH_DEFAULT_OUTER_LEVEL, TessLevelOuter );
      break;
   case TP_l: // Diminuer le niveau de tessallation externe
      if ( --TessLevelOuter < 1 ) TessLevelOuter = 1;
      std::cout << " TessLevelInner=" << TessLevelInner << " TessLevelOuter=" << TessLevelOuter << std::endl;
      glPatchParameteri( GL_PATCH_DEFAULT_OUTER_LEVEL, TessLevelOuter );
      break;

   case TP_u: // Augmenter les deux niveaux de tessellation
      if ( ++TessLevelOuter > 64 ) TessLevelOuter = 64;
      TessLevelInner = TessLevelOuter;
      std::cout << " TessLevelInner=" << TessLevelInner << " TessLevelOuter=" << TessLevelOuter << std::endl;
      glPatchParameteri( GL_PATCH_DEFAULT_OUTER_LEVEL, TessLevelOuter );
      glPatchParameteri( GL_PATCH_DEFAULT_INNER_LEVEL, TessLevelInner );
      break;
   case TP_j: // Diminuer les deux niveaux de tessellation
      if ( --TessLevelOuter < 1 ) TessLevelOuter = 1;
      TessLevelInner = TessLevelOuter;
      std::cout << " TessLevelInner=" << TessLevelInner << " TessLevelOuter=" << TessLevelOuter << std::endl;
      glPatchParameteri( GL_PATCH_DEFAULT_OUTER_LEVEL, TessLevelOuter );
      glPatchParameteri( GL_PATCH_DEFAULT_INNER_LEVEL, TessLevelInner );
      break;

   case TP_MINUS: // Moduler l'effet du déplacement
      facteurZ -= 0.01;
      std::cout << " facteurZ=" << facteurZ << std::endl;
      break;

   case TP_PLUS: // Moduler l'effet du déplacement
   case TP_EQUALS:
      facteurZ += 0.01;
      std::cout << " facteurZ=" << facteurZ << std::endl;
      break;

   case TP_HOME: // Sélectionner l'autre source lumineuse
      curLumi = 1 - curLumi;
      std::cout << " curLumi=" << curLumi << std::endl;
      break;
   case TP_UP: // Déplacer la source lumineuse en X
      posLumi[curLumi][0] -= 0.1;
      break;
   case TP_DOWN: // Déplacer la source lumineuse en X
      posLumi[curLumi][0] += 0.1;
      break;
   case TP_LEFT: // Déplacer la source lumineuse en Y
      posLumi[curLumi][1] -= 0.1;
      break;
   case TP_RIGHT: // Déplacer la source lumineuse en Y
      posLumi[curLumi][1] += 0.1;
      break;
   case TP_PAGEDOWN: // Déplacer la source lumineuse en Z
      posLumi[curLumi][2] -= 0.1;
      break;
   case TP_PAGEUP: // Déplacer la source lumineuse en Z
      posLumi[curLumi][2] += 0.1;
      break;

   case TP_p: // Permuter lumière positionnelle ou directionnelle
      positionnelle = !positionnelle;
      std::cout << " positionnelle=" << positionnelle << std::endl;
      break;

   case TP_w: // Permuter l'utilisation de l'observateur local
      localViewer = !localViewer;
      std::cout << " localViewer=" << localViewer << std::endl;
      break;

   case TP_s: // Varier le type d'affichage stéréo: mono, stéréo anaglyphe, stéréo double
      if ( ++affichageStereo > 2 ) affichageStereo = 0;
      std::cout << " affichageStereo=" << affichageStereo << std::endl;
      break;

   case TP_0: // Revenir à la surface de base (ne pas utiliser de textures)
      posLumi[0] = glm::vec3( -0.7, -1.0, 1.5 );
      posLumi[1] = glm::vec3(  0.7, -1.0, 1.5 );
      indiceFonction = indiceTexture = indiceDiffuse = 0;
      std::cout << " indiceFonction=" << indiceFonction << " indiceTexture=" << indiceTexture << " indiceDiffuse=" << indiceDiffuse << std::endl;
      chargerNuanceurs(); // pour le #define
      break;

   case TP_f: // Varier la fonction mathématique utilisée pour le déplacement
      if ( ++indiceFonction > 6 ) indiceFonction = 0;
      indiceTexture = 0;
      std::cout << " indiceFonction=" << indiceFonction << " indiceTexture=" << indiceTexture << " indiceDiffuse=" << indiceDiffuse << std::endl;
      chargerNuanceurs(); // pour le #define
      break;

   case TP_t: // Varier l'indice de la texture utilisée pour le déplacement
      if ( ++indiceTexture > 9 ) indiceTexture = 0;
      indiceFonction = 0;
      std::cout << " indiceFonction=" << indiceFonction << " indiceTexture=" << indiceTexture << " indiceDiffuse=" << indiceDiffuse << std::endl;
      chargerNuanceurs(); // pour le #define
      break;

   case TP_c: // Varier l'indice de la texture utilisée pour la composante diffuse de la couleur
      if ( ++indiceDiffuse > 9 ) indiceDiffuse = 0;
      std::cout << " indiceFonction=" << indiceFonction << " indiceTexture=" << indiceTexture << " indiceDiffuse=" << indiceDiffuse << std::endl;
      chargerNuanceurs(); // pour le #define
      break;

   case TP_e: // Varier l'indice de la texture utilisée pour la couleur ET le déplacement
      if ( ++indiceTexture > 9 ) indiceTexture = 0;
      indiceDiffuse = indiceTexture;
      std::cout << " indiceFonction=" << indiceFonction << " indiceTexture=" << indiceTexture << " indiceDiffuse=" << indiceDiffuse << std::endl;
      chargerNuanceurs(); // pour le #define
      break;

   case TP_g: // Permuter l'affichage en fil de fer ou plein
      modePolygone = ( modePolygone == GL_FILL ) ? GL_LINE : GL_FILL;
      glPolygonMode( GL_FRONT_AND_BACK, modePolygone );
      break;

   case TP_n: // Utiliser ou non les normales calculées comme couleur (pour le débogage)
      afficheNormales = !afficheNormales;
      chargerNuanceurs(); // pour le #define
      break;

   case TP_z: // Appliquer ou non le déplacement en Z
      appliquerDepl = !appliquerDepl;
      chargerNuanceurs(); // pour le #define
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

// fonction callback pour un clic de souris
int dernierX = 0; // la dernière valeur en X de position de la souris
int dernierY = 0; // la derniere valeur en Y de position de la souris
static enum { deplaceCam, deplaceLumProfondeur, deplaceLum } deplace = deplaceCam;
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
      case TP_BUTTON_LEFT: // Déplacer la caméra
         deplace = deplaceCam;
         break;
      case TP_BUTTON_MIDDLE: // Déplacer la lumière en profondeur uniquement
         deplace = deplaceLumProfondeur;
         break;
      case TP_BUTTON_RIGHT: // Déplacer la lumière à la position de la souris (sans changer la profondeur)
         deplace = deplaceLum;
         break;
      }
      if ( deplace != deplaceCam )
      {
         glm::mat4 VM = matrVisu.getMatr()*matrModel.getMatr();
         glm::mat4 P = matrProj.getMatr();
         glm::vec4 cloture( 0, 0, largeur_, hauteur_ );
         glm::vec2 ecranLumi0 = glm::vec2( glm::project( posLumi[0], VM, P, cloture ) );
         glm::vec2 ecranLumi1 = glm::vec2( glm::project( posLumi[1], VM, P, cloture ) );
         glm::vec2 ecranXY( x, hauteur_-y );
         curLumi = ( glm::distance( ecranLumi0, ecranXY ) <
                     glm::distance( ecranLumi1, ecranXY ) ) ? 0 : 1;
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
   facteurZ += 0.01 * sens * y;
   std::cout << " facteurZ=" << facteurZ << std::endl;
}

// fonction de mouvement de la souris
void FenetreTP::sourisMouvement( int x, int y )
{
   if ( pressed )
   {
      int dx = x - dernierX;
      int dy = y - dernierY;
      glm::mat4 VM = matrVisu.getMatr()*matrModel.getMatr();
      glm::mat4 P = matrProj.getMatr();
      glm::vec4 cloture( 0, 0, largeur_, hauteur_ );
      // obtenir les coordonnées d'écran correspondant à la position de la lumière
      glm::vec3 ecranLumi = glm::project( posLumi[curLumi], VM, P, cloture );
      switch ( deplace )
      {
      case deplaceCam: // déplacer la caméra par incrément
         thetaCam -= dx / 3.0;
         phiCam   -= dy / 3.0;
         break;
      case deplaceLumProfondeur:
         // modifier seulement la profondeur de la lumière
         ecranLumi[2] -= dy * 0.001;
         posLumi[curLumi] = glm::unProject( ecranLumi, VM, P, cloture );
         break;
      case deplaceLum:
         // placer la lumière à la nouvelle position (en utilisant la profondeur actuelle)
         glm::vec3 ecranPos( x, hauteur_-y, ecranLumi[2] );
         posLumi[curLumi] = glm::unProject( ecranPos, VM, P, cloture );
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
