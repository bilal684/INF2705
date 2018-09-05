//#version 410

layout(triangles) in;
layout(triangle_strip, max_vertices = 4) out;

uniform mat4 matrModel;
uniform mat4 matrVisu;
uniform mat4 matrProj;

in Attribs {
   vec3 normale, lumiDir[2], obsVec;
   vec2 texCoord;
} AttribsIn[];

out Attribs {
   vec3 normale, lumiDir[2], obsVec;
   vec2 texCoord;
} AttribsOut;

void main()
{
   // émettre les sommets
   for ( int i = 0 ; i < gl_in.length() ; ++i )
   {
      gl_Position = gl_in[i].gl_Position;
      gl_ClipDistance[0] = gl_in[i].gl_ClipDistance[0];
      gl_ClipDistance[1] = gl_in[i].gl_ClipDistance[1];
      AttribsOut.normale = AttribsIn[i].normale;
      AttribsOut.lumiDir[0] = AttribsIn[i].lumiDir[0];
      AttribsOut.lumiDir[1] = AttribsIn[i].lumiDir[1];
      AttribsOut.obsVec = AttribsIn[i].obsVec;
      AttribsOut.texCoord = AttribsIn[i].texCoord;
      EmitVertex();
   }
}
