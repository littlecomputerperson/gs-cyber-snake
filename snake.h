/*============================================================================================+
 | Game System (GS) Library                                                                   |
 |--------------------------------------------------------------------------------------------|
 | FILES: snake.cpp, snake.h                                                                  |
 |--------------------------------------------------------------------------------------------|
 | CLASS: Snake                                                                               |
 |--------------------------------------------------------------------------------------------|
 | ABOUT: Class to create and render a snake  :-)                                             |
 |--------------------------------------------------------------------------------------------|
 |                                                                                    03/2023 |
 +============================================================================================*/


#ifndef SNAKE_H
#define SNAKE_H


//================================================================================================
// Include Game System (GS) header files.
// -----------------------------------------------------------------------------------------------
#include "gs_list.h"
#include "gs_ogl_texture.h"
#include "gs_ogl_sprite_ex.h"
#include "gs_ogl_particle.h"
#include "gs_ogl_collide.h"
#include "gs_timer.h"
//================================================================================================


//================================================================================================
// Include standard C library header files.
// -----------------------------------------------------------------------------------------------
#include <iostream>
#include <time.h>
//================================================================================================


//==============================================================================================
// Snake defines.
// ---------------------------------------------------------------------------------------------
#define SNAKE_MAX_LENGTH    200
#define SNAKE_MOVE_NONE     -1
#define SNAKE_MOVE_UP       0
#define SNAKE_MOVE_RIGHT    1
#define SNAKE_MOVE_DOWN     2
#define SNAKE_MOVE_LEFT     3
// ---------------------------------------------------------------------------------------------
#define SNAKE_ELEMENT_WIDTH  16
#define SNAKE_ELEMENT_HEIGHT 16
// ---------------------------------------------------------------------------------------------
#define SNAKE_FRAME_MOVING 0
// ---------------------------------------------------------------------------------------------
#define SNAKE_STATE_MOVING  0
#define SNAKE_STATE_GROWING 1
#define SNAKE_STATE_DYING   2
#define SNAKE_STATE_DEAD    3
// ---------------------------------------------------------------------------------------------
#define SNAKE_MOVEMENT_AMOUNT       16
#define SNAKE_MOVEMENT_INTERVAL     100
#define SNAKE_MOVEMENT_INTERVAL_MIN 10
#define SNAKE_MOVEMENT_INTERVAL_MAX 100
// ---------------------------------------------------------------------------------------------
#define SNAKE_COLLIDE_NONE  0
#define SNAKE_COLLIDE_HEAD  1
#define SNAKE_COLLIDE_BODY  2
// ---------------------------------------------------------------------------------------------

//==============================================================================================
// Snake structures
// ---------------------------------------------------------------------------------------------
typedef struct SNAKE_SEGMENTS
{
    bool bIsActive;     // Whether the segment is active or not
    int  nCoordX;       // The x coordinate of the segment
    int  nCoordY;       // The y coordinate of the segment
    int  nDirection;    // The direction in which the segment should move
                        // (see definitaions above for available directions)
} SnakeSegments;
//==============================================================================================


////////////////////////////////////////////////////////////////////////////////////////////////
// Class Definition. ///////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


class Snake
{

private:

    GS_OGLSpriteEx m_snakeSprite;   // The sprite object used to draw the snake

    GS_OGLParticle m_snakeParticle; // A particle object used for snake particle effects
    GS_OGLColor m_particleColor;    // The snake particle effect color
    float m_particleAlphaMod;       // A variable to modify the snake particle effect transparency

    GS_OGLCollide m_gsCollide; // An object to handle collisions

    GS_Timer m_gsTimer; // Timer object

    bool m_isCreated;

    int m_posX; // The X coordinate in pixels of the snake head
    int m_posY; // The Y coordinate in pixels of the snake head

    int m_length; // The length of the snake

    int m_movementAmount;    // The amount which the snake should move every movement interval
    int m_movementInterval;  // The interval (in ms) at which the snake should move
    int m_newDirection;      // The new direction in which the snake should move

    int m_snakeFrame; // The currently selected snake frame (see defines above for available frames)

    int m_state;  // The state of the snake (see defines above for available states)

    SnakeSegments m_segments[SNAKE_MAX_LENGTH];

    float m_velocityX;
    float m_velocityY;

    RECT m_areaRect; // A rectangle indicating the cooridates of the area in which the snake can move

    // Set the animation fram for the snake sprite
    inline void SetFrame( int frameNum )
    {
        m_snakeSprite.SetFrame(frameNum);
    }

protected:

    // No protected members.

public:

    Snake();
    ~Snake();

    bool Create
    (
        RECT areaRect,
        int startX = -1,
        int startY = -1,
        bool filtered = false,
        GS_OGLColor snakeColor = GS_OGLColor(1.0f, 1.0f, 1.0f, 1.0f)
    );

    void Destroy();

    // Reset the snake
    void Reset(int startX = -1, int startY = -1, int interval = -1);

    // Set the snake state
    void SetState( int stateNum );

    // Get the snake state
    inline int GetState()
    {
        return m_state;
    }

    // Grow the snake with a number of segments
    bool Grow( int numSegments = 5 );

    // Speed the snake up
    inline void Speedup( int interval = 5 ) {
        if( m_movementInterval > SNAKE_MOVEMENT_INTERVAL_MIN ) {
            m_movementInterval = m_movementInterval - interval;
        }
    }

    // Slow the snake down
    inline void Slowdown( int interval = 5 ) {
        if( m_movementInterval < SNAKE_MOVEMENT_INTERVAL_MAX ) {
            m_movementInterval = m_movementInterval - interval;
        }
    }

    // Set the position for the snake sprite
    inline void SetPosition( int x, int y )
    {
        m_snakeSprite.SetDestXY(x, y);
    }

    inline int GetDestX()
    {
        return m_snakeSprite.GetDestX();
    }

    inline int GetDestY()
    {
        return m_snakeSprite.GetDestY();
    }

    inline int GetWidth()
    {
        return m_snakeSprite.GetFrameWidth();
    }

    inline int GetHeight()
    {
        return m_snakeSprite.GetFrameHeight();
    }

    // Collision functions
    void GetCollideRect(RECT* pSourceRect);
    int CheckCollision(RECT rcTarget, BOOL bExcludeHead = false);

    // Update the movement direction of the snake
    void SetMovementDirection( int direction );
    inline int GetMovementDirection()
    {
        return m_segments[0].nDirection;
    }

    // Update the snake (position, animation, etc,.)
    void Update();

    bool Render(float fAlpha = 1.0f); // Render the snake
};

////////////////////////////////////////////////////////////////////////////////////////////////

#endif
