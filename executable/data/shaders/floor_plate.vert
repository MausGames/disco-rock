//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Disco Rock (https://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////


// constant values
const float c_v1GameHeight = -20.0;   // logical height of the dance floor

// shader output
varying vec3 v_v3Relative;            // position relative to the viewer


void VertexMain()
{
    // calculate orientation-sign (used when the camera is upside down)
    float v1Sign = sign(u_m4Camera[2][1]);

    // transform position and texture coordinates
    vec4 v4NewPosition = vec4(coreObject3DTransformLow(), 1.0);
    gl_Position        = u_m4ViewProj    * v4NewPosition;
    v_av2TexCoord[0]   = a_v2LowTexCoord + u_v2TexSize;   // add size

#if (_CORE_QUALITY_) < 1

    // calculate only the distance for fast and simple lighting (invert at 0.0)
    float v1Distance = dot(vec4(u_m4Camera[0][1],
                                u_m4Camera[1][1],
                                u_m4Camera[2][1],
                                u_m4Camera[1][1]*u_v3Position.y +
                                u_m4Camera[2][1]*c_v1GameHeight +
                                u_m4Camera[3][1]), vec4(a_v2LowPosition, 0.0, 1.0)) * v1Sign;
    v_v3Relative.y  = 1.15 - v1Distance * (step(0.0, v1Distance)*0.027 - 0.0225);

#else

    #if (_CORE_QUALITY_) > 1

        // transform texture coordinates for disco light effect
        v_av2TexCoord[1] = (a_v2LowTexCoord + u_v2TexOffset) * 0.6;

    #endif

    // calculate full relative position and light direction
    v_v3Relative = (u_m4Camera * v4NewPosition).xyz;
    v_av4LightDir[0].xyz = normalize(vec3(-v_v3Relative.x * v1Sign,
                                           v_v3Relative.y * v1Sign,
                                          -v_v3Relative.z));
    v_v3Relative.z *= v1Sign;

#endif
}