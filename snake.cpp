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


//==============================================================================================
// Include Game System (GS) header files.
// ---------------------------------------------------------------------------------------------
#include "snake.h"
#include <iostream>
//==============================================================================================


////////////////////////////////////////////////////////////////////////////////////////////////
// Constructor/Destructor Methods //////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

//==============================================================================================
// Snake::Snake():
// ---------------------------------------------------------------------------------------------
// Purpose: The constructor, initializes class data when class object is created.
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

Snake::Snake()
{
    m_isCreated = false;

    m_posX = 0;
    m_posY = 0;

    m_length = 1;

    m_snakeFrame = 0;

    m_particleColor.fRed = 0.0f;
    m_particleColor.fGreen = 1.0f;
    m_particleColor.fBlue = 1.0f;
    m_particleColor.fAlpha = 1.0f;
    m_particleAlphaMod = 0.01f;

    m_state = SNAKE_STATE_MOVING;
}

////////////////////////////////////////////////////////////////////////////////////////////////

//==============================================================================================
// Snake::~Snake():
// ---------------------------------------------------------------------------------------------
// Purpose: The de-structor, de-initializes class data when class object is destroyed.
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

Snake::~Snake()
{
    this->Destroy();
}

////////////////////////////////////////////////////////////////////////////////////////////////

//==============================================================================================
// Snake::Create():
// ---------------------------------------------------------------------------------------------
// Purpose:
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

bool Snake::Create(RECT areaRect, int startX, int startY, bool filtered, GS_OGLColor snakeColor)
{
    bool bResult;

    // Has the snake already been created?
    if( m_isCreated )
    {
        this->Destroy();
    }

    // Set the snake movement area
    m_areaRect = areaRect;

    // Set snake details
    m_posX = ( startX > -1 ? startX : m_posX );
    m_posY = ( startY > -1 ? startY : m_posY );

    // Create a sprite with 1 frames, 1 frames per line and dimensions of 16x16
    bResult = m_snakeSprite.Create("data/snake.tga", 1, 1, SNAKE_ELEMENT_WIDTH, SNAKE_ELEMENT_HEIGHT, filtered, filtered);

    // Were we not successful?
    if (FALSE == bResult)
    {
        GS_Error::Report("SNAKE.CPP", 97, "Failed to create snake sprite!");
    }

    // Create a particle using the "snake_particle.tga" image.
    bResult = m_snakeParticle.Create("data/snake_particle.tga", 1);

    // Were we not successful?
    if (FALSE == bResult)
    {
        GS_Error::Report("SNAKE.CPP", 106, "Failed to create snake particle!");
    }

    m_snakeParticle.SetScaleX(0, 1.5);
    m_snakeParticle.SetScaleY(0, 1.5);

    // Restore the default values for the particle alpha animation
    m_particleColor = snakeColor;
    m_particleAlphaMod = 0.01f;

    // Update values that could have been lost when re-creting the sprite
    m_snakeSprite.SetDestXY(m_posX, m_posY);
    m_snakeSprite.SetFrame(m_snakeFrame);

    // Reset all the snake moves
    for(int i = 0; i < SNAKE_MAX_LENGTH; i++)
    {
        m_segments[i].bIsActive  = false;
        m_segments[i].nCoordX  = -1;
        m_segments[i].nCoordY  = -1;
        m_segments[i].nDirection  = SNAKE_MOVE_NONE;
    }

    // Set the default starting length
    m_length = 2;

    // Set the postion of the available snake elements
    for(int i = 0; i < m_length; i++) {
        m_segments[i].bIsActive  = true;
        m_segments[i].nCoordX  = m_posX;
        m_segments[i].nCoordY  = m_posY;
        m_segments[i].nDirection  = SNAKE_MOVE_NONE;
    }

    // Activate the first segment
    m_segments[0].bIsActive  = true;
    m_segments[0].nCoordX  = m_posX;
    m_segments[0].nCoordY  = m_posY;
    m_segments[0].nDirection = SNAKE_MOVE_RIGHT;
    m_newDirection = SNAKE_MOVE_RIGHT;

    // Reset the timer
    m_gsTimer.Reset();

    // Remember that the snake was created
    m_isCreated = bResult;

    return bResult;
}

