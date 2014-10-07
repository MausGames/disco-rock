/////////////////////////////////////////////////////
//*-----------------------------------------------*//
//| Part of Disco Rock (http://www.maus-games.at) |//
//*-----------------------------------------------*//
//| Released under the zlib License               |//
//| More information available in the readme file |//
//*-----------------------------------------------*//
/////////////////////////////////////////////////////


// shader input
varying float v_fIntensity;   // lighting intensity (Gouraud shading)


void FragmentMain()
{
    // draw intensity-modified color
    gl_FragColor = vec4(coreTexture2D(0, v_av2TexCoord[0]).rgb * v_fIntensity, u_v4Color.a);
}