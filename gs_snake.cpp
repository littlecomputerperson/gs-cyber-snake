/*============================================================================================+
 | Game System (GS) Library                                                                   |
 |--------------------------------------------------------------------------------------------|
 | FILES: gs_snake.cpp, gs_snake.h                                                            |
 |--------------------------------------------------------------------------------------------|
 | CLASS: GS_Snake                                                                            |
 |--------------------------------------------------------------------------------------------|
 | ABOUT: ...                                                                                 |
 |--------------------------------------------------------------------------------------------|
 |                                                                                    03/2024 |
 +============================================================================================*/

//==============================================================================================
// Include header files.
// ---------------------------------------------------------------------------------------------
#include "gs_snake.h"
//==============================================================================================

//==============================================================================================
// Global variables.
// ---------------------------------------------------------------------------------------------
GS_Snake g_gsSnake;
//==============================================================================================

// *********************************************************************************************
// *** Constructor & Destructor Methods ********************************************************
// *********************************************************************************************

//==============================================================================================
// GS_Snake::GS_Snake():
// ---------------------------------------------------------------------------------------------
// Purpose: The constructor, initializes class data to defaults when class object is created.
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

GS_Snake::GS_Snake() : GS_Application()
{
    m_bKeepAspectRatio = true;

    m_gsSettings.nDisplayWidth = -1;
    m_gsSettings.nDisplayHeight = -1;
    m_gsSettings.nColorDepth = -1;
    m_gsSettings.bWindowedMode = -1;
    m_gsSettings.bEnableVSync = -1;
    m_gsSettings.bEnableAliasing = -1;
    m_gsSettings.bEnableAliasing = -1;
    m_gsSettings.fFrameCap = -1;
    m_gsSettings.nMusicVolume = -1;
    m_gsSettings.nEffectsVolume = -1;

    for (int nLoop = 0; nLoop < MAX_SCORES; nLoop++)
    {
        lstrcpy(m_gsHiscores[nLoop].szName, "..........");
        lstrcpy(m_gsHiscores[nLoop].szLevel, "0-0");
        lstrcpy(m_gsHiscores[nLoop].szMode, ".......");
        m_gsHiscores[nLoop].lScore = 0l;
    }

    m_nGameProgress = 0;
    m_nPrevProgress = 0;
    m_nNextProgress = 0;

    m_bIsInitialized = FALSE;
    m_bWasKeyReleased = FALSE;
    m_nOptionSelected = -1;
    m_nCounter = 0;

    m_fInterval = 0.0f;
    m_fRotate = 0.0f;
    m_fScale = 0.0f;
    m_fAlpha = 0.0f;

    m_gsMenuTextColor.fRed = 1.0f;
    m_gsMenuTextColor.fGreen = 1.0f;
    m_gsMenuTextColor.fBlue = 1.0f;
    m_gsMenuTextColor.fAlpha = 0.3f;

    m_gsMenuHighColor.fRed = 1.0f;
    m_gsMenuHighColor.fGreen = 1.0f;
    m_gsMenuHighColor.fBlue = 1.0f;
    m_gsMenuHighColor.fAlpha = 1.0f;

    m_currentWorld = 1;
    m_currentLevel = 1;
    m_ratsEaten = 0;
    m_lives = DEFAULT_LIVES;

    m_nScoreIndex = 0;
    m_lScore = 0;

    m_defaultRatState = RAT_STATE_MOVING;
    m_gameMode = NORMAL_MODE;

    this->GameSetup();
}

// *********************************************************************************************

//==============================================================================================
// GS_Snake::~GS_Snake():
// ---------------------------------------------------------------------------------------------
// Purpose: The de-structor, de-initializes class data when class object is destroyed.
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

GS_Snake::~GS_Snake()
{
    // Does nothing at the moment.
}

// *********************************************************************************************

// *********************************************************************************************
// *** Setup Methods ***************************************************************************
// *********************************************************************************************

//==============================================================================================
// GS_Snake::GameSetup():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successful, FALSE if not.
//==============================================================================================

BOOL GS_Snake::GameSetup()
{
    // Were we not able to load game settings?
    if (FALSE == this->LoadSettings())
    {
        // Display a message allowing the user to choose full screen or windowed mode.
        if (MessageBox(NULL, "Would you like to run in fullscreen mode?", "PLEASE SELECT",
                       MB_YESNO | MB_ICONQUESTION) == IDYES)
        {
            // Run game in fullscreen mode.
            m_gsSettings.bWindowedMode = FALSE;
        }
        else
        {
            // Run game in windowed mode.
            m_gsSettings.bWindowedMode = TRUE;
        }

        // Set all other game settings to their defaults.
        m_gsSettings.nDisplayWidth = DEFAULT_WIDTH;
        m_gsSettings.nDisplayHeight = DEFAULT_HEIGHT;
        m_gsSettings.nColorDepth = DEFAULT_DEPTH;
        m_gsSettings.bEnableVSync = DEFAULT_VSYNC;
        m_gsSettings.bEnableAliasing = DEFAULT_ALIAS;
        m_gsSettings.fFrameCap = DEFAULT_FRAMECAP;
        m_gsSettings.nMusicVolume = DEFAULT_MUSIC;
        m_gsSettings.nEffectsVolume = DEFAULT_SOUND;
    }

    // Set the display mode
    this->SetMode(
        m_gsSettings.nDisplayWidth,
        m_gsSettings.nDisplayHeight,
        m_gsSettings.nColorDepth,
        m_gsSettings.bWindowedMode);

    // Set render scaling values
    this->SetRenderScaling(this->GetWidth(), this->GetHeight(), m_bKeepAspectRatio);

    // Load the game hiscores.
    this->LoadHiscores();

    // Set the famerate cap
    this->SetFrameRate(m_gsSettings.fFrameCap);

    return TRUE;
}

// *********************************************************************************************

// *********************************************************************************************
// *** Initialize & Shutdown Methods ***********************************************************
// *********************************************************************************************

//==============================================================================================
// GS_Snake::GameInit():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successful, FALSE if not.
//==============================================================================================

