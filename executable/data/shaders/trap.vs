/////////////////////////////////////////////////////
//*-----------------------------------------------*//
//| Part of Disco Rock (http://www.maus-games.at) |//
//*-----------------------------------------------*//
//| Released under the zlib License               |//
//| More information available in the readme file |//
//*-----------------------------------------------*//
/////////////////////////////////////////////////////


// shader output
varying vec3 v_v3Strength;


void main()
{
    gl_Position = u_m4ModelViewProj * vec4(a_v3Position, 1.0);

    v_v3Strength.xy = 1.7 * a_v3Position.xy;
    v_v3Strength.z  = 0.5 - a_v3Position.z;
}