/////////////////////////////////////////////////////
//*-----------------------------------------------*//
//| Part of Disco Rock (http://www.maus-games.at) |//
//*-----------------------------------------------*//
//| Released under the zlib License               |//
//| More information available in the readme file |//
//*-----------------------------------------------*//
/////////////////////////////////////////////////////


// vertex attributes
attribute vec2  a_v2Position;
attribute float a_fHeight;
attribute vec4  a_v4Color;

// shader output
varying vec4 v_v4Color;
varying vec3 v_v3Relative;


void main()
{
    if(a_fHeight > 0.0) v_v4Color.a = 0.0;
    else
    {
        vec4 v4Position = vec4(a_v2Position.xy, a_fHeight, 1.0);
        v_v3Relative    = (u_m4ModelView * v4Position).xyz;

        gl_Position      = u_m4ModelViewProj * v4Position;
        v_av2TexCoord[0] = a_v2Texture;
        
        v_v4Color = a_v4Color;
        
#ifdef GL_ES

        v_v3Relative.y = 1.0 - v_v3Relative.y * 0.004;
        
#else

        v_av2TexCoord[1] = (a_v2Texture + u_v2TexOffset) * 0.55;

        const vec3 n = vec3( 0.0, 0.0, -1.0);
        const vec3 t = vec3(-1.0, 0.0,  0.0);
        const vec3 b = vec3( 0.0, 1.0,  0.0);
        
        vec3 v3MathLightDir    = normalize(v_v3Relative);
        v_av4LightDir[0].x   = dot(v3MathLightDir, t);
        v_av4LightDir[0].y   = dot(v3MathLightDir, b);
        v_av4LightDir[0].z   = dot(v3MathLightDir, n);
        v_av4LightDir[0].xyz = normalize(v_av4LightDir[0].xyz);
        
#endif 
    }
}