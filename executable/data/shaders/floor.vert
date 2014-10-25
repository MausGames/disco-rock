/////////////////////////////////////////////////////
//*-----------------------------------------------*//
//| Part of Disco Rock (http://www.maus-games.at) |//
//*-----------------------------------------------*//
//| Released under the zlib License               |//
//| More information available in the readme file |//
//*-----------------------------------------------*//
/////////////////////////////////////////////////////


// vertex attributes
attribute vec2  a_v2Position;   // position on the grid
attribute float a_v1Height;     // height value
attribute vec4  a_v4Color;      // per-vertex color

// shader output
varying vec3 v_v3Relative;      // position relative to the viewer


void VertexMain()
{
    if(a_v1Height > 1.0)
    {
        // ignore vertices in the air
        gl_Position      = vec4(0.0);
        v_v4VarColor     = vec4(0.0);
        v_av2TexCoord[0] = vec2(0.0);
        v_av4LightDir[0] = vec4(0.0);
        v_v3Relative     = vec3(0.0);
    }
    else
    {
        mat4 m4ModelView = u_m4Camera * mat4(    u_v3Size.x,            0.0,            0.0, 0.0,
                                                        0.0,     u_v3Size.y,            0.0, 0.0,
                                                        0.0,            0.0,     u_v3Size.z, 0.0,
                                             u_v3Position.x, u_v3Position.y, u_v3Position.z, 1.0);
        
        // calculate orientation-sign (used when the camera is upside down)
        float v1Sign = sign(m4ModelView[2][1]);

        // transform position and texture coordinates
        gl_Position      = u_m4ViewProj * vec4(coreObject3DTransform(vec3(a_v2Position.xy, 0.0)), 1.0);
        v_av2TexCoord[0] = a_v2RawTexCoord;

        // forward per-vertex color
        v_v4VarColor = a_v4Color;
        
#if (_CORE_QUALITY_) < 1

        // calculate only the distance for fast and simple lighting (invert at 0.0)
        float v1Distance = dot(vec4(m4ModelView[0][1],
                                    m4ModelView[1][1],
                                    m4ModelView[2][1],
                                    m4ModelView[3][1]), vec4(a_v2Position.xy, 0.0, 1.0)) * v1Sign;
        v_v3Relative.y  = 1.15 - v1Distance * ((v1Distance > 0.0) ? 0.0045 : -0.0225);
        
#else

    #if (_CORE_QUALITY_) > 1
    
        // transform texture coordinates for disco light effect
        v_av2TexCoord[1] = (a_v2RawTexCoord + u_v2TexOffset) * 0.6;
        
    #endif

        // calculate full relative position and light direction
        v_v3Relative = (m4ModelView * vec4(a_v2Position.xy, 0.0, 1.0)).xyz;
        v_av4LightDir[0].xyz = normalize(vec3(-v_v3Relative.x * v1Sign,
                                               v_v3Relative.y * v1Sign, 
                                              -v_v3Relative.z));
        v_v3Relative.z *= v1Sign;

#endif 
    }
}