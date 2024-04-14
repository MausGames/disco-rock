//////////////////////////////////////////////////////
//*------------------------------------------------*//
//| Part of Disco Rock (https://www.maus-games.at) |//
//*------------------------------------------------*//
//| Copyright (c) 2013 Martin Mauersics            |//
//| Released under the zlib License                |//
//*------------------------------------------------*//
//////////////////////////////////////////////////////
#include "engine/data_transform_2d.glsl"
#include "engine/util_base.glsl"


void VertexMain()
{
    // transform position (with maximum z-value to override depth testing)
    gl_Position   = coreObject2DPosition();
    gl_Position.z = 1.0;

#if (_CORE_QUALITY_) > 1

    // transform texture coordinates
    vec2 v2MapCoord  = a_v2LowTexCoord * u_v2TexSize;
    v_av2TexCoord[0] =  v2MapCoord + u_v2TexOffset;
    v_av2TexCoord[1] = (v2MapCoord - u_v2TexOffset) * 0.8 * PI;

#endif
}