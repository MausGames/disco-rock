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
cBackground::cBackground()noexcept
: m_fPositionTime  (0.0f)
, m_fFloorTime     (0.0f)
, m_fFillTime      (0.0f)
, m_pfHeight       (NULL)
, m_iOffset        (0)
, m_fLightStrength (0.0f)
, m_iLightTick     (0)
, m_fLightTime     (0.0f)
{
    // load dance floor geometry
    m_pModel = Core::Manager::Resource->LoadNew<coreModel>();
    __LoadGeometry();

    // load object resources
    this->DefineTextureFile(0, "data/textures/background.png");
    this->DefineTextureFile(1, "data/textures/background_norm.png");
    this->DefineProgramShare("floor_shader")
          ->AttachShaderFile("data/shaders/floor.vs")
          ->AttachShaderFile("data/shaders/floor.fs")
          ->BindAttribute("a_v2Position", 0)
          ->BindAttribute("a_v4Color",    2)
          ->BindAttribute("a_fHeight",    3)
          ->Finish();

    // create filling background
    m_Fill.DefineTextureFile(0, "data/textures/background.png");
    m_Fill.DefineProgramShare("fill_shader")
           ->AttachShaderFile("data/shaders/fill.vs")
           ->AttachShaderFile("data/shaders/fill.fs")
           ->Finish();
    m_Fill.FitToScreen();
    m_Fill.SetTexSize(m_Fill.GetSize()*7.2f);
}


// ****************************************************************
cBackground::~cBackground()
{
    // free model
    Core::Manager::Resource->Free(&m_pModel);

    // delete height data
    SAFE_DELETE_ARRAY(m_pfHeight)
}


// ****************************************************************
void cBackground::Render()
{
    // enable the shader-program
    if(!m_pProgram) return;
    if(!m_pProgram->Enable()) return;

    // light flash (made in render-function to animate even in pause-mode)
    {
        // get music speed (yes this is currently hardcoded, needs to be improved)
        float fSpeed = 0.0f;
             if(g_pMusicPlayer->Control() == g_pMusicPlayer->GetMusic(0)) fSpeed = 1.9166667f;
        else if(g_pMusicPlayer->Control() == g_pMusicPlayer->GetMusic(1)) fSpeed = 2.00f;
        else if(g_pMusicPlayer->Control() == g_pMusicPlayer->GetMusic(2)) fSpeed = 1.95f;

        // update light time
        m_fLightTime += Core::System->GetTime() * fSpeed * (1.0f + MAX((g_fCurSpeed - 1.5f) * 0.16667f, 0.0f));

        // check for new tick
        const int iNewTick = (int)std::floor(m_fLightTime);
        if(m_iLightTick < iNewTick)
        {
            // create flash
            m_iLightTick = iNewTick;
            m_fLightStrength = 0.3f;
        }

        // reduce light strength (flash) over time
        m_fLightStrength = MAX(m_fLightStrength * (1.0f - Core::System->GetTime() * 5.0f), 0.0f);

        if(!g_bPause)
        {
            // calculate final light power
            const float fPower = MAX(g_fCurSpeed-1.0f, 0.0f) * 1.5f * m_fLightStrength;

            // set alpha values used as light power in the shader
            m_Fill.SetAlpha(1.0f + 2.0f*fPower);
            this->SetAlpha(1.0f + fPower);
        }
    }

    // calculate model-view matrices
    const coreMatrix4 mModelView     = m_mTransform * Core::Graphics->GetCamera();
    const coreMatrix4 mModelViewProj = mModelView   * Core::Graphics->GetPerspective();

    // update all object uniforms
    m_pProgram->SendUniform(CORE_SHADER_UNIFORM_3D_MODELVIEW,     mModelView,     false);
    m_pProgram->SendUniform(CORE_SHADER_UNIFORM_3D_MODELVIEWPROJ, mModelViewProj, false);
    m_pProgram->SendUniform(CORE_SHADER_UNIFORM_COLOR,            m_vColor);
    m_pProgram->SendUniform(CORE_SHADER_UNIFORM_TEXSIZE,          m_vTexSize);
    m_pProgram->SendUniform(CORE_SHADER_UNIFORM_TEXOFFSET,        m_vTexOffset);

    // enable all active textures
    for(int i = 0; i < CORE_TEXTURE_UNITS; ++i)
        if(m_apTexture[i].IsActive()) m_apTexture[i]->Enable(i);

    // draw the model
    m_pModel->Enable();
    glDrawRangeElements(m_pModel->GetPrimitiveType(), 0, BACK_BLOCKS * BACK_PER_VERTICES, BACK_RANGE, m_pModel->GetIndexType(), r_cast<const GLvoid*>(m_iOffset));

    // render the filling background
    m_Fill.Render();
}


