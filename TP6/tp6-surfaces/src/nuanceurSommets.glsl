//#version 410

uniform mat4 matrModel;
uniform mat4 matrVisu;
uniform mat4 matrProj;

/////////////////////////////////////////////////////////////////

layout(location=0) in vec4 Vertex;

void main( void )
{
   // transformation standard du sommet
   gl_Position = Vertex;
}
