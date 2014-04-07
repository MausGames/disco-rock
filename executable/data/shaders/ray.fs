/////////////////////////////////////////////////////
//*-----------------------------------------------*//
//| Part of Disco Rock (http://www.maus-games.at) |//
//*-----------------------------------------------*//
//| Released under the zlib License               |//
//| More information available in the readme file |//
//*-----------------------------------------------*//
/////////////////////////////////////////////////////


// normalized camera vector
const vec3 c_v3CamDir = vec3(0.0, -0.813731849, 0.581237018);

// shader input
varying float v_fIntensity;


void main()
{
    float fAlpha = pow(abs(dot(v_v3ViewDir, c_v3CamDir)), 4.0) * v_fIntensity;
    gl_FragColor = vec4(u_v4Color.rgb, u_v4Color.a * fAlpha);
}