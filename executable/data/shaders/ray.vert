/////////////////////////////////////////////////////
//*-----------------------------------------------*//
//| Part of Disco Rock (http://www.maus-games.at) |//
//*-----------------------------------------------*//
//| Released under the zlib License               |//
//| More information available in the readme file |//
//*-----------------------------------------------*//
/////////////////////////////////////////////////////


// shader output
varying float v_fIntensity;   // lighting intensity (semi-Gouraud shading)


void VertexMain()
{
    // transform position 
    gl_Position = coreObject3DPosition();

    // calculate intensity value and transformed normal (in view direction)
    v_fIntensity = a_v3Position.z * 2.5 + 1.25;
    v_v3ViewDir  = coreObject3DNormalMatrix() * a_v3Normal;
}