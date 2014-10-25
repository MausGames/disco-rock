/////////////////////////////////////////////////////
//*-----------------------------------------------*//
//| Part of Disco Rock (http://www.maus-games.at) |//
//*-----------------------------------------------*//
//| Released under the zlib License               |//
//| More information available in the readme file |//
//*-----------------------------------------------*//
/////////////////////////////////////////////////////


// shader output
varying float v_v1Intensity;   // lighting intensity (semi-Gouraud shading)


void VertexMain()
{
    // transform position 
    gl_Position = coreObject3DPosition();

    // calculate intensity value and transformed normal (in view direction)
    v_v1Intensity = a_v3RawPosition.z * 2.5 + 1.25;
    v_v3ViewDir   = coreQuatApply(u_v4Rotation, a_v3RawNormal);
}