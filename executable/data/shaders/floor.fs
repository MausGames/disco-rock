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

// shader input
varying vec4 v_v4Color;
varying vec3 v_v3Relative;

// faster pow(x, 40.0) calculation
float pow40(in float x)
{
    x *= x;             // 2
    x *= x;             // 4
    float x8 = x * x;   // 8
    x = x8 * x8;        // 16
    x *= x;             // 32
    return x * x8;      // 40
}


void main()
{
    if(v_v4Color.a <= 0.5) gl_FragColor = vec4(0.0); // discard;
    else
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
            gl_FragColor     = vec4(v_v4Color.rgb * (0.9 * fIntensity * min(v_v4Color.a * fIntensity, 1.0)), 1.0);
        }

#else

        vec3 v3TextureNorm  = texture2D(u_as2Texture[1], v_av2TexCoord[0]).rgb;
        vec2 v2TextureColor = texture2D(u_as2Texture[0], v_av2TexCoord[0]).rg;
        #if (_CORE_QUALITY_) > 1
        
            float fTextureDisco  = texture2D(u_as2Texture[0], v_av2TexCoord[1]).b;
            
        #endif
        
        float fRsqrt     = inversesqrt(coreLengthSq(v_v3Relative));
        float fIntensity = 406.25 * fRsqrt * min(abs(dot(v_v3Relative * fRsqrt, c_v3CamDir)), 1.0) - 0.4063;

        vec3  v3MathLightDir = normalize(v_av4LightDir[0].xyz);
        vec3  v3MathNormal   = normalize(v3TextureNorm * 2.0 - 1.0);
        float fBumpFactor    = max(0.0, dot(v3MathLightDir, v3MathNormal) * 2.0 - 1.0);
   
        vec4 v4Color = vec4(vec3(v2TextureColor.r), 1.0) * v_v4Color * (fIntensity * (0.9 * fBumpFactor + 0.25 * pow40(fBumpFactor)));

        gl_FragColor.rgb = v4Color.rgb * min(v4Color.a, 1.0);
        #if (_CORE_QUALITY_) > 1
        
            gl_FragColor.rgb += vec3(u_v4Color.a * fTextureDisco * (0.5075 - 0.435 * sin(v_av2TexCoord[0].y * 0.25 * PI) * sin(v_av2TexCoord[1].x * 0.5 * PI)));

        #endif
        gl_FragColor.rgb *= v2TextureColor.g;
        gl_FragColor.a    = 1.0;
    
#endif
    }
}