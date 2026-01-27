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


//==============================================================================================
// Include Game System (GS) header files.
// ---------------------------------------------------------------------------------------------
#include "rat.h"
#include <iostream>
//==============================================================================================


////////////////////////////////////////////////////////////////////////////////////////////////
// Constructor/Destructor Methods //////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

//==============================================================================================
// Rat::Rat():
// ---------------------------------------------------------------------------------------------
// Purpose: The constructor, initializes class data when class object is created.
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

Rat::Rat()
{
    m_isCreated = false;

    m_posX = 0;
    m_posY = 0;

    m_length = 1;

    m_ratFrame = 0;

    m_particleColor.fRed = 1.0f;
    m_particleColor.fGreen = 0.0f;
    m_particleColor.fBlue = 1.0f;
    m_particleColor.fAlpha = 1.0f;
    m_particleAlphaMod = 0.01f;

    m_state = RAT_STATE_STATIC;
}

////////////////////////////////////////////////////////////////////////////////////////////////

//==============================================================================================
// Rat::~Rat():
// ---------------------------------------------------------------------------------------------
// Purpose: The de-structor, de-initializes class data when class object is destroyed.
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

Rat::~Rat()
{
    this->Destroy();
}

////////////////////////////////////////////////////////////////////////////////////////////////

//==============================================================================================
// Rat::Create():
// ---------------------------------------------------------------------------------------------
// Purpose:
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

bool Rat::Create(RECT areaRect, int startX, int startY, bool filtered, GS_OGLColor ratColor)
{
    bool bResult;

    // Has the rat already been created?
    if( m_isCreated )
    {
        this->Destroy();
    }

    // Set the rat movement area
    m_areaRect = areaRect;

    // Set rat details
    m_posX = ( startX > -1 ? startX : m_posX );
    m_posY = ( startY > -1 ? startY : m_posY );

    // Create a sprite with 1 frames, 1 frames per line and dimensions of 16x16
    bResult = m_ratSprite.Create("data/rat.tga", 1, 1, RAT_ELEMENT_WIDTH, RAT_ELEMENT_HEIGHT, filtered, filtered);

    // Were we not successful?
    if (FALSE == bResult)
    {
        GS_Error::Report("RAT.CPP", 97, "Failed to create rat sprite!");
    }

    // Create a particle using the "rat_particle.tga" image.
    bResult = m_ratParticle.Create("data/rat_particle.tga", 1);

    // Were we not successful?
    if (FALSE == bResult)
    {
        GS_Error::Report("RAT.CPP", 106, "Failed to create rat particle!");
    }

    m_ratParticle.SetScaleX(0, 1.5);
    m_ratParticle.SetScaleY(0, 1.5);

    // Restore the default values for the particle alpha animation
    m_particleColor = ratColor;
    m_particleAlphaMod = 0.05f;

    // Update values that could have been lost when re-creting the sprite
    m_ratSprite.SetDestXY(m_posX, m_posY);
    m_ratSprite.SetFrame(m_ratFrame);

    // Set the default state for the rat
    m_state = RAT_STATE_STATIC;

    // Set the initial move direction
    m_newDirection = RAT_MOVE_LEFT;

    // Reset the timer
    m_gsTimer.Reset();

    // Remember that the rat was created
    m_isCreated = bResult;

    return bResult;
}

////////////////////////////////////////////////////////////////////////////////////////////////

//==============================================================================================
// Rat::Destroy():
// ---------------------------------------------------------------------------------------------
// Purpose: The deatroys the rat
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void Rat::Destroy()
{
    m_isCreated = false;

    m_posX = 0;
    m_posY = 0;

    m_ratFrame = 0;

    m_state = RAT_STATE_MOVING;

    m_movementAmount = RAT_MOVEMENT_AMOUNT;
    m_movementInterval = RAT_MOVEMENT_INTERVAL;
    m_newDirection = RAT_MOVE_RIGHT;

    m_gsTimer.Reset();

    m_ratSprite.Destroy();
}

////////////////////////////////////////////////////////////////////////////////////////////////

//==============================================================================================
// Rat::Reset():
// ---------------------------------------------------------------------------------------------
// Purpose: Reset the snake
// ---------------------------------------------------------------------------------------------
// Returns: Nothing
//==============================================================================================

void Rat::Reset(int startX, int startY, int interval)
{
    // Reset all variables to default
    m_posX = ( startX > -1 ? startX : m_posX );
    m_posY = ( startY > -1 ? startY : m_posY );

    // Restore the default values for the particle alpha animation
    m_ratParticle.SetScaleX(0, 1.5);
    m_ratParticle.SetScaleY(0, 1.5);

    m_particleColor.fRed = 1.0f;
    m_particleColor.fGreen = 0.0f;
    m_particleColor.fBlue = 0.0f;
    m_particleColor.fAlpha = 1.0f;
    m_particleAlphaMod = 0.05f;

    // Update values that could have been lost when re-creting the sprite
    m_ratSprite.SetDestXY(m_posX, m_posY);
    m_ratSprite.SetFrame(m_ratFrame);

    // Set the default state for the rat
    m_state = RAT_STATE_STATIC;

    // Update the movement interval
    if( interval > RAT_MOVEMENT_INTERVAL_MIN ) {
        m_movementInterval = interval;
    }

    // Set the initial move direction
    m_movementAmount = RAT_MOVEMENT_AMOUNT;
    m_newDirection = RAT_MOVE_LEFT;

    // Reset the timer
    m_gsTimer.Reset();
}

