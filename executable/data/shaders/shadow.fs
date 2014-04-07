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
#ifdef GL_ES

    vec2 v2Diff  = v_av2TexCoord[0] - vec2(0.5);
    gl_FragColor = vec4(0.0, 0.0, 0.0, smoothstep(0.156, 0.141, dot(v2Diff, v2Diff)) * 0.5 * u_v4Color.a);
    
#else

    gl_FragColor = vec4(0.0, 0.0, 0.0, texture2D(u_as2Texture[0], v_av2TexCoord[0]).a * u_v4Color.a);
    
#endif
}