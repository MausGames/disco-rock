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

// shader output
varying float v_v1Intensity;   // lighting intensity (Gouraud shading)


void VertexMain()
{
    // transform position and texture coordinates
    gl_Position      = coreObject3DPositionRaw();
    v_av2TexCoord[0] = a_v2RawTexCoord;

    // calculate intensity value
    vec3 v3NewNormal = coreQuatApply(u_v4Rotation, a_v3RawNormal);
    v_v1Intensity    = 1.4 * abs(dot(v3NewNormal, c_v3CamDir)) + 0.28 * abs(v3NewNormal.z);
}