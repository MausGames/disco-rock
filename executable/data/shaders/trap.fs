/////////////////////////////////////////////////////
//*-----------------------------------------------*//
//| Part of Disco Rock (http://www.maus-games.at) |//
//*-----------------------------------------------*//
//| Released under the zlib License               |//
//| More information available in the readme file |//
//*-----------------------------------------------*//
/////////////////////////////////////////////////////


// shader input
varying vec3 v_v3Strength;


void main()
{
    if(v_v3Strength.z <= 0.0) discard;
    
    gl_FragColor = vec4(1.0, 1.0, 1.0, pow(v_v3Strength.z, 3.0) * length(v_v3Strength.xy) * u_v4Color.a);
}