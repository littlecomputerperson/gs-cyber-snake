/*============================================================================================+
 | Game System (GS) Library                                                                   |
 |--------------------------------------------------------------------------------------------|
 | FILES: rat.cpp, rat.h                                                                      |
 |--------------------------------------------------------------------------------------------|
 | CLASS: Rat                                                                                 |
 |--------------------------------------------------------------------------------------------|
 | ABOUT: Class to create and render a rat  :-)                                               |
 |--------------------------------------------------------------------------------------------|
 |                                                                                    03/2023 |
 +============================================================================================*/


#ifndef RAT_H
#define RAT_H


//================================================================================================
// Include Game System (GS) header files.
// -----------------------------------------------------------------------------------------------
#include "gs_list.h"
#include "gs_ogl_texture.h"
#include "gs_ogl_sprite_ex.h"
#include "gs_ogl_particle.h"
#include "gs_timer.h"
//================================================================================================


//================================================================================================
// Include standard C library header files.
// -----------------------------------------------------------------------------------------------
#include <iostream>
#include <time.h>
//================================================================================================


//==============================================================================================
// Rat defines.
// ---------------------------------------------------------------------------------------------
#define RAT_MOVE_NONE    -1
#define RAT_MOVE_UP       0
#define RAT_MOVE_RIGHT    1
#define RAT_MOVE_DOWN     2
#define RAT_MOVE_LEFT     3
// ---------------------------------------------------------------------------------------------
#define RAT_ELEMENT_WIDTH  16
#define RAT_ELEMENT_HEIGHT 16
// ---------------------------------------------------------------------------------------------
#define RAT_FRAME_MOVING 0
// ---------------------------------------------------------------------------------------------
#define RAT_STATE_STATIC 0
#define RAT_STATE_MOVING 1
#define RAT_STATE_EATEN  2
#define RAT_STATE_DEAD   3
// ---------------------------------------------------------------------------------------------
#define RAT_MOVEMENT_AMOUNT   16
#define RAT_MOVEMENT_INTERVAL 500
#define RAT_MOVEMENT_INTERVAL_MIN 100
#define RAT_MOVEMENT_INTERVAL_MAX 500
// ---------------------------------------------------------------------------------------------

//==============================================================================================
// Rat structures
// ---------------------------------------------------------------------------------------------
typedef struct RAT_SEGMENTS
{
    bool bIsActive;     // Whether the segment is active or not
    int  nCoordX;       // The x coordinate of the segment
    int  nCoordY;       // The y coordinate of the segment
    int  nDirection;    // The direction in which the segment should move
                        // (see definitaions above for available directions)
} RatSegments;
//==============================================================================================


////////////////////////////////////////////////////////////////////////////////////////////////
// Class Definition. ///////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


class Rat
{

private:

    GS_OGLSpriteEx m_ratSprite;

    GS_OGLParticle m_ratParticle;
    GS_OGLColor m_particleColor;
    float m_particleAlphaMod;

    GS_Timer m_gsTimer; // Timer object

    bool m_isCreated;

    int m_posX; // The X coordinate in pixels of the rat head
    int m_posY; // The Y coordinate in pixels of the rat head

    int m_length; // The length of the rat

    int m_movementAmount;    // The amount which the rat should move every movement interval
    int m_movementInterval;  // The interval (in ms) at which the rat should move
    int m_newDirection;      // The new direction in which the rat should move

    int m_ratFrame; // The currently selected rat frame (see defines above for available frames)

    int m_state;  // The state of the rat (see defines above for available states)

    float m_velocityX;
    float m_velocityY;

    RECT m_areaRect; // A rectangle indicating the cooridates of the area in which the rat can move

    // Set the animation fram for the rat sprite
    inline void SetFrame( int frameNum )
    {
        m_ratSprite.SetFrame(frameNum);
    }

protected:

    // No protected members.

public:

    Rat();
    ~Rat();

    bool Create
    (
        RECT areaRect,
        int startX = -1,
        int startY = -1,
        bool filtered = false,
        GS_OGLColor ratColor = GS_OGLColor(1.0f, 0.0f, 0.0f, 1.0f)
    );

    void Destroy();

    // Reset the rat
    void Reset(int startX = -1, int startY = -1, int interval = -1);

    // Set the rat state
    void SetState( int stateNum );

    // Get the rat state
    inline int GetState()
    {
        return m_state;
    }

    // Set the position for the rat sprite
    inline void SetPosition( int x, int y )
    {
        m_ratSprite.SetDestXY(x, y);
    }

    inline int GetDestX()
    {
        return m_ratSprite.GetDestX();
    }

    inline int GetDestY()
    {
        return m_ratSprite.GetDestY();
    }

    inline int GetWidth()
    {
        return m_ratSprite.GetFrameWidth();
    }

    inline int GetHeight()
    {
        return m_ratSprite.GetFrameHeight();
    }

    // Speed the rat up
    inline void Speedup( int interval = 30 ) {
        if( m_movementInterval > RAT_MOVEMENT_INTERVAL_MIN ) {
            m_movementInterval = m_movementInterval - interval;
        }
    }

    // Slow the rat down
    inline void Slowdown( int interval = 30 ) {
        if( m_movementInterval < RAT_MOVEMENT_INTERVAL_MAX ) {
            m_movementInterval = m_movementInterval - interval;
        }
    }

    void GetCollideRect(RECT* pSourceRect, int nMoveDirection = -1);

    // Update the movement direction of the rat
    void SetMovementDirection( int direction );

    // Update the rat (position, animation, etc,.)
    void Update();

    bool Render(float fAlpha = 1.0f); // Render the rat
};

////////////////////////////////////////////////////////////////////////////////////////////////

#endif
