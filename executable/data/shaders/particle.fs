/////////////////////////////////////////////////////
//*-----------------------------------------------*//
//| Part of Disco Rock (http://www.maus-games.at) |//
//*-----------------------------------------------*//
//| Released under the zlib License               |//
//| More information available in the readme file |//
//*-----------------------------------------------*//
/////////////////////////////////////////////////////


// shader input
varying vec4 v_v4Color;


void main()
{
    gl_FragColor = vec4(v_v4Color.rgb, texture2D(u_as2Texture[0], v_av2TexCoord[0]).a * v_v4Color.a);
}