////////////////////////////////////////////////////////////////////////////////////////////////

//==============================================================================================
// Snake::Destroy():
// ---------------------------------------------------------------------------------------------
// Purpose: The deatroys the snake
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void Snake::Destroy()
{
    m_isCreated = false;

    m_posX = 0;
    m_posY = 0;

    m_snakeFrame = 0;

    m_state = SNAKE_STATE_MOVING;

    m_movementAmount = SNAKE_MOVEMENT_AMOUNT;
    m_movementInterval = SNAKE_MOVEMENT_INTERVAL;
    m_newDirection = SNAKE_MOVE_RIGHT;

    m_gsTimer.Reset();

    m_snakeSprite.Destroy();
}

////////////////////////////////////////////////////////////////////////////////////////////////

//==============================================================================================
// Snake::Reset():
// ---------------------------------------------------------------------------------------------
// Purpose: Reset the snake
// ---------------------------------------------------------------------------------------------
// Returns: Nothing
//==============================================================================================

void Snake::Reset(int startX, int startY, int interval)
{
    // Reset all variables to default
    m_posX = ( startX > -1 ? startX : m_posX );
    m_posY = ( startY > -1 ? startY : m_posY );

    m_snakeFrame = 0;

    // Set the defaults snake state
    m_state = SNAKE_STATE_MOVING;

    // Update the movement interval
    if( interval > SNAKE_MOVEMENT_INTERVAL_MIN ) {
        m_movementInterval = interval;
    }
    m_movementAmount = SNAKE_MOVEMENT_AMOUNT;
    m_newDirection = SNAKE_MOVE_RIGHT;

    m_snakeParticle.SetScaleX(0, 1.5);
    m_snakeParticle.SetScaleY(0, 1.5);

    // Restore the default values for the particle alpha animation
    m_particleColor.fRed = 1.0f;
    m_particleColor.fGreen = 1.0f;
    m_particleColor.fBlue = 1.0f;
    m_particleColor.fAlpha = 1.0f;
    m_particleAlphaMod = 0.01f;

    // Update values that could have been lost when re-creting the sprite
    m_snakeSprite.SetDestXY(m_posX, m_posY);
    m_snakeSprite.SetFrame(m_snakeFrame);

    // Reset all the snake moves
    for(int i = 0; i < SNAKE_MAX_LENGTH; i++)
    {
        m_segments[i].bIsActive  = false;
        m_segments[i].nCoordX  = -1;
        m_segments[i].nCoordY  = -1;
        m_segments[i].nDirection  = SNAKE_MOVE_NONE;
    }

    // Set the default starting length
    m_length = 2;

    // Set the postion of the available snake elements
    for(int i = 0; i < m_length; i++) {
        m_segments[i].bIsActive  = true;
        m_segments[i].nCoordX  = m_posX;
        m_segments[i].nCoordY  = m_posY;
        m_segments[i].nDirection  = SNAKE_MOVE_NONE;
    }

    // Activate the first segment
    m_segments[0].bIsActive  = true;
    m_segments[0].nCoordX  = m_posX;
    m_segments[0].nCoordY  = m_posY;
    m_segments[0].nDirection = SNAKE_MOVE_RIGHT;
    m_newDirection = SNAKE_MOVE_RIGHT;

    // Reset the timer
    m_gsTimer.Reset();
}

////////////////////////////////////////////////////////////////////////////////////////////////

//==============================================================================================
// Snake::SetState():
// ---------------------------------------------------------------------------------------------
// Purpose: Set the snake state
// ---------------------------------------------------------------------------------------------
// Returns: Nothing
//==============================================================================================

