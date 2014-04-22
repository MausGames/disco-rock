/////////////////////////////////////////////////////
//*-----------------------------------------------*//
//| Part of Disco Rock (http://www.maus-games.at) |//
//*-----------------------------------------------*//
//| Released under the zlib License               |//
//| More information available in the readme file |//
//*-----------------------------------------------*//
/////////////////////////////////////////////////////


#if (__VERSION__) >= 300   // >= OpenGL 3.3 and OpenGL ES 3.0

    // particle attributes
    in vec3 a_v3DivPosition;
    in vec3 a_v3DivData;
    in uint a_iDivColor;
    
    // shader output
    out vec4 v_v4Color;

    
    void main()
    {
        float fDivScale = a_v3DivData.x;
        float fDivValue = a_v3DivData.z;
    
        vec3 v3Range = u_m3Normal * vec3(a_v3Position.xy * fDivScale, 0.0);
        
        gl_Position      = u_m4ViewProj * vec4(v3Range + a_v3DivPosition, 1.0);
        v_av2TexCoord[0] = a_v2Texture;

        v_v4Color    = coreUnpackUnorm4x8(a_iDivColor);
        v_v4Color.a *= fDivValue;
    }

#else

    // particle uniforms
    uniform vec3 u_v3DivPosition;
    uniform vec3 u_v3DivData;
    uniform vec4 u_v4DivColor;
    
    // shader output
    varying vec4 v_v4Color;

    
    void main()
    {
        float fDivScale = u_v3DivData.x;
        float fDivValue = u_v3DivData.z;
    
        vec3 v3Range = u_m3Normal * vec3(a_v3Position.xy * fDivScale, 0.0);
        
        gl_Position      = u_m4ViewProj * vec4(v3Range + u_v3DivPosition, 1.0);
        v_av2TexCoord[0] = a_v2Texture;

        v_v4Color    = u_v4DivColor;
        v_v4Color.a *= fDivValue;
    }

#endif