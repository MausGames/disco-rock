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
    vec3  v3TextureNorm  = texture2D(u_as2Texture[1], v_av2TexCoord[0]).rgb;
    float fTextureColor  = texture2D(u_as2Texture[0], v_av2TexCoord[0]).r;
    
    vec3  v3MathLightDir = normalize(v_av4LightDir[0].xyz);
    vec3  v3MathNormal   = normalize(v3TextureNorm * 2.0 - 1.0);
    float fBumpFactor    = abs((dot(v3MathLightDir, v3MathNormal) + 0.1) * 0.91);

    gl_FragColor = vec4(vec3(fTextureColor * max(fBumpFactor + 0.2 * pow(fBumpFactor, 30.0), 0.15)), 1.0);
}