////////////////////////////////////////////////////////////////////////////////////////////////

//==============================================================================================
// Rat::SetState():
// ---------------------------------------------------------------------------------------------
// Purpose: Set the rat state
// ---------------------------------------------------------------------------------------------
// Returns: Nothing
//==============================================================================================

void Rat::SetState( int stateNum )
{
    // Can't change to the current state
    if( stateNum == m_state )
    {
        return;
    }

    // Depending on the state selected
    if( (stateNum == RAT_STATE_MOVING) || (stateNum == RAT_STATE_STATIC) )
    {
        m_ratSprite.SetFrame(RAT_FRAME_MOVING);
        m_ratFrame = RAT_FRAME_MOVING;

        m_ratSprite.SetRotationPointXY(-1, -1);
        m_ratSprite.SetRotateZ(0);

        m_ratSprite.SetScaleXY(1.0f, 1.0f);

        m_ratParticle.SetRotateZ(0);

        m_ratParticle.SetScaleX(1.5f);
        m_ratParticle.SetScaleY(1.5f);

        m_particleColor.fAlpha = 1.0f;
        m_particleAlphaMod = 0.05f;
    }
    else if( stateNum == RAT_STATE_EATEN )
    {
        m_particleColor.fAlpha = 1.0f;
        m_particleAlphaMod = 0.0f;
    }
    else if( stateNum == RAT_STATE_DEAD )
    {
        m_particleColor.fAlpha = 0.0f;
        m_particleAlphaMod = 0.0f;
    }
    else
    {
        return;
    }

    // Remember the rat state
    m_state = stateNum;
}

////////////////////////////////////////////////////////////////////////////////////////////////

//==============================================================================================
// Rat::GetDestRect():
// ---------------------------------------------------------------------------------------------
// Purpose: Get the screen coordinates of the collision area of the rat
// ---------------------------------------------------------------------------------------------
// Returns: Nothing
//==============================================================================================

