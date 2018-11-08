//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Disco Rock (https://www.maus-games.at) |//
//*------------------------------------------------*//
//| Released under the zlib License                |//
//| More information available in the readme file  |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////


// NOTE: floor uses v_v4VarColor, floor_plate uses u_v4Color

// constant values
const vec3 c_v3CamDir = vec3(0.0, -0.8137, 0.5812);   // normalized camera vector

// shader input
varying vec3 v_v3Relative;   // position relative to the viewer

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


void FragmentMain()
{
    // ignore vertices in the air
    if(v_v4VarColor.a <= 0.5) gl_FragColor = vec4(0.0); // discard;
    else
    {
#if (_CORE_QUALITY_) < 1

        vec2 v2TexCoord = fract(v_av2TexCoord[0]);
        
        // compare modified texture coordinates
        if(any(lessThan   (v2TexCoord, vec2(0.06))) ||
           any(greaterThan(v2TexCoord, vec2(0.94))))
        {
            // draw plain black grid (aliased)
            gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
        }
        else
        {
            // draw plain color block
            float v1Intensity = v_v3Relative.y;
            gl_FragColor      = vec4(v_v4VarColor.rgb * (0.9 * v1Intensity * min(v_v4VarColor.a * v1Intensity, 1.0)), 1.0);
        }

#else

        // lookup textures
        vec3 v3TextureNorm  = coreTexture2D(1, v_av2TexCoord[0]).rgb;
        vec2 v2TextureColor = coreTexture2D(0, v_av2TexCoord[0]).rg;
        #if (_CORE_QUALITY_) > 1

            float v1TextureDisco = coreTexture2D(0, v_av2TexCoord[1]).b;
            
        #endif
        
        // calculate perspective intensity value
        float v1Rsqrt     = inversesqrt(coreLengthSq(v_v3Relative));
        float v1Intensity = 406.25 * v1Rsqrt * min(abs(dot(v_v3Relative * v1Rsqrt, c_v3CamDir)), 1.0) - 0.4063;

        // calculate dot-3 bump factor
        vec3  v3MathLightDir = normalize(v_av4LightDir[0].xyz);
        vec3  v3MathNormal   = normalize(v3TextureNorm * 2.0 - 1.0);
        float v1BumpFactor   = max(0.0, dot(v3MathLightDir, v3MathNormal) * 2.0 - 1.0);
   
        // calculate final diffuse color
        vec4 v4Color   = vec4(vec3(v2TextureColor.r), 1.0) * v_v4VarColor * (v1Intensity * (0.9 * v1BumpFactor + 0.25 * pow40(v1BumpFactor)));
        vec3 v3Diffuse = v4Color.rgb * min(v4Color.a, 1.0);

        #if (_CORE_QUALITY_) > 1
        
            // add disco light effect
            vec2 v2Sin = sin(vec2(v_av2TexCoord[0].y * 0.5, v_av2TexCoord[1].x) * (0.5*PI));
            v3Diffuse += vec3(u_v4Color.a * v1TextureDisco * (0.48 - 0.48 * v2Sin.x * v2Sin.y));

        #endif
        
        // add detail map
        v3Diffuse *= v2TextureColor.g;
        
        // draw diffuse color
        gl_FragColor = vec4(v3Diffuse, 1.0);
    
#endif
    }
}