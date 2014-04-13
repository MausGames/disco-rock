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

void main()
{
    gl_Position      = u_m4ModelViewProj * vec4(a_v3Position, 1.0);
    v_av2TexCoord[0] = a_v2Texture;
    
    vec3 n = normalize(u_m3Normal * a_v3Normal);
    vec3 t = normalize(u_m3Normal * a_v4Tangent.xyz);
    vec3 b = cross(n, t) * a_v4Tangent.w;

    v_av4LightDir[0].x   = dot(c_v3CamDir, t);
    v_av4LightDir[0].y   = dot(c_v3CamDir, b);
    v_av4LightDir[0].z   = dot(c_v3CamDir, n);
    v_av4LightDir[0].xyz = normalize(v_av4LightDir[0].xyz);
}