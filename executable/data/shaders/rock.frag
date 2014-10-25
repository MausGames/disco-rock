/////////////////////////////////////////////////////
//*-----------------------------------------------*//
//| Part of Disco Rock (http://www.maus-games.at) |//
//*-----------------------------------------------*//
//| Released under the zlib License               |//
//| More information available in the readme file |//
//*-----------------------------------------------*//
/////////////////////////////////////////////////////


// faster pow(x, 32.0) calculation
float pow32(in float x)
{
    x *= x;         // 2
    x *= x;         // 4
    x *= x;         // 8
    x *= x;         // 16
    return x * x;   // 32
}


void FragmentMain()
{
    // lookup texture (rgb = normal, a = grayscale color)
    vec4 v4Texture = coreTexture2D(0, v_av2TexCoord[0]);

    // calculate dot-3 bump factor
    vec3  v3MathLightDir = normalize(v_av4LightDir[0].xyz);
    vec3  v3MathNormal   = normalize(v4Texture.rgb * 2.0 - 1.0);
    float v1BumpFactor   = abs((dot(v3MathLightDir, v3MathNormal) + 0.1) * 0.91);

    // draw final color
    gl_FragColor = vec4(vec3(v4Texture.a * max(v1BumpFactor + 0.2 * pow32(v1BumpFactor), 0.15)), 1.0);
}