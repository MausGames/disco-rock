//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Disco Rock (https://www.maus-games.at) |//
//*------------------------------------------------*//
//| Copyright (c) 2013 Martin Mauersics            |//
//| Released under the zlib License                |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#include "engine/data_transform_3d.glsl"


// constant values
const vec3 c_v3CamDir = vec3(0.0, -0.8137, 0.5812);   // normalized camera vector

// shader output
smoothEx varying float v_v1Intensity;                 // lighting intensity (Gouraud shading)


void VertexMain()
{
    // transform position and texture coordinates
    gl_Position      = u_m4ViewProj * vec4(coreObject3DTransform(a_v3RawPosition * 1.15), 1.0);
    v_av2TexCoord[0] = a_v2RawTexCoord;

    // remove top of the glass
    float v1AbsNorm = abs(a_v3RawNormal.z);
    if(v1AbsNorm > 0.99) v_v1Intensity = 0.0;
    else
    {
        // calculate intensity value
        vec3 v3NewNormal = coreQuatApply(u_v4Rotation, a_v3RawNormal);
        v_v1Intensity    = 1.4 * abs(dot(v3NewNormal, c_v3CamDir)) + 0.56 * v1AbsNorm;
    }
}