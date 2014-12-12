/////////////////////////////////////////////////////
//*-----------------------------------------------*//
//| Part of Disco Rock (http://www.maus-games.at) |//
//*-----------------------------------------------*//
//| Released under the zlib License               |//
//| More information available in the readme file |//
//*-----------------------------------------------*//
/////////////////////////////////////////////////////


void VertexMain()
{
    // transform position (override depth to reduce z-fighting)
    gl_Position    = coreObject3DPosition();
    gl_Position.z -= 0.01;
    
    // transform texture coordinates
    v_av2TexCoord[0] = a_v2RawTexCoord;
}