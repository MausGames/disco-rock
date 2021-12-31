//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Disco Rock (https://www.maus-games.at) |//
//*------------------------------------------------*//
//| Copyright (c) 2013 Martin Mauersics            |//
//| Released under the zlib License                |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////


void VertexMain()
{
    // transform position (override depth to reduce z-fighting)
    gl_Position    = coreObject3DPositionLow();
    gl_Position.z -= 0.01;

    // transform texture coordinates
    v_av2TexCoord[0] = a_v2LowTexCoord;
}