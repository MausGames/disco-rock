//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Disco Rock (https://www.maus-games.at) |//
//*------------------------------------------------*//
//| Copyright (c) 2013 Martin Mauersics            |//
//| Released under the zlib License                |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////


// shader input
varying float v_v1Intensity;   // lighting intensity (Gouraud shading)


void FragmentMain()
{
    // draw intensity-modified color
    gl_FragColor = vec4(coreTexture2D(0, v_av2TexCoord[0]).rgb * v_v1Intensity, u_v4Color.a);
}