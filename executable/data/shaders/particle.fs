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
#ifdef GL_ES

    vec2 v2Diff  = v_av2TexCoord[0] - vec2(0.5);
    gl_FragColor = vec4(v_v4Color.rgb, (0.9 - 6.0 * dot(v2Diff, v2Diff)) * v_v4Color.a);
    
#else

    gl_FragColor = vec4(v_v4Color.rgb, texture2D(u_as2Texture[0], v_av2TexCoord[0]).a * v_v4Color.a);
    
#endif
}
