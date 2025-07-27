//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Disco Rock (https://www.maus-games.at) |//
//*------------------------------------------------*//
//| Copyright (c) 2013 Martin Mauersics            |//
//| Released under the zlib License                |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////


// shader input
smoothEx varying float v_v1Intensity;   // lighting intensity (Gouraud shading)


void FragmentMain()
{
    // remove top of the glass
    if(v_v1Intensity <= 0.0) gl_FragColor = vec4(0.0); // discard;
    else
    {
        // draw intensity-modified color (with opaque drink-label)
        gl_FragColor = vec4(coreTexture2D(0, v_av2TexCoord[0]).rgb * v_v1Intensity, (v_av2TexCoord[0].x < 0.31) ? 1.0 : 0.5);
    }
}