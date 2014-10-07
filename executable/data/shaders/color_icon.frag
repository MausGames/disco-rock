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
    // use texture as alpha map and cut-out the icon
    float fAlpha = coreTexture2D(0, v_av2TexCoord[0]).r;
    gl_FragColor = vec4(u_v4Color.rgb, u_v4Color.a * fAlpha);
}