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
attribute float a_fHeight;      // height value
attribute vec4  a_v4Color;      // per-vertex color

// shader output
varying vec3 v_v3Relative;      // position relative to the viewer


void VertexMain()
{
    if(a_fHeight > 1.0)
    {
        // ignore vertices in the air
        gl_Position      = vec4(0.0);
        v_v4VarColor     = vec4(0.0);
        v_v3Relative     = vec3(0.0);
        v_av2TexCoord[0] = vec2(0.0);
        v_av4LightDir[0] = vec4(0.0);
    }
    else
    {
        vec4 v4FullPosition = vec4(a_v2Position.xy, 0.0, 1.0);
        mat4 m4ModelView    = u_m4Camera * u_m4Transform;
        
        // calculate orientation-sign (used when the camera is upside down)
        float fSign = sign(m4ModelView[2][1]);

        // transform position and texture coordinates
        gl_Position      = u_m4ViewProj * (u_m4Transform * v4FullPosition);
        v_av2TexCoord[0] = a_v2Texture;

        // forward per-vertex color
        v_v4VarColor = a_v4Color;
        
#if (_CORE_QUALITY_) < 1

        // calculate only the distance for fast and simple lighting (invert at 0.0)
        float fDistance = dot(vec4(m4ModelView[0][1],
                                   m4ModelView[1][1],
                                   m4ModelView[2][1],
                                   m4ModelView[3][1]), v4FullPosition) * fSign;
        v_v3Relative.y  = 1.15 - fDistance * ((fDistance > 0.0) ? 0.0045 : -0.0225);
        
#else

    #if (_CORE_QUALITY_) > 1
    
        // transform texture coordinates for disco light effect
        v_av2TexCoord[1] = (a_v2Texture + u_v2TexOffset) * 0.6;
        
    #endif

        // calculate full relative position and light direction
        v_v3Relative = (m4ModelView * v4FullPosition).xyz;
        v_av4LightDir[0].xyz = normalize(vec3(-v_v3Relative.x * fSign,
                                               v_v3Relative.y * fSign, 
                                              -v_v3Relative.z));
        v_v3Relative.z *= fSign;

#endif 
    }
}