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
#if (_CORE_QUALITY_) < 2

    // draw plain black
    gl_FragColor = vec4(0.05, 0.05, 0.05, 1.0);

#else

    // lookup texture (b = disco lights)
    float v1Texture = coreTexture2D(0, v_av2TexCoord[0]).b;

    // draw color with varying intensity
    vec2 v2Sin   = sin(v_av2TexCoord[1]);
    gl_FragColor = vec4(vec3(u_v4Color.a * v1Texture * (1.0 - v2Sin.x * v2Sin.y) + 0.02), 1.0);

#endif
}