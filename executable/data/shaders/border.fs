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
    if(v_av2TexCoord[0].x < u_v2TexSize.x || v_av2TexCoord[0].x > 1.0-u_v2TexSize.x ||
       v_av2TexCoord[0].y < u_v2TexSize.y || v_av2TexCoord[0].y > 1.0-u_v2TexSize.y)
    {
        gl_FragColor = vec4(1.0, 1.0, 1.0, u_v4Color.a);
    }
    else
    {
        gl_FragColor = u_v4Color;
    }
}