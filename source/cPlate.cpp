/////////////////////////////////////////////////////
//*-----------------------------------------------*//
//| Part of Disco Rock (http://www.maus-games.at) |//
//*-----------------------------------------------*//
//| Released under the zlib License               |//
//| More information available in the readme file |//
//*-----------------------------------------------*//
/////////////////////////////////////////////////////
#include "main.h"


// ****************************************************************
cPlate::cPlate(const float& fStartY, const coreVector2& vFixedOffset)noexcept
: m_Animation    (coreTimer(1.0f, Core::Rand->Float(0.45f,0.55f), 1))
, m_fStartY      (fStartY)
, m_vFixedOffset (vFixedOffset)
{
    m_Animation.SetTimeID(0);

    // load object resources
    this->DefineModelFile("data/models/standard_square.md5mesh");
    this->DefineTextureFile(0, "data/textures/background.png");
    this->DefineTextureFile(1, "data/textures/background_norm.png");
    this->DefineProgramShare("floor_plate_shader")
        ->AttachShaderFile("data/shaders/floor_plate.vs")
        ->AttachShaderFile("data/shaders/floor_plate.fs")
        ->Finish();

    // set object properties
    this->SetSize(coreVector3(BACK_DETAIL_X, BACK_DETAIL_Y, 1.0f));
    this->SetDirection(coreVector3(0.0f,0.0f,-1.0f));
    this->SetTexSize(m_vFixedOffset);

    // set random plate color
    const coreVector4& vColor = g_avColor[Core::Rand->Int(0, COLOR_NUM-1)];
    this->SetColor4(coreVector4(vColor.xyz(), Core::Rand->Float(0.9f, 1.0f) * COLOR_BRIGHTNESS));
}

// ****************************************************************
cPlate::~cPlate()
{
}


// ****************************************************************
void cPlate::Render()
{
    // render the plate
    if(this->Enable())
        m_pModel->DrawArrays();
}


// ****************************************************************
void cPlate::Move()
{
    // check minimal distance to the front
    if((this->GetPosition().y <= m_fStartY) && (m_fStartY > 0.0f))
    {
        // start lift-animation and invalidate start-parameter
        m_Animation.Play(true);
        m_fStartY = -1.0f;
    }

    if(m_Animation.GetStatus())
    {
        // update the lift-animation
        if(m_Animation.Update(0.91f))
            m_iStatus = 1;   // mark as finished

        // calculate the position
        this->SetPosition(coreVector3(this->GetPosition().xy(), 110.0f * m_Animation.GetCurrent(false) + GAME_HEIGHT));
    }

    // calculate the texture-offset for the disco-lights
    this->SetTexOffset(g_pBackground->GetTexOffset() + m_vFixedOffset);

    // move the object
    coreObject3D::Move();
}