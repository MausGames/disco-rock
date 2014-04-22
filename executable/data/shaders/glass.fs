/////////////////////////////////////////////////////
//*-----------------------------------------------*//
//| Part of Disco Rock (http://www.maus-games.at) |//
//*-----------------------------------------------*//
//| Released under the zlib License               |//
//| More information available in the readme file |//
//*-----------------------------------------------*//
/////////////////////////////////////////////////////


// shader input
varying float v_fIntensity;


void main()
{
    if(v_fIntensity <= 0.0) gl_FragColor = vec4(0.0); // discard;
    else
    {
        gl_FragColor = vec4(texture2D(u_as2Texture[0], v_av2TexCoord[0]).rgb * v_fIntensity, 0.5);
    }
}