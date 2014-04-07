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
#ifdef GL_ES

    gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);

#else

    float fTextureColor = texture2D(u_as2Texture[0], v_av2TexCoord[0]).b;
    gl_FragColor        = vec4(vec3(u_v4Color.a * fTextureColor * 0.5 * (0.8 - 0.7 * sin(v_av2TexCoord[1].x) * sin(v_av2TexCoord[1].y))), 1.0);
    
#endif
}