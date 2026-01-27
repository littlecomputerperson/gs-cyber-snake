/*============================================================================================+
 | Game System (GS) Library                                                                   |
 |--------------------------------------------------------------------------------------------|
 | FILES: level.cpp, level.h                                                                  |
 |--------------------------------------------------------------------------------------------|
 | CLASS: Level                                                                               |
 |--------------------------------------------------------------------------------------------|
 | ABOUT: Class to create and render a level  :-)                                             |
 |--------------------------------------------------------------------------------------------|
 |                                                                                    03/2023 |
 +============================================================================================*/


//==============================================================================================
// Include Game System (GS) header files.
// ---------------------------------------------------------------------------------------------
#include "level.h"
#include <iostream>
//==============================================================================================


////////////////////////////////////////////////////////////////////////////////////////////////
// Constructor/Destructor Methods //////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

//==============================================================================================
// Level::Level():
// ---------------------------------------------------------------------------------------------
// Purpose: The constructor, initializes class data when class object is created.
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

Level::Level()
{
    m_isCreated = false;

    m_snakeStartPosX = 0;
    m_snakeStartPosY = 0;

    m_currentLevel = 0;

    m_state = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////

//==============================================================================================
// Level::~Level():
// ---------------------------------------------------------------------------------------------
// Purpose: The de-structor, de-initializes class data when class object is destroyed.
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

Level::~Level()
{
    this->Destroy();
}

////////////////////////////////////////////////////////////////////////////////////////////////

//==============================================================================================
// Level::Create():
// ---------------------------------------------------------------------------------------------
// Purpose:
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

bool Level::Create(RECT areaRect, bool filtered, GS_OGLColor levelColor)
{
    bool bResult;

    // Has the level already been created?
    if( m_isCreated )
    {
        this->Destroy();
    }

    bResult = m_levelMap.Create("data/level.tga", 512, 32, LEVEL_TILE_WIDTH, LEVEL_TILE_HEIGHT, filtered);
    m_levelMap.LoadMap("data/level_01.map", LEVEL_MAP_ROWS, LEVEL_MAP_COLS);
    m_levelMap.SetClipBox(areaRect);
    m_levelMap.SetMapX(0);
    m_levelMap.SetMapY(0);
    m_levelMap.SetLimitX(FALSE);
    m_levelMap.SetLimitY(FALSE);
    m_levelMap.SetWrapX(FALSE);
    m_levelMap.SetWrapY(FALSE);
    m_levelMap.SetClearTileID(CLEAR_TILE_ID);
    m_levelMap.SetModulateColor(levelColor);

    m_currentLevel = 1;

    // Were we not successful?
    if (FALSE == bResult) {
        GS_Error::Report("LEVEL.CPP", 102, "Failed to create level!");
    }

    // Reset the timer
    m_gsTimer.Reset();

    // Remember that the level was created
    m_isCreated = bResult;

    return bResult;
}

////////////////////////////////////////////////////////////////////////////////////////////////

//==============================================================================================
// Level::Destroy():
// ---------------------------------------------------------------------------------------------
// Purpose: The deatroys the level
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void Level::Destroy()
{
    m_isCreated = false;

    m_snakeStartPosX = 0;
    m_snakeStartPosY = 0;

    m_currentLevel = 0;

    m_state = 0;

    m_gsTimer.Reset();

    m_levelMap.Destroy();
}

////////////////////////////////////////////////////////////////////////////////////////////////

//==============================================================================================
// Level::SetState():
// ---------------------------------------------------------------------------------------------
// Purpose: Set the level state
// ---------------------------------------------------------------------------------------------
// Returns: Nothing
//==============================================================================================

void Level::SetState( int stateNum )
{
    // Can't change to the current state
    if( stateNum == m_state )
    {
        return;
    }

    // Depending on the state selected
    if( stateNum == 0 )
    {
    }
    else
    {
        return;
    }

    // Remember the level state
    m_state = stateNum;
}

////////////////////////////////////////////////////////////////////////////////////////////////

//==============================================================================================
// Level::CheckCollision():
// ---------------------------------------------------------------------------------------------
// Purpose: Function to check for a collision with a map tile
// ---------------------------------------------------------------------------------------------
// Returns: true if there is a collision, false if not
//==============================================================================================

bool Level::CheckCollision( RECT rcRect ) {
    // Is the rectangle not on the clear tile?
    if( !m_levelMap.IsOnTile(rcRect, CLEAR_TILE_ID) ) {
        return true;
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////

//==============================================================================================
// Level::Load():
// ---------------------------------------------------------------------------------------------
// Purpose: Function to load a new level
// ---------------------------------------------------------------------------------------------
// Returns: Nothing
//==============================================================================================

bool Level::Load( int nLevelNum )
{
    bool bResult;

    // Is the level out of bounds?
    if( (nLevelNum < MIN_LEVEL) || (nLevelNum > MAX_LEVEL) ) {
        return FALSE;
    }

    switch (nLevelNum) {
        case 1:
            bResult = m_levelMap.LoadMap("data/level_01.map", LEVEL_MAP_ROWS, LEVEL_MAP_COLS);
            m_snakeStartPosX = 1 * LEVEL_TILE_WIDTH;
            m_snakeStartPosY = 15 * LEVEL_TILE_HEIGHT;
            break;
        case 2:
            bResult = m_levelMap.LoadMap("data/level_02.map", LEVEL_MAP_ROWS, LEVEL_MAP_COLS);
            m_snakeStartPosX = 1 * LEVEL_TILE_WIDTH;
            m_snakeStartPosY = 16 * LEVEL_TILE_HEIGHT;
            break;
        case 3:
            bResult = m_levelMap.LoadMap("data/level_03.map", LEVEL_MAP_ROWS, LEVEL_MAP_COLS);
            m_snakeStartPosX = 1 * LEVEL_TILE_WIDTH;
            m_snakeStartPosY = 15 * LEVEL_TILE_HEIGHT;
            break;
        case 4:
            bResult = m_levelMap.LoadMap("data/level_04.map", LEVEL_MAP_ROWS, LEVEL_MAP_COLS);
            m_snakeStartPosX = 1 * LEVEL_TILE_WIDTH;
            m_snakeStartPosY = 14 * LEVEL_TILE_HEIGHT;
            break;
        case 5:
            bResult = m_levelMap.LoadMap("data/level_05.map", LEVEL_MAP_ROWS, LEVEL_MAP_COLS);
            m_snakeStartPosX = 1 * LEVEL_TILE_WIDTH;
            m_snakeStartPosY = 15 * LEVEL_TILE_HEIGHT;
            break;
        case 6:
            bResult = m_levelMap.LoadMap("data/level_06.map", LEVEL_MAP_ROWS, LEVEL_MAP_COLS);
            m_snakeStartPosX = 1 * LEVEL_TILE_WIDTH;
            m_snakeStartPosY = 15 * LEVEL_TILE_HEIGHT;
            break;
        case 7:
            bResult = m_levelMap.LoadMap("data/level_07.map", LEVEL_MAP_ROWS, LEVEL_MAP_COLS);
            m_snakeStartPosX = 1 * LEVEL_TILE_WIDTH;
            m_snakeStartPosY = 15 * LEVEL_TILE_HEIGHT;
            break;
        case 8:
            bResult = m_levelMap.LoadMap("data/level_08.map", LEVEL_MAP_ROWS, LEVEL_MAP_COLS);
            m_snakeStartPosX = 1 * LEVEL_TILE_WIDTH;
            m_snakeStartPosY = 15 * LEVEL_TILE_HEIGHT;
            break;
        case 9:
            bResult = m_levelMap.LoadMap("data/level_09.map", LEVEL_MAP_ROWS, LEVEL_MAP_COLS);
            m_snakeStartPosX = 1 * LEVEL_TILE_WIDTH;
            m_snakeStartPosY = 15 * LEVEL_TILE_HEIGHT;
            break;
        case 10:
            bResult = m_levelMap.LoadMap("data/level_10.map", LEVEL_MAP_ROWS, LEVEL_MAP_COLS);
            m_snakeStartPosX = 1 * LEVEL_TILE_WIDTH;
            m_snakeStartPosY = 15 * LEVEL_TILE_HEIGHT;
            break;
        default:
            return FALSE;
    }

    // Were we not successful?
    if( FALSE == bResult ) {
        GS_Error::Report("LEVEL.CPP", 241, "Failed to load map!");
    }

    // Remember which level was loaded
    m_currentLevel = nLevelNum;

    return bResult;
}


////////////////////////////////////////////////////////////////////////////////////////////////

//==============================================================================================
// Level::Update():
// ---------------------------------------------------------------------------------------------
// Purpose: Update the level (position, animation, etc,.)
// ---------------------------------------------------------------------------------------------
// Returns: Nothing
//==============================================================================================

void Level::Update()
{

    return;
}

////////////////////////////////////////////////////////////////////////////////////////////////

//==============================================================================================
// Level::Render():
// ---------------------------------------------------------------------------------------------
// Purpose: Draws the level
// ---------------------------------------------------------------------------------------------
// Returns: True if rendering was successful, false if not
    //==============================================================================================

bool Level::Render(float fAlpha)
{
    // Set the alpha value to determine the transparency
    m_levelMap.SetModulateColor(-1.0f, -1.0f, -1.0f, fAlpha);

    // Display the map
    m_levelMap.Render();

    return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////
