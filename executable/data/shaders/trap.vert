//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Disco Rock (https://www.maus-games.at) |//
//*------------------------------------------------*//
//| Copyright (c) 2013 Martin Mauersics            |//
//| Released under the zlib License                |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////


// shader output
varying vec3 v_v3Strength;   // alpha strength vector


void VertexMain()
{
    // transform position
    gl_Position = coreObject3DPositionRaw();

    // calculate alpha strength vector
    v_v3Strength.xy = 1.7 * a_v3RawPosition.xy;
    v_v3Strength.z  = 0.5 - a_v3RawPosition.z;
}