void Rat::GetCollideRect(RECT* pSourceRect, int nMoveDirection)
{
    // Is the rat static?
    if( nMoveDirection <  0 ) {
        // Return the current position of the rat
        m_ratSprite.GetDestRect(pSourceRect);
    }
    else {
        // Get the current position of the rat
        int posX = m_ratSprite.GetDestX();
        int posY = m_ratSprite.GetDestY();

        // Depending on the direction the rat is moving in
        if( nMoveDirection == RAT_MOVE_UP ) {
            posY = posY + m_movementAmount;
        }
        else if( nMoveDirection == RAT_MOVE_RIGHT ) {
            posX = posX + m_movementAmount;
        }
        else if( nMoveDirection == RAT_MOVE_DOWN ) {
            posY = posY - m_movementAmount;
        }
        else if( nMoveDirection == RAT_MOVE_LEFT ) {
            posX = posX - m_movementAmount;
        }

        // Wrap the rat if it move outside of the play area
        if( posY + RAT_ELEMENT_HEIGHT > m_areaRect.top ) {
            posY = m_areaRect.bottom;
        }
        else if( posX + RAT_ELEMENT_WIDTH > m_areaRect.right ) {
            posX = m_areaRect.left;
        }
        else if( posY < m_areaRect.bottom ) {
            posY = m_areaRect.top - (m_areaRect.top % RAT_ELEMENT_HEIGHT) - RAT_ELEMENT_HEIGHT;
        }
        else if( posX < m_areaRect.left ) {
            posX = m_areaRect.right - (m_areaRect.right % RAT_ELEMENT_WIDTH) - RAT_ELEMENT_WIDTH;
        }

        // Get the position rectangle
        SetRect(
            pSourceRect,
            posX,
            posY + RAT_ELEMENT_HEIGHT - 1,
            posX + RAT_ELEMENT_WIDTH - 1,
            posY
        );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////

//==============================================================================================
// Rat::SetMovementDirection():
// ---------------------------------------------------------------------------------------------
// Purpose: Set the rats movement direction
// ---------------------------------------------------------------------------------------------
// Returns: Nothing
//==============================================================================================

void Rat::SetMovementDirection( int direction ) {
    // Is it not a valid direction?
    if( (direction < 0) || (direction > 3) ) return;

    // Remember the new movement direction
    m_newDirection = direction;
}

////////////////////////////////////////////////////////////////////////////////////////////////

//==============================================================================================
// Rat::Update():
// ---------------------------------------------------------------------------------------------
// Purpose: Update the rat (movement, animation, etc,.)
// ---------------------------------------------------------------------------------------------
// Returns: Nothing
//==============================================================================================

void Rat::Update()
{
    // Update the particle effect depending on the stale
    if( (m_state == RAT_STATE_MOVING) || (m_state == RAT_STATE_STATIC) ) {
        // Is the particle alpha out of bounds?
        if( (m_particleColor.fAlpha > 1.0) || (m_particleColor.fAlpha < 0.0) ) {
            // Reverse particle animation
            m_particleAlphaMod = m_particleAlphaMod * -1;
        }
        m_particleColor.fAlpha = m_particleColor.fAlpha + m_particleAlphaMod;
    }
    else if( m_state == RAT_STATE_EATEN ) {
        // Fade out the particle
        m_particleColor.fAlpha = m_particleColor.fAlpha - m_particleAlphaMod;

        // Has the rat faded away completely
        if( m_particleColor.fAlpha <= 0.0f ) {
            this->SetState( RAT_STATE_DEAD );
        }
    }

    // Is the rat moving and is it time to move?
    if( (m_state == RAT_STATE_MOVING) && (m_gsTimer.GetTime() >= m_movementInterval) ) {

        // Get the current position of the rat
        int posX = m_ratSprite.GetDestX();
        int posY = m_ratSprite.GetDestY();

        // Depending on the direction the rat is moving in
        if( m_newDirection == RAT_MOVE_UP ) {
            posY = posY + m_movementAmount;
        }
        else if( m_newDirection == RAT_MOVE_RIGHT ) {
            posX = posX + m_movementAmount;
        }
        else if( m_newDirection == RAT_MOVE_DOWN ) {
            posY = posY - m_movementAmount;
        }
        else if( m_newDirection == RAT_MOVE_LEFT ) {
            posX = posX - m_movementAmount;
        }

        // Wrap the rat if it move outside of the play area
        if( posY + RAT_ELEMENT_HEIGHT > m_areaRect.top ) {
            posY = m_areaRect.bottom;
        }
        else if( posX + RAT_ELEMENT_WIDTH > m_areaRect.right ) {
            posX = m_areaRect.left;
        }
        else if( posY < m_areaRect.bottom ) {
            posY = m_areaRect.top - (m_areaRect.top % RAT_ELEMENT_HEIGHT) - RAT_ELEMENT_HEIGHT;
        }
        else if( posX < m_areaRect.left ) {
            posX = m_areaRect.right - (m_areaRect.right % RAT_ELEMENT_WIDTH) - RAT_ELEMENT_WIDTH;
        }

        // Update the rat position
        m_ratSprite.SetDestXY(posX, posY);

        // Reset the timer
        m_gsTimer.Reset();
    }
    else if( m_state == RAT_STATE_EATEN ) {
        // Simulate a little explosion
        m_ratParticle.AddScaleX( 0.1 );
        m_ratParticle.AddScaleY( 0.1 );

        if( m_ratParticle.GetScaleX(0) > 2.5f ) {
            // m_ratParticle.AddRotateZ( 15 );
            m_particleAlphaMod = 0.05;
        }
    }

    return;
}

////////////////////////////////////////////////////////////////////////////////////////////////

//==============================================================================================
// Rat::Render():
// ---------------------------------------------------------------------------------------------
// Purpose: Draws the rat
// ---------------------------------------------------------------------------------------------
// Returns: True if rendering was successful, false if not
//==============================================================================================

bool Rat::Render(float fAlpha)
{
    // Set the alpha value to determine the transparency
    m_ratSprite.SetModulateColor(-1.0f, -1.0f, -1.0f, fAlpha / 2.0f);

    // Set the color for the particle
    GS_OGLColor color = m_particleColor;

    // Set the destination of the specified rat rat
    // m_ratSprite.SetDestXY( m_posX, m_posY );

    // Render the rat rat
    m_ratSprite.Render();

    // Render the particle effect centered on the rat
    m_ratParticle.SetDestX(0, m_ratSprite.GetDestX() - ((m_ratParticle.GetScaledWidth(0) - RAT_ELEMENT_WIDTH) / 2));
    m_ratParticle.SetDestY(0, m_ratSprite.GetDestY() - ((m_ratParticle.GetScaledHeight(0) - RAT_ELEMENT_HEIGHT) /2));

    // Adjust the particle effect alpha if a render alpha was given
    if( fAlpha < color.fAlpha) {
        color.fAlpha = fAlpha;
    }

    // Set the particle effect color
    m_ratParticle.SetModulateColor(color.fRed, color.fGreen, color.fBlue, color.fAlpha);

    // Fade the particle effect color towrds the end of the rat
    color.fRed = color.fRed - (color.fRed / m_length);
    color.fGreen = color.fGreen - (color.fGreen / m_length);
    color.fBlue = color.fBlue - (color.fBlue / m_length);

    // Render the particle effect over the rat
    m_ratParticle.Render();

    // Reset the rat to the head coordinates
    // m_ratSprite.SetDestXY( m_posX, m_posY );

    return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////
