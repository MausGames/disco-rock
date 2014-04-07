/////////////////////////////////////////////////////
//*-----------------------------------------------*//
//| Part of Disco Rock (http://www.maus-games.at) |//
//*-----------------------------------------------*//
//| Released under the zlib License               |//
//| More information available in the readme file |//
//*-----------------------------------------------*//
/////////////////////////////////////////////////////


// shader output
varying float v_fIntensity;


void main()
{
    gl_Position = u_m4ModelViewProj * vec4(a_v3Position, 1.0);

    v_fIntensity = a_v3Position.z * 2.5 + 1.25;
    v_v3ViewDir  = u_m3Normal * a_v3Normal;
}