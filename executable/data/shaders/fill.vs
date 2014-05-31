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
    gl_Position   = u_m4ScreenView * vec4(a_v3Position.xy, 1.0, 1.0);
    gl_Position.z = 1.0;
    
#if (_CORE_QUALITY_) >= 1

    vec2 v2TexCoord = a_v2Texture * u_v2TexSize;
    
    v_av2TexCoord[0] =  v2TexCoord + u_v2TexOffset;
    v_av2TexCoord[1] = (v2TexCoord - u_v2TexOffset) * 2.5;
    
#endif
}