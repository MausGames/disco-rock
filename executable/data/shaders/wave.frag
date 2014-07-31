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
    gl_FragColor = vec4(1.0, 1.0, 1.0, texture2D(u_as2Texture[0], v_av2TexCoord[0]).r * u_v4Color.a);
}