// ****************************************************************
void cBackground::Move()
{
    // update dance floor position
    m_fPositionTime.Update(2.5f, 0);
    if(m_fPositionTime < 0.0f)        m_fPositionTime += BACK_REPEAT;
    if(m_fPositionTime > BACK_REPEAT) m_fPositionTime -= BACK_REPEAT;

    // calculate drawing offset and background position
    m_iOffset = coreUint(std::floor(m_fPositionTime)) * BACK_BLOCKS_X * BACK_PER_INDICES * sizeof(coreWord);
    this->SetPosition(coreVector3(0.0f, -m_fPositionTime * BACK_DETAIL_Y, 0.0f));

    // update dance floor light animation
    m_fFloorTime.Update(0.5f, 0);
    this->SetTexOffset(coreVector2(1.0f,-1.0f)*m_fFloorTime);

    // update and move the filling background
    m_fFillTime.Update(10.0f, 0);
    m_Fill.SetTexOffset(coreVector2(-0.075f * m_fFillTime, 0.0f));
    m_Fill.Move();

    // move the object
    coreObject3D::Move();
}


// ****************************************************************
void cBackground::UpdateHoles(const coreUint& iLine, const bool* pbIndex)
{
    coreModel::Lock();
    {
        const coreUint iNum  = BACK_BLOCKS_X * BACK_PER_VERTICES;
        const coreUint iSize = iNum * sizeof(float);

        // map required area
        float* pfData = m_pModel->GetVertexBuffer(1)->Map<float>(iLine*iSize, iSize);
        SDL_assert((iLine+1) * iSize < BACK_TOTAL_INDICES * sizeof(float));

        // set height values of the selected line
        for(int i = 0; i < iNum; ++i)
        {
            pfData[i] = m_pfHeight[i + iLine*iNum] = pbIndex[i/BACK_PER_VERTICES] ? 100.0f : GAME_HEIGHT;
        }

        // unmap area
        m_pModel->GetVertexBuffer(1)->Unmap(pfData);  

        // reset current model object
        coreModel::Disable(false);
    }
    coreModel::Unlock();
}


// ****************************************************************
float cBackground::GetHeight(const coreVector2& vPos, const coreVector2& vBackPos)const
{
    // convert real position to block position
    const float fX = (vPos.x-vBackPos.x) / BACK_DETAIL_X + float(BACK_BLOCKS_X)/2.0f;
    const float fY = (vPos.y-vBackPos.y) / BACK_DETAIL_Y + BACK_OFFSET_Y;

     // retrieve height value of the block
    return m_pfHeight[(int(floor(fX)) + int(floor(fY))*BACK_BLOCKS_X) * BACK_PER_VERTICES];

/*
    // retrieve all four corners of the block
    const int iI00 = (int(floor(fX)) + int(floor(fY))*BACK_BLOCKS_X) * BACK_PER_VERTICES;
    const int iI01 = iI00 + 1;
    const int iI10 = iI00 + 2;
    const int iI11 = iI00 + 3;
    SDL_assert(iI11 < BACK_BLOCKS * BACK_PER_VERTICES);
    
    // retrieve height values of the corners
    const float& fH00 = m_pfHeight[iI00];
    const float& fH01 = m_pfHeight[iI01];
    const float& fH10 = m_pfHeight[iI10];
    const float& fH11 = m_pfHeight[iI11];

    // interpolate between all height values
    float fTemp;
    const float fFractX = std::modf(fX, &fTemp);
    const float fFractY = std::modf(fY, &fTemp);
    return LERP(LERP(fH00, fH10, fFractX), LERP(fH01, fH11, fFractX), fFractY);
*/
}


