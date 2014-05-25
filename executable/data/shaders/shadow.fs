//////////////////////////////////////////////////////////
//*----------------------------------------------------*//
//| Part of the Core Engine (http://www.maus-games.at) |//
//*----------------------------------------------------*//
//| Released under the zlib License                    |//
//| More information available in the readme file      |//
//*----------------------------------------------------*//
//////////////////////////////////////////////////////////


void main()
{
    gl_FragColor = vec4(0.0, 0.0, 0.0, texture2D(u_as2Texture[0], v_av2TexCoord[0]).a * u_v4Color.a);
}