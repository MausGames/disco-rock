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
    // compare modified texture coordinates
    if(any(lessThan   (v_av2TexCoord[0],           u_v2TexSize)) ||
       any(greaterThan(v_av2TexCoord[0], vec2(1.0)-u_v2TexSize)))
    {
        // draw plain border
        gl_FragColor = vec4(1.0, 1.0, 1.0, u_v4Color.a);
    }
    else
    {
        // draw interpolated interior
        gl_FragColor = vec4(u_v4Color.rgb * (1.1 - v_av2TexCoord[0].y * 0.6), u_v4Color.a);
    }
}