// ****************************************************************
void cBackground::__LoadGeometry()
{
    std::vector<coreVector4> avColor;   avColor.reserve(BACK_BLOCKS);
    std::vector<sVertex> m_pVertexData; m_pVertexData.reserve(BACK_TOTAL_VERTICES);
    std::vector<coreWord> m_pIndexData; m_pIndexData.reserve(BACK_TOTAL_INDICES);

    // create base geometry
    sVertex* pBaseVertex = new sVertex[BACK_WIDTH * BACK_HEIGHT];
    for(int i = 0; i < BACK_WIDTH * BACK_HEIGHT; i++)
    {
        const int x = i % BACK_WIDTH;
        const int y = i / BACK_WIDTH;

        // set positions and texture coordinates of the grid
        pBaseVertex[i].vPosition = coreVector2(float(x - BACK_WIDTH/2) * BACK_DETAIL_X, (float(y) - BACK_OFFSET_Y) * BACK_DETAIL_Y);
        pBaseVertex[i].vTexture  = coreVector2(float(x), float(-y));
    }

    // create persistent array with mutable height data (the holes are just out of screen)
    m_pfHeight = new float[BACK_TOTAL_VERTICES];
    for(int i = 0; i < BACK_TOTAL_VERTICES; ++i) m_pfHeight[i] = GAME_HEIGHT;

    // define color values
    int iCurColor = 0;
    for(int i = 0; i < BACK_BLOCKS; ++i)
    {
        // get random color, but not the same twice (only X, Y may be twice)
        iCurColor = (iCurColor + Core::Rand->Int(1, COLOR_NUM-1)) % COLOR_NUM;
        avColor.push_back(g_avColor[iCurColor]);

        // add additional random parameter for the shader
        avColor.back().a = Core::Rand->Float(0.9f, 1.0f);
    }

    // sync beginning and ending colors to create an infinit looking grid when resetting the position
    for(int y = 20; y < BACK_BLOCKS_Y; ++y)
    {
        for(int x = 0; x < BACK_BLOCKS_X; ++x)
        {
            avColor[x + y*BACK_BLOCKS_X] = avColor[x + (y-20)*BACK_BLOCKS_X];
        }
    }

    // create final vertex and index data
    for(int y = 0; y < BACK_BLOCKS_Y; ++y)
    {
        for(int x = 0; x < BACK_BLOCKS_X; ++x)
        {
            const coreUint j = x + y*BACK_WIDTH;

            const coreVector4& vColor  = avColor[x + y*BACK_BLOCKS_X];
            const coreWord iStartIndex = m_pVertexData.size();

            // copy base vertices to create unique plates and add generated color values
            m_pVertexData.push_back(pBaseVertex[j]);              m_pVertexData.back().vColor = vColor;
            m_pVertexData.push_back(pBaseVertex[j+1]);            m_pVertexData.back().vColor = vColor;
            m_pVertexData.push_back(pBaseVertex[j  +BACK_WIDTH]); m_pVertexData.back().vColor = vColor;
            m_pVertexData.push_back(pBaseVertex[j+1+BACK_WIDTH]); m_pVertexData.back().vColor = vColor;

            // add indices for the new plate
            m_pIndexData.push_back(iStartIndex + 0);
            m_pIndexData.push_back(iStartIndex + 1);
            m_pIndexData.push_back(iStartIndex + 2);
            m_pIndexData.push_back(iStartIndex + 1);
            m_pIndexData.push_back(iStartIndex + 3);
            m_pIndexData.push_back(iStartIndex + 2);
        }
    }

    coreVertexBuffer* pBuffer;

    // create static vertex buffer
    pBuffer = m_pModel->CreateVertexBuffer(BACK_TOTAL_VERTICES, sizeof(sVertex), m_pVertexData.data(), GL_STATIC_DRAW);
    pBuffer->DefineAttribute(0, 2, GL_FLOAT, 0);
    pBuffer->DefineAttribute(1, 2, GL_FLOAT, 2*sizeof(float));
    pBuffer->DefineAttribute(2, 4, GL_FLOAT, 4*sizeof(float));

    // create dynamic height data buffer
    pBuffer = m_pModel->CreateVertexBuffer(BACK_TOTAL_VERTICES, sizeof(float), m_pfHeight, GL_DYNAMIC_DRAW);
    pBuffer->DefineAttribute(3, 1, GL_FLOAT, 0);

    // create index buffer
    m_pModel->CreateIndexBuffer(BACK_TOTAL_INDICES, sizeof(coreWord), m_pIndexData.data(), GL_STATIC_DRAW);

    // clear memory
    avColor.clear();
    m_pVertexData.clear();
    m_pIndexData.clear();
    SAFE_DELETE_ARRAY(pBaseVertex)

    // cthulhu fhtagn cheezburger
    Core::Log->Info("Background loaded");
}