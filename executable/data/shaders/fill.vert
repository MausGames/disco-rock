/////////////////////////////////////////////////////
//*-----------------------------------------------*//
//| Part of Disco Rock (http://www.maus-games.at) |//
//*-----------------------------------------------*//
//| Released under the zlib License               |//
//| More information available in the readme file |//
//*-----------------------------------------------*//
/////////////////////////////////////////////////////


void main()
{
    vec3 v3Position = u_m3ScreenView * vec3(a_v3Position.xy, 1.0);
    gl_Position     = vec4(v3Position.xy, 1.0, v3Position.z);
    
#if (_CORE_QUALITY_) > 1

    vec2 v2TexCoord = a_v2Texture * u_v2TexSize;
    
    v_av2TexCoord[0] =  v2TexCoord + u_v2TexOffset;
    v_av2TexCoord[1] = (v2TexCoord - u_v2TexOffset) * 0.8 * PI;
    
#endif
}