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
varying vec4 v_v4Color;
varying vec3 v_v3Relative;


void main()
{
    if(v_v4Color.a <= 0.0) gl_FragColor = vec4(0.0); // discard;
    else
    {
#if (GL_QUALITY) < 1

    vec2 v2TexCoord = fract(v_av2TexCoord[0]);
    
    if(v2TexCoord.x < 0.06 || v2TexCoord.x > 0.94 ||
       v2TexCoord.y < 0.06 || v2TexCoord.y > 0.94)
    {
        gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
    else
    {
        float fIntensity = v_v3Relative.y;
        gl_FragColor     = vec4(v_v4Color.rgb * (0.9 * fIntensity * min(v_v4Color.a * fIntensity, 1.0)), 1.0);
    }

#else

    vec3  v3TextureNorm  = texture2D(u_as2Texture[1], v_av2TexCoord[0]).rgb;
    vec2  v2TextureColor = texture2D(u_as2Texture[0], v_av2TexCoord[0]).rg;
    float fTextureDisco  = texture2D(u_as2Texture[0], v_av2TexCoord[1]).b;
    
    float fIntensity = 250.0 * inversesqrt(dot(v_v3Relative, v_v3Relative));
    fIntensity      *= abs(dot(normalize(v_v3Relative), c_v3CamDir));
    fIntensity       = (fIntensity - 0.25) * 1.3 * 1.25;

    vec3  v3MathLightDir = normalize(v_av4LightDir[0].xyz);
    vec3  v3MathNormal   = normalize(v3TextureNorm * 2.0 - 1.0);
    float fBumpFactor    = max(0.0, (dot(v3MathLightDir, v3MathNormal) - 0.5) * 2.0);

    vec4 v4Color = vec4(vec3(v2TextureColor.r), 1.0) * v_v4Color * (fIntensity * (0.9 * fBumpFactor + 0.25 * pow(fBumpFactor, 40.0)));

    gl_FragColor.rgb  = v4Color.rgb * min(v4Color.a, 1.0) + vec3(u_v4Color.a * fTextureDisco * 0.5 * 1.45 * (0.7 - 0.6 * sin(v_av2TexCoord[0].y * 0.25 * PI) * sin(v_av2TexCoord[1].x)));
    gl_FragColor.rgb *= v2TextureColor.g;
    gl_FragColor.a    = 1.0;
    
#endif
}
}