/////////////////////////////////////////////////////
//*-----------------------------------------------*//
//| Part of Disco Rock (http://www.maus-games.at) |//
//*-----------------------------------------------*//
//| Released under the zlib License               |//
//| More information available in the readme file |//
//*-----------------------------------------------*//
/////////////////////////////////////////////////////


// constant values
const vec3 c_v3CamDir = vec3(0.0, -0.8137, 0.5812);   // normalized camera vector

// shader output
varying float v_fIntensity;   // lighting intensity (Gouraud shading)


void VertexMain()
{
    // transform position and texture coordinates
    gl_Position      = u_m4ViewProj * (u_m4Transform * vec4(a_v3Position * 1.15, 1.0));
    v_av2TexCoord[0] = a_v2Texture;

    // remove top of the glass
    if(a_v3Normal.z > 0.99) v_fIntensity = 0.0;
    else
    {
        // calculate intensity value
        vec3 v3NewNormal = coreObject3DNormalMatrix() * a_v3Normal;
        v_fIntensity     = 1.4 * abs(dot(v3NewNormal, c_v3CamDir)) + 0.56 * abs(a_v3Normal.z);
    }
}