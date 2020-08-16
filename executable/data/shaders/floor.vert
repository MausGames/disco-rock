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

// vertex attributes
attribute vec2  a_v2Position;         // position on the grid
attribute float a_v1Height;           // height value
attribute vec4  a_v4Color;            // per-vertex color

// shader output       
varying vec3 v_v3Relative;            // position relative to the viewer


void VertexMain()
{
    if(a_v1Height > 1.0)
    {
        // ignore vertices in the air
        gl_Position  = vec4(0.0);
        v_v4VarColor = vec4(0.0);
    }
    else
    {
        // calculate orientation-sign (used when the camera is upside down)
        float v1Sign = sign(u_m4Camera[2][1]);

        // transform position and texture coordinates
        gl_Position      = u_m4ViewProj * vec4(coreObject3DTransform(vec3(a_v2Position, 0.0)), 1.0);
        v_av2TexCoord[0] = a_v2RawTexCoord;

        // forward per-vertex color
        v_v4VarColor = a_v4Color;
        
#if (_CORE_QUALITY_) < 1

        // calculate only the distance for fast and simple lighting (invert at 0.0)
        float v1Distance = (dot(vec2(u_m4Camera[0][1],
                                     u_m4Camera[1][1]),  a_v2Position)  +
                                     u_m4Camera[1][1]  * u_v3Position.y +
                                     u_m4Camera[2][1]  * c_v1GameHeight + 
                                     u_m4Camera[3][1]) * v1Sign;
        v_v3Relative.y  = 1.15 - v1Distance * (step(0.0, v1Distance)*0.027 - 0.0225);
        
#else

    #if (_CORE_QUALITY_) > 1
    
        // transform texture coordinates for disco light effect
        v_av2TexCoord[1] = (a_v2RawTexCoord + u_v2TexOffset) * 0.6;
        
    #endif

        // calculate full relative position and light direction
        v_v3Relative = (u_m4Camera * vec4(a_v2Position.x, a_v2Position.y + u_v3Position.y, c_v1GameHeight, 1.0)).xyz;
        v_av4LightDir[0].xyz = normalize(vec3(-v_v3Relative.x * v1Sign,
                                               v_v3Relative.y * v1Sign, 
                                              -v_v3Relative.z));
        v_v3Relative.z *= v1Sign;

#endif 
    }
}