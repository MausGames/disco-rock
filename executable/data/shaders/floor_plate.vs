/////////////////////////////////////////////////////
//*-----------------------------------------------*//
//| Part of Disco Rock (http://www.maus-games.at) |//
//*-----------------------------------------------*//
//| Released under the zlib License               |//
//| More information available in the readme file |//
//*-----------------------------------------------*//
/////////////////////////////////////////////////////


// shader output
varying vec3 v_v3Relative;


void main()
{
    vec4 v4Position = vec4(a_v3Position, 1.0);
    float fSign     = sign(u_m4ModelView[2][1]);
    
    gl_Position      = u_m4ModelViewProj * v4Position;
    v_av2TexCoord[0] = a_v2Texture + u_v2TexSize;   // add
    
#if (GL_QUALITY) < 1

    v_v3Relative.y = dot(vec4(u_m4ModelView[0][1],
                              u_m4ModelView[1][1],
                              u_m4ModelView[2][1],
                              u_m4ModelView[3][1]), v4Position);
                              
    v_v3Relative.y *= 0.0045 * fSign;
    v_v3Relative.y  = 1.15 - v_v3Relative.y * ((v_v3Relative.y > 0.0) ? 1.0 : -5.0);
        
#else

    #if (GL_QUALITY) > 1
    
        v_av2TexCoord[1] = (a_v2Texture + u_v2TexOffset) * 0.55;
        
    #endif
    
    const vec3 n = vec3( 0.0, 0.0, -1.0);
    const vec3 t = vec3(-1.0, 0.0,  0.0);
    const vec3 b = vec3( 0.0, 1.0,  0.0);
    
    v_v3Relative = (u_m4ModelView * v4Position).xyz;
    
    vec3 v3MathLightDir  = normalize(v_v3Relative);
    v_av4LightDir[0].x   = dot(v3MathLightDir, t) * fSign;
    v_av4LightDir[0].y   = dot(v3MathLightDir, b) * fSign;
    v_av4LightDir[0].z   = dot(v3MathLightDir, n);
    v_av4LightDir[0].xyz = normalize(v_av4LightDir[0].xyz);
    
    v_v3Relative.z *= fSign;
    
#endif
}