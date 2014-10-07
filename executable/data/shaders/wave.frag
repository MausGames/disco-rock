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
    // use texture as alpha map and cut-out the wave (.r)
    float fAlpha = coreTexture2D(0, v_av2TexCoord[0]).r;
    gl_FragColor = vec4(1.0, 1.0, 1.0, u_v4Color.a * fAlpha);
}