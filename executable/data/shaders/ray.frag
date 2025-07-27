//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Disco Rock (https://www.maus-games.at) |//
//*------------------------------------------------*//
//| Copyright (c) 2013 Martin Mauersics            |//
//| Released under the zlib License                |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////


// constant values
const vec3 c_v3CamDir = vec3(0.0, -0.8137, 0.5812);   // normalized camera vector

// shader input
smoothEx varying float v_v1Intensity;                 // lighting intensity (semi-Gouraud shading)
smoothEx varying vec3  v_v3ViewNorm;                  // view normal


void FragmentMain()
{
    // calculate smooth alpha offset
    float v1Alpha = dot(v_v3ViewNorm, c_v3CamDir);
    v1Alpha *= v1Alpha;   // 2
    v1Alpha *= v1Alpha;   // 4

    // draw smooth intensity-modified color
    gl_FragColor = vec4(u_v4Color.rgb, u_v4Color.a * v1Alpha * v_v1Intensity);
}