/////////////////////////////////////////////////////
//*-----------------------------------------------*//
//| Part of Disco Rock (http://www.maus-games.at) |//
//*-----------------------------------------------*//
//| Released under the zlib License               |//
//| More information available in the readme file |//
//*-----------------------------------------------*//
/////////////////////////////////////////////////////


void main()
{
    vec4 v4Texture = texture2D(u_as2Texture[0], v_av2TexCoord[0]);

    vec3  v3MathLightDir = normalize(v_av4LightDir[0].xyz);
    vec3  v3MathNormal   = normalize(v4Texture.rgb * 2.0 - 1.0);
    float fBumpFactor    = abs((dot(v3MathLightDir, v3MathNormal) + 0.1) * 0.91);

    gl_FragColor = vec4(vec3(v4Texture.a * max(fBumpFactor + 0.2 * pow(fBumpFactor, 30.0), 0.15)), 1.0);
}