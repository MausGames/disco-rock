/////////////////////////////////////////////////////
//*-----------------------------------------------*//
//| Part of Disco Rock (http://www.maus-games.at) |//
//*-----------------------------------------------*//
//| Released under the zlib License               |//
//| More information available in the readme file |//
//*-----------------------------------------------*//
/////////////////////////////////////////////////////


// normalized camera vector
const vec3 c_v3CamDir = vec3(0.0, -0.8137, 0.5812);

// shader input
varying float v_fIntensity;


void main()
{
    float fAlpha = dot(v_v3ViewDir, c_v3CamDir);
    fAlpha *= fAlpha;   // 2
    fAlpha *= fAlpha;   // 4
    
    gl_FragColor = vec4(u_v4Color.rgb, u_v4Color.a * fAlpha * v_fIntensity);
}