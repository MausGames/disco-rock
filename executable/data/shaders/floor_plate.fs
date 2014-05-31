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
varying vec3 v_v3Relative;


void main()
{
#if (_CORE_QUALITY_) < 1

    vec2 v2TexCoord = fract(v_av2TexCoord[0]);
    
    if(v2TexCoord.x < 0.06 || v2TexCoord.x > 0.94 ||
       v2TexCoord.y < 0.06 || v2TexCoord.y > 0.94)
    {
        gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
    else
    {
        float fIntensity = v_v3Relative.y;
        gl_FragColor     = vec4(u_v4Color.rgb * (0.9 * fIntensity * min(u_v4Color.a * fIntensity, 1.0)), 1.0);
    }

#else

    vec3  v3TextureNorm  = texture2D(u_as2Texture[1], v_av2TexCoord[0]).rgb;
    vec2  v2TextureColor = texture2D(u_as2Texture[0], v_av2TexCoord[0]).rg;
    #if (_CORE_QUALITY_) > 1
    
        float fTextureDisco  = texture2D(u_as2Texture[0], v_av2TexCoord[1]).b;
        
    #endif
    
    float fRsqrt     = inversesqrt(dot(v_v3Relative, v_v3Relative));
    float fIntensity = 250.0 * fRsqrt;
    fIntensity      *= abs(dot(v_v3Relative * fRsqrt, c_v3CamDir));
    fIntensity       = min((fIntensity - 0.25) * 1.3 * 1.25, 1.25);

    vec3  v3MathLightDir = normalize(v_av4LightDir[0].xyz);
    vec3  v3MathNormal   = normalize(v3TextureNorm * 2.0 - 1.0);
    float fBumpFactor    = max(0.0, (dot(v3MathLightDir, v3MathNormal) - 0.5) * 2.0);

    vec4 v4Color = vec4(vec3(v2TextureColor.r), 1.0) * u_v4Color * (fIntensity * (0.9 * fBumpFactor + 0.25 * pow(fBumpFactor, 40.0)));

    gl_FragColor.rgb = v4Color.rgb * min(v4Color.a, 1.0);
    #if (_CORE_QUALITY_) > 1
    
        gl_FragColor.rgb += vec3(fTextureDisco * 1.45 * (0.35 - 0.3 * sin(v_av2TexCoord[0].y * 0.25 * PI) * sin(v_av2TexCoord[1].x)));
    
    #endif
    gl_FragColor.rgb *= v2TextureColor.g;
    gl_FragColor.a    = 1.0;
    
#endif
}