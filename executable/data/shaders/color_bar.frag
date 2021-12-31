//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Disco Rock (https://www.maus-games.at) |//
//*------------------------------------------------*//
//| Copyright (c) 2013 Martin Mauersics            |//
//| Released under the zlib License                |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////


void FragmentMain()
{
    // draw interpolated color
    gl_FragColor = vec4(u_v4Color.rgb * (1.1 - v_av2TexCoord[0].y * 0.6), u_v4Color.a);
}