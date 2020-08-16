//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Disco Rock (https://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////


// constant values
const vec3 c_v3CamDir = vec3(0.0, -0.8137, 0.5812);   // normalized camera vector


void VertexMain()
{
    // transform position and texture coordinates
    gl_Position      = coreObject3DPositionRaw();
    v_av2TexCoord[0] = a_v2RawTexCoord;
    
    // transform lighting properties
    mat3 TBN = coreTangentSpaceMatrix();
    v_av4LightDir[0].xyz = TBN * c_v3CamDir;
}