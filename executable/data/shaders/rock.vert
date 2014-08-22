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


void main()
{
    gl_Position      = u_m4ModelViewProj * vec4(a_v3Position, 1.0);
    v_av2TexCoord[0] = a_v2Texture;
    
    coreDot3Init();
    coreDot3Transform(c_v3CamDir, v_av4LightDir[0].xyz);
}