void Snake::SetState( int stateNum )
{
    // Can't change to the current state
    if( stateNum == m_state )
    {
        return;
    }

    // Depending on the state selected
    if( stateNum == SNAKE_STATE_MOVING )
    {
        m_snakeSprite.SetFrame(SNAKE_FRAME_MOVING);
        m_snakeFrame = SNAKE_FRAME_MOVING;

        m_snakeSprite.SetRotationPointXY(-1, -1);
        m_snakeSprite.SetRotateZ(0);

        m_snakeSprite.SetScaleXY(1.0f, 1.0f);

        m_snakeParticle.SetRotateZ(0);

        m_snakeParticle.SetScaleX(1.5f);
        m_snakeParticle.SetScaleY(1.5f);

        m_particleColor.fAlpha = 1.0f;
        m_particleAlphaMod = 0.01f;
    }
    else if( stateNum == SNAKE_STATE_GROWING )
    {
        m_particleColor.fAlpha = 1.0f;
        m_particleAlphaMod = 0.0f;
    }
    else if( stateNum == SNAKE_STATE_DYING )
    {
        m_particleColor.fAlpha = 1.0f;
        m_particleAlphaMod = 0.0f;
    }
    else if( stateNum == SNAKE_STATE_DEAD ) {
    }
    else
    {
        return;
    }

    // Remember the snake state
    m_state = stateNum;
}

////////////////////////////////////////////////////////////////////////////////////////////////

//==============================================================================================
// Snake::SetState():
// ---------------------------------------------------------------------------------------------
// Purpose: Grow the snake a given number of segments
// ---------------------------------------------------------------------------------------------
// Returns: true if snake could grow false if not
//==============================================================================================

