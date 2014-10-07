/////////////////////////////////////////////////////
//*-----------------------------------------------*//
//| Part of Disco Rock (http://www.maus-games.at) |//
//*-----------------------------------------------*//
//| Released under the zlib License               |//
//| More information available in the readme file |//
//*-----------------------------------------------*//
/////////////////////////////////////////////////////


void FragmentMain()
{
    // use texture as alpha map and cut-out the shadow (.a)
    float fAlpha = coreTexture2D(0, v_av2TexCoord[0]).a;
    gl_FragColor = vec4(0.0, 0.0, 0.0, u_v4Color.a * fAlpha);
}