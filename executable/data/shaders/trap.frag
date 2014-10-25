/////////////////////////////////////////////////////
//*-----------------------------------------------*//
//| Part of Disco Rock (http://www.maus-games.at) |//
//*-----------------------------------------------*//
//| Released under the zlib License               |//
//| More information available in the readme file |//
//*-----------------------------------------------*//
/////////////////////////////////////////////////////


// shader input
varying vec3 v_v3Strength;   // alpha strength vector


void FragmentMain()
{
    // ignore upper area
    if(v_v3Strength.z <= 0.0) gl_FragColor = vec4(0.0); // discard;
    else
    {
        // draw smooth glow effect
        float v1Pow3 = v_v3Strength.z * v_v3Strength.z * v_v3Strength.z;
        gl_FragColor = vec4(1.0, 1.0, 1.0, v1Pow3 * length(v_v3Strength.xy) * u_v4Color.a);
    }
}