bool Snake::Grow( int numSegments ) {

    // Do not grow the snake more than the maximum allowed length
    if( (m_length + numSegments ) > SNAKE_MAX_LENGTH ) return false;

    // Activate the indicated segments
    for( int i = m_length; i < m_length + numSegments; i++ ) {
        m_segments[i].bIsActive  = true;
        m_segments[i].nCoordX  = m_segments[m_length-1].nCoordX;
        m_segments[i].nCoordY  = m_segments[m_length-1].nCoordY;
        m_segments[i].nDirection  = SNAKE_MOVE_NONE;
    }

    // Update the snake length
    m_length = m_length + numSegments;

    // Set the state of the snake to growing
    this->SetState( SNAKE_STATE_GROWING );

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////

//==============================================================================================
// Snake::GetDestRect():
// ---------------------------------------------------------------------------------------------
// Purpose: Get the screen coordinates of the collision area of the snake
// ---------------------------------------------------------------------------------------------
// Returns: Nothing
//==============================================================================================

void Snake::GetCollideRect(RECT* pSourceRect)
{
    SetRect(
        pSourceRect,
        m_segments[0].nCoordX,
        m_segments[0].nCoordY + SNAKE_ELEMENT_HEIGHT - 1,
        m_segments[0].nCoordX + SNAKE_ELEMENT_WIDTH - 1,
        m_segments[0].nCoordY
    );
}

////////////////////////////////////////////////////////////////////////////////////////////////

//==============================================================================================
// Snake::CheckCollision():
// ---------------------------------------------------------------------------------------------
// Purpose: To check whether a target rectangle has collided with the snakes head or body
// ---------------------------------------------------------------------------------------------
// Returns: 0 if no collision, 1 if collision with head, 2 if collision with body
//==============================================================================================

int Snake::CheckCollision(RECT rcTarget, BOOL bExcludeHead)
{
    // For every snake segment
    for( int i = 0; i < m_length; i++ ) {
        // Only check active segments and segments that have started moving
        if( (true != m_segments[i].bIsActive) || (SNAKE_MOVE_NONE == m_segments[i].nDirection) ) break;

        // Skip the head if not included
        if( (TRUE == bExcludeHead) && (i == 0) ) continue;

        // Get the segment rectangle
        RECT rcSegment;
        SetRect(
            &rcSegment,
            m_segments[i].nCoordX,
            m_segments[i].nCoordY + SNAKE_ELEMENT_HEIGHT - 1,
            m_segments[i].nCoordX + SNAKE_ELEMENT_WIDTH - 1,
            m_segments[i].nCoordY
        );

// if(  i == 0 ) {
// std::cout << rcSegment.left << "," << rcSegment.top << "," << rcSegment.right << "," << rcSegment.bottom << "\n";
// }

        // Is there a collsion with the target rectangle?
        if( TRUE == m_gsCollide.IsRectOnRect(rcTarget, rcSegment) ) {
            // Was it the head?
            if( i == 0 ) {
                return SNAKE_COLLIDE_HEAD;
            }
            else {
                return SNAKE_COLLIDE_BODY;
            }
        }
    }

    return SNAKE_COLLIDE_NONE;
}


////////////////////////////////////////////////////////////////////////////////////////////////

//==============================================================================================
// Snake::SetMovementDirection():
// ---------------------------------------------------------------------------------------------
// Purpose: Set the snakes movement direction
// ---------------------------------------------------------------------------------------------
// Returns: Nothing
//==============================================================================================

void Snake::SetMovementDirection( int direction ) {
    // Is it not a valid direction?
    if( (direction < 0) || (direction > 3) ) return;

    // Prevent moving in the opposite direction
    if ( ((direction == SNAKE_MOVE_UP) && (m_segments[0].nDirection == SNAKE_MOVE_DOWN)) ||
         ((direction == SNAKE_MOVE_DOWN) && (m_segments[0].nDirection == SNAKE_MOVE_UP)) ||
         ((direction == SNAKE_MOVE_LEFT) && (m_segments[0].nDirection == SNAKE_MOVE_RIGHT)) ||
         ((direction == SNAKE_MOVE_RIGHT) && (m_segments[0].nDirection == SNAKE_MOVE_LEFT)) ) {
        return;
    }

    // Remember the new movement direction
    m_newDirection = direction;
}

////////////////////////////////////////////////////////////////////////////////////////////////

//==============================================================================================
// Snake::Update():
// ---------------------------------------------------------------------------------------------
// Purpose: Update the snake (position, animation, etc,.)
// ---------------------------------------------------------------------------------------------
// Returns: Nothing
//==============================================================================================

void Snake::Update()
{
    // Get current position of snake
    // m_posX = m_snakeSprite.GetDestX();
    // m_posY = m_snakeSprite.GetDestY();

    // Depending on the snake state
    if( m_state == SNAKE_STATE_MOVING ) {
        // Is the particle alpha out of bounds?
        if( (m_particleColor.fAlpha > 1.0) || (m_particleColor.fAlpha < 0.5) ) {
            // Reverse particle animation
            m_particleAlphaMod = m_particleAlphaMod * -1;
        }
        // Set the snake color to create pulsing effect
        m_particleColor.fAlpha = m_particleColor.fAlpha + m_particleAlphaMod;
    }
    else if( m_state == SNAKE_STATE_GROWING ) {
        // Fade the snake color
        m_particleColor.fAlpha = m_particleColor.fAlpha - m_particleAlphaMod;
    }
    else if( m_state == SNAKE_STATE_DYING ) {
        // Fade the snake color
        m_particleColor.fAlpha = m_particleColor.fAlpha - m_particleAlphaMod;

        // Has the snake faded completely?
        if( m_particleColor.fAlpha <= -1.0f ) {
            this->SetState( SNAKE_STATE_DEAD );
        }
    }

    // Is the snake moving and is it time to move?
    if( ((m_state == SNAKE_STATE_MOVING) || (m_state == SNAKE_STATE_GROWING)) && (m_gsTimer.GetTime() >= m_movementInterval) ) {
        // Set new direction, if any
        m_segments[0].nDirection = m_newDirection;

        // Adjust all the snake coordinates
        for( int i = 0; i < m_length; i++ ) {
            // Only move the active segments that have a direction specified
            if( (true != m_segments[i].bIsActive) && (m_segments[i].nDirection != SNAKE_MOVE_NONE) ) break;

            // Get the current position of the segment
            int posX = m_segments[i].nCoordX;
            int posY = m_segments[i].nCoordY;

            // Depending on the direction the snake is moving in
            if( m_segments[i].nDirection == SNAKE_MOVE_UP ) {
                posY = posY + m_movementAmount;
            }
            else if( m_segments[i].nDirection == SNAKE_MOVE_RIGHT ) {
                posX = posX + m_movementAmount;
            }
            else if( m_segments[i].nDirection == SNAKE_MOVE_DOWN ) {
                posY = posY - m_movementAmount;
            }
            else if( m_segments[i].nDirection == SNAKE_MOVE_LEFT ) {
                posX = posX - m_movementAmount;
            }

            // Wrap the snake if it move outside of the play area
            if( posY + SNAKE_ELEMENT_HEIGHT > m_areaRect.top ) {
                posY = m_areaRect.bottom;
            }
            else if( posX + SNAKE_ELEMENT_WIDTH > m_areaRect.right ) {
                posX = m_areaRect.left;
            }
            else if( posY < m_areaRect.bottom ) {
                posY = m_areaRect.top - (m_areaRect.top % SNAKE_ELEMENT_HEIGHT) - SNAKE_ELEMENT_HEIGHT;
            }
            else if( posX < m_areaRect.left ) {
                posX = m_areaRect.right - (m_areaRect.right % SNAKE_ELEMENT_WIDTH) - SNAKE_ELEMENT_WIDTH;
            }

            // Save the new coordinates
            m_segments[i].nCoordX = posX;
            m_segments[i].nCoordY = posY;
        }

        // Adjust the direction so that next time, the next element will move the same way as the current element did
        for( int i = m_length - 1; i > 0; i-- ) {
            // Only update the directions of the active segments
            if( true != m_segments[i].bIsActive ) continue;

            // The next segment should have the move direction of the previous segment
            m_segments[i].nDirection = m_segments[i-1].nDirection;

        }

        // Has the snake finished gowing (i.e., has the last segment started moving)?
        if( (SNAKE_STATE_GROWING == m_state) && (m_segments[m_length-1].nDirection != SNAKE_MOVE_NONE) ) {
            // Set the snake state to moving again
            this->SetState( SNAKE_STATE_MOVING );

            // Set a random color for the snake
            m_particleColor.fRed = (float(rand()%256) / 256.0f) + 0.0f;
            m_particleColor.fGreen = (float(rand()%256) / 256.0f) + 0.0f;
            m_particleColor.fBlue = (float(rand()%256) / 256.0f) + 0.0f;

            // Make certain one of the colors is always fully bright
            int randomColor = (rand()%3);
            if( randomColor == 0 ) {
                m_particleColor.fRed = 1.0f;
            }
            else if( randomColor == 1 ) {
                m_particleColor.fGreen = 1.0f;
            }
            else if( randomColor == 2 ) {
                m_particleColor.fBlue = 1.0f;
            }
        }

        // Update the snake position
        // m_snakeSprite.SetDestXY(m_posX, m_posY);

        // Reset the timer
        m_gsTimer.Reset();
    }
    else if( m_state == SNAKE_STATE_GROWING ) {
        // Cycle through random colors to create shimmer effect
        m_particleColor.fRed = (float(rand()%256) / 256.0f) + 0.0f;
        m_particleColor.fGreen = (float(rand()%256) / 256.0f) + 0.0f;
        m_particleColor.fBlue = (float(rand()%256) / 256.0f) + 0.0f;

        // Grow the particle to cause a shimmering effect when combined with the random colors
        // m_snakeParticle.AddScaleX( 0.01 );
        m_snakeParticle.AddScaleY( 0.02 );

        // if( m_snakeParticle.GetScaleX(0) > 1.5f ) {
            // m_snakeParticle.AddRotateZ( 15 );
            // m_particleAlphaMod = 0.05;
        // }
    }
    else if( m_state == SNAKE_STATE_DYING ) {
        // Create a mini color explosion effect
        if( m_snakeParticle.GetScaleX(0) > 2.0f ) {
            // m_snakeParticle.AddRotateZ( 20 );
            m_particleAlphaMod = 0.05;
        }
        else {
            // Simulate a little explosion
            m_snakeParticle.AddScaleX(0, 0.1 );
            m_snakeParticle.AddScaleY(0, 0.2 );
        }
    }

    return;
}

////////////////////////////////////////////////////////////////////////////////////////////////

//==============================================================================================
// Snake::Render():
// ---------------------------------------------------------------------------------------------
// Purpose: Draws the snake
// ---------------------------------------------------------------------------------------------
// Returns: True if rendering was successful, false if not
//==============================================================================================

bool Snake::Render(float fAlpha)
{
    // Set the alpha value to determine the transparency
    m_snakeSprite.SetModulateColor(-1.0f, -1.0f, -1.0f, (fAlpha < 0.5f ? fAlpha : 0.5f));

    // Set the color for the particle
    GS_OGLColor color = m_particleColor;

    // Render the body of the snake
    for( int i = 0; i < m_length; i++ ) {
        // Only render the active segments
        if( true != m_segments[i].bIsActive ) break;

        // The following code enables smooth movement for the snake (but it needs a lot of work)
        float modX = 0;
        float modY = 0;

        float mod = m_gsTimer.GetTime() / m_movementInterval;

//        // Is the snake not dead or dying?
//        if( m_state < SNAKE_STATE_DYING ) {
//            if( m_segments[i].nDirection == SNAKE_MOVE_UP ) {
//                modY = round(mod * SNAKE_ELEMENT_HEIGHT);
//            }
//            else if( m_segments[i].nDirection == SNAKE_MOVE_DOWN ) {
//                modY = round(-mod * SNAKE_ELEMENT_HEIGHT);
//            }
//            else if( m_segments[i].nDirection == SNAKE_MOVE_RIGHT ) {
//                modX = round(mod * SNAKE_ELEMENT_WIDTH);
//            }
//            else if( m_segments[i].nDirection == SNAKE_MOVE_LEFT ) {
//                modX = round(-mod * SNAKE_ELEMENT_WIDTH);
//            }
//        }

        // Set the destination of the specified snake segment
        m_snakeSprite.SetDestXY( m_segments[i].nCoordX + modX, m_segments[i].nCoordY + modY );

        // Render the snake segment
        m_snakeSprite.Render();

        // Skip the particale rendering for non-moving segments
        if( m_segments[i].nDirection < 0 ) continue;

        // Render the particle effect centered on the segment
        m_snakeParticle.SetDestX(0, m_segments[i].nCoordX + modX - ((m_snakeParticle.GetScaledWidth(0) - SNAKE_ELEMENT_WIDTH) / 2));
        m_snakeParticle.SetDestY(0, m_segments[i].nCoordY + modY - ((m_snakeParticle.GetScaledHeight(0) - SNAKE_ELEMENT_HEIGHT) /2));

        // Depedning on the snake state
        if( m_state == SNAKE_STATE_MOVING ) {
            // Don't fade the snake's head
            if( i > 0 ) {
                // Fade the particle effect color towards the end of the snake
                color.fRed = color.fRed - (color.fRed / m_length);
                color.fGreen = color.fGreen - (color.fGreen / m_length);
                color.fBlue = color.fBlue - (color.fBlue / m_length);
            }
        }
        else if( m_state == SNAKE_STATE_GROWING ) {
//            // Set random colors for drawing the particle
//            color.fRed = (float(rand()%256) / 256.0f) + 0.0f;
//            color.fGreen = (float(rand()%256) / 256.0f) + 0.0f;
//            color.fBlue = (float(rand()%256) / 256.0f) + 0.0f;
        }

        // Adjust the particle effect alpha if a render alpha was given
        if( fAlpha < color.fAlpha) {
            color.fAlpha = fAlpha;
        }

        // Set the particle effect color
        m_snakeParticle.SetModulateColor(color.fRed, color.fGreen, color.fBlue, color.fAlpha);

        // Render the particle effect over the segment
        m_snakeParticle.Render();
    }

    // Reset the snake to the head coordinates
    // m_snakeSprite.SetDestXY( m_posX, m_posY );

    return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////
