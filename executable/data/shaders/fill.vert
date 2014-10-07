/////////////////////////////////////////////////////
//*-----------------------------------------------*//
//| Part of Disco Rock (http://www.maus-games.at) |//
//*-----------------------------------------------*//
//| Released under the zlib License               |//
//| More information available in the readme file |//
//*-----------------------------------------------*//
/////////////////////////////////////////////////////


void VertexMain()
{
    // transform position with maximum z-value to override depth testing
    gl_Position   = coreObject2DPosition();
    gl_Position.z = 1.0;
    
#if (_CORE_QUALITY_) > 1

    // transform texture coordinates
    vec2 v2TexCoord  = a_v2Texture * u_v2TexSize;
    v_av2TexCoord[0] =  v2TexCoord + u_v2TexOffset;
    v_av2TexCoord[1] = (v2TexCoord - u_v2TexOffset) * 0.8 * PI;
    
#endif
}