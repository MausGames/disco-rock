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

// shader output
varying float v_fIntensity;


void main()
{
    gl_Position      = u_m4ModelViewProj * vec4(a_v3Position * 1.15, 1.0);
    v_av2TexCoord[0] = a_v2Texture;

    float fTopDot = dot(a_v3Normal, vec3(0.0, 0.0, 1.0));
    if(fTopDot > 0.95) v_fIntensity = 0.0;
    else
    {
        vec3 v3Normal = normalize(u_m3Normal * a_v3Normal);
        v_fIntensity  = 1.4 * abs(dot(v3Normal, c_v3CamDir)) + 0.56 * abs(fTopDot);
    }
}