BOOL GS_Snake::GameInit()
{

    // Seed the timer for random numbers.
    time_t t;
    srand((unsigned)time(&t));

    // Setup the text for the window title bar.
    this->SetTitle("Cyber Snake");

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Create And Initialize The OpenGL Display /////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Did we fail creating the OpenGL display?
    if (!m_gsDisplay.Create(this->GetDevice(), this->GetWidth(), this->GetHeight(), this->GetColorDepth()))
    {
        GS_Error::Report("GS_SNAKE.CPP", 186, "Failed to create OpenGL display!");
        return FALSE;
    }

    // Enable vertical sync if specified in the game settings.
    m_gsDisplay.EnableVSync(m_gsSettings.bEnableVSync);

    // Enable anti-aliasing if specified in the game settings.
    m_gsDisplay.EnableAliasing(m_gsSettings.bEnableAliasing);

    // Enable blending for transparency in textures.
    m_gsDisplay.EnableBlending(TRUE);

    // Are we in fullscreen mode?
    // if (!this->IsWindowed())
    // {
    // Hide the mouse cursor.
    m_gsMouse.HideCursor(TRUE);
    // }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Initialize Controller Input //////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Initialize controller input.
    // (Controller auto-initializes in constructor, just log if connected)
    if (m_gsController.GetConnectedCount() > 0)
    {
        char szDebugMsg[256];
        sprintf(szDebugMsg, "Controller connected: %s\n", m_gsController.GetControllerName(0));
        GS_Platform::OutputDebugString(szDebugMsg);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Initialize The FMOD Sound System /////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Did we fail to initialize the FMOD sound system?
    if (!m_gsSound.Initialize(GetWindow()))
    {
        GS_Error::Report("GS_SNAKE.CPP", 211, "Failed to initialize FMOD sound!");
        return FALSE;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Custom Graphics Initialization ////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Create a  filtered sprite that will be used for the background.
    BOOL bResult = m_gsBackSprite.Create("data/image_01.tga", TRUE);

    // Were we not successful?
    if (FALSE == bResult)
    {
        GS_Error::Report("GS_SNAKE.CPP", 224, "Failed to create background sprite!");
    }

    // Set the source rectangle of the background sprite to the default.
    m_gsBackSprite.SetSourceRect(0, INTERNAL_RES_Y, INTERNAL_RES_X, 0);
    // Postion the background at the bottom left of the screen.
    m_gsBackSprite.SetDestXY(0, 0);

    // Create an unfiltered sprite that will be used for the cursor.
    bResult = m_gsCursorSprite.Create("data/cursor_01.tga", FALSE);

    // Were we not successful?
    if (FALSE == bResult)
    {
        GS_Error::Report("GS_SNAKE.CPP", 237, "Failed to create cursor sprite!");
    }

    // Create a filtered sprite that will be used for tiling effects.
    // bResult = m_gsTileSprite.Create("data/image_02.tga", TRUE);

    // Were we not successful?
    // if (FALSE == bResult)
    // {
    // GS_Error::Report("GS_SNAKE.CPP", 245, "Failed to create tile sprite!");
    // }

    // Set up a rectangle the size of the screen, play area, and scorearea (left, top, right, bottom)
    SetRect(&m_rcScreen, 0, INTERNAL_RES_Y, INTERNAL_RES_X, 0);
    SetRect(&m_rcPlayArea, 0, PLAY_AREA_HEIGHT, PLAY_AREA_WIDTH, 0);
    SetRect(&m_rcScoreArea, 0, (PLAY_AREA_HEIGHT + SCORE_AREA_HEIGHT), SCORE_AREA_HEIGHT, (PLAY_AREA_HEIGHT + 1));

    // Create an unfiltered sprite that will be used for the title.
    bResult = m_gsTitleSprite.Create("data/image_03.tga", FALSE);

    // Were we not successful?
    if (FALSE == bResult)
    {
        GS_Error::Report("GS_SNAKE.CPP", 256, "Failed to create title sprite!");
    }

    // Create an unfiltered mipmap texture from the "font_01.tga" image, for the game font.
    bResult = m_gsFontTexture.Create("data/font_01.tga", GS_MIPMAP, GL_NEAREST, GL_NEAREST);

    // Were we not successful?
    if (FALSE == bResult)
    {
        GS_Error::Report("GS_SNAKE.CPP", 264, "Failed to create font texture!");
    }

    // Use the font texture to create a font with 16 characters per line and with a width
    // and height of 16x16 pixels per character.
    bResult = m_gsFont.Create(m_gsFontTexture.GetID(), m_gsFontTexture.GetWidth(),
                              m_gsFontTexture.GetHeight(), 16, 16, 16);

    // Were we not successful?
    if (FALSE == bResult)
    {
        GS_Error::Report("GS_SNAKE.CPP", 274, "Failed to create font!");
    }

    // Create a menu using the "menu_01.tga" image and the font texture created earlier.
    bResult = m_gsMenu.Create("data/menu_01.tga", m_gsFontTexture.GetID(),
                              m_gsFontTexture.GetWidth(), m_gsFontTexture.GetHeight(),
                              16, 16, 16);

    // Were we not successful?
    if (FALSE == bResult)
    {
        GS_Error::Report("GS_SNAKE.CPP", 284, "Failed to create game menu!");
    }

    // Create a background sprite with 12 frames, 12 frames per line and dimensions of 16x20
    bResult = m_gsScoreBackground.Create("data/score_background.tga", 12, 12, 16, 20, false, false);

    // Were we not successful?
    if (FALSE == bResult)
    {
        GS_Error::Report("GS_DEMO.CPP", 322, "Failed to create score background!");
    }

    // Create a particle using the "snake_particle.tga" image.
    bResult = m_snakeParticle.Create("data/snake_particle.tga", 1);

    // Were we not successful?
    if (FALSE == bResult)
    {
        GS_Error::Report("GS_SNAKE.CPP", 320, "Failed to create sparticle for particle snake!");
    }

    // Create the snake
    m_snake.Create(m_rcPlayArea, -1, -1, true);

    // Create the rat
    m_rat.Create(m_rcPlayArea, -1, -1, true);

    // Create the level
    m_level.Create(m_rcPlayArea, true);

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Custom Sound Initialization ///////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Add the game music to a list.
    m_gsSound.AddStream("data/music_01.mp3");
    m_gsSound.AddStream("data/music_02.mp3");

    // Add the game samples to list.
    m_gsSound.AddSample("data/sfx_menu_navigate.wav");
    m_gsSound.AddSample("data/sfx_menu_select.wav");
    m_gsSound.AddSample("data/sfx_snake_input.mp3");
    m_gsSound.AddSample("data/sfx_snake_growing.mp3");
    m_gsSound.AddSample("data/sfx_snake_dying.mp3");
    m_gsSound.AddSample("data/sfx_rat_moving.mp3");
    m_gsSound.AddSample("data/sfx_rat_dying.mp3");

    // Set the master volume for all the game sound effects.
    m_gsSound.SetSampleMaster(m_gsSettings.nEffectsVolume);

    // Are we already playing the game?
    if ((m_nGameProgress >= PLAY_GAME) && (m_nGameProgress < PLAY_OUTRO))
    {
        // Start playing game music at the specified volume, 1x speed and looping infinitely.
        m_gsSound.PlayStream(MUSIC_GAME, m_gsSettings.nMusicVolume);
    }
    else if (m_nGameProgress > GAME_INTRO)
    {
        // Start playing title music at the specified volume, 1x speed and looping infinitely.
        m_gsSound.PlayStream(MUSIC_TITLE, m_gsSettings.nMusicVolume);
    }

    return TRUE;
}

// *********************************************************************************************

//==============================================================================================
// GS_Snake::GameShutdown():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successful, FALSE if not.
//==============================================================================================

BOOL GS_Snake::GameShutdown()
{

    // Reset keyboard data.
    m_gsKeyboard.Reset();

    // Reset mouse data.
    m_gsMouse.Reset();

    // Reset controller data.
    m_gsController.Reset();

    // Destroy the OpenGL display.
    m_gsDisplay.Destroy();

    // Shutdown the sound system.
    m_gsSound.Shutdown();

    return TRUE;
}

// *********************************************************************************************

// *********************************************************************************************
// *** Release & Restore Methods ***************************************************************
// *********************************************************************************************

//==============================================================================================
// GS_Snake::GameRelease():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successful, FALSE if not.
//==============================================================================================

BOOL GS_Snake::GameRelease()
{

    // Is the application not active?
    if (!this->IsActive())
    {
        return TRUE;
    }

    // Set OpenGL clear color to black.
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    // Clear screen and depth buffer.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reset the current modelview matrix.
    glLoadIdentity();

    // Swap buffers (double buffering) to display results.
    SwapBuffers(this->GetDevice());

    // Pause all game sound.
    m_gsSound.Pause(TRUE);

    // Pause the game.
    this->Pause(TRUE);

    return TRUE;
}

// *********************************************************************************************

//==============================================================================================
// GS_Snake::GameRestore():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successful, FALSE if not.
//==============================================================================================

BOOL GS_Snake::GameRestore()
{

    // Is the application active?
    if (this->IsActive())
    {
        return TRUE;
    }

    // Unpause all sound.
    m_gsSound.Pause(FALSE);

    return TRUE;
}

// *********************************************************************************************

// *********************************************************************************************
// *** Main Game Loop **************************************************************************
// *********************************************************************************************

//==============================================================================================
// GS_Snake::GameLoop():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successful, FALSE if not.
//==============================================================================================

BOOL GS_Snake::GameLoop()
{

    // Run method depending on progress.
    switch (m_nGameProgress)
    {
    // Game intro & outro methods.
    case GAME_INTRO:
        return GameIntro();
    case GAME_OUTRO:
        return GameOutro();
    // Title screen methods.
    case TITLE_INTRO:
        return TitleIntro();
    case TITLE_SCREEN:
        return TitleScreen();
    case TITLE_OUTRO:
        return TitleOutro();
    // Option screen methods.
    case OPTION_INTRO:
        return OptionIntro();
    case OPTION_SCREEN:
        return OptionScreen();
    case OPTION_OUTRO:
        return OptionOutro();
    // Game play methods.
    case PLAY_INTRO:
        return PlayIntro();
    case PLAY_GAME:
        return PlayGame();
    case PLAY_UPDATE:
        return PlayUpdate();
    case PLAY_PAUSE:
        return PlayPause();
    case PLAY_EXIT:
        return PlayExit();
    case PLAY_OUTRO:
        return PlayOutro();
    // Hiscore methods.
    case SCORES_INTRO:
        return ScoresIntro();
    case SCORES_VIEW:
        return ScoresView();
    case SCORES_ADD:
        return ScoresAdd();
    case SCORES_OUTRO:
        return ScoresOutro();
    // ...
    default:
        m_nGameProgress = GAME_OUTRO;
        break;
    } // end switch

    return TRUE;
}

// *********************************************************************************************

// *********************************************************************************************
// *** Message Handling Methods ****************************************************************
// *********************************************************************************************

//==============================================================================================
// GS_Snake::OnChangeMode():
// ---------------------------------------------------------------------------------------------
// Purpose: Overrides the OnChangeMode() function of the base class
// ---------------------------------------------------------------------------------------------
// Returns: Nothing
//==============================================================================================

void GS_Snake::OnChangeMode()
{
    // Change to windowed or fullscreen mode as selected by the user
    this->SetMode(
        m_gsSettings.nDisplayWidth,
        m_gsSettings.nDisplayHeight,
        this->GetColorDepth(),
        !this->IsWindowed());

    // Set values for scaling
    this->SetRenderScaling(this->GetWidth(), this->GetHeight(), m_bKeepAspectRatio);

    // Has the mode change been successful?
    m_gsSettings.bWindowedMode = this->IsWindowed();

    // Save game settings.
    this->SaveSettings();
}

// *********************************************************************************************

//==============================================================================================
// GS_Snake::MsgProc():
// ---------------------------------------------------------------------------------------------
// Purpose: Overrides the main WndProc() and MsgProc() of the base class, so the application
//          can do custom message handling (e.g. processing mouse, keyboard, or menu commands).
// ---------------------------------------------------------------------------------------------
// Returns: LRESULT depending on the message processed.
//==============================================================================================

LRESULT GS_Snake::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

    switch (uMsg)
    {

    // Received when a WM_KEYDOWN message is translated by the TranslateMessage()
    // function. WM_CHAR contains the character code of the key that was pressed.
    case WM_CHAR:
        // wParam contains information on which key was pressed.
        switch (wParam)
        {
        // User pressed 'b' or 'B'.
        case 'B':
        case 'b':
            // Are at the not at the ScoresAdd() method?
            if (m_nGameProgress != SCORES_ADD)
            {
                // Is alpha blending enabled?
                if (m_gsDisplay.IsBlendingEnabled())
                {
                    // Disable alpha blending.
                    m_gsDisplay.EnableBlending(FALSE);
                }
                else
                {
                    // Enable alpha blending.
                    m_gsDisplay.EnableBlending(TRUE);
                }
            }
            break;
        // User pressed 'p' or 'P'.
        case 'P':
        case 'p':
            // Is the application pause?
            if (this->IsPaused())
            {
                // Unpause the application.
                this->Pause(FALSE);
            }
            else
            {
                // Pause the application.
                this->Pause(TRUE);
            }
            break;
        // User pressed 't' or 'T'.
        case 'T':
        case 't':
            // Are at the not at the ScoresAdd() method?
            if (m_nGameProgress != SCORES_ADD)
            {
                // Is the frame rate greater than 0?
                if (this->GetFrameRate() > 0.0f)
                {
                    this->SetFrameRate(0.0f);
                }
                else
                {
                    this->SetFrameRate(60.0f);
                }
            }
            break;
        // User pressed 'v' or 'V'.
        case 'V':
        case 'v':
            // Are at the not at the ScoresAdd() method?
            if (m_nGameProgress != SCORES_ADD)
            {
                // Is vertical sync enabled?
                if (m_gsDisplay.IsVSyncEnabled())
                {
                    // Disable vertical sync.
                    m_gsDisplay.EnableVSync(FALSE);
                }
                else
                {
                    // Enable vertical sync.
                    m_gsDisplay.EnableVSync(TRUE);
                }
            }
            break;
        // Default processing.
        default:
            break;
        } // end switch (wParam...
        break;

    // Received when a nonsystem key is pressed. A nonsystem key is a key
    // that is pressed when the ALT key is not pressed.
    case WM_KEYDOWN:
        // Mark the key that was pressed.
        m_gsKeyboard.KeyDown(wParam);
        // Add the key to the keyboard buffer.
        m_gsKeyboard.AddKeyToBuffer(wParam);

        // Also add to controller buffer if it's a controller button
        if (wParam >= GSC_BUTTON_A && wParam <= GSC_BUTTON_DPAD_RIGHT)
        {
            m_gsController.SetButtonDown(wParam);
            m_gsController.AddButtonToBuffer(wParam);
        }
        break;

    // Received when a nonsystem key is released. A nonsystem key is a key
    // that is pressed  when the ALT key is not pressed.
    case WM_KEYUP:
        // Mark the key that was released.
        m_gsKeyboard.KeyUp(wParam);

        // Also add to controller buffer if it's a controller button
        if (wParam >= GSC_BUTTON_A && wParam <= GSC_BUTTON_DPAD_RIGHT)
        {
            m_gsController.SetButtonUp(wParam);
        }
        break;

    // Received when the user holds down the ALT key and then presses another key.
    case WM_SYSKEYDOWN:
        // Mark the key that was pressed.
        m_gsKeyboard.KeyDown(wParam);
        // Add the key to the keyboard buffer.
        m_gsKeyboard.AddKeyToBuffer(wParam);
        return 0L;

    // Received when the user releases a key that was pressed while the ALT key held down.
    case WM_SYSKEYUP:
        // Mark the key that was released.
        m_gsKeyboard.KeyUp(wParam);
        return 0L;

    // Received when the cursor moves within a window and mouse input is not captured.
    case WM_SETCURSOR:
        // Are we in windowed mode?
        if (this->IsWindowed())
        {
            // Is the mouse cursor in the window client?
            if (m_gsMouse.IsInClient(hWnd))
            {
                // Hide the cursor.
                SetCursor(NULL);
                return TRUE;
            }
        }
        break;

    // The WM_MOUSEMOVE message is posted to a window when the cursor moves.
    case WM_MOUSEMOVE:
        // Save mouse coordinates (coordinates are relative to the top left of the client
        // area of the window). Note that the Y coordinates are inverted for OpenGL.
        m_gsMouse.SetX(LOWORD(lParam));
        m_gsMouse.SetY(INTERNAL_RES_Y - HIWORD(lParam) - 1);
        return 0L;

    // The WM_LBUTTONDOWN message is posted when the user presses the left mouse
    // button while/ the cursor is in the client area of a window.
    case WM_LBUTTONDOWN:
        // The left mouse button is down.
        m_gsMouse.LeftPressed(TRUE);
        return 0L;

    // The WM_MBUTTONDOWN message is posted when the user presses the middle
    // mouse button while the cursor is in the client area of a window.
    case WM_MBUTTONDOWN:
        // The middle mouse button is down.
        m_gsMouse.MiddlePressed(TRUE);
        return 0L;

    // The WM_RBUTTONDOWN message is posted when the user presses the right
    // mouse button while the cursor is in the client area of a window.
    case WM_RBUTTONDOWN:
        // The right mouse button is down.
        m_gsMouse.RightPressed(TRUE);
        return 0L;

    // The WM_LBUTTONUP message is posted when the user releases the left mouse
    // button while the cursor is in the client area of a window.
    case WM_LBUTTONUP:
        // The left mouse button is up.
        m_gsMouse.LeftPressed(FALSE);
        return 0L;

    // The WM_MBUTTONUP message is posted when the user releases the middle
    // mouse button while the cursor is in the client area of a window.
    case WM_MBUTTONUP:
        // The middle mouse button is up.
        m_gsMouse.MiddlePressed(FALSE);
        return 0L;

    // The WM_RBUTTONUP message is posted when the user releases the right
    // mouse button while the cursor is in the client area of a window.
    case WM_RBUTTONUP:
        // The right mouse button is up.
        m_gsMouse.RightPressed(FALSE);
        return 0L;
    } // end switch (uMsg...

    // Provide default processing by the base class.
    return GS_Application::MsgProc(hWnd, uMsg, wParam, lParam);
}

// *********************************************************************************************

// *********************************************************************************************
// *** Game Intro & Outro Methods **************************************************************
// *********************************************************************************************

//==============================================================================================
// GS_Snake::GameIntro():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull and FALSE if not.
//==============================================================================================

BOOL GS_Snake::GameIntro()
{

    // Has the display not been initialized?
    if (!this->m_gsDisplay.IsReady())
    {
        return FALSE;
    }

    // Has the game been paused?
    if (this->IsPaused())
    {
        // Unpause the game.
        this->Pause(FALSE);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Method Initialization /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Has the method not been initialized?
    if (m_bIsInitialized == FALSE)
    {
        // Initialize variables.
        m_fInterval = 0.0f;
        m_nCounter = 0;
        m_fAlpha = 0.0f;
        // Clear the keyboard buffer.
        m_gsKeyboard.ClearBuffer();
        // Clear the controller buffer.
        m_gsController.ClearBuffer();
        // Where we want to go to next.
        m_nNextProgress = TITLE_INTRO;
        // Initialization completed.
        m_bIsInitialized = TRUE;
        // Return to prevent the time it took to initialize the game, which constitutes
        // the first measured frame, from affecting the timing of the game.
        return TRUE;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Method Logic //////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Determine the interval required to perform an action 60 times every second at the
    // current frame rate, and add it to the previous intervals.
    m_fInterval += this->GetActionInterval(60);

    // Have all the intervals added up enough to perform an action?
    while (m_fInterval >= 1.0f)
    {
        // Increase counter.
        m_nCounter++;
        // Act depending on how many counts have elapsed.
        if ((m_nCounter >= 50) && (m_nCounter < 150))
        {
            // Decrease transparency.
            m_fAlpha += 0.025;
        }
        else if ((m_nCounter >= 150) && (m_nCounter < 250))
        {
            // Increase transparency.
            m_fAlpha -= 0.025;
        }
        else if (m_nCounter >= 250)
        {
            // Progress to the next section.
            m_nGameProgress = m_nNextProgress;
        }
        // One action less to be taken.
        m_fInterval -= 1.0f;
    }

    // Has the user pressed a key?
    if (m_gsKeyboard.GetBufferedKey() > 0)
    {
        // Progress to the next section.
        m_nGameProgress = m_nNextProgress;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Method Rendering //////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Clear the screen to the specified color.
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reset display.
    glLoadIdentity();

    // Setup the display for rendering in 2D.
    m_gsDisplay.BeginRender2D(this->GetWindow());

    // Set the alpha value (transparency) for the font.
    m_gsFont.SetModulateColor(-1.0, -1.0, -1.0, m_fAlpha);

    // Display a message centered in the screen.
    m_gsFont.SetText("LITTLE COMPUTER PERSON");
    m_gsFont.SetDestX((INTERNAL_RES_X - m_gsFont.GetTextWidth()) / 2);
    m_gsFont.SetDestY(((INTERNAL_RES_Y - m_gsFont.GetTextHeight()) / 2) +
                      m_gsFont.GetTextHeight());
    m_gsFont.Render();

    m_gsFont.SetText("PRESENTS");
    m_gsFont.SetDestX((INTERNAL_RES_X - m_gsFont.GetTextWidth()) / 2);
    m_gsFont.SetDestY(((INTERNAL_RES_Y - m_gsFont.GetTextHeight()) / 2) -
                      m_gsFont.GetTextHeight());
    m_gsFont.Render();

    // Reset display after rendering in 2D.
    m_gsDisplay.EndRender2D();

    // Swap buffers to display results.
    SwapBuffers(this->GetDevice());

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Method Cleanup ////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Are we leaving this section?
    if (m_nGameProgress != GAME_INTRO)
    {
        // Remember where we've come from.
        m_nPrevProgress = GAME_INTRO;
        // Is title music not already playing?
        if (!m_gsSound.IsStreamPlaying())
        {
            // Start playing title music at the specified volume, 1x speed and looping infinitely.
            m_gsSound.PlayStream(MUSIC_TITLE, m_gsSettings.nMusicVolume);
        }
        // Reset method variables.
        m_bIsInitialized = FALSE;
    }

    return TRUE;
}

// *********************************************************************************************

//==============================================================================================
// GS_Snake::GameOutro():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull and FALSE if not.
//==============================================================================================

BOOL GS_Snake::GameOutro()
{

    // Has the display not been initialized?
    if (!this->m_gsDisplay.IsReady())
    {
        return FALSE;
    }

    // Has the game been paused?
    if (this->IsPaused())
    {
        // Unpause the game.
        this->Pause(FALSE);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Method Initialization /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Has the method not been initialized?
    if (m_bIsInitialized == FALSE)
    {
        // Clear the keyboard buffer.
        m_gsKeyboard.ClearBuffer();
        // Clear the controller buffer.
        m_gsController.ClearBuffer();
        // Initialize method variables.
        m_fInterval = 0.0f;
        m_nCounter = 0;
        m_fAlpha = 1.0f;
        // Is the title music playing?
        if (m_gsSound.IsStreamPlaying())
        {
            // Stop title music from playing.
            m_gsSound.StopStream();
        }
        // Initialization completed.
        m_bIsInitialized = TRUE;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Get Keyboard Input ///////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Has the user pressed a key?
    if (m_gsKeyboard.GetBufferedKey() > 0)
    {
        // Quit the application.
        this->Quit();
        return TRUE;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Method Logic //////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Determine the interval required to perform an action 60 times every second at the
    // current frame rate, and add it to the previous intervals.
    m_fInterval += this->GetActionInterval(60);

    // Have all the intervals added up enough to perform an action?
    while (m_fInterval >= 1.0f)
    {
        // Increase counter.
        m_nCounter++;
        // Act depending on how many counts have elapsed.
        if ((m_nCounter >= 350) && (m_nCounter < 400))
        {
            // Increase transparency.
            m_fAlpha -= 0.025;
        }
        else if (m_nCounter >= 400)
        {
            // Quit the application.
            this->Quit();
            return TRUE;
        }
        // One action less to be taken.
        m_fInterval -= 1.0f;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Method Rendering //////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Clear the screen to the specified color.
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reset display.
    glLoadIdentity();

    // Setup the display for rendering in 2D.
    m_gsDisplay.BeginRender2D(this->GetWindow());

    // Display the game credits centered in the screen.
    m_gsFont.SetText("GAME CREDITS");
    m_gsFont.SetScaleXY(1.5f, 1.5f);
    m_gsFont.SetDestX((INTERNAL_RES_X - m_gsFont.GetTextWidth()) / 2);
    m_gsFont.SetDestY(INTERNAL_RES_Y - (m_gsFont.GetScaledHeight() * 3));
    m_gsFont.SetModulateColor(1.0f, 0.0f, 1.0f, m_fAlpha);
    m_gsFont.Render();

    m_gsFont.SetText("DEVELOPED BY");
    m_gsFont.SetScaleXY(1.0f, 1.0f);
    m_gsFont.SetDestX((INTERNAL_RES_X - m_gsFont.GetTextWidth()) / 2);
    m_gsFont.SetDestY(INTERNAL_RES_Y - (m_gsFont.GetScaledHeight() * 9));
    m_gsFont.SetModulateColor(0.0f, 1.0f, 1.0f, m_fAlpha);
    m_gsFont.Render();

    m_gsFont.SetText("LITTLE COMPUTER PERSON");
    m_gsFont.SetDestX((INTERNAL_RES_X - m_gsFont.GetTextWidth()) / 2);
    m_gsFont.SetDestY(INTERNAL_RES_Y - (m_gsFont.GetScaledHeight() * 11));
    m_gsFont.SetModulateColor(1.0f, 1.0f, 1.0f, m_fAlpha);
    m_gsFont.Render();

    m_gsFont.SetText("TITLE MUSIC");
    m_gsFont.SetDestX((INTERNAL_RES_X - m_gsFont.GetTextWidth()) / 2);
    m_gsFont.SetDestY(INTERNAL_RES_Y - (m_gsFont.GetScaledHeight() * 14));
    m_gsFont.SetModulateColor(0.0f, 1.0f, 1.0f, m_fAlpha);
    m_gsFont.Render();

    m_gsFont.SetText("Stomp Rock for Champions - Musictown");
    m_gsFont.SetDestX((INTERNAL_RES_X - m_gsFont.GetTextWidth()) / 2);
    m_gsFont.SetDestY(INTERNAL_RES_Y - (m_gsFont.GetScaledHeight() * 16));
    m_gsFont.SetModulateColor(1.0f, 1.0f, 1.0f, m_fAlpha);
    m_gsFont.Render();

    m_gsFont.SetText("GAME MUSIC");
    m_gsFont.SetDestX((INTERNAL_RES_X - m_gsFont.GetTextWidth()) / 2);
    m_gsFont.SetDestY(INTERNAL_RES_Y - (m_gsFont.GetScaledHeight() * 19));
    m_gsFont.SetModulateColor(0.0f, 1.0f, 1.0f, m_fAlpha);
    m_gsFont.Render();

    // m_gsFont.SetText("............    ............");
    m_gsFont.SetText("Cyber Race - FASSounds");
    m_gsFont.SetDestX((INTERNAL_RES_X - m_gsFont.GetTextWidth()) / 2);
    m_gsFont.SetDestY(INTERNAL_RES_Y - (m_gsFont.GetScaledHeight() * 21));
    m_gsFont.SetModulateColor(1.0f, 1.0f, 1.0f, m_fAlpha);
    m_gsFont.Render();

    m_gsFont.SetText("TITLE BACKGROUND IMAGE");
    m_gsFont.SetDestX((INTERNAL_RES_X - m_gsFont.GetTextWidth()) / 2);
    m_gsFont.SetDestY(INTERNAL_RES_Y - (m_gsFont.GetScaledHeight() * 24));
    m_gsFont.SetModulateColor(0.0f, 1.0f, 1.0f, m_fAlpha);
    m_gsFont.Render();

    m_gsFont.SetText("Unknown");
    m_gsFont.SetDestX((INTERNAL_RES_X - m_gsFont.GetTextWidth()) / 2);
    m_gsFont.SetDestY(INTERNAL_RES_Y - (m_gsFont.GetScaledHeight() * 26));
    m_gsFont.SetModulateColor(1.0f, 1.0f, 1.0f, m_fAlpha);
    m_gsFont.Render();

    m_gsFont.SetText("THANKS FOR PLAYING!");
    m_gsFont.SetDestX((INTERNAL_RES_X - m_gsFont.GetTextWidth()) / 2);
    m_gsFont.SetDestY(INTERNAL_RES_Y - (m_gsFont.GetScaledHeight() * 29));
    m_gsFont.SetModulateColor(1.0f, 0.0f, 1.0f, m_fAlpha);
    m_gsFont.Render();

    m_gsFont.SetText("PRESS ANY KEY TO EXIT");
    m_gsFont.SetDestX((INTERNAL_RES_X - m_gsFont.GetTextWidth()) / 2);
    m_gsFont.SetDestY(INTERNAL_RES_Y - (m_gsFont.GetScaledHeight() * 32));
    m_gsFont.SetModulateColor(1.0f, 1.0f, 1.0f, m_fAlpha);
    m_gsFont.Render();

    // Reset display after rendering in 2D.
    m_gsDisplay.EndRender2D();

    // Swap buffers to display results.
    SwapBuffers(this->GetDevice());

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Method Cleanup ////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Are we leaving this section?
    if (m_nGameProgress != GAME_OUTRO)
    {
        // Remember where we've come from.
        m_nPrevProgress = GAME_OUTRO;
        // Reset method variables.
        m_bIsInitialized = FALSE;
    }

    return TRUE;
}

// *********************************************************************************************

// *********************************************************************************************
// *** Title Screen Methods ********************************************************************
// *********************************************************************************************

//==============================================================================================
// GS_Snake::TitleIntro():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull and FALSE if not.
//==============================================================================================

BOOL GS_Snake::TitleIntro()
{

    // Has the display not been initialized?
    if (!this->m_gsDisplay.IsReady())
    {
        return FALSE;
    }

    // Has the game been paused?
    if (this->IsPaused())
    {
        // Unpause the game.
        this->Pause(FALSE);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Method Initialization /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Has the method not been initialized?
    if (m_bIsInitialized == FALSE)
    {
        // Initialize method variables.
        m_fAlpha = 0.0f;
        // Where we want to go to next.
        m_nNextProgress = TITLE_SCREEN;
        // Initialization completed.
        m_bIsInitialized = TRUE;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Method Logic //////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Are images not entirely solid?
    if (m_fAlpha < 1.0f)
    {
        // Decrease transparency.
        m_fAlpha += this->GetActionInterval(60 * 0.025f);
    }
    else
    {
        // Progress to the next section.
        m_nGameProgress = m_nNextProgress;
        // No transparency.
        m_fAlpha = 1.0f;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Method Rendering //////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Clear the screen to the specified color.
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reset display.
    glLoadIdentity();

    // Setup the display for rendering in 2D.
    m_gsDisplay.BeginRender2D(this->GetWindow());

    // Render the background image.
    this->RenderBackground(m_fAlpha);

    // Render and scroll the tile images.
    // m_gsTileSprite.SetModulateColor(1.0f, 1.0f, 1.0f, m_fAlpha);
    // m_gsTileSprite.AddScrollX(this->GetActionInterval(60));
    // m_gsTileSprite.AddScrollY(this->GetActionInterval(30));
    // m_gsTileSprite.RenderTiles(m_rcScreen);

    // Reset display after rendering in 2D.
    m_gsDisplay.EndRender2D();

    // Swap buffers to display results.
    SwapBuffers(this->GetDevice());

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Method Cleanup ////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Are we leaving this section?
    if (m_nGameProgress != TITLE_INTRO)
    {
        // Remember where we've come from.
        m_nPrevProgress = TITLE_INTRO;
        // Reset method variables.
        m_bIsInitialized = FALSE;
    }

    return TRUE;
}

// *********************************************************************************************

//==============================================================================================
// GS_Snake::TitleScreen():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull and FALSE if not.
//==============================================================================================

BOOL GS_Snake::TitleScreen()
{

    // Has the display not been initialized?
    if (!this->m_gsDisplay.IsReady())
    {
        return FALSE;
    }

    // Has the game been paused?
    if (this->IsPaused())
    {
        // Unpause the game.
        this->Pause(FALSE);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Method Initialization /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    static float fTitleAlpha = 1.0f;
    static float fTitleAlphaFadeAmount = 0.03f;
    static float fTitleAlphaInterval = 0;

    // Has the method not been initialized?
    if (m_bIsInitialized == FALSE)
    {
        // Initialize method variables.
        m_nOptionSelected = -1;
        m_fInterval = 0.0f;
        fTitleAlpha = 1.0f;
        fTitleAlphaFadeAmount = 0.03f;
        fTitleAlphaInterval = 0;

        // Clear the keyboard buffer.
        m_gsKeyboard.ClearBuffer();

        // Clear the controller buffer.
        m_gsController.ClearBuffer();

        // Setup variables for mouse input.
        m_nOldMouseX = m_gsMouse.GetX();
        m_nOldMouseY = m_gsMouse.GetY();
        m_bWasMouseReleased = FALSE;

        // Clear all current menu items.
        m_gsMenu.ClearOptions();

        // Menu don't have a title.
        m_gsMenu.SetTitle("");

        // Add the menu options.
        m_gsMenu.AddOption("PLAY GAME    ");
        m_gsMenu.AddOption("VIEW HISCORES");
        m_gsMenu.AddOption("SET OPTIONS  ");
        m_gsMenu.AddOption("QUIT         ");

        // Highlight the first option.
        m_gsMenu.SetHighlight(0);

        // Is the title music not already playing?
        if (!m_gsSound.IsStreamPlaying())
        {
            // Start playing title music at the specified volume, 1x speed and looping infinitely.
            m_gsSound.PlayStream(MUSIC_TITLE, m_gsSettings.nMusicVolume);
        }

        // For every particle snake
        for (int i = 0; i < MAX_PARTICLE_SNAKES; i++)
        {
            // Determine a random start direction for the particle snake
            m_particleSnake[i].direction = (rand() % 4);

            // Depending on the directio, set random starting coordinates
            if (m_particleSnake[i].direction == SNAKE_MOVE_UP)
            {
                m_particleSnake[i].x = (rand() % ((m_rcScreen.right - m_rcScreen.left) - ((m_rcScreen.right - m_rcScreen.left) % SNAKE_ELEMENT_WIDTH)));
                m_particleSnake[i].y = 0;
            }
            else if (m_particleSnake[i].direction == SNAKE_MOVE_RIGHT)
            {
                m_particleSnake[i].x = 0;
                m_particleSnake[i].y = (rand() % ((m_rcScreen.top - m_rcScreen.bottom) - ((m_rcScreen.top - m_rcScreen.bottom) % SNAKE_ELEMENT_HEIGHT)));
            }
            else if (m_particleSnake[i].direction == SNAKE_MOVE_DOWN)
            {
                m_particleSnake[i].x = (rand() % ((m_rcScreen.right - m_rcScreen.left) - ((m_rcScreen.right - m_rcScreen.left) % SNAKE_ELEMENT_WIDTH)));
                m_particleSnake[i].y = ((m_rcScreen.top - m_rcScreen.bottom) - ((m_rcScreen.top - m_rcScreen.bottom) % SNAKE_ELEMENT_HEIGHT));
            }
            else if (m_particleSnake[i].direction == SNAKE_MOVE_LEFT)
            {
                m_particleSnake[i].x = ((m_rcScreen.right - m_rcScreen.left) - ((m_rcScreen.right - m_rcScreen.left) % SNAKE_ELEMENT_WIDTH));
                m_particleSnake[i].y = (rand() % ((m_rcScreen.top - m_rcScreen.bottom) - ((m_rcScreen.top - m_rcScreen.bottom) % SNAKE_ELEMENT_HEIGHT)));
                ;
            }

            // Determine a random direction interval the particle snake
            m_particleSnake[i].intervalCounter = 0;
            m_particleSnake[i].directionInterval = (rand() % 90) + 10;

            // Set a random movement speed
            m_particleSnake[i].speed = (rand() % MAX_PARTICLE_SNAKE_SPEED) + MIN_PARTICLE_SNAKE_SPEED;

            // Set a random length
            m_particleSnake[i].length = (rand() % MAX_PARTICLE_SNAKE_LENGTH) + MIN_PARTICLE_SNAKE_LENGTH;

            // Create random colors for the particle snake
            // int randomColor = (rand()%2);
            // if(randomColor == 0 ) {
            // m_particleSnake[i].color.fRed = 1.0f;
            // m_particleSnake[i].color.fGreen = 0.0f;
            // m_particleSnake[i].color.fBlue = 1.0f;
            // }
            // else if(randomColor == 1 ) {
            // m_particleSnake[i].color.fRed = 0.0f;
            // m_particleSnake[i].color.fGreen = 1.0f;
            // m_particleSnake[i].color.fBlue = 1.0f;
            // }
            m_particleSnake[i].color.fRed = (float(rand() % 90) / 100.0f) + 0.1f;
            m_particleSnake[i].color.fGreen = float(rand() % 90) / 100.0f + 0.1f;
            m_particleSnake[i].color.fBlue = float(rand() % 90) / 100.0f + 0.1f;
            m_particleSnake[i].color.fAlpha = (float(rand() % 75) / 100.0f) + 0.25f;
        }

        // Initialization completed.
        m_bIsInitialized = TRUE;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Get Keyboard Input ///////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Check to see wether a key was pressed.
    int nKey = m_gsKeyboard.GetBufferedKey();

    // Act depending on key pressed.
    switch (nKey)
    {
    // Was the up key pressed?
    case GSK_UP:
    case GSC_BUTTON_DPAD_UP:
        // Highlight the previous option.
        m_gsMenu.HighlightPrev();
        // Play the appropriate sound effect.
        m_gsSound.PlaySample(SAMPLE_OPTION);
        // Reset time counter.
        m_fInterval = 0.0f;
        break;
    // Was the down key pressed?
    case GSK_DOWN:
    case GSC_BUTTON_DPAD_DOWN:
        // Highlight the next option.
        m_gsMenu.HighlightNext();
        // Play the appropriate sound effect.
        m_gsSound.PlaySample(SAMPLE_OPTION);
        // Reset time counter.
        m_fInterval = 0.0f;
        break;
    // Was the enter key pressed?
    case GSK_ENTER:
    case GSC_BUTTON_A:
        // Remeber which option is highlighted.
        m_nOptionSelected = m_gsMenu.GetHighlight();
        // Play the appropriate sound effect.
        m_gsSound.PlaySample(SAMPLE_SELECT);
        // Reset time counter.
        m_fInterval = 0.0f;
        break;
    // Was the escape key pressed?
    case GSK_ESCAPE:
    case GSC_BUTTON_B:
        // Have the quit option been highlighted?
        if (m_gsMenu.GetHighlight() == 3)
        {
            // Set game progress to quit game.
            m_nNextProgress = GAME_OUTRO;
            m_nGameProgress = TITLE_OUTRO;
        }
        else
        {
            // Highlight the quit option.
            m_gsMenu.SetHighlight(3);
        }
        // Play the appropriate sound effect.
        m_gsSound.PlaySample(SAMPLE_OPTION);
        // Reset time counter.
        m_fInterval = 0.0f;
        break;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Get Mouse Input //////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    RECT rcOption;

    // Has the mouse cursor moved from its previous position?
    if ((m_nOldMouseX != m_gsMouse.GetX()) || (m_nOldMouseY != m_gsMouse.GetY()))
    {
        // Check each option to see if the mouse coordinates are within the option area.
        for (int nLoop = 0; nLoop < m_gsMenu.GetNumOptions(); nLoop++)
        {
            // Determine the screen coordinates of the next option.
            m_gsMenu.GetOptionRect(nLoop, &rcOption);
            // Are the mouse coordinates within the option area?
            if (m_gsCollide.IsCoordInRect(m_gsMouse.GetX(), m_gsMouse.GetY(), rcOption))
            {
                // Is the option not highlighted already?
                if (nLoop != m_gsMenu.GetHighlight())
                {
                    // Highlight the selected option.
                    m_gsMenu.SetHighlight(nLoop);
                    // Play the appropriate sound effect.
                    m_gsSound.PlaySample(SAMPLE_OPTION);
                }
            }
        }
        // Assign new mouse coordinates.
        m_nOldMouseX = m_gsMouse.GetX();
        m_nOldMouseY = m_gsMouse.GetY();
        // Reset time counter.
        m_fInterval = 0.0f;
    }

    // Is the left mouse button not pressed?
    if (!m_gsMouse.IsLeftPressed())
    {
        m_bWasMouseReleased = TRUE;
    }

    // Was the left mouse button pressed?
    if ((m_gsMouse.IsLeftPressed()) && (m_bWasMouseReleased))
    {
        // Determine the screen coordinates of the highlighted option.
        m_gsMenu.GetHighlightRect(&rcOption);
        // Check if mouse coordinates are within the area of the highligted option.
        if (m_gsCollide.IsCoordInRect(m_gsMouse.GetX(), m_gsMouse.GetY(), rcOption))
        {
            // Remeber which option is highlighted.
            m_nOptionSelected = m_gsMenu.GetHighlight();
            m_bWasMouseReleased = FALSE;
            // Play the appropriate sound effect.
            m_gsSound.PlaySample(SAMPLE_SELECT);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Process Menu Commands ////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Act depending on option selected.
    switch (m_nOptionSelected)
    {
    case 0:
        // Set game progress.
        m_nNextProgress = PLAY_INTRO;
        m_nGameProgress = TITLE_OUTRO;
        // Reset selected option.
        m_nOptionSelected = -1;
        break;
    case 1:
        // Set game progress.
        m_nNextProgress = SCORES_INTRO;
        m_nGameProgress = TITLE_OUTRO;
        // Reset selected option.
        m_nOptionSelected = -1;
        break;
    case 2:
        // Set game progress.
        m_nNextProgress = OPTION_INTRO;
        m_nGameProgress = TITLE_OUTRO;
        // Reset selected option.
        m_nOptionSelected = -1;
        break;
    case 3:
        // Set game progress.
        m_nNextProgress = GAME_OUTRO;
        m_nGameProgress = TITLE_OUTRO;
        // Reset selected option.
        m_nOptionSelected = -1;
        break;
    default:
        break;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Method Logic //////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Determine the interval required to perform an action 60 times every second at the
    // current frame rate, and add it to the previous intervals.
    m_fInterval += this->GetActionInterval(60);

    // Create an animation for the title
    // nTitleRotation += 3;
    // if( nTitleRotation > 360 ) {
    // nTitleRotation = 0;
    // }

    // Add to the title alpha interval counter
    fTitleAlphaInterval += this->GetActionInterval(60);

    // Should an action be taken?
    if (fTitleAlphaInterval > 1.0f)
    {
        // Reset the interval counter
        fTitleAlphaInterval = 0;

        // Increase or decrease the fade amount
        fTitleAlpha = fTitleAlpha + fTitleAlphaFadeAmount;

        // Has the fde amount exceeded the bounds?
        if (fTitleAlpha <= 0.0f)
        {
            // Reverse the fade
            fTitleAlphaFadeAmount = fTitleAlphaFadeAmount * -1;
            fTitleAlpha = 0.0f;
        }
        else if (fTitleAlpha >= 1.0f)
        {
            // Reverse the fade
            fTitleAlphaFadeAmount = fTitleAlphaFadeAmount * -1;
            fTitleAlpha = 1.0f;
        }
    }

    // For every particle snake
    for (int i = 0; i < MAX_PARTICLE_SNAKES; i++)
    {
        // Increase the particle snake action interval counter
        m_particleSnake[i].intervalCounter += this->GetActionInterval(60);

        // Have we reached the time for the particle snake to change direction?
        if (m_particleSnake[i].intervalCounter > m_particleSnake[i].directionInterval)
        {
            // Move in a new random direction that is not the old direction or the sopposite direction
            int oldDirection = m_particleSnake[i].direction;
            while ((oldDirection % 2) == (m_particleSnake[i].direction % 2))
            {
                // Set the new direction
                m_particleSnake[i].direction = (rand() % 4);
            }

            // Determine a new random direction interval the particle snake
            m_particleSnake[i].directionInterval = (rand() % 90) + 10;

            // Reset the timer
            m_particleSnake[i].intervalCounter = 0;
        }

        // Move the particle snake in the specified direction
        if (m_particleSnake[i].direction == SNAKE_MOVE_UP)
        {
            m_particleSnake[i].y += (m_particleSnake[i].speed * this->GetActionInterval(60) / 1.0f);
        }
        else if (m_particleSnake[i].direction == SNAKE_MOVE_RIGHT)
        {
            m_particleSnake[i].x += (m_particleSnake[i].speed * this->GetActionInterval(60) / 1.0f);
        }
        else if (m_particleSnake[i].direction == SNAKE_MOVE_DOWN)
        {
            m_particleSnake[i].y -= (m_particleSnake[i].speed * this->GetActionInterval(60) / 1.0f);
        }
        else if (m_particleSnake[i].direction == SNAKE_MOVE_LEFT)
        {
            m_particleSnake[i].x -= (m_particleSnake[i].speed * this->GetActionInterval(60) / 1.0f);
        }

        // Has the particle snake reached the edge of the screen?
        if (
            (m_particleSnake[i].x > (INTERNAL_RES_X + SNAKE_ELEMENT_WIDTH)) ||  // ((m_rcScreen.right - m_rcScreen.left) - ((m_rcScreen.right - m_rcScreen.left) % SNAKE_ELEMENT_WIDTH))) ||
            (m_particleSnake[i].y > (INTERNAL_RES_Y + SNAKE_ELEMENT_HEIGHT)) || // ((m_rcScreen.top - m_rcScreen.bottom) - ((m_rcScreen.top - m_rcScreen.bottom) % SNAKE_ELEMENT_HEIGHT))) ||
            (m_particleSnake[i].x < 0) ||
            (m_particleSnake[i].y < 0))
        {
            // Determine a random start direction for the particle snake
            m_particleSnake[i].direction = (rand() % 4);

            // Depending on the directio, set random starting coordinates
            if (m_particleSnake[i].direction == SNAKE_MOVE_UP)
            {
                m_particleSnake[i].x = (rand() % ((m_rcScreen.right - m_rcScreen.left) - ((m_rcScreen.right - m_rcScreen.left) % SNAKE_ELEMENT_WIDTH)));
                m_particleSnake[i].y = 0;
            }
            else if (m_particleSnake[i].direction == SNAKE_MOVE_RIGHT)
            {
                m_particleSnake[i].x = 0;
                m_particleSnake[i].y = (rand() % ((m_rcScreen.top - m_rcScreen.bottom) - ((m_rcScreen.top - m_rcScreen.bottom) % SNAKE_ELEMENT_HEIGHT)));
            }
            else if (m_particleSnake[i].direction == SNAKE_MOVE_DOWN)
            {
                m_particleSnake[i].x = (rand() % ((m_rcScreen.right - m_rcScreen.left) - ((m_rcScreen.right - m_rcScreen.left) % SNAKE_ELEMENT_WIDTH)));
                m_particleSnake[i].y = ((m_rcScreen.top - m_rcScreen.bottom) - ((m_rcScreen.top - m_rcScreen.bottom) % SNAKE_ELEMENT_HEIGHT));
            }
            else if (m_particleSnake[i].direction == SNAKE_MOVE_LEFT)
            {
                m_particleSnake[i].x = ((m_rcScreen.right - m_rcScreen.left) - ((m_rcScreen.right - m_rcScreen.left) % SNAKE_ELEMENT_WIDTH));
                m_particleSnake[i].y = (rand() % ((m_rcScreen.top - m_rcScreen.bottom) - ((m_rcScreen.top - m_rcScreen.bottom) % SNAKE_ELEMENT_HEIGHT)));
                ;
            }

            // Set a random movement speed
            m_particleSnake[i].speed = (rand() % MAX_PARTICLE_SNAKE_SPEED) + MIN_PARTICLE_SNAKE_SPEED;

            // Set a random length
            m_particleSnake[i].length = (rand() % MAX_PARTICLE_SNAKE_LENGTH) + MIN_PARTICLE_SNAKE_LENGTH;

            // Create random colors for the particle snake
            // int randomColor = (rand()%2);
            // if(randomColor == 0 ) {
            // m_particleSnake[i].color.fRed = 1.0f;
            // m_particleSnake[i].color.fGreen = 0.0f;
            // m_particleSnake[i].color.fBlue = 1.0f;
            // }
            // else if(randomColor == 1 ) {
            // m_particleSnake[i].color.fRed = 0.0f;
            // m_particleSnake[i].color.fGreen = 1.0f;
            // m_particleSnake[i].color.fBlue = 1.0f;
            // }
            m_particleSnake[i].color.fRed = (float(rand() % 90) / 100.0f) + 0.1f;
            m_particleSnake[i].color.fGreen = float(rand() % 90) / 100.0f + 0.1f;
            m_particleSnake[i].color.fBlue = float(rand() % 90) / 100.0f + 0.1f;
            m_particleSnake[i].color.fAlpha = (float(rand() % 75) / 100.0f) + 0.25f;
        }
    }

    // Have counter reached specified amount?
    if (m_fInterval >= 1200.0f)
    {
        // Set game progress.
        m_nNextProgress = SCORES_INTRO;
        m_nGameProgress = TITLE_OUTRO;
        m_fInterval = 0.0f;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Method Rendering //////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Clear the screen to the specified color.
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reset display.
    glLoadIdentity();

    // Setup the display for rendering in 2D.
    m_gsDisplay.BeginRender2D(this->GetWindow());

    // Render the background image.
    this->RenderBackground();

    // For every particle snake
    for (int i = 0; i < MAX_PARTICLE_SNAKES; i++)
    {
        // Set the particle effect color
        m_snakeParticle.SetModulateColor(m_particleSnake[i].color.fRed, m_particleSnake[i].color.fGreen, m_particleSnake[i].color.fBlue, m_particleSnake[i].color.fAlpha);

        // Update the particle snake position
        m_snakeParticle.SetDestX(m_particleSnake[i].x);
        m_snakeParticle.SetDestY(m_particleSnake[i].y);

        // render the particle snake
        m_snakeParticle.Render();
    }

    // Render and scroll the tile images.
    // m_gsTileSprite.AddScrollX(this->GetActionInterval(60));
    // m_gsTileSprite.AddScrollY(this->GetActionInterval(30));
    // m_gsTileSprite.RenderTiles(m_rcScreen);

    // Display the title centered on the screen
    m_gsFont.SetModulateColor(1.0f, 0.0f, 1.0f, fTitleAlpha);
    m_gsFont.SetText("SNAKE");
    m_gsFont.SetScaleXY(5.0f, 5.0f);
    m_gsFont.SetDestX((INTERNAL_RES_X - m_gsFont.GetTextWidth()) / 2);
    m_gsFont.SetDestY((INTERNAL_RES_Y / 2) + (((INTERNAL_RES_Y / 2) - (m_gsFont.GetTextHeight() * 3.25))));
    m_gsFont.Render();

    m_gsFont.SetModulateColor(0.0f, 1.0f, 1.0f, fTitleAlpha);
    m_gsFont.SetText("CYBER");
    m_gsFont.SetScaleXY(5.0f, 5.0f);
    m_gsFont.SetDestX((INTERNAL_RES_X - m_gsFont.GetTextWidth()) / 2);
    m_gsFont.SetDestY((INTERNAL_RES_Y / 2) + (((INTERNAL_RES_Y / 2) - (m_gsFont.GetTextHeight() * 2.25))));
    m_gsFont.Render();

    m_gsFont.SetModulateColor(1.0f, 1.0f, 1.0f, 1.0f);

    // m_gsFont.SetRotateX(0);

    //    // Adjust the title sprite alpha effect
    //    m_gsTitleSprite.SetModulateColor(1.0f, 1.0f, 1.0f, fTitleAlpha);
    //
    //    // Adjust the title sprite scale
    //    m_gsTitleSprite.SetScaleXY( (1.0f + fTitleAlpha * 0.05f), (1.0f + fTitleAlpha * 0.05f) );
    //
    //    // Position the title centered in the top half of the screen.
    //    m_gsTitleSprite.SetDestX((INTERNAL_RES_X - m_gsTitleSprite.GetScaledWidth()) / 2);
    //    m_gsTitleSprite.SetDestY((INTERNAL_RES_Y / 2) + (((INTERNAL_RES_Y / 2) - m_gsTitleSprite.GetHeight()) / 4));
    //
    //    // Render the title images.
    //    m_gsTitleSprite.Render();

    // Position the menu centered in the bottom half of the screen.
    m_gsMenu.SetDestX((INTERNAL_RES_X - m_gsMenu.GetWidth()) / 2);
    m_gsMenu.SetDestY(((INTERNAL_RES_Y / 2) - m_gsMenu.GetHeight()) / 2);

    // Render the menu using the specified colors.
    m_gsMenu.Render(m_gsMenuTextColor, m_gsMenuHighColor);

    // Render the frame rate.
    this->RenderFrameRate(0.25f);

    // Display the version number at the bottom right of the screen.
    m_gsFont.SetModulateColor(-1.0f, -1.0f, -1.0f, 0.25f);
    m_gsFont.SetText("VERSION %s", GAME_VERSION);
    m_gsFont.SetScaleXY(0.5f, 0.5f);
    m_gsFont.SetDestX(INTERNAL_RES_X - m_gsFont.GetTextWidth() - m_gsFont.GetLetterWidth());
    m_gsFont.SetDestY((m_gsFont.GetTextHeight() * 2));
    m_gsFont.Render();

    // Reset the default font values
    m_gsFont.SetModulateColor(1.0f, 1.0f, 1.0f, 1.0f);
    m_gsFont.SetScaleXY(1.0f, 1.0f);

    // Render the mouse cursor.
    this->RenderCursor();

    // Reset display after rendering in 2D.
    m_gsDisplay.EndRender2D();

    // Swap buffers to display results.
    SwapBuffers(this->GetDevice());

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Method Cleanup ////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Are we about to leave this section?
    if (m_nGameProgress != TITLE_SCREEN)
    {
        // Remember where we've come from.
        m_nPrevProgress = TITLE_SCREEN;
        // Reset method variables.
        m_bIsInitialized = FALSE;
    }

    return TRUE;
}

// *********************************************************************************************

//==============================================================================================
// GS_Snake::TitleOutro():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull and FALSE if not.
//==============================================================================================

BOOL GS_Snake::TitleOutro()
{

    // Has the display not been initialized?
    if (!this->m_gsDisplay.IsReady())
    {
        return FALSE;
    }

    // Has the game been paused?
    if (this->IsPaused())
    {
        // Unpause the game.
        this->Pause(FALSE);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Method Initialization /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Has the method not been initialized?
    if (m_bIsInitialized == FALSE)
    {
        // Initialize method variables.
        m_fAlpha = 1.0f;
        // Initialization completed.
        m_bIsInitialized = TRUE;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Method Logic //////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Are images not entirely transparent?
    if (m_fAlpha > 0.0f)
    {
        // Increase the transparency.
        m_fAlpha -= this->GetActionInterval(60 * 0.025f);
    }
    else
    {
        // Progress to the next section.
        m_nGameProgress = m_nNextProgress;
        // Images are transparent.
        m_fAlpha = 0.0f;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Method Rendering //////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Clear the screen to the specified color.
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reset display.
    glLoadIdentity();

    // Setup the display for rendering in 2D.
    m_gsDisplay.BeginRender2D(this->GetWindow());

    // Render the background image.
    this->RenderBackground(m_fAlpha);

    // // For every particle snake
    // for( int i = 0; i < MAX_PARTICLE_SNAKES; i++ )
    // {
    // // Set the particle effect color
    // m_snakeParticle.SetModulateColor(
    // m_particleSnake[i].color.fRed,
    // m_particleSnake[i].color.fGreen,
    // m_particleSnake[i].color.fBlue,
    // (m_particleSnake[i].color.fAlpha < m_fAlpha ? m_particleSnake[i].color.fAlpha : m_fAlpha)
    // );

    // // Update the particle snake position
    // m_snakeParticle.SetDestX(m_particleSnake[i].x);
    // m_snakeParticle.SetDestY(m_particleSnake[i].y);

    // // render the particle snake
    // m_snakeParticle.Render();
    // }

    // // Display the title centered on the screen
    // m_gsFont.SetModulateColor(0.0f, 1.0f, 1.0f, m_fAlpha);
    // m_gsFont.SetText("CYBER");
    // m_gsFont.SetScaleXY(5.0f, 5.0f);
    // m_gsFont.SetDestX((INTERNAL_RES_X - m_gsFont.GetTextWidth()) / 2);
    // m_gsFont.SetDestY((INTERNAL_RES_Y / 2) + (((INTERNAL_RES_Y / 2) - (m_gsFont.GetTextHeight() * 2.25))));
    // m_gsFont.Render();

    // m_gsFont.SetModulateColor(1.0f, 0.0f, 1.0f, m_fAlpha);
    // m_gsFont.SetText("SNAKE");
    // m_gsFont.SetScaleXY(5.0f, 5.0f);
    // m_gsFont.SetDestX((INTERNAL_RES_X - m_gsFont.GetTextWidth()) / 2);
    // m_gsFont.SetDestY((INTERNAL_RES_Y / 2) + (((INTERNAL_RES_Y / 2) - (m_gsFont.GetTextHeight() * 3.25))));
    // m_gsFont.Render();
    // m_gsFont.SetModulateColor(1.0f, 1.0f, 1.0f, 1.0f);

    // // Render and scroll the tile images.
    // m_gsTileSprite.SetModulateColor(1.0f, 1.0f, 1.0f, m_fAlpha);
    // m_gsTileSprite.AddScrollX(this->GetActionInterval(60));
    // m_gsTileSprite.AddScrollY(this->GetActionInterval(30));
    // m_gsTileSprite.RenderTiles(m_rcScreen);

    // Reset the default font values
    m_gsFont.SetModulateColor(1.0f, 1.0f, 1.0f, 1.0f);
    m_gsFont.SetScaleXY(1.0f, 1.0f);

    // Reset display after rendering in 2D.
    m_gsDisplay.EndRender2D();

    // Swap buffers to display results.
    SwapBuffers(this->GetDevice());

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Method Cleanup ////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Are we leaving this section?
    if (m_nGameProgress != TITLE_OUTRO)
    {
        // Remember where we've come from.
        m_nPrevProgress = TITLE_OUTRO;
        // Reset method variables.
        // m_gsTileSprite.SetModulateColor(1.0f, 1.0f, 1.0f, 1.0f);
        m_bIsInitialized = FALSE;
    }

    return TRUE;
}

// *********************************************************************************************

// *********************************************************************************************
// *** Option Screen Methods *******************************************************************
// *********************************************************************************************

//==============================================================================================
// GS_Snake::OptionIntro():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull and FALSE if not.
//==============================================================================================

BOOL GS_Snake::OptionIntro()
{

    // Has the display not been initialized?
    if (!this->m_gsDisplay.IsReady())
    {
        return FALSE;
    }

    // Has the game been paused?
    if (this->IsPaused())
    {
        // Unpause the game.
        this->Pause(FALSE);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Method Initialization /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Has the method not been initialized?
    if (m_bIsInitialized == FALSE)
    {
        // Where we want to go to next.
        m_nNextProgress = OPTION_SCREEN;
        // Initialization completed.
        m_bIsInitialized = TRUE;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Method Logic //////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // ...

    // Progress to the next section.
    m_nGameProgress = m_nNextProgress;

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Method Rendering //////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Clear the screen to the specified color.
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reset display.
    glLoadIdentity();

    // Setup the display for rendering in 2D.
    m_gsDisplay.BeginRender2D(this->GetWindow());

    // ...

    // Reset display after rendering in 2D.
    m_gsDisplay.EndRender2D();

    // Swap buffers to display results.
    SwapBuffers(this->GetDevice());

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Method Cleanup ////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Are we leaving this section?
    if (m_nGameProgress != OPTION_INTRO)
    {
        // Remember where we've come from.
        m_nPrevProgress = OPTION_INTRO;
        // Reset method variables.
        m_bIsInitialized = FALSE;
    }

    return TRUE;
}

// *********************************************************************************************

//==============================================================================================
// GS_Snake::OptionScreen():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull and FALSE if not.
//==============================================================================================

BOOL GS_Snake::OptionScreen()
{

    // Has the display not been initialized?
    if (!this->m_gsDisplay.IsReady())
    {
        return FALSE;
    }

    // Has the game been paused?
    if (this->IsPaused())
    {
        // Unpause the game.
        this->Pause(FALSE);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Method Initialization /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    char cTempString[64] = {0};

    BOOL bLeftPressed = TRUE;

    // Has the method not been initialized?
    if (m_bIsInitialized == FALSE)
    {
        // Initialize method variables.
        m_nOptionSelected = -1;
        // Clear the keyboard buffer.
        m_gsKeyboard.ClearBuffer();
        // Clear the controller buffer.
        m_gsController.ClearBuffer();
        // Setup variables for mouse input.
        m_nOldMouseX = m_gsMouse.GetX();
        m_nOldMouseY = m_gsMouse.GetY();
        m_bWasMouseReleased = FALSE;
        // Set the game settings to the current display settings.
        m_gsSettings.bEnableVSync = m_gsDisplay.IsVSyncEnabled();
        m_gsSettings.fFrameCap = this->GetFrameRate();
        m_gsSettings.bWindowedMode = this->IsWindowed();
        // Clear all current menu items.
        m_gsMenu.ClearOptions();
        // Set the menu title.
        m_gsMenu.SetTitle("   SELECT AN OPTION:  ");
        // Add menu options depending on settings.
        m_gsMenu.AddOption("");
        if (m_gsSettings.bWindowedMode)
        {
            m_gsMenu.AddOption("FULLSCREEN MODE.....NO");
        }
        else
        {
            m_gsMenu.AddOption("FULLSCREEN MODE....YES");
        }
        if (m_gsSettings.fFrameCap == 0.0f)
        {
            m_gsMenu.AddOption("CAP FRAMERATE...... NO");
        }
        else
        {
            m_gsMenu.AddOption("CAP FRAMERATE......YES");
        }
        if (m_gsSettings.bEnableVSync)
        {
            m_gsMenu.AddOption("ENABLE VSYNC.......YES");
        }
        else
        {
            m_gsMenu.AddOption("ENABLE VSYNC........NO");
        }
        m_gsMenu.AddOption("");
        sprintf(cTempString, "MUSIC VOLUME.......%03d", int(float(m_gsSettings.nMusicVolume) / 255.0f * 100.0f));
        m_gsMenu.AddOption(cTempString);
        sprintf(cTempString, "SOUND FX VOLUME....%03d", int(float(m_gsSettings.nEffectsVolume) / 255.0f * 100.0f));
        m_gsMenu.AddOption(cTempString);
        m_gsMenu.AddOption("");
        m_gsMenu.AddOption("         DONE         ");
        m_gsMenu.AddOption("");
        // Highlight the first option.
        m_gsMenu.SetHighlight(1);
        // Where we want to go to next.
        m_nNextProgress = OPTION_OUTRO;
        // Initialization completed.
        m_bIsInitialized = TRUE;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Get Keyboard Input ///////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Reset the selected option.
    m_nOptionSelected = -1;

    // Check to see wether a key was pressed.
    int nKey = m_gsKeyboard.GetBufferedKey();

    // Act depending on key pressed.
    switch (nKey)
    {
    // Was the up key pressed?
    case GSK_UP:
    case GSC_BUTTON_DPAD_UP:
        // Highlight previous option.
        m_gsMenu.HighlightPrev();
        m_gsSound.PlaySample(SAMPLE_OPTION);
        break;
    // Was the down key pressed?
    case GSK_DOWN:
    case GSC_BUTTON_DPAD_DOWN:
        // Highlight next option.
        m_gsMenu.HighlightNext();
        m_gsSound.PlaySample(SAMPLE_OPTION);
        break;
    // Was the left key pressed?
    case GSK_LEFT:
    case GSC_BUTTON_DPAD_LEFT:
        // Is the last option not highlighted?
        if (m_gsMenu.GetHighlight() != 7)
        {
            // Remeber which option is highlighted.
            m_nOptionSelected = m_gsMenu.GetHighlight();
            m_gsSound.PlaySample(SAMPLE_SELECT);
            bLeftPressed = TRUE;
        }
        break;
    // Was the right key pressed?
    case GSK_RIGHT:
    case GSC_BUTTON_DPAD_RIGHT:
        // Is the last option not highlighted?
        if (m_gsMenu.GetHighlight() != 7)
        {
            // Remeber which option is highlighted.
            m_nOptionSelected = m_gsMenu.GetHighlight();
            m_gsSound.PlaySample(SAMPLE_SELECT);
            bLeftPressed = FALSE;
        }
        break;
    // Was the enter key pressed?
    case GSK_ENTER:
    case GSC_BUTTON_A:
        // Remeber which option is highlighted.
        m_nOptionSelected = m_gsMenu.GetHighlight();
        m_gsSound.PlaySample(SAMPLE_SELECT);
        break;
    // Was the escape key pressed?
    case GSK_ESCAPE:
    case GSC_BUTTON_B:
        // Progress to the next section.
        m_nGameProgress = m_nNextProgress;
        m_gsSound.PlaySample(SAMPLE_SELECT);
        break;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Get Mouse Input //////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    RECT rcOption;

    // Has the mouse cursor moved from its previous position?
    if ((m_nOldMouseX != m_gsMouse.GetX()) || (m_nOldMouseY != m_gsMouse.GetY()))
    {
        // Check each option to see if the mouse coordinates are within the option area.
        for (int nLoop = 0; nLoop < m_gsMenu.GetNumOptions(); nLoop++)
        {
            // Determine the screen coordinates of the next option.
            m_gsMenu.GetOptionRect(nLoop, &rcOption);
            // Are the mouse coordinates within the option area?
            if (m_gsCollide.IsCoordInRect(m_gsMouse.GetX(), m_gsMouse.GetY(), rcOption))
            {
                // Is the option not highlighted already?
                if (nLoop != m_gsMenu.GetHighlight())
                {
                    // Highlight the selected option.
                    m_gsMenu.SetHighlight(nLoop);
                    // Play the appropriate sound effect.
                    m_gsSound.PlaySample(SAMPLE_OPTION);
                }
            }
        }
        // Assign new mouse coordinates.
        m_nOldMouseX = m_gsMouse.GetX();
        m_nOldMouseY = m_gsMouse.GetY();
    }

    // Is the left mouse button not pressed?
    if (!m_gsMouse.IsLeftPressed())
    {
        m_bWasMouseReleased = TRUE;
    }

    // Was the left mouse button pressed?
    if ((m_gsMouse.IsLeftPressed()) && (m_bWasMouseReleased))
    {
        // Determine the screen coordinates of the highlighted option.
        m_gsMenu.GetHighlightRect(&rcOption);
        // Check if mouse coordinates are within the area of the highligted option.
        if (m_gsCollide.IsCoordInRect(m_gsMouse.GetX(), m_gsMouse.GetY(), rcOption))
        {
            // Remeber which option is highlighted.
            m_nOptionSelected = m_gsMenu.GetHighlight();
            m_bWasMouseReleased = FALSE;
            // Play the appropriate sound effect.
            m_gsSound.PlaySample(SAMPLE_SELECT);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Process Menu Commands ////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Act depending on option selected.
    switch (m_nOptionSelected)
    {
    // Fullscreen or windowed mode?
    case 1:
        // Are we currently in windowed mode?
        if (m_gsSettings.bWindowedMode)
        {
            // Set menu text to reflect changes.
            m_gsMenu.SetOption(1, "FULLSCREEN MODE....YES");
            // Set to fullscreen mode.
            m_gsSettings.bWindowedMode = FALSE;
        }
        else
        {
            // Set menu text to reflect changes.
            m_gsMenu.SetOption(1, "FULLSCREEN MODE.....NO");
            // Set to windowed mode.
            m_gsSettings.bWindowedMode = TRUE;
        }
        break;
    // Framecap enabled?
    case 2:
        // Is there no frame cap?
        if (m_gsSettings.fFrameCap == 0.0f)
        {
            // Set menu text to reflect changes.
            m_gsMenu.SetOption(2, "CAP FRAMERATE......YES");
            // Set the framerate cap to 60fps
            m_gsSettings.fFrameCap = 60.0f;
        }
        else
        {
            // Set menu text to reflect changes.
            m_gsMenu.SetOption(2, "CAP FRAMERATE...... NO");
            // Turn the framerate cap off
            m_gsSettings.fFrameCap = 0.0f;
        }
        // Set the framerate to the cap the user selected.
        this->SetFrameRate(m_gsSettings.fFrameCap);
        break;
    // VSync enabled or disabled?
    case 3:
        // Is vertical sync currently enabled?
        if (m_gsSettings.bEnableVSync)
        {
            // Set menu text to reflect changes.
            m_gsMenu.SetOption(3, "ENABLE VSYNC........NO");
            // Disable vertical sync.
            m_gsSettings.bEnableVSync = FALSE;
        }
        else
        {
            // Set menu text to reflect changes.
            m_gsMenu.SetOption(3, "ENABLE VSYNC.......YES");
            // Enable vertical sync.
            m_gsSettings.bEnableVSync = TRUE;
        }
        // Enable / disable vertical sync depending on selection.
        m_gsDisplay.EnableVSync(m_gsSettings.bEnableVSync);
        break;
    // Determine music volume.
    case 5:
        // Was the left arrow pressed?
        if (bLeftPressed)
        {
            // Decrease music volume.
            m_gsSettings.nMusicVolume -= 3;
            // Wrap volume if out of bounds.
            if (m_gsSettings.nMusicVolume < 0)
            {
                m_gsSettings.nMusicVolume = 255;
            }
        }
        else
        {
            // Increase music volume.
            m_gsSettings.nMusicVolume += 3;
            // Wrap volume if out of bounds.
            if (m_gsSettings.nMusicVolume > 255)
            {
                m_gsSettings.nMusicVolume = 0;
            }
        }
        // Set music volume to reflect changes.
        m_gsSound.SetStreamVolume(m_gsSettings.nMusicVolume);
        // Set menu text to reflect changes.
        sprintf(cTempString, "MUSIC VOLUME.......%03d", int(float(m_gsSettings.nMusicVolume) / 255.0f * 100.0f));
        m_gsMenu.SetOption(5, cTempString);
        break;
    // Determine sound effects volume.
    case 6:
        if (bLeftPressed)
        {
            // Decrease sound effects volume.
            m_gsSettings.nEffectsVolume -= 3;
            // Wrap volume if out of bounds.
            if (m_gsSettings.nEffectsVolume < 0)
            {
                m_gsSettings.nEffectsVolume = 255;
            }
        }
        else
        {
            // Increase sound effects volume.
            m_gsSettings.nEffectsVolume += 3;
            // Wrap volume if out of bounds.
            if (m_gsSettings.nEffectsVolume > 255)
            {
                m_gsSettings.nEffectsVolume = 0;
            }
        }
        // Set sound effects volume to reflect changes.
        m_gsSound.SetSampleMaster(m_gsSettings.nEffectsVolume);
        // Set menu text to reflect changes.
        sprintf(cTempString, "SOUND FX VOLUME....%03d", int(float(m_gsSettings.nEffectsVolume) / 255.0f * 100.0f));
        m_gsMenu.SetOption(6, cTempString);
        break;
    // Done setting options.
    case 8:
        // Progress to the next section.
        m_nGameProgress = m_nNextProgress;
        break;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Method Rendering //////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Clear the screen to the specified color.
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reset display.
    glLoadIdentity();

    // Setup the display for rendering in 2D.
    m_gsDisplay.BeginRender2D(this->GetWindow());

    // Render the background image.
    this->RenderBackground(0.5f);

    // Position the menu centered horizontally and vertically in the screen.
    m_gsMenu.SetDestX((INTERNAL_RES_X - m_gsMenu.GetWidth()) / 2);
    m_gsMenu.SetDestY((INTERNAL_RES_Y - m_gsMenu.GetHeight()) / 2);

    // Render the menu using the specified colors.
    m_gsMenu.Render(m_gsMenuTextColor, m_gsMenuHighColor);

    // Render the frame rate.
    this->RenderFrameRate(0.25f);

    // Render the mouse cursor.
    this->RenderCursor();

    // Reset display after rendering in 2D.
    m_gsDisplay.EndRender2D();

    // Swap buffers to display results.
    SwapBuffers(this->GetDevice());

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Method Cleanup ////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Are we about to leave this section?
    if (m_nGameProgress != OPTION_SCREEN)
    {
        // Remember where we've come from.
        m_nPrevProgress = OPTION_SCREEN;
        // Have we changed the graphics mode?
        if (m_gsSettings.bWindowedMode != this->IsWindowed())
        {
            // Change to windowed or fullscreen mode as selected by the user.
            this->SetMode(m_gsSettings.nDisplayWidth, m_gsSettings.nDisplayHeight, this->GetColorDepth(), !this->IsWindowed());

            // Set values for scaling
            this->SetRenderScaling(this->GetWidth(), this->GetHeight(), m_bKeepAspectRatio);

            // Has the mode change been successful?
            m_gsSettings.bWindowedMode = this->IsWindowed();
        }
        // Save game settings.
        this->SaveSettings();
        // Reset method variables.
        m_bIsInitialized = FALSE;
    }

    return TRUE;
}

// *********************************************************************************************

//==============================================================================================
// GS_Snake::OptionOutro():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull and FALSE if not.
//==============================================================================================

BOOL GS_Snake::OptionOutro()
{

    // Has the display not been initialized?
    if (!this->m_gsDisplay.IsReady())
    {
        return FALSE;
    }

    // Has the game been paused?
    if (this->IsPaused())
    {
        // Unpause the game.
        this->Pause(FALSE);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Method Initialization /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Has the method not been initialized?
    if (m_bIsInitialized == FALSE)
    {
        // Where we want to go to next.
        m_nNextProgress = TITLE_INTRO;
        // Initialization completed.
        m_bIsInitialized = TRUE;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Method Logic //////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // ...

    // Progress to the next section.
    m_nGameProgress = m_nNextProgress;

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Method Rendering //////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Clear the screen to the specified color.
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reset display.
    glLoadIdentity();

    // Setup the display for rendering in 2D.
    m_gsDisplay.BeginRender2D(this->GetWindow());

    // ...

    // Reset display after rendering in 2D.
    m_gsDisplay.EndRender2D();

    // Swap buffers to display results.
    SwapBuffers(this->GetDevice());

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Method Cleanup ////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Are we leaving this section?
    if (m_nGameProgress != OPTION_OUTRO)
    {
        // Remember where we've come from.
        m_nPrevProgress = OPTION_OUTRO;
        // Reset method variables.
        m_bIsInitialized = FALSE;
    }

    return TRUE;
}

// *********************************************************************************************

// *********************************************************************************************
// *** Game Play Methods ***********************************************************************
// *********************************************************************************************

//==============================================================================================
// GS_Snake::PlayIntro():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull and FALSE if not.
//==============================================================================================

BOOL GS_Snake::PlayIntro()
{

    // Has the display not been initialized?
    if (!this->m_gsDisplay.IsReady())
    {
        return FALSE;
    }

    // Has the game been paused?
    if (this->IsPaused())
    {
        // Unpause the game.
        this->Pause(FALSE);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Method Initialization /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Has the method not been initialized?
    if (m_bIsInitialized == FALSE)
    {
        // Initialize method variables.
        m_nOptionSelected = -1;
        // Clear the keyboard buffer.
        m_gsKeyboard.ClearBuffer();
        // Clear the controller buffer.
        m_gsController.ClearBuffer();
        // Setup variables for mouse input.
        m_nOldMouseX = m_gsMouse.GetX();
        m_nOldMouseY = m_gsMouse.GetY();
        m_bWasMouseReleased = FALSE;
        // Clear all current menu items.
        m_gsMenu.ClearOptions();
        // Set the menu title.
        // m_gsMenu.SetTitle("SELECT YOUR DIFFICULTY:");
        m_gsMenu.SetTitle(" SELECT GAME MODE: ");
        // Add the menu options.
        m_gsMenu.AddOption("");
        m_gsMenu.AddOption("     CLASSIC      ");
        m_gsMenu.AddOption("      REMIX       ");
        // m_gsMenu.AddOption("        EXPERT         ");
        m_gsMenu.AddOption("");
        // Highlight the second option.
        m_gsMenu.SetHighlight(2);
        // Is the title music playing?
        if (m_gsSound.IsStreamPlaying())
        {
            // Stop title music from playing.
            m_gsSound.StopStream();
        }
        // Where we want to go to next.
        m_nNextProgress = PLAY_GAME;
        // Initialization completed.
        m_bIsInitialized = TRUE;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Get Keyboard Input ///////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Check to see wether a key was pressed.
    int nKey = m_gsKeyboard.GetBufferedKey();

    // Act depending on key pressed.
    switch (nKey)
    {
    // Was the up key pressed?
    case GSK_UP:
    case GSC_BUTTON_DPAD_UP:
        // Highlight the previous option.
        m_gsMenu.HighlightPrev();
        m_gsSound.PlaySample(SAMPLE_OPTION);
        break;
    // Was the down key pressed?
    case GSK_DOWN:
    case GSC_BUTTON_DPAD_DOWN:
        // Highlight the next option.
        m_gsMenu.HighlightNext();
        m_gsSound.PlaySample(SAMPLE_OPTION);
        break;
    // Was the enter key pressed?
    case GSK_ENTER:
    case GSC_BUTTON_A:
        // Remeber which option is highlighted.
        m_nOptionSelected = m_gsMenu.GetHighlight();
        m_gsSound.PlaySample(SAMPLE_SELECT);
        break;
    // Was the escape key pressed?
    case GSK_ESCAPE:
    case GSC_BUTTON_B:
        // Return to the title screen.
        m_nGameProgress = TITLE_INTRO;
        m_bWasKeyReleased = FALSE;
        break;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Get Mouse Input //////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    RECT rcOption;

    // Has the mouse cursor moved from its previous position?
    if ((m_nOldMouseX != m_gsMouse.GetX()) || (m_nOldMouseY != m_gsMouse.GetY()))
    {
        // Check each option to see if the mouse coordinates are within the option area.
        for (int nLoop = 0; nLoop < m_gsMenu.GetNumOptions(); nLoop++)
        {
            // Determine the screen coordinates of the next option.
            m_gsMenu.GetOptionRect(nLoop, &rcOption);
            // Are the mouse coordinates within the option area?
            if (m_gsCollide.IsCoordInRect(m_gsMouse.GetX(), m_gsMouse.GetY(), rcOption))
            {
                // Is the option not highlighted already?
                if (nLoop != m_gsMenu.GetHighlight())
                {
                    // Highlight the selected option.
                    m_gsMenu.SetHighlight(nLoop);
                    // Play the appropriate sound effect.
                    m_gsSound.PlaySample(SAMPLE_OPTION);
                }
            }
        }
        // Assign new mouse coordinates.
        m_nOldMouseX = m_gsMouse.GetX();
        m_nOldMouseY = m_gsMouse.GetY();
    }

    // Is the left mouse button not pressed?
    if (!m_gsMouse.IsLeftPressed())
    {
        m_bWasMouseReleased = TRUE;
    }

    // Was the left mouse button pressed?
    if ((m_gsMouse.IsLeftPressed()) && (m_bWasMouseReleased))
    {
        // Determine the screen coordinates of the highlighted option.
        m_gsMenu.GetHighlightRect(&rcOption);
        // Check if mouse coordinates are within the area of the highligted option.
        if (m_gsCollide.IsCoordInRect(m_gsMouse.GetX(), m_gsMouse.GetY(), rcOption))
        {
            // Remeber which option is highlighted.
            m_nOptionSelected = m_gsMenu.GetHighlight();
            m_bWasMouseReleased = FALSE;
            // Play the appropriate sound effect.
            m_gsSound.PlaySample(SAMPLE_SELECT);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Process Menu Commands ////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Act depending on option selected.
    switch (m_nOptionSelected)
    {
    // ...
    case 1:
        // Remember what game mode was selected
        m_gameMode = EASY_MODE;

        // Rat is static by default
        m_defaultRatState = RAT_STATE_STATIC;

        // Progress to the next section.
        m_nGameProgress = m_nNextProgress;
        break;
    // ...
    case 2:
        // Remember what game mode was selected
        m_gameMode = NORMAL_MODE;

        // Rat is moving by default
        m_defaultRatState = RAT_STATE_MOVING;

        // Progress to the next section.
        m_nGameProgress = m_nNextProgress;
        break;
    /*
    // ...
    case 3:
        // Remember what game mode was selected
        m_gameMode = HARD_MODE;

        // Rat is moving by default
        m_defaultRatState = RAT_STATE_MOVING;

        // Progress to the next section.
        m_nGameProgress = m_nNextProgress;
        break;
    */
    // ...
    default:
        break;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Method Rendering //////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Clear the screen to the specified color.
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reset display.
    glLoadIdentity();

    // Setup the display for rendering in 2D.
    m_gsDisplay.BeginRender2D(this->GetWindow());

    // Center the menu horizontally and vertically in the screen.
    m_gsMenu.SetDestX((INTERNAL_RES_X - m_gsMenu.GetWidth()) / 2);
    m_gsMenu.SetDestY((INTERNAL_RES_Y - m_gsMenu.GetHeight()) / 2);

    // Render the menu to the back surface using the specified colors.
    m_gsMenu.Render(m_gsMenuTextColor, m_gsMenuHighColor);

    // Render the mouse cursor.
    this->RenderCursor();

    // Reset display after rendering in 2D.
    m_gsDisplay.EndRender2D();

    // Swap buffers to display results.
    SwapBuffers(this->GetDevice());

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Method Cleanup ////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Are we leaving this section?
    if (m_nGameProgress != PLAY_INTRO)
    {
        // Remember where we've come from.
        m_nPrevProgress = PLAY_INTRO;
        // Reset method variables.
        m_bIsInitialized = FALSE;
        // Are we about to start playing the game?
        if (m_nGameProgress == PLAY_GAME)
        {
            // Start playing game music at the specified volume, 1x speed and looping infinitely.
            m_gsSound.PlayStream(MUSIC_GAME, m_gsSettings.nMusicVolume);
        }
    }

    return TRUE;
}

// *********************************************************************************************

//==============================================================================================
// GS_Snake::PlayGame():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull and FALSE if not.
//==============================================================================================

BOOL GS_Snake::PlayGame()
{

    // Has the display not been initialized?
    if (!this->m_gsDisplay.IsReady())
    {
        return FALSE;
    }

    // Has the game been paused?
    if (this->IsPaused())
    {
        // Remember where we've come from.
        m_nPrevProgress = m_nGameProgress;
        // Progress to pause section.
        m_nGameProgress = PLAY_PAUSE;
        return TRUE;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Method Initialization /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Are we starting a new game?
    if (m_nPrevProgress == PLAY_INTRO)
    {
        // Reset class variables.
        m_nPrevProgress = PLAY_GAME;
        m_fInterval = 0.0f;
        m_nCounter = 0;

        // Set default game values
        m_currentWorld = 1;
        m_currentLevel = 1;
        m_ratsEaten = 0;
        m_lives = DEFAULT_LIVES;
        m_lScore = 0l;

        // Load the level
        m_level.Load(m_currentLevel);
        m_nGameProgress = PLAY_UPDATE;

        // Re-create the snake
        m_snake.Reset(m_level.GetSnakeStartX(), m_level.GetSnakeStartY(), SNAKE_MOVEMENT_INTERVAL_MAX);
        m_snake.SetState(SNAKE_STATE_MOVING);

        // Re-create the rat
        int ratX = (m_rcPlayArea.right - m_rcPlayArea.left) - (((m_rcPlayArea.right - m_rcPlayArea.left) - ((m_rcPlayArea.right - m_rcPlayArea.left) % RAT_ELEMENT_WIDTH)) / 4);
        int ratY = (((m_rcPlayArea.top - m_rcPlayArea.bottom) - ((m_rcPlayArea.top - m_rcPlayArea.bottom) % RAT_ELEMENT_HEIGHT)) / RAT_ELEMENT_HEIGHT) / 2 * RAT_ELEMENT_HEIGHT;
        m_rat.Reset(ratX, ratY, RAT_MOVEMENT_INTERVAL_MAX);
        m_rat.SetState(m_defaultRatState);

        // Is the game in hard mode?
        if (m_gameMode == HARD_MODE)
        {
            // Speed-up the game
            for (int i = 0; i < 3; i++)
            {
                m_snake.Speedup();
                m_rat.Speedup();
            }
        }

        // Speed-up the game depending on the world level
        for (int i = 0; i < m_currentWorld; i++)
        {
            m_snake.Speedup();
            m_rat.Speedup();
        }

        // Clear the keyboard buffer.
        m_gsKeyboard.ClearBuffer();

        // Clear the controller buffer.
        m_gsController.ClearBuffer();
    }
    // Are we returning to the same game?
    else if (m_nPrevProgress != PLAY_GAME)
    {
        // Reset class variables.
        m_nPrevProgress = PLAY_GAME;

        // Clear the keyboard buffer.
        m_gsKeyboard.ClearBuffer();
        
        // Clear the controller buffer.
        m_gsController.ClearBuffer();
}

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Get Keyboard Input ///////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Check to see wether a key was pressed.
    int nKey = m_gsKeyboard.GetBufferedKey(FALSE);

    // Act depending on which key was pressed.
    switch (nKey)
    {
    // Was left arrow key pressed?
    case GSK_LEFT:
    case GSC_BUTTON_DPAD_LEFT:
        // Move the snake in the appropriate direction
        m_snake.SetMovementDirection(SNAKE_MOVE_LEFT);
        break;
    // Was right arrow key pressed?
    case GSK_RIGHT:
    case GSC_BUTTON_DPAD_RIGHT:
        // Move the snake in the appropriate direction
        m_snake.SetMovementDirection(SNAKE_MOVE_RIGHT);
        break;
    // Was down arrow key pressed?
    case GSK_DOWN:
    case GSC_BUTTON_DPAD_DOWN:
        // Move the snake in the appropriate direction
        m_snake.SetMovementDirection(SNAKE_MOVE_DOWN);
        break;
    // Was up arrow key pressed?
    case GSK_UP:
    case GSC_BUTTON_DPAD_UP:
        // Move the snake in the appropriate direction
        m_snake.SetMovementDirection(SNAKE_MOVE_UP);
        break;
    // Was the 'M' key pressed?
    case GSK_M:
        // Is the music volume muted?
        if (m_gsSound.GetStreamVolume() == 0)
        {
            // Set the music volume to the game settings volume.
            m_gsSound.SetStreamVolume(m_gsSettings.nMusicVolume);
        }
        else
        {
            // Mute the music volume.
            m_gsSound.SetStreamVolume(0);
        }
        m_gsKeyboard.ClearBuffer();
        m_gsController.ClearBuffer();
        break;
    // Was the 'S' key pressed?
    case GSK_S:
        // Is the sound effects volume muted?
        if (m_gsSound.GetSampleMaster() == 0)
        {
            // Set the sound effects volume to the game settings volume.
            m_gsSound.SetSampleMaster(m_gsSettings.nEffectsVolume);
        }
        else
        {
            // Mute the sound effects volume.
            m_gsSound.SetSampleMaster(0);
        }
        m_gsKeyboard.ClearBuffer();
        m_gsController.ClearBuffer();
        break;
    /*
    // Was the spacebar key pressed?
    case GSK_SPACE:
        // Go to the play exit method.
        m_nGameProgress = PLAY_OUTRO;
        m_gsKeyboard.ClearBuffer();
        m_gsController.ClearBuffer();
        break;
    */
    // Was the escape key pressed?
    case GSK_ESCAPE:
    case GSC_BUTTON_B:
        // Go to the play exit method.
        m_nGameProgress = PLAY_EXIT;
        m_gsKeyboard.ClearBuffer();
        m_gsController.ClearBuffer();
        break;
    default:
        // ...
        m_gsKeyboard.ClearBuffer();
        m_gsController.ClearBuffer();
        break;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Method Logic //////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    static int nOldSnakeDirection = SNAKE_MOVE_RIGHT;

    // Determine the interval required to perform an action 60 times every second at the
    // current frame rate, and add it to the previous intervals.
    m_fInterval += this->GetActionInterval(60);

    // Have all the intervals added up enough to perform an action?
    while (m_fInterval >= 1.0f)
    {
        RECT rcRat, rcSnakeHead;

        // Update the snake
        m_snake.Update();

        // Has the snake's movement direction changed?
        if (m_snake.GetMovementDirection() != nOldSnakeDirection)
        {
            // Play the appropriate sound effect
            m_gsSound.PlaySample(SAMPLE_SNAKE_INPUT);

            // Remember the current direction
            nOldSnakeDirection = m_snake.GetMovementDirection();
        }

        // Get the snake head coordinates at the current position
        m_snake.GetCollideRect(&rcSnakeHead);

        // Is the snake not dead or dying?
        if (m_snake.GetState() < SNAKE_STATE_DYING)
        {
            // Has the snake head collided with it's body or an element on the map?
            if ((m_snake.CheckCollision(rcSnakeHead, TRUE) == SNAKE_COLLIDE_BODY) || (m_level.CheckCollision(rcSnakeHead)))
            {
                // The snake died
                m_snake.SetState(SNAKE_STATE_DYING);

                // Play the appropriate sound effect
                m_gsSound.PlaySample(SAMPLE_SNAKE_DYING);

                // Kill the rat as well
                m_rat.SetState(RAT_STATE_EATEN);
                // std::cout << "Bonk!\n";
            }
        }
        else if (m_snake.GetState() == SNAKE_STATE_DEAD)
        {
            // The player loses a life
            m_lives--;
            m_nGameProgress = PLAY_UPDATE;

            // Has the player lost all his lives?
            if (m_lives <= 0)
            {
                // Stop playing the game
                m_nGameProgress = PLAY_OUTRO;
            }
            else
            {
                // Reset the snake
                m_snake.Reset(m_level.GetSnakeStartX(), m_level.GetSnakeStartY());
                m_snake.SetState(SNAKE_STATE_MOVING);

                // Respawn the rat
                int ratX = (rand() % (((m_rcPlayArea.right - m_rcPlayArea.left) - ((m_rcPlayArea.right - m_rcPlayArea.left) % RAT_ELEMENT_WIDTH)) / RAT_ELEMENT_WIDTH) * RAT_ELEMENT_WIDTH);
                int ratY = (rand() % (((m_rcPlayArea.top - m_rcPlayArea.bottom) - ((m_rcPlayArea.top - m_rcPlayArea.bottom) % RAT_ELEMENT_HEIGHT)) / RAT_ELEMENT_HEIGHT) * RAT_ELEMENT_HEIGHT);
                m_rat.SetPosition(ratX, ratY);
                m_rat.GetCollideRect(&rcRat, RAT_MOVE_NONE);

                // Make certain the rat does not respawn on the snake or the level
                while ((m_snake.CheckCollision(rcRat) != SNAKE_COLLIDE_NONE) || (m_level.CheckCollision(rcRat)))
                {
                    ratX = (rand() % (((m_rcPlayArea.right - m_rcPlayArea.left) - ((m_rcPlayArea.right - m_rcPlayArea.left) % RAT_ELEMENT_WIDTH)) / RAT_ELEMENT_WIDTH) * RAT_ELEMENT_WIDTH);
                    ratY = (rand() % (((m_rcPlayArea.top - m_rcPlayArea.bottom) - ((m_rcPlayArea.top - m_rcPlayArea.bottom) % RAT_ELEMENT_HEIGHT)) / RAT_ELEMENT_HEIGHT) * RAT_ELEMENT_HEIGHT);
                    m_rat.SetPosition(ratX, ratY);
                    m_rat.GetCollideRect(&rcRat, RAT_MOVE_NONE);
                }

                // Reset the rat and start it moving again
                m_rat.Reset(ratX, ratY);
                m_rat.SetState(m_defaultRatState);

                // Clear the keyboard buffer
                m_gsKeyboard.ClearBuffer();

                // Clear the controller buffer.
                m_gsController.ClearBuffer();
            }
            // std::cout << "Game Over\n";
        }

        // Can the rat be eaten?
        if (m_rat.GetState() == m_defaultRatState)
        {
            // Get the rat coordinates at the current position
            m_rat.GetCollideRect(&rcRat, RAT_MOVE_NONE);

            // Has the snake head collided with the rat?
            if (m_snake.CheckCollision(rcRat) == SNAKE_COLLIDE_HEAD)
            {
                // Update the player score
                if (m_gameMode == EASY_MODE)
                {
                    m_lScore += (((RATE_BASE_SCORE * m_currentLevel) / 2) * m_currentWorld);
                }
                else if (m_gameMode == HARD_MODE)
                {
                    m_lScore += (((RATE_BASE_SCORE * m_currentLevel) + ((RATE_BASE_SCORE * m_currentLevel) / 2)) * m_currentWorld);
                }
                else
                {
                    m_lScore += ((RATE_BASE_SCORE * m_currentLevel) * m_currentWorld);
                }
                m_ratsEaten++;

                // Eat the rat
                m_rat.SetState(RAT_STATE_EATEN);

                // Play the appropriate sound effect
                m_gsSound.SetSamplePanning(SAMPLE_RAT_DYING, round(float(rcRat.right) / m_rcPlayArea.right * 255.0f));
                m_gsSound.PlaySample(SAMPLE_RAT_DYING);

                // Grow the snake by the specified number of segments
                if (m_snake.Grow(16))
                {
                    // Play the appropriate sound effect
                    m_gsSound.PlaySample(SAMPLE_SNAKE_GROWING);
                }

                // Make the snake faster
                m_snake.Speedup();

                // Make the rats faster
                m_rat.Speedup();

                // std::cout << "Yum!\n";
            }
        }
        else if ((m_rat.GetState() == RAT_STATE_DEAD) && (m_ratsEaten < RATS_PER_LEVEL))
        {
            // Is the snake still alive?
            if (m_snake.GetState() < SNAKE_STATE_DYING)
            {
                // Respawn the rat
                int ratX = (rand() % (((m_rcPlayArea.right - m_rcPlayArea.left) - ((m_rcPlayArea.right - m_rcPlayArea.left) % RAT_ELEMENT_WIDTH)) / RAT_ELEMENT_WIDTH) * RAT_ELEMENT_WIDTH);
                int ratY = (rand() % (((m_rcPlayArea.top - m_rcPlayArea.bottom) - ((m_rcPlayArea.top - m_rcPlayArea.bottom) % RAT_ELEMENT_HEIGHT)) / RAT_ELEMENT_HEIGHT) * RAT_ELEMENT_HEIGHT);
                m_rat.SetPosition(ratX, ratY);
                m_rat.GetCollideRect(&rcRat, RAT_MOVE_NONE);

                // Make certain the rat does not respawn on the snake or the level
                while ((m_snake.CheckCollision(rcRat) != SNAKE_COLLIDE_NONE) || (m_level.CheckCollision(rcRat)))
                {
                    ratX = (rand() % (((m_rcPlayArea.right - m_rcPlayArea.left) - ((m_rcPlayArea.right - m_rcPlayArea.left) % RAT_ELEMENT_WIDTH)) / RAT_ELEMENT_WIDTH) * RAT_ELEMENT_WIDTH);
                    ratY = (rand() % (((m_rcPlayArea.top - m_rcPlayArea.bottom) - ((m_rcPlayArea.top - m_rcPlayArea.bottom) % RAT_ELEMENT_HEIGHT)) / RAT_ELEMENT_HEIGHT) * RAT_ELEMENT_HEIGHT);
                    m_rat.SetPosition(ratX, ratY);
                    m_rat.GetCollideRect(&rcRat, RAT_MOVE_NONE);
                }

                // Start the rat moving again
                m_rat.SetState(m_defaultRatState);
            }
        }

        // Move the rat in a random direction
        int ratMoveDirection = (rand() % 4);

        // Get the coordinates of the rat at the new position
        m_rat.GetCollideRect(&rcRat, ratMoveDirection);

        // Check if the movement causes a collision with the snake or the level
        if ((m_snake.CheckCollision(rcRat) != SNAKE_COLLIDE_NONE) || (m_level.CheckCollision(rcRat)))
        {
            // Try to move the rat in a different direction
            for (int i = 0; i < 4; i++)
            {
                // Skip the direction already tested
                if (i == ratMoveDirection)
                    continue;

                // Get the coordinates of the rat at the new position
                m_rat.GetCollideRect(&rcRat, i);

                // Was there no collision?
                if ((m_snake.CheckCollision(rcRat) == SNAKE_COLLIDE_NONE) && (!m_level.CheckCollision(rcRat)))
                {
                    // Move the rat in the specified direction
                    m_rat.SetMovementDirection(i);
                    break;
                }
            }
        }
        else
        {
            // Move the rat in the specified direction
            m_rat.SetMovementDirection(ratMoveDirection);
        }

        // Get the rat coordinates at the current position
        m_rat.GetCollideRect(&rcRat, RAT_MOVE_NONE);

        // Update the rat
        m_rat.Update();

        // Has the rat moved?
        if ((m_rat.GetState() == RAT_STATE_MOVING) && ((m_rat.GetDestX() != rcRat.left) || (m_rat.GetDestY() != rcRat.bottom)))
        {

            // Play the appropriate sound effect
            m_gsSound.SetSampleVolume(SAMPLE_RAT_MOVING, 63);
            m_gsSound.SetSamplePanning(SAMPLE_RAT_MOVING, round(float(rcRat.right) / m_rcPlayArea.right * 255.0f));
            m_gsSound.PlaySample(SAMPLE_RAT_MOVING);
        }

        // Have all the rats for the level been eaten?
        if ((m_ratsEaten >= RATS_PER_LEVEL))
        { // && (m_rat.GetState() == RAT_STATE_DEAD) ) {
            // Go to the next level
            m_currentLevel++;

            // Have we reached the last level?
            if (m_currentLevel > MAX_LEVEL)
            {
                // Go to the next world
                m_currentWorld++;

                // Restart at the first level
                m_currentLevel = MIN_LEVEL;
            }

            // Load the new level
            m_level.Load(m_currentLevel);
            m_nGameProgress = PLAY_UPDATE;

            // Reset the snake
            m_snake.Reset(m_level.GetSnakeStartX(), m_level.GetSnakeStartY(), SNAKE_MOVEMENT_INTERVAL_MAX);
            m_snake.SetState(SNAKE_STATE_MOVING);
            m_ratsEaten = 0;

            // Respawn the rat
            int ratX = (rand() % (((m_rcPlayArea.right - m_rcPlayArea.left) - ((m_rcPlayArea.right - m_rcPlayArea.left) % RAT_ELEMENT_WIDTH)) / RAT_ELEMENT_WIDTH) * RAT_ELEMENT_WIDTH);
            int ratY = (rand() % (((m_rcPlayArea.top - m_rcPlayArea.bottom) - ((m_rcPlayArea.top - m_rcPlayArea.bottom) % RAT_ELEMENT_HEIGHT)) / RAT_ELEMENT_HEIGHT) * RAT_ELEMENT_HEIGHT);
            m_rat.SetPosition(ratX, ratY);
            m_rat.GetCollideRect(&rcRat, RAT_MOVE_NONE);

            // Make certain the rat does not respawn on the snake or the level
            while ((m_snake.CheckCollision(rcRat) != SNAKE_COLLIDE_NONE) || (m_level.CheckCollision(rcRat)))
            {
                ratX = (rand() % (((m_rcPlayArea.right - m_rcPlayArea.left) - ((m_rcPlayArea.right - m_rcPlayArea.left) % RAT_ELEMENT_WIDTH)) / RAT_ELEMENT_WIDTH) * RAT_ELEMENT_WIDTH);
                ratY = (rand() % (((m_rcPlayArea.top - m_rcPlayArea.bottom) - ((m_rcPlayArea.top - m_rcPlayArea.bottom) % RAT_ELEMENT_HEIGHT)) / RAT_ELEMENT_HEIGHT) * RAT_ELEMENT_HEIGHT);
                m_rat.SetPosition(ratX, ratY);
                m_rat.GetCollideRect(&rcRat, RAT_MOVE_NONE);
            }

            // Reset the rat and start it moving again
            m_rat.Reset(ratX, ratY, RAT_MOVEMENT_INTERVAL_MAX);
            m_rat.SetState(m_defaultRatState);

            // Is the game in hard mode?
            if (m_gameMode == HARD_MODE)
            {
                // Speed-up the game
                for (int i = 0; i < 3; i++)
                {
                    m_snake.Speedup();
                    m_rat.Speedup();
                }
            }

            // Speed-up the game depending on the world level
            for (int i = 0; i < m_currentWorld; i++)
            {
                m_snake.Speedup();
                m_rat.Speedup();
            }

            // Clear the keyboard buffer
            m_gsKeyboard.ClearBuffer();

            // Clear the controller buffer.
            m_gsController.ClearBuffer();
        }

        // One action less to be taken.
        m_fInterval -= 1.0f;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Method Rendering //////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Clear the screen to the specified color.
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reset display.
    glLoadIdentity();

    // Setup the display for rendering in 2D.
    m_gsDisplay.BeginRender2D(this->GetWindow());

    // Render the background image.
    // this->RenderBackground();

    // Render the score area
    this->RenderScore();

    // Render the level
    m_level.Render();

    // Render the snake
    m_snake.Render();

    // Render the rat
    m_rat.Render();

    // Render the frame rate.
    this->RenderFrameRate(0.25f);

    // Reset display after rendering in 2D.
    m_gsDisplay.EndRender2D();

    // Swap buffers to display results.
    SwapBuffers(this->GetDevice());

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Method Cleanup ////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Are we about to leave the game?
    if (m_nGameProgress != PLAY_GAME)
    {
        // Remember where we've come from.
        m_nPrevProgress = PLAY_GAME;
    }

    return TRUE;
}

// *********************************************************************************************

//==============================================================================================
// GS_Snake::PlayUpdate():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull and FALSE if not.
//==============================================================================================

BOOL GS_Snake::PlayUpdate()
{
    // Has the display not been initialized?
    if (!this->m_gsDisplay.IsReady())
    {
        return FALSE;
    }

    // Has the game been paused?
    if (this->IsPaused())
    {
        // Remember where we've come from.
        m_nPrevProgress = m_nGameProgress;
        // Progress to pause section.
        m_nGameProgress = PLAY_PAUSE;
        return TRUE;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Method Initialization /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    static float fAlpha = 1.0f;
    static int nCounter = 3;

    // Has the method not been initialized?
    if (m_bIsInitialized == FALSE)
    {
        // Reset variables
        fAlpha = 1.0f;
        nCounter = 3;

        // Initialization completed.
        m_bIsInitialized = TRUE;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Method Logic //////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Are images not entirely transparent?
    if (fAlpha > 0.0f)
    {
        // Increase the transparency.
        fAlpha -= this->GetActionInterval(60 * 0.020f);
    }
    else
    {
        // Decrease the counter
        nCounter--;

        // Has the counter reached zero?
        if (nCounter <= 0)
        {
            // Progress to the next section
            m_nGameProgress = PLAY_GAME;
        }

        // Reset the alpha value
        fAlpha = 1.0f;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Method Rendering //////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Clear the screen to the specified color.
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reset display.
    glLoadIdentity();

    // Setup the display for rendering in 2D.
    m_gsDisplay.BeginRender2D(this->GetWindow());

    // Render the background image.
    // this->RenderBackground();

    // Render the score area
    this->RenderScore(0.25f);

    // Render the level
    m_level.Render(0.25f);

    // Render the snake
    m_snake.Render(0.25f);

    // Render the rat
    m_rat.Render(0.25f);

    // Render the frame rate.
    this->RenderFrameRate(0.25f);

    // Display the level
    m_gsFont.SetText("GET READY!");
    m_gsFont.SetModulateColor(1.0f, 0.25f, 0.25f, 1.0f);
    m_gsFont.SetScaleXY(2.0f, 2.0f);
    m_gsFont.SetDestX((INTERNAL_RES_X / 2) - (m_gsFont.GetTextWidth() / 2));
    m_gsFont.SetDestY((((INTERNAL_RES_Y / 2) - m_gsFont.GetTextHeight())) + m_gsFont.GetTextHeight());
    m_gsFont.Render();

    // Display the counter
    if (nCounter > 0)
    {
        m_gsFont.SetText("%d", nCounter);
        m_gsFont.SetModulateColor(1.0f, 1.0f, 1.0f, fAlpha);
        m_gsFont.SetScaleXY(2.0f, 2.0f);
        m_gsFont.SetDestX((INTERNAL_RES_X / 2) - (m_gsFont.GetTextWidth() / 2));
        m_gsFont.SetDestY((((INTERNAL_RES_Y / 2) - m_gsFont.GetTextHeight())) - m_gsFont.GetTextHeight());
        m_gsFont.Render();
    }

    // Reset display after rendering in 2D.
    m_gsDisplay.EndRender2D();

    // Swap buffers to display results.
    SwapBuffers(this->GetDevice());

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Method Cleanup ////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Are we leaving this section?
    if (m_nGameProgress != PLAY_UPDATE)
    {
        // Remember where we've come from.
        m_nPrevProgress = PLAY_UPDATE;
        // Reset method variables.
        m_bIsInitialized = FALSE;
    }

    return TRUE;
}

// *********************************************************************************************

//==============================================================================================
// GS_Snake::PlayPause():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull and FALSE if not.
//==============================================================================================

BOOL GS_Snake::PlayPause()
{

    // Has the display not been initialized?
    if (!this->m_gsDisplay.IsReady())
    {
        return FALSE;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Method Initialization /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Has the method not been initialized?
    if (m_bIsInitialized == FALSE)
    {
        // Play the appropriate sound effect.
        m_gsSound.PlaySample(SAMPLE_OPTION);
        // Initialization completed.
        m_bIsInitialized = TRUE;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Method Logic //////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Is the game no longer paused?
    if (!this->IsPaused())
    {
        // Return to the section we came from.
        m_nGameProgress = m_nPrevProgress;
        // Remember where we've come from.
        m_nPrevProgress = PLAY_PAUSE;
        // Play the appropriate sound effect.
        m_gsSound.PlaySample(SAMPLE_SELECT);
        // Reset method variables.
        m_bIsInitialized = FALSE;
        return TRUE;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Method Rendering //////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Clear the screen to the specified color.
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reset display.
    glLoadIdentity();

    // Setup the display for rendering in 2D.
    m_gsDisplay.BeginRender2D(this->GetWindow());

    // Render the background image.
    // this->RenderBackground(0.5f);

    // Render the score area
    this->RenderScore(0.25f);

    // Render the level
    m_level.Render(0.25f);

    // Render the snake
    m_snake.Render(0.25f);

    // Render the rat
    m_rat.Render(0.25f);

    // Render the tile images.
    // m_gsTileSprite.SetModulateColor(1.0f, 1.0f, 1.0f, 0.5f);
    // m_gsTileSprite.RenderTiles(m_rcScreen);
    // m_gsTileSprite.SetModulateColor(1.0f, 1.0f, 1.0f, 1.0f);

    // Render the frame rate.
    this->RenderFrameRate(0.25f);

    // Display a message centered in the screen.
    m_gsFont.SetText("GAME PAUSED");
    m_gsFont.SetDestX((INTERNAL_RES_X - m_gsFont.GetTextWidth()) / 2);
    m_gsFont.SetDestY(((INTERNAL_RES_Y - m_gsFont.GetTextHeight()) / 2) +
                      m_gsFont.GetTextHeight());
    m_gsFont.Render();

    m_gsFont.SetText("(PRESS P)");
    m_gsFont.SetDestX((INTERNAL_RES_X - m_gsFont.GetTextWidth()) / 2);
    m_gsFont.SetDestY(((INTERNAL_RES_Y - m_gsFont.GetTextHeight()) / 2) -
                      m_gsFont.GetTextHeight());
    m_gsFont.Render();

    // Reset display after rendering in 2D.
    m_gsDisplay.EndRender2D();

    // Swap buffers to display results.
    SwapBuffers(this->GetDevice());

    return TRUE;
}

// *********************************************************************************************

//==============================================================================================
// GS_Snake::PlayExit():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull and FALSE if not.
//==============================================================================================

BOOL GS_Snake::PlayExit()
{
    int KeyList[6] = {GSK_ENTER, GSK_UP, GSK_DOWN, GSK_Y, GSK_N, GSK_ESCAPE};
    int ButtonList[4] = {GSC_BUTTON_A, GSC_BUTTON_DPAD_UP, GSC_BUTTON_DPAD_DOWN, GSC_BUTTON_B};

    // Has the display not been initialized?
    if (!this->m_gsDisplay.IsReady())
    {
        return FALSE;
    }

    // Has the game been paused?
    if (this->IsPaused())
    {
        // Unpause the game.
        this->Pause(FALSE);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Method Initialization /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Has the method not been initialized?
    if (m_bIsInitialized == FALSE)
    {
        // Initialize method variables.
        m_bWasKeyReleased = FALSE;
        // Clear keyboard buffer.
        m_gsKeyboard.ClearBuffer();
        // Clear controller buffer.
        m_gsController.ClearBuffer();
        // Setup variables for mouse input.
        m_nOldMouseX = m_gsMouse.GetX();
        m_nOldMouseY = m_gsMouse.GetY();
        m_bWasMouseReleased = FALSE;
        // Play the appropriate sound effect.
        m_gsSound.PlaySample(SAMPLE_OPTION);
        // Clear all current menu items.
        m_gsMenu.ClearOptions();
        // Set menu title.
        m_gsMenu.SetTitle("  END GAME?  ");
        // Highlight the first option.
        m_gsMenu.SetHighlight(0);
        // Initialization completed.
        m_bIsInitialized = TRUE;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Set Menu Options /////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Clear all current menu items.
    m_gsMenu.ClearOptions();

    // Add menu options depending on settings.
    m_gsMenu.AddOption("     YES     ");
    m_gsMenu.AddOption("     NO      ");

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Get Keyboard / Controller Input //////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Reset the selected option.
    m_nOptionSelected = -1;

    // Were all the keys in the key list released?
    if (TRUE == m_gsKeyboard.AreKeysUp(6, KeyList) &&
        TRUE == m_gsController.AreButtonsUp(4, ButtonList))
    {
        m_bWasKeyReleased = TRUE;
    }

    // Check to see wether a key was pressed.
    int nKey = m_gsKeyboard.GetKeyPressed();

    // Check to see wether a button was pressed.
    int nButton = m_gsController.GetBufferedButton();

    if (nButton != -1)
    {
        nKey = nButton;
    }

    // Act depending on key pressed.
    switch (nKey)
    {
    // Was the up key pressed?
    case GSK_UP:
    case GSC_BUTTON_DPAD_UP:
        if (m_bWasKeyReleased)
        {
            // Highlight previous option.
            m_gsMenu.HighlightPrev();
            m_gsSound.PlaySample(SAMPLE_OPTION);
            m_bWasKeyReleased = FALSE;
        }
        break;
    // Was the down key pressed?
    case GSK_DOWN:
    case GSC_BUTTON_DPAD_DOWN:
        if (m_bWasKeyReleased)
        {
            // Highlight next option.
            m_gsMenu.HighlightNext();
            m_gsSound.PlaySample(SAMPLE_OPTION);
            m_bWasKeyReleased = FALSE;
        }
        break;
    // Was the Y key pressed?
    case GSK_Y:
        if (m_bWasKeyReleased)
        {
            // Highlight yes option.
            m_gsMenu.SetHighlight(0);
            m_gsSound.PlaySample(SAMPLE_OPTION);
            m_bWasKeyReleased = FALSE;
        }
        break;
    // Was the N key pressed?
    case GSK_N:
        if (m_bWasKeyReleased)
        {
            // Highlight no option.
            m_gsMenu.SetHighlight(1);
            m_gsSound.PlaySample(SAMPLE_OPTION);
            m_bWasKeyReleased = FALSE;
        }
        break;
    // Was the enter key pressed?
    case GSK_ENTER:
    case GSC_BUTTON_A:
        if (m_bWasKeyReleased)
        {
            // Remeber the option selected.
            m_nOptionSelected = m_gsMenu.GetHighlight();
            m_bWasKeyReleased = FALSE;
        }
        break;
    // Was the escape key pressed?
    case GSK_ESCAPE:
    case GSC_BUTTON_B:
        if (m_bWasKeyReleased)
        {
            // The no option was selected.
            m_nOptionSelected = 1;
            m_bWasKeyReleased = FALSE;
        }
        break;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Get Mouse Input //////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    RECT rcOption;

    // Has the mouse cursor moved from its previous position?
    if ((m_nOldMouseX != m_gsMouse.GetX()) || (m_nOldMouseY != m_gsMouse.GetY()))
    {
        // Check each option to see if the mouse coordinates are within the option area.
        for (int nLoop = 0; nLoop < m_gsMenu.GetNumOptions(); nLoop++)
        {
            // Determine the screen coordinates of the next option.
            m_gsMenu.GetOptionRect(nLoop, &rcOption);
            // Are the mouse coordinates within the option area?
            if (m_gsCollide.IsCoordInRect(m_gsMouse.GetX(), m_gsMouse.GetY(), rcOption))
            {
                // Is the option not highlighted already?
                if (nLoop != m_gsMenu.GetHighlight())
                {
                    // Highlight the selected option.
                    m_gsMenu.SetHighlight(nLoop);
                    // Play the appropriate sound effect.
                    m_gsSound.PlaySample(SAMPLE_OPTION);
                }
            }
        }
        // Assign new mouse coordinates.
        m_nOldMouseX = m_gsMouse.GetX();
        m_nOldMouseY = m_gsMouse.GetY();
    }

    // Is the left mouse button not pressed?
    if (!m_gsMouse.IsLeftPressed())
    {
        m_bWasMouseReleased = TRUE;
    }

    // Was the left mouse button pressed?
    if ((m_gsMouse.IsLeftPressed()) && (m_bWasMouseReleased))
    {
        // Determine the screen coordinates of the highlighted option.
        m_gsMenu.GetHighlightRect(&rcOption);
        // Check if mouse coordinates are within the area of the highligted option.
        if (m_gsCollide.IsCoordInRect(m_gsMouse.GetX(), m_gsMouse.GetY(), rcOption))
        {
            // Remeber which option is highlighted.
            m_nOptionSelected = m_gsMenu.GetHighlight();
            m_bWasMouseReleased = FALSE;
            // Play the appropriate sound effect.
            m_gsSound.PlaySample(SAMPLE_SELECT);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Process Menu Commands ////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Act depending on option selected.
    switch (m_nOptionSelected)
    {
    // Determine wether to increase levels or not.
    case 0:
        // Stop playing the game.
        m_nGameProgress = PLAY_OUTRO;
        // Play appropriate sound.
        m_gsSound.PlaySample(SAMPLE_SELECT);
        break;
    // Determine wether to preview drops or not.
    case 1:
        // Continue playing the game.
        m_nGameProgress = PLAY_GAME;
        // Play appropriate sound.
        m_gsSound.PlaySample(SAMPLE_SELECT);
        break;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Method Rendering //////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Clear the screen to the specified color.
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reset display.
    glLoadIdentity();

    // Setup the display for rendering in 2D.
    m_gsDisplay.BeginRender2D(this->GetWindow());

    // Render the background image.
    // this->RenderBackground(0.5f);

    // Render the tile images.
    // m_gsTileSprite.SetModulateColor(1.0f, 1.0f, 1.0f, 0.5f);
    // m_gsTileSprite.RenderTiles(m_rcScreen);
    // m_gsTileSprite.SetModulateColor(1.0f, 1.0f, 1.0f, 1.0f);

    // Render the level
    m_level.Render(0.25f);

    // Render the snake
    m_snake.Render(0.25f);

    // Render the rat
    m_rat.Render(0.25f);

    // Render the frame rate.
    this->RenderFrameRate(0.25f);

    // Center the menu horizontally and vertically on the screen.
    m_gsMenu.SetDestX((INTERNAL_RES_X - m_gsMenu.GetWidth()) / 2);
    m_gsMenu.SetDestY((INTERNAL_RES_Y - m_gsMenu.GetHeight()) / 2);

    // Render the menu to the back surface using the specified colors.
    m_gsMenu.Render(m_gsMenuTextColor, m_gsMenuHighColor);

    // Render the mouse cursor.
    this->RenderCursor();

    // Reset display after rendering in 2D.
    m_gsDisplay.EndRender2D();

    // Swap buffers to display results.
    SwapBuffers(this->GetDevice());

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Method Cleanup ////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Are we leaving this section?
    if (m_nGameProgress != PLAY_EXIT)
    {
        // Remember where we've come from.
        m_nPrevProgress = PLAY_EXIT;
        // Reset method variables.
        m_bIsInitialized = FALSE;
    }

    return TRUE;
}

// *********************************************************************************************

//==============================================================================================
// GS_Snake::PlayOutro():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull and FALSE if not.
//==============================================================================================

BOOL GS_Snake::PlayOutro()
{

    // Has the display not been initialized?
    if (!this->m_gsDisplay.IsReady())
    {
        return FALSE;
    }

    // Has the game been paused?
    if (this->IsPaused())
    {
        // Unpause the game.
        this->Pause(FALSE);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Method Initialization /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Has the method not been initialized?
    if (m_bIsInitialized == FALSE)
    {
        // Initialize method variables.
        m_fInterval = 0.0f;
        m_nCounter = 0;
        m_fAlpha = 1.0f;
        // Initialization completed.
        m_bIsInitialized = TRUE;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Method Logic //////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Determine the interval required to perform an action 60 times every second at the
    // current frame rate, and add it to the previous intervals.
    m_fInterval += this->GetActionInterval(60);

    // Have all the intervals added up enough to perform an action?
    while (m_fInterval >= 1.0f)
    {
        // Increase counter.
        m_nCounter++;
        // Act depending on how many counts have elapsed.
        if (m_nCounter < 125)
        {
            // Are images not entirely transparent?
            if (m_fAlpha > 0.0f)
            {
                // Increase the transparency.
                m_fAlpha -= 0.01f;
                // Is the game music playing?
                if (m_gsSound.IsStreamPlaying())
                {
                    // Fade the game music volume.
                    m_gsSound.SetStreamVolume(m_gsSettings.nMusicVolume * m_fAlpha);
                }
            }
            else
            {
                // Images are transparent.
                m_fAlpha = 0.0f;
            }
        }
        else
        {
            // Is the score good enough to be a hiscore?
            if (m_lScore > m_gsHiscores[MAX_SCORES - 1].lScore)
            {
                // Add the score to the hiscore table.
                m_nNextProgress = SCORES_ADD;
                m_nGameProgress = SCORES_INTRO;
            }
            else
            {
                // Go to the view hiscore section.
                m_nNextProgress = SCORES_VIEW;
                m_nGameProgress = SCORES_INTRO;
            }
        }
        // One action less to be taken.
        m_fInterval -= 1.0f;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Method Rendering //////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Clear the screen to the specified color.
    glClearColor(1.0f - m_fAlpha, 1.0f - m_fAlpha, 1.0f - m_fAlpha, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reset display.
    glLoadIdentity();

    // Setup the display for rendering in 2D.
    m_gsDisplay.BeginRender2D(this->GetWindow());

    // Render the background image.
    // this->RenderBackground(m_fAlpha);

    // Render the tile images.
    // m_gsTileSprite.SetModulateColor(1.0f, 1.0f, 1.0f, m_fAlpha);
    // m_gsTileSprite.RenderTiles(m_rcScreen);

    // Render the level
    m_level.Render(m_fAlpha);

    // Render the snake
    m_snake.Render(m_fAlpha);

    // Render the rat
    m_rat.Render(m_fAlpha);

    // Render the frame rate.
    this->RenderFrameRate(m_fAlpha);

    // Display a message centered in the screen.
    m_gsFont.SetText("GAME OVER");
    m_gsFont.SetDestX((INTERNAL_RES_X - m_gsFont.GetTextWidth()) / 2);
    m_gsFont.SetDestY((INTERNAL_RES_Y - m_gsFont.GetTextHeight()) / 2);
    m_gsFont.SetModulateColor(-1.0f, -1.0f, -1.0f, 1.0f - m_fAlpha);
    m_gsFont.Render();

    // Reset display after rendering in 2D.
    m_gsDisplay.EndRender2D();

    // Swap buffers to display results.
    SwapBuffers(this->GetDevice());

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Method Cleanup ////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Are we leaving this section?
    if (m_nGameProgress != PLAY_OUTRO)
    {
        // Remember where we've come from.
        m_nPrevProgress = PLAY_OUTRO;
        // Reset all variables used in the game.
        m_gsBackSprite.SetScaleXY(1.0f, 1.0f);
        m_gsBackSprite.SetRotateX(0.0f);
        m_gsBackSprite.SetRotateY(0.0f);
        m_gsBackSprite.SetRotateZ(0.0f);
        // m_gsTileSprite.SetScaleX(1.0f);
        // m_gsTileSprite.SetScaleY(1.0f);
        // Set the sound effects volume to game settings volume.
        m_gsSound.SetSampleMaster(m_gsSettings.nEffectsVolume);
        // Set the music volume to the game settings volume.
        m_gsSound.SetStreamVolume(m_gsSettings.nMusicVolume);
        // Is the game music playing?
        if (m_gsSound.IsStreamPlaying())
        {
            // Restore the game music volume.
            m_gsSound.SetStreamVolume(m_gsSettings.nMusicVolume);
            // Stop game music from playing.
            m_gsSound.StopStream();
        }
        // Reset method variables.
        m_bIsInitialized = FALSE;
    }

    return TRUE;
}

// *********************************************************************************************

// *********************************************************************************************
// *** Hiscore Methods *************************************************************************
// *********************************************************************************************

//==============================================================================================
// GS_Snake::ScoresIntro():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull and FALSE if not.
//==============================================================================================

BOOL GS_Snake::ScoresIntro()
{

    // Has the display not been initialized?
    if (!this->m_gsDisplay.IsReady())
    {
        return FALSE;
    }

    // Has the game been paused?
    if (this->IsPaused())
    {
        // Unpause the game.
        this->Pause(FALSE);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Method Initialization /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Has the method not been initialized?
    if (m_bIsInitialized == FALSE)
    {
        // Are we not going to the add hiscore section?
        if (m_nNextProgress != SCORES_ADD)
        {
            // Go to view hiscores section by default.
            m_nNextProgress = SCORES_VIEW;
        }
        // Initialization completed.
        m_bIsInitialized = TRUE;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Method Logic //////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Do we want to view hiscores?
    if (m_nNextProgress == SCORES_VIEW)
    {
        // ...
    }
    // Do we want to add hiscores?
    else if (m_nNextProgress == SCORES_ADD)
    {
        // ...
    }

    // ...

    // Progress to the next section.
    m_nGameProgress = m_nNextProgress;

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Method Rendering //////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Clear the screen to the specified color.
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reset display.
    glLoadIdentity();

    // Setup the display for rendering in 2D.
    m_gsDisplay.BeginRender2D(this->GetWindow());

    // ...

    // Reset display after rendering in 2D.
    m_gsDisplay.EndRender2D();

    // Swap buffers to display results.
    SwapBuffers(this->GetDevice());

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Method Cleanup ////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Are we leaving this section?
    if (m_nGameProgress != SCORES_INTRO)
    {
        // Remember where we've come from.
        m_nPrevProgress = SCORES_INTRO;
        // Reset method variables.
        m_bIsInitialized = FALSE;
    }

    return TRUE;
}

// *********************************************************************************************

//==============================================================================================
// GS_Snake::ScoresView():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull and FALSE if not.
//==============================================================================================

BOOL GS_Snake::ScoresView()
{

    // Has the display not been initialized?
    if (!this->m_gsDisplay.IsReady())
    {
        return FALSE;
    }

    // Has the game been paused?
    if (this->IsPaused())
    {
        // Unpause the game.
        this->Pause(FALSE);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Method Initialization /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    char szTempString[128] = {0};
    char szPaddedLevel[8] = {0};
    char szPaddedMode[8] = {0};

    // Has the method not been initialized?
    if (m_bIsInitialized == FALSE)
    {
        // Initialize variables.
        m_fInterval = 0.0f;
        // Clear the keyboard buffer.
        m_gsKeyboard.ClearBuffer();
        // Clear the controller buffer.
        m_gsController.ClearBuffer();
        // Where we want to go to next.
        m_nNextProgress = SCORES_OUTRO;
        // Initialization completed.
        m_bIsInitialized = TRUE;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Method Logic //////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Determine the interval required to perform an action 60 times every second at the
    // current frame rate, and add it to the previous intervals.
    m_fInterval += this->GetActionInterval(60);

    // Have counter reached specified count?
    if (m_fInterval >= 900.0f)
    {
        // Progress to the next section.
        m_nGameProgress = m_nNextProgress;
        // Reset time counter.
        m_fInterval = 0.0f;
    }

    // Exit method if user presses any key.
    if (m_gsKeyboard.GetBufferedKey() > 0)
    {
        // Progress to the next section.
        m_nGameProgress = m_nNextProgress;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Method Rendering //////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Clear the screen to the specified color.
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reset display.
    glLoadIdentity();

    // Setup the display for rendering in 2D.
    m_gsDisplay.BeginRender2D(this->GetWindow());

    // Render the background image.
    this->RenderBackground(0.5f);

    // Setup and display the hiscores heading.
    sprintf(szTempString, " TOP %02d SCORES ", MAX_SCORES);
    m_gsFont.SetScaleXY(1.5f, 1.5f);
    m_gsFont.SetText(szTempString);
    m_gsFont.SetDestX((INTERNAL_RES_X - m_gsFont.GetTextWidth()) / 2);
    m_gsFont.SetDestY(((INTERNAL_RES_Y - (m_gsFont.GetTextHeight() * MAX_SCORES)) / 2) + ((MAX_SCORES + 1) * m_gsFont.GetTextHeight()));
    m_gsFont.Render();

    // ender the headings
    sprintf(szTempString, "#  NAME       MODE    %7s %6s", "LEVEL", "SCORE");
    m_gsFont.SetScaleXY(1.0f, 1.0f);
    m_gsFont.SetText(szTempString);
    m_gsFont.SetDestX((INTERNAL_RES_X - m_gsFont.GetTextWidth()) / 2);
    m_gsFont.SetDestY(((INTERNAL_RES_Y - (m_gsFont.GetTextHeight() * MAX_SCORES)) / 2) + ((MAX_SCORES)*m_gsFont.GetTextHeight()));
    // Render the text.
    m_gsFont.Render();

    // Reset modulate color.
    m_gsFont.SetModulateColor(1.0f, 1.0f, 1.0f, 1.0f);
    // For every score in the hiscores array.
    for (int nLoop = 0; nLoop < MAX_SCORES; nLoop++)
    {
        // Set color to highlight the first score
        if (nLoop == m_nScoreIndex)
        {
            m_gsFont.SetModulateColor(0.0f, 1.0f, 1.0f, 1.0f);
        }

        // Pad the level string
        lstrcpy(szPaddedLevel, m_gsHiscores[nLoop].szLevel);

        // Pad the mode string
        lstrcpy(szPaddedMode, m_gsHiscores[nLoop].szMode);
        if (lstrlen(szPaddedMode) < 7)
        {
            for (int i = (lstrlen(szPaddedMode)); i < sizeof(szPaddedMode) - 1; i++)
            {
                szPaddedMode[i] = ' ';
            }
            szPaddedMode[sizeof(szPaddedLevel) - 1] = '\0';
        }

        // Setup a temporary string with all the relevant hiscore data and assign it.
        sprintf(szTempString, " %02d %s %s %7s %06ld ", nLoop + 1, m_gsHiscores[nLoop].szName, szPaddedMode, szPaddedLevel, m_gsHiscores[nLoop].lScore);
        m_gsFont.SetText(szTempString);
        // Set the position of the next hiscore.
        m_gsFont.SetDestX((INTERNAL_RES_X - m_gsFont.GetTextWidth()) / 2);
        m_gsFont.SetDestY(((INTERNAL_RES_Y - (m_gsFont.GetTextHeight() * MAX_SCORES)) / 2) + ((MAX_SCORES - nLoop - 2) * m_gsFont.GetTextHeight()));
        // Render the text.
        m_gsFont.Render();
        // Reset modulate color.
        m_gsFont.SetModulateColor(1.0f, 1.0f, 1.0f, 1.0f);
    }

    // Display a message centered at the bottom of the screen.
    m_gsFont.SetText("PRESS ANY KEY");
    m_gsFont.SetDestX((INTERNAL_RES_X - m_gsFont.GetTextWidth()) / 2);
    m_gsFont.SetDestY(m_gsFont.GetTextHeight() * 2);
    m_gsFont.Render();

    // Reset display after rendering in 2D.
    m_gsDisplay.EndRender2D();

    // Swap buffers to display results.
    SwapBuffers(this->GetDevice());

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Method Cleanup ////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Are we leaving this section?
    if (m_nGameProgress != SCORES_VIEW)
    {
        // Remember where we've come from.
        m_nPrevProgress = SCORES_VIEW;
        // Reset method variables.
        m_nScoreIndex = 0;
        m_bIsInitialized = FALSE;
    }

    return TRUE;
}

// *********************************************************************************************

//==============================================================================================
// GS_Snake::ScoresAdd():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull and FALSE if not.
//==============================================================================================

BOOL GS_Snake::ScoresAdd()
{

    // Has the display not been initialized?
    if (!this->m_gsDisplay.IsReady())
    {
        return FALSE;
    }

    // Has the game been paused?
    if (this->IsPaused())
    {
        // Unpause the game.
        this->Pause(FALSE);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Method Initialization /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    static int nCharCount = 0;

    static float fInputRepeatFraction = 0.0f;

    static char szTempString[11] = {0};

    // Has the method not been initialized?
    if (m_bIsInitialized == FALSE)
    {
        // Initialize variables.
        m_nScoreIndex = 0;
        nCharCount = 0;
        fInputRepeatFraction = 0.0f;
        // Loop through all the hiscores starting from the lowest.
        for (int nLoop = MAX_SCORES - 1; nLoop >= 0; nLoop--)
        {
            // Is the current score is greater than the hiscore?
            if (m_lScore > m_gsHiscores[nLoop].lScore)
            {
                // Save place where score should be inserted.
                m_nScoreIndex = nLoop;
            }
        }
        // Setup a temporary string for the player name.
        lstrcpy(szTempString, "..........");
        // Clear the keyboard buffer.
        m_gsKeyboard.ClearBuffer();
        // Clear the controller buffer.
        m_gsController.ClearBuffer();
        // Where we want to go to next.
        m_nNextProgress = SCORES_OUTRO;
        // Initialization completed.
        m_bIsInitialized = TRUE;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Get Keyboard Input ///////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Check to see wether a key was pressed.
    int nKey = m_gsKeyboard.GetBufferedKey();

    // Implement input delay to allow for repeated inputs.
    fInputRepeatFraction += this->GetActionInterval(60);

    // Has enough time passed to allow for repeated input?
    if (fInputRepeatFraction >= 5.0f && nKey <= 0)
    {
        // Check if keys are held down.
        int nKeyPressed = m_gsKeyboard.GetKeyPressed();

        // Was the up or down key held down?
        if (nKeyPressed == GSK_UP ||
            nKeyPressed == GSK_DOWN)
        {
            nKey = nKeyPressed;
        }

        // Check if buttons are held down.
        int nPressedButton = m_gsController.GetButtonPressed();

        // Was a D-Pad button held down?
        if (nPressedButton == GSC_BUTTON_DPAD_UP ||
            nPressedButton == GSC_BUTTON_DPAD_DOWN)
        {
            nKey = nPressedButton;
        }

        // Reset input delay fraction.
        fInputRepeatFraction = 0.0f;
    }

    // Act depending on key pressed.
    switch (nKey)
    {
    // Was the up key pressed?
    case GSK_UP:
    case GSC_BUTTON_DPAD_UP:
        // Go to the next character.
        szTempString[nCharCount]++;
        // Allow only certain characters.
        if (szTempString[nCharCount] == GSK_DELETE + 1)
        {
            szTempString[nCharCount] = GSK_0;
        }
        else if (szTempString[nCharCount] == GSK_9 + 1)
        {
            szTempString[nCharCount] = GSK_A;
        }
        else if (szTempString[nCharCount] == GSK_Z + 1)
        {
            szTempString[nCharCount] = GSK_DELETE;
        }
        // Play the appropriate sound effect.
        m_gsSound.PlaySample(SAMPLE_OPTION);
        // Reset input delay fraction.
        fInputRepeatFraction = 0.0f;
        break;
    // Was the up key pressed?
    case GSK_DOWN:
    case GSC_BUTTON_DPAD_DOWN:
        // Go to the previous character.
        szTempString[nCharCount]--;
        // Allow only certain characters.
        if (szTempString[nCharCount] == GSK_DELETE - 1)
        {
            szTempString[nCharCount] = GSK_Z;
        }
        else if (szTempString[nCharCount] == GSK_A - 1)
        {
            szTempString[nCharCount] = GSK_9;
        }
        else if (szTempString[nCharCount] == GSK_0 - 1)
        {
            szTempString[nCharCount] = GSK_DELETE;
        }
        // Play the appropriate sound effect.
        m_gsSound.PlaySample(SAMPLE_OPTION);
        // Reset input delay fraction.
        fInputRepeatFraction = 0.0f;
        break;
    // Was the left key pressed?
    case GSK_LEFT:
    case GSC_BUTTON_DPAD_LEFT:
        // Move one character left.
        nCharCount--;
        // Wrap around edges.
        if (nCharCount < 0)
        {
            nCharCount = 9;
        }
        // Play the appropriate sound effect.
        m_gsSound.PlaySample(SAMPLE_OPTION);
        break;
    // Was the right key pressed?
    case GSK_RIGHT:
    case GSC_BUTTON_DPAD_RIGHT:
        // Move one character right.
        nCharCount++;
        // Wrap around edges.
        if (nCharCount > 9)
        {
            nCharCount = 0;
        }
        // Play the appropriate sound effect.
        m_gsSound.PlaySample(SAMPLE_OPTION);
        break;
    // Was the backspace key pressed?
    case GSK_BACK:
        // Are we not at the first letter?
        if (nCharCount > 0)
        {
            // Shift all letters one back starting at active letter.
            for (int nLoop = nCharCount - 1; nLoop < 9; nLoop++)
            {
                szTempString[nLoop] = szTempString[nLoop + 1];
            }
            // Delete the last letter.
            szTempString[9] = GSK_DELETE;
            // Move one character left.
            nCharCount--;
            // Play the appropriate sound effect.
            m_gsSound.PlaySample(SAMPLE_OPTION);
        }
        break;
    // Was the delete key pressed?
    case GSK_DELETE:
        // Shift all letters one back starting at active letter.
        for (int nLoop = nCharCount; nLoop < 9; nLoop++)
        {
            szTempString[nLoop] = szTempString[nLoop + 1];
        }
        // Delete the last letter.
        szTempString[9] = GSK_DELETE;
        // Play the appropriate sound effect.
        m_gsSound.PlaySample(SAMPLE_OPTION);
        break;
    // Was the enter key pressed?
    case GSK_ENTER:
    case GSC_BUTTON_A:
        // Loop through all the scores lower than the current score.
        for (int nLoop = MAX_SCORES - 1; nLoop > m_nScoreIndex; nLoop--)
        {
            // Shift scores to make place for the new score.
            m_gsHiscores[nLoop] = m_gsHiscores[nLoop - 1];
        }
        // Save the player name and statistics.
        if (strcmp(szTempString, "..........") == 0)
            lstrcpy(szTempString, "PLAYER.1..");
        lstrcpy(m_gsHiscores[m_nScoreIndex].szName, szTempString);
        lstrcpy(m_gsHiscores[m_nScoreIndex].szMode, "       ");
        lstrcpy(m_gsHiscores[m_nScoreIndex].szMode, (m_gameMode == EASY_MODE ? "CLASSIC" : "REMIX"));
        lstrcpy(m_gsHiscores[m_nScoreIndex].szLevel, "       ");
        sprintf(m_gsHiscores[m_nScoreIndex].szLevel, "%d-%d", m_currentWorld, m_currentLevel);
        m_gsHiscores[m_nScoreIndex].lScore = m_lScore;
        // Play the appropriate sound effect.
        m_gsSound.PlaySample(SAMPLE_SELECT);
        // Progress to the next section.
        m_nGameProgress = m_nNextProgress;
        break;
    // Was the escape key pressed?
    case GSK_ESCAPE:
    case GSC_BUTTON_B:
        // Play the appropriate sound effect.
        m_gsSound.PlaySample(SAMPLE_SELECT);
        // Progress to the next section.
        m_nGameProgress = m_nNextProgress;
        m_nScoreIndex = 0;
        break;
    default:
        // Was the key pressed a recognized key (0-9, A-Z, ' ' and '.')?
        if (((nKey >= GSK_A) && (nKey <= GSK_Z)) || ((nKey >= GSK_0) && (nKey <= GSK_9)))
        {
            // Assign key and move one letter right.
            szTempString[nCharCount] = (char)nKey;
            nCharCount++;
            // Play the appropriate sound effect.
            m_gsSound.PlaySample(SAMPLE_OPTION);
        }
        else if ((nKey == GSK_GREATER) || (nKey == GSK_SPACE) || (nKey == GSK_DECIMAL))
        {
            // Assign key and move one letter right.
            szTempString[nCharCount] = GSK_DELETE;
            nCharCount++;
            // Play the appropriate sound effect.
            m_gsSound.PlaySample(SAMPLE_OPTION);
        }
        // Have we reached the end of the string?
        if (nCharCount > 9)
        {
            // Don't go beyond end of string.
            nCharCount = 9;
        }
        break;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Method Logic //////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // ...

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Method Rendering //////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Clear the screen to the specified color.
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reset display.
    glLoadIdentity();

    // Setup the display for rendering in 2D.
    m_gsDisplay.BeginRender2D(this->GetWindow());

    // Display a message centered on the screen.
    m_gsFont.SetText("ENTER YOUR NAME:");
    m_gsFont.SetDestX((INTERNAL_RES_X - m_gsFont.GetTextWidth()) / 2);
    m_gsFont.SetDestY(((INTERNAL_RES_Y - m_gsFont.GetTextHeight()) / 2) +
                      m_gsFont.GetTextHeight());
    m_gsFont.Render();

    // Display all the letters of the temporary string.
    for (int nLoop = 0; nLoop < 10; nLoop++)
    {
        // Set the screen coordinates for each letter.
        m_gsFont.SetDestX(((INTERNAL_RES_X - (m_gsFont.GetLetterWidth() * 10)) / 2) +
                          (nLoop * m_gsFont.GetLetterWidth()));
        m_gsFont.SetDestY(((INTERNAL_RES_Y - m_gsFont.GetTextHeight()) / 2) -
                          m_gsFont.GetTextHeight());
        // Are we at the currently active letter?
        if (nLoop == nCharCount)
        {
            // Set the modulate color for the active letter.
            m_gsFont.SetModulateColor(0.0f, 1.0f, 1.0f, 1.0f);
        }
        else
        {
            // Set the modulate color for all other letters.
            m_gsFont.SetModulateColor(1.0f, 1.0f, 1.0f, 1.0f);
        }
        // Render the selected letter.
        m_gsFont.RenderChar(szTempString[nLoop]);
    }

    // Reset the modulate color of the game font.
    m_gsFont.SetModulateColor(1.0f, 1.0f, 1.0f, 1.0f);

    // Reset display after rendering in 2D.
    m_gsDisplay.EndRender2D();

    // Swap buffers to display results.
    SwapBuffers(this->GetDevice());

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Method Cleanup ////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Are we leaving this section?
    if (m_nGameProgress != SCORES_ADD)
    {
        // Remember where we've come from.
        m_nPrevProgress = SCORES_ADD;
        // Save the new hiscores.
        this->SaveHiscores();
        // Reset method variables.
        m_bIsInitialized = FALSE;
    }

    return TRUE;
}

// *********************************************************************************************

//==============================================================================================
// GS_Snake::ScoresOutro():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull and FALSE if not.
//==============================================================================================

BOOL GS_Snake::ScoresOutro()
{

    // Has the display not been initialized?
    if (!this->m_gsDisplay.IsReady())
    {
        return FALSE;
    }

    // Has the game been paused?
    if (this->IsPaused())
    {
        // Unpause the game.
        this->Pause(FALSE);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Method Initialization /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Has the method not been initialized?
    if (m_bIsInitialized == FALSE)
    {
        // ...
        // Initialization completed.
        m_bIsInitialized = TRUE;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Method Logic //////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Did we view hiscores?
    if (m_nPrevProgress == SCORES_VIEW)
    {
        // Progress to the next section.
        m_nGameProgress = TITLE_INTRO;
    }
    // Did we add hiscores?
    else if (m_nPrevProgress == SCORES_ADD)
    {
        // Progress to the next section.
        m_nNextProgress = SCORES_VIEW;
        m_nGameProgress = SCORES_INTRO;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Method Rendering //////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Clear the screen to the specified color.
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reset display.
    glLoadIdentity();

    // Setup the display for rendering in 2D.
    m_gsDisplay.BeginRender2D(this->GetWindow());

    // ...

    // Reset display after rendering in 2D.
    m_gsDisplay.EndRender2D();

    // Swap buffers to display results.
    SwapBuffers(this->GetDevice());

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Do Method Cleanup ////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Are we leaving this section?
    if (m_nGameProgress != SCORES_OUTRO)
    {
        // Remember where we've come from.
        m_nPrevProgress = SCORES_OUTRO;
        // Reset method variables.
        m_bIsInitialized = FALSE;
    }

    return TRUE;
}

// *********************************************************************************************

// *********************************************************************************************
// *** Load & Save Methods *********************************************************************
// *********************************************************************************************

//==============================================================================================
// GS_Snake::LoadSettings():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successful, FALSE if failed.
//==============================================================================================

BOOL GS_Snake::LoadSettings()
{

    char szTempString[_MAX_PATH] = {0};
    GS_IniFile gsIniFile;

    // Determine the full pathname of the INI file.
    GetCurrentDirectory(_MAX_PATH, szTempString);
    lstrcat(szTempString, "\\");
    lstrcat(szTempString, SETTINGS_FILE);

    // Open the INI file.
    if (!gsIniFile.Open(szTempString))
    {
        // Exit function
        return FALSE;
    }

    // Read all the display settings.
    m_gsSettings.nDisplayWidth = gsIniFile.ReadInt("Display", "DisplayWidth", -1);
    m_gsSettings.nDisplayHeight = gsIniFile.ReadInt("Display", "DisplayHeight", -1);
    m_gsSettings.nColorDepth = gsIniFile.ReadInt("Display", "ColorDepth", -1);
    m_gsSettings.bWindowedMode = gsIniFile.ReadInt("Display", "WindowMode", -1);
    m_gsSettings.bEnableVSync = gsIniFile.ReadInt("Display", "EnableVSync", -1);
    m_gsSettings.bEnableAliasing = gsIniFile.ReadInt("Display", "EnableAliasing", -1);
    m_gsSettings.fFrameCap = gsIniFile.ReadInt("Display", "FrameCap", -1);

    // Did reading display settings fail?
    if (-1 == m_gsSettings.nDisplayWidth)
    {
        return FALSE;
    }
    else if (-1 == m_gsSettings.nDisplayHeight)
    {
        return FALSE;
    }
    else if (-1 == m_gsSettings.nColorDepth)
    {
        return FALSE;
    }
    else if (-1 == m_gsSettings.bWindowedMode)
    {
        return FALSE;
    }
    else if (-1 == m_gsSettings.bEnableVSync)
    {
        return FALSE;
    }
    else if (-1 == m_gsSettings.bEnableAliasing)
    {
        return FALSE;
    }
    else if (-1 == m_gsSettings.fFrameCap)
    {
        return FALSE;
    }

    // Read all the sound settings.
    m_gsSettings.nMusicVolume = gsIniFile.ReadInt("Sound", "MusicVolume", -1);
    m_gsSettings.nEffectsVolume = gsIniFile.ReadInt("Sound", "EffectsVolume", -1);

    // Did reading sound settings fail?
    if (-1 == m_gsSettings.nMusicVolume)
    {
        return FALSE;
    }
    else if (-1 == m_gsSettings.nEffectsVolume)
    {
        return FALSE;
    }

    // Close the INI file.
    gsIniFile.Close();

    return TRUE;
}

// *********************************************************************************************

//==============================================================================================
// GS_Snake::SaveSettings():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successful, FALSE if failed.
//==============================================================================================

BOOL GS_Snake::SaveSettings()
{

    char szTempString[_MAX_PATH] = {0};
    GS_IniFile gsIniFile;

    // Determine the full pathname of the INI file.
    GetCurrentDirectory(_MAX_PATH, szTempString);
    lstrcat(szTempString, "\\");
    lstrcat(szTempString, SETTINGS_FILE);

    // Open the INI file.
    if (!gsIniFile.Open(szTempString))
    {
        // Exit function
        return FALSE;
    }

    // Write display settings.
    gsIniFile.WriteInt("Display", "DisplayWidth", m_gsSettings.nDisplayWidth);
    gsIniFile.WriteInt("Display", "DisplayHeight", m_gsSettings.nDisplayHeight);
    gsIniFile.WriteInt("Display", "ColorDepth", m_gsSettings.nColorDepth);
    gsIniFile.WriteInt("Display", "WindowMode", m_gsSettings.bWindowedMode);
    gsIniFile.WriteInt("Display", "EnableVSync", m_gsSettings.bEnableVSync);
    gsIniFile.WriteInt("Display", "EnableAliasing", m_gsSettings.bEnableAliasing);
    gsIniFile.WriteInt("Display", "FrameCap", m_gsSettings.fFrameCap);

    // Write sound settings.
    gsIniFile.WriteInt("Sound", "MusicVolume", m_gsSettings.nMusicVolume);
    gsIniFile.WriteInt("Sound", "EffectsVolume", m_gsSettings.nEffectsVolume);

    // Close the INI file.
    gsIniFile.Close();

    return TRUE;
}

// *********************************************************************************************

//==============================================================================================
// GS_Snake::LoadHiscores():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successful, FALSE if failed.
//==============================================================================================

BOOL GS_Snake::LoadHiscores()
{

    char szTempString[_MAX_PATH] = {0};
    GS_IniFile gsIniFile;

    // Determine the full pathname of the INI file.
    GetCurrentDirectory(_MAX_PATH, szTempString);
    lstrcat(szTempString, "\\");
    lstrcat(szTempString, HISCORES_FILE);

    // Open the INI file.
    if (!gsIniFile.Open(szTempString))
    {
        // Exit function
        return FALSE;
    }

    // For every score in the hiscores array.
    for (int nLoop = 0; nLoop < MAX_SCORES; nLoop++)
    {
        // Determine the score heading.
        sprintf(szTempString, "Score%d", nLoop);
        // Load the player name from the hiscores file.
        lstrcpy(m_gsHiscores[nLoop].szName, gsIniFile.ReadString(szTempString, "Name", ".........."));
        // Load the game mode from the hiscores file.
        lstrcpy(m_gsHiscores[nLoop].szMode, gsIniFile.ReadString(szTempString, "Mode", "......."));
        // Load the player level from the hiscores file.
        lstrcpy(m_gsHiscores[nLoop].szLevel, gsIniFile.ReadString(szTempString, "Level", "0-0"));
        // Load the player score from the hiscores file.
        m_gsHiscores[nLoop].lScore = (long)gsIniFile.ReadFloat(szTempString, "Score", 0);
    }

    // Close the INI file.
    gsIniFile.Close();

    return TRUE;
}

// *********************************************************************************************

//==============================================================================================
// GS_Snake::SaveHiscores():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successful, FALSE if failed.
//==============================================================================================

BOOL GS_Snake::SaveHiscores()
{

    char szTempString[_MAX_PATH] = {0};
    GS_IniFile gsIniFile;

    // Determine the full pathname of the INI file.
    GetCurrentDirectory(_MAX_PATH, szTempString);
    lstrcat(szTempString, "\\");
    lstrcat(szTempString, HISCORES_FILE);

    // Open the INI file.
    if (!gsIniFile.Open(szTempString))
    {
        // Exit function
        return FALSE;
    }

    // For every score in the hiscores array.
    for (int nLoop = 0; nLoop < MAX_SCORES; nLoop++)
    {
        // Save the score heading.
        sprintf(szTempString, "Score%d", nLoop);
        // Save the player name and score to the hiscores file.
        gsIniFile.WriteString(szTempString, "Name", m_gsHiscores[nLoop].szName);
        gsIniFile.WriteString(szTempString, "Mode", m_gsHiscores[nLoop].szMode);
        gsIniFile.WriteString(szTempString, "Level", m_gsHiscores[nLoop].szLevel);
        gsIniFile.WriteFloat(szTempString, "Score", (float)m_gsHiscores[nLoop].lScore);
    }

    // Close the INI file.
    gsIniFile.Close();

    return TRUE;
}

// *********************************************************************************************

// *********************************************************************************************
// *** Helper Methods **************************************************************************
// *********************************************************************************************

//==============================================================================================
// GS_Snake::RenderBackground():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_Snake::RenderBackground(float fAlpha)
{
    // Set the alpha value to determine the transparency of the background.
    m_gsBackSprite.SetModulateColor(-1.0f, -1.0f, -1.0f, fAlpha);

    // Postion the background centered in the screen.
    m_gsBackSprite.SetDestXY((INTERNAL_RES_X - m_gsBackSprite.GetScaledWidth()) / 2,
                             (INTERNAL_RES_Y - m_gsBackSprite.GetScaledHeight()) / 2);

    // Display the background image.
    m_gsBackSprite.Render();

    // Is background semi-transparent?
    if (fAlpha != 1.0f)
    {
        // Reset the alpha value (transparency) of the background.
        m_gsBackSprite.SetModulateColor(-1.0f, -1.0f, -1.0f, 1.0f);
    }
}

// *********************************************************************************************

//==============================================================================================
// GS_Snake::RenderScore():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_Snake::RenderScore(float fAlpha)
{
    // Set the alpha value to determine the transparency of the background.
    m_gsBackSprite.SetModulateColor(-1.0f, -1.0f, -1.0f, fAlpha);

    m_gsBackSprite.SetSourceRect(0, INTERNAL_RES_Y + SCORE_AREA_HEIGHT, INTERNAL_RES_X, INTERNAL_RES_Y);

    // Postion the background centered in the screen.
    m_gsBackSprite.SetDestXY(0, PLAY_AREA_HEIGHT);

    // Display the background image.
    m_gsBackSprite.Render();

    // Is background semi-transparent?
    if (fAlpha != 1.0f)
    {
        // Reset the alpha value (transparency) of the background.
        m_gsBackSprite.SetModulateColor(-1.0f, -1.0f, -1.0f, 1.0f);
    }

    // Restore the background source and position
    m_gsBackSprite.SetSourceRect(0, INTERNAL_RES_Y, INTERNAL_RES_X, 0);
    m_gsBackSprite.SetDestXY(0, 0);

    // Display the level
    m_gsFont.SetText("LEVEL %d-%d", m_currentWorld, m_currentLevel);
    m_gsFont.SetModulateColor(1.0f, 1.0f, 1.0f, fAlpha);
    m_gsFont.SetScaleXY(1.0f, 1.0f);
    m_gsFont.SetDestX(((INTERNAL_RES_X / 4) - m_gsFont.GetTextWidth()) / 2); // - (INTERNAL_RES_X / 4 * 3));
    m_gsFont.SetDestY(INTERNAL_RES_Y - SCORE_AREA_HEIGHT + ((SCORE_AREA_HEIGHT - m_gsFont.GetTextHeight()) / 2));
    m_gsFont.Render();

    // Display rats for next leavel
    m_gsFont.SetText("NEXT %d/%d", m_ratsEaten, RATS_PER_LEVEL);
    m_gsFont.SetModulateColor(1.0f, 1.0f, 1.0f, fAlpha);
    m_gsFont.SetScaleXY(1.0f, 1.0f);
    m_gsFont.SetDestX((((INTERNAL_RES_X / 4) - m_gsFont.GetTextWidth()) / 2) + (INTERNAL_RES_X / 4 * 1));
    m_gsFont.SetDestY(INTERNAL_RES_Y - SCORE_AREA_HEIGHT + ((SCORE_AREA_HEIGHT - m_gsFont.GetTextHeight()) / 2));
    m_gsFont.Render();

    // Display the player lives
    m_gsFont.SetText("LIVES %d", m_lives);
    m_gsFont.SetModulateColor(1.0f, 1.0f, 1.0f, fAlpha);
    m_gsFont.SetScaleXY(1.0f, 1.0f);
    m_gsFont.SetDestX((((INTERNAL_RES_X / 4) - m_gsFont.GetTextWidth()) / 2) + (INTERNAL_RES_X / 4 * 2));
    m_gsFont.SetDestY(INTERNAL_RES_Y - SCORE_AREA_HEIGHT + ((SCORE_AREA_HEIGHT - m_gsFont.GetTextHeight()) / 2));
    m_gsFont.Render();

    // Display the player score
    m_gsFont.SetText("SCORE %ld", m_lScore);
    m_gsFont.SetModulateColor(1.0f, 1.0f, 1.0f, fAlpha);
    m_gsFont.SetScaleXY(1.0f, 1.0f);
    m_gsFont.SetDestX((((INTERNAL_RES_X / 4) - m_gsFont.GetTextWidth()) / 2) + (INTERNAL_RES_X / 4 * 3));
    m_gsFont.SetDestY(INTERNAL_RES_Y - SCORE_AREA_HEIGHT + ((SCORE_AREA_HEIGHT - m_gsFont.GetTextHeight()) / 2));
    m_gsFont.Render();

    // Reset the font modulate color
    m_gsFont.SetModulateColor(1.0f, 1.0f, 1.0f, 1.0f);
    return;

    // Set the alpha value to determine the transparency of the score backround
    m_gsScoreBackground.SetModulateColor(-1.0f, -1.0f, -1.0f, fAlpha);

    // Setup variables that will be used to render the scorearea
    int nRows = 3;
    int nCols = 60;

    long lDestX = 0;
    long lDestY = PLAY_AREA_HEIGHT + 1;

    float fWidth = nCols * m_gsScoreBackground.GetFrameWidth();
    float fHeight = nRows * m_gsScoreBackground.GetFrameHeight();

    float fTileWidth = m_gsScoreBackground.GetFrameWidth();
    float fTileHeight = m_gsScoreBackground.GetFrameHeight();

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Render the score area ////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Render top left corner.
    m_gsScoreBackground.SetFrame(2);
    m_gsScoreBackground.SetDestX(lDestX);
    m_gsScoreBackground.SetDestY(lDestY + fHeight - fTileHeight);
    m_gsScoreBackground.Render();

    // Render top right corner.
    m_gsScoreBackground.SetFrame(4);
    m_gsScoreBackground.SetDestX(lDestX + fWidth - fTileWidth);
    m_gsScoreBackground.SetDestY(lDestY + fHeight - fTileHeight);
    m_gsScoreBackground.Render();

    // Render bottom left corner.
    m_gsScoreBackground.SetFrame(5);
    m_gsScoreBackground.SetDestX(lDestX);
    m_gsScoreBackground.SetDestY(lDestY);
    m_gsScoreBackground.Render();

    // Render bottom right corner.
    m_gsScoreBackground.SetFrame(7);
    m_gsScoreBackground.SetDestX(lDestX + fWidth - fTileWidth);
    m_gsScoreBackground.SetDestY(lDestY);
    m_gsScoreBackground.Render();

    int xLoop, yLoop;

    // Render the top and bottom borders.
    for (xLoop = 1; xLoop < (nCols - 1); xLoop++)
    {
        // Render top border.
        m_gsScoreBackground.SetFrame(3);
        m_gsScoreBackground.SetDestX(lDestX + (xLoop * fTileWidth));
        m_gsScoreBackground.SetDestY(lDestY + fHeight - fTileHeight);
        m_gsScoreBackground.Render();

        // Render bottom border.
        m_gsScoreBackground.SetFrame(6);
        m_gsScoreBackground.SetDestX(lDestX + (xLoop * fTileWidth));
        m_gsScoreBackground.SetDestY(lDestY);
        m_gsScoreBackground.Render();
    }

    // Render the left and right borders.
    for (yLoop = 1; yLoop < (nRows - 1); yLoop++)
    {
        // Render left border or seperator.
        m_gsScoreBackground.SetFrame(0);
        m_gsScoreBackground.SetDestX(lDestX);
        m_gsScoreBackground.SetDestY(lDestY + fHeight - ((yLoop + 1) * fTileHeight));
        m_gsScoreBackground.Render();

        // Render right border or seperator.
        m_gsScoreBackground.SetFrame(1);
        m_gsScoreBackground.SetDestX(lDestX + fWidth - fTileWidth);
        m_gsScoreBackground.SetDestY(lDestY + fHeight - ((yLoop + 1) * fTileHeight));
        m_gsScoreBackground.Render();
    }

    // Render the score area background
    for (xLoop = 1; xLoop < (nCols - 1); xLoop++)
    {
        for (yLoop = 1; yLoop < (nRows - 1); yLoop++)
        {
            // Render background
            m_gsScoreBackground.SetFrame(11);
            m_gsScoreBackground.SetDestX(lDestX + (xLoop * fTileWidth));
            m_gsScoreBackground.SetDestY(lDestY + fHeight - ((yLoop + 1) * fTileHeight));
            m_gsScoreBackground.Render();
        }
    }

    // Is score backround semi-transparent?
    if (fAlpha != 1.0f)
    {
        // Reset the alpha value (transparency) of the background
        m_gsScoreBackground.SetModulateColor(-1.0f, -1.0f, -1.0f, 1.0f);
    }
}

// *********************************************************************************************

//==============================================================================================
// GS_Snake::RenderCursor():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_Snake::RenderCursor(float fAlpha)
{

    // Set the alpha value to determine the transparency of the cursor.
    m_gsCursorSprite.SetModulateColor(-1.0f, -1.0f, -1.0f, fAlpha);

    // Set the cursor position at the current mouse coordinates.
    m_gsCursorSprite.SetDestY(m_gsMouse.GetY() - (int)m_gsCursorSprite.GetScaledHeight() + 1);
    m_gsCursorSprite.SetDestX(m_gsMouse.GetX());

    // Display the cursor image.
    m_gsCursorSprite.Render();

    // Is cursor semi-transparent?
    if (fAlpha != 1.0f)
    {
        // Reset the alpha value (transparency) of the cursor.
        m_gsCursorSprite.SetModulateColor(-1.0f, -1.0f, -1.0f, 1.0f);
    }
}

// *********************************************************************************************

//==============================================================================================
// GS_Snake::RenderFrameRate():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_Snake::RenderFrameRate(float fAlpha)
{
    // Set the transparency of the font (0.0f is totally transparent).
    m_gsFont.SetModulateColor(-1.0f, -1.0f, -1.0f, fAlpha);

    // Display the frame rate at the top right of the screen
    m_gsFont.SetText("%0.2f", this->GetCurrentFrameRate());
    m_gsFont.SetScaleXY(0.5f, 0.5f);
    m_gsFont.SetDestX(m_gsFont.GetLetterWidth());
    m_gsFont.SetDestY(m_gsFont.GetTextHeight() * 2);
    // m_gsFont.SetDestX(INTERNAL_RES_X - m_gsFont.GetTextWidth() - m_gsFont.GetLetterWidth());
    // m_gsFont.SetDestY(INTERNAL_RES_Y - (m_gsFont.GetTextHeight() * 2));
    m_gsFont.Render();

    // Reset the default font values
    m_gsFont.SetScaleXY(1.0f, 1.0f);

    // Is font semi-transparent?
    if (fAlpha != 1.0f)
    {
        // Reset the alpha value (transparency) of the font.
        m_gsFont.SetModulateColor(-1.0f, -1.0f, -1.0f, 1.0f);
    }
}

// *********************************************************************************************

//==============================================================================================
// GS_Snake::SetRenderScaling():
// ---------------------------------------------------------------------------------------------
// Purpose: Set the values used for upscaling or downscaling when rendering
// ---------------------------------------------------------------------------------------------
// Returns: Nothing
//==============================================================================================

void GS_Snake::SetRenderScaling(int nWidth, int nHeight, bool bKeepAspect)
{
    // Is no scaling required?
    if ((nWidth == INTERNAL_RES_X) && (nHeight == INTERNAL_RES_Y))
    {
        // Reset the default scaling values
        GS_OGLDisplay::SetScaleFactorX(1.0f);
        GS_OGLDisplay::SetScaleFactorY(1.0f);

        GS_OGLDisplay::SetRenderModX(0.0f);
        GS_OGLDisplay::SetRenderModY(0.0f);

        return;
    }

    // Determine the aspect ratio of the internal resolution
    float aspect = (float)INTERNAL_RES_X / (float)INTERNAL_RES_Y;

    // Set up default values for upscaling or downscaling
    float fAspectWidth = (float)nWidth;
    float fAspectHeight = (float)nHeight;
    float fRenderModX = 0;
    float fRenderModY = 0;

    // Should we keep the aspect ratio?
    if (bKeepAspect)
    {
        // Is the horizontal dimension greater than the vertical?
        if (INTERNAL_RES_X >= INTERNAL_RES_Y)
        {
            // Try to scale the rendering to the given width
            fAspectWidth = (float)nWidth;
            fAspectHeight = (float)fAspectWidth / aspect;
            fRenderModX = 0;
            fRenderModY = (nHeight - fAspectHeight) / 2.0f;

            // Have we scaled beyond the given height
            if (fAspectHeight > (float)nHeight)
            {
                // Scale the rendering to the given height
                fAspectHeight = (float)nHeight;
                fAspectWidth = fAspectHeight * aspect;
                fRenderModX = (nWidth - fAspectWidth) / 2.0f;
                fRenderModY = 0;
            }
        }
        else
        {
            // Try to scale the rendering to the given height
            fAspectHeight = (float)nHeight;
            fAspectWidth = fAspectHeight * aspect;
            fRenderModX = (nWidth - fAspectWidth) / 2.0f;
            fRenderModY = 0;

            // Have we scaled beyond the given width
            if (fAspectWidth > (float)nWidth)
            {
                // Scale the rendering to the given width
                fAspectWidth = (float)nWidth;
                fAspectHeight = (float)fAspectWidth / aspect;
                fRenderModX = 0;
                fRenderModY = (nHeight - fAspectHeight) / 2.0f;
            }
        }
    }

    // Set the amount with which the rendering coordinates should be modified
    GS_OGLDisplay::SetRenderModX(int(fRenderModX));
    GS_OGLDisplay::SetRenderModY(int(fRenderModY));

    // Set the scale factor to effect upscaling or downscaling depending on the resolution
    GS_OGLDisplay::SetScaleFactorX((float)fAspectWidth / (float)INTERNAL_RES_X);
    GS_OGLDisplay::SetScaleFactorY((float)fAspectHeight / (float)INTERNAL_RES_Y);
}

//==============================================================================================
// GS_Snake::GetActionInterval():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: The interval into which any action or actions must be divided in order to be
//          completed in a second at the current frame rate of the application.
//==============================================================================================

float GS_Snake::GetActionInterval(float fActionsPerSecond)
{

    // Convert frame time to seconds and multiply it by the number of actions to determine
    // what fraction of the action needs to be completed in order to complete the action in
    // one second (at the current frame rate).
    return ((this->GetFrameTime() / 1000) * fActionsPerSecond);
}

// *********************************************************************************************

// GS_Error::Report("GS_SNAKE.CPP", 441, "Break Point!");
