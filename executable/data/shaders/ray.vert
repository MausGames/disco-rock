//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Disco Rock (https://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////


// shader output
varying float v_v1Intensity;   // lighting intensity (semi-Gouraud shading)
varying vec3  v_v3ViewNorm;    // view normal


void VertexMain()
{
    // transform position 
    gl_Position = coreObject3DPositionRaw();

    // calculate intensity value and transformed normal (in view direction)
    v_v1Intensity = a_v3RawPosition.z * 2.5 + 1.25;
    v_v3ViewNorm  = coreQuatApply(u_v4Rotation, a_v3RawNormal);
}