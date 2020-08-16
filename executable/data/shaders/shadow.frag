//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Disco Rock (https://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////


void FragmentMain()
{
    // use texture as alpha map and cut-out the shadow (.g)
    float v1Alpha = coreTexture2D(0, v_av2TexCoord[0]).g;
    gl_FragColor  = vec4(0.0, 0.0, 0.0, u_v4Color.a * v1Alpha);
}