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


#ifndef LEVEL_H
#define LEVEL_H


//================================================================================================
// Include Game System (GS) header files.
// -----------------------------------------------------------------------------------------------
#include "gs_ogl_collide.h"
#include "gs_ogl_map.h"
#include "gs_ogl_particle.h"
#include "gs_ogl_sprite_ex.h"
#include "gs_timer.h"
//================================================================================================


//================================================================================================
// Include standard C library header files.
// -----------------------------------------------------------------------------------------------
#include <iostream>
#include <time.h>
//================================================================================================


//==============================================================================================
// Level defines.
// ---------------------------------------------------------------------------------------------
#define LEVEL_TILE_WIDTH  16
#define LEVEL_TILE_HEIGHT 16
// ---------------------------------------------------------------------------------------------
#define LEVEL_MAP_COLS  60
#define LEVEL_MAP_ROWS 30
// ---------------------------------------------------------------------------------------------
#define MIN_LEVEL 1
#define MAX_LEVEL 10
// ---------------------------------------------------------------------------------------------
#define CLEAR_TILE_ID 0
// ---------------------------------------------------------------------------------------------


//==============================================================================================
// Level structures
// ---------------------------------------------------------------------------------------------
//==============================================================================================


////////////////////////////////////////////////////////////////////////////////////////////////
// Class Definition. ///////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


class Level
{

private:
    GS_OGLMap m_levelMap;           // The layout for the level

    GS_OGLSpriteEx m_levelSprite;   // The sprite object used to draw the level

    GS_OGLParticle m_levelParticle; // A particle object used for level particle effects
    GS_OGLColor m_particleColor;    // The level particle effect color
    float m_particleAlphaMod;       // A variable to modify the level particle effect transparency

    GS_OGLCollide m_gsCollide;      // An object to handle collisions

    GS_Timer m_gsTimer; // Timer object

    bool m_isCreated;

    int m_snakeStartPosX; // The X coordinate in pixels where the snake should start
    int m_snakeStartPosY; // The Y coordinate in pixels where the snake should start

    int m_state;  // The state of the level (see defines above for available states)

    int m_currentLevel; // The currently loaded level

protected:

    // No protected members.

public:

    Level();
    ~Level();

    bool Create
    (
        RECT areaRect,
        bool filtered = false,
        GS_OGLColor levelColor = GS_OGLColor(1.0f, 1.0f, 1.0f, 1.0f)
    );

    void Destroy();

    // Set the level state
    void SetState( int stateNum );

    // Get the level state
    inline int GetState()
    {
        return m_snakeStartPosX;
    }

    // Get the starting X coordinates of the snake
    inline int GetSnakeStartX()
    {
        return m_snakeStartPosX;
    }

    // Get the starting Y coordinates of the snake
    inline int GetSnakeStartY()
    {
        return m_snakeStartPosY;
    }

    bool CheckCollision( RECT rcRect );

    bool Load( int nLevelNum );

    // Update the level (position, animation, etc,.)
    void Update();

    bool Render(float fAlpha = 1.0f); // Render the level
};

////////////////////////////////////////////////////////////////////////////////////////////////

#endif
