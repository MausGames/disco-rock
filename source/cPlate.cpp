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
    this->DefineModel("default_square.md5mesh");
    this->DefineTexture(0, "background.png");
    this->DefineTexture(1, "background_norm.png");
    this->DefineProgram("floor_plate_program");

    // set object properties
    this->SetSize(coreVector3(BACK_DETAIL_X, BACK_DETAIL_Y, 1.0f));
    this->SetDirection(coreVector3(0.0f,0.0f,-1.0f));
    this->SetTexSize(m_vFixedOffset);

    // set random plate color
    const coreVector3& vColor = g_pBackground->GetColor(Core::Rand->Int(0, COLOR_NUM-1));
    this->SetColor4(coreVector4(vColor, Core::Rand->Float(0.9f, 1.0f) * COLOR_BRIGHTNESS));
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
        m_Animation.Play(CORE_TIMER_PLAY_RESET);
        m_fStartY = -1.0f;
    }

    if(m_Animation.GetStatus())
    {
        // update the lift-animation
        if(m_Animation.Update(0.91f))
            m_iStatus = 1;   // mark as finished

        // calculate the position
        this->SetPosition(coreVector3(this->GetPosition().xy(), 110.0f * m_Animation.GetValue(CORE_TIMER_GET_NORMAL) + GAME_HEIGHT));
    }

    // calculate the texture-offset for the disco-lights
    this->SetTexOffset(g_pBackground->GetTexOffset() + m_vFixedOffset);

    // move the object
    coreObject3D::Move();
}