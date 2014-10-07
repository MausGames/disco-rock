/////////////////////////////////////////////////////
//*-----------------------------------------------*//
//| Part of Disco Rock (http://www.maus-games.at) |//
//*-----------------------------------------------*//
//| Released under the zlib License               |//
//| More information available in the readme file |//
//*-----------------------------------------------*//
/////////////////////////////////////////////////////


// shader output
varying vec3 v_v3Strength;   // alpha strength vector


void VertexMain()
{
    // transform position
    gl_Position = coreObject3DPosition();

    // calculate alpha strength vector
    v_v3Strength.xy = 1.7 * a_v3Position.xy;
    v_v3Strength.z  = 0.5 - a_v3Position.z;
}