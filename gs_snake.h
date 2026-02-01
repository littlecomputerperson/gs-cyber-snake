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

#ifndef GS_SNAKE_H
#define GS_SNAKE_H

//================================================================================================
// Include Game System (GS) header files.
// ---------------------------------------------------------------------------------------------
#include "gs_app.h"
#include "gs_keyboard.h"
#include "gs_mouse.h"
#include "gs_ogl_collide.h"
#include "gs_ini_file.h"
#include "gs_ogl_display.h"
#include "gs_ogl_font.h"
#include "gs_ogl_menu.h"
#include "gs_ogl_sprite_ex.h"
#include "gs_ogl_particle.h"
#include "gs_sdl_controller.h"

#ifdef GS_USE_SDL_MIXER
#include "gs_sdl_mixer_sound.h"
#else
#include "gs_fmod_sound.h"
#endif
//================================================================================================

//================================================================================================
// Include custom game header files.
// -----------------------------------------------------------------------------------------------
#include "snake.h"
#include "rat.h"
#include "level.h"
//================================================================================================

//================================================================================================
// Include standard C library header files.
// -----------------------------------------------------------------------------------------------
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <time.h>
//================================================================================================

//==============================================================================================
// Game defines.
// ---------------------------------------------------------------------------------------------
#define GAME_VERSION "1.0.1"
#define SETTINGS_FILE "settings.ini"
#define HISCORES_FILE "hiscores.ini"
// ---------------------------------------------------------------------------------------------
#define INTERNAL_RES_X 960
#define INTERNAL_RES_Y 540
#define PLAY_AREA_WIDTH 960
#define PLAY_AREA_HEIGHT 480
#define PLAY_AREA_ROWS 30
#define PLAY_AREA_COLS 60
#define SCORE_AREA_WIDTH 960
#define SCORE_AREA_HEIGHT 60
// ---------------------------------------------------------------------------------------------
#define DEFAULT_WIDTH 960
#define DEFAULT_HEIGHT 540
#define DEFAULT_DEPTH 32
#define DEFAULT_MODE 1
#define DEFAULT_VSYNC 0
#define DEFAULT_ALIAS 0
#define DEFAULT_FRAMECAP 60.0f
#define DEFAULT_LIMIT 1
#define DEFAULT_MUSIC 255
#define DEFAULT_SOUND 255
// ---------------------------------------------------------------------------------------------
#define GAME_INTRO 0
#define GAME_OUTRO 1
#define TITLE_INTRO 2
#define TITLE_SCREEN 3
#define TITLE_OUTRO 4
#define OPTION_INTRO 5
#define OPTION_SCREEN 6
#define OPTION_OUTRO 7
#define PLAY_INTRO 8
#define PLAY_GAME 9
#define PLAY_UPDATE 10
#define PLAY_PAUSE 11
#define PLAY_EXIT 12
#define PLAY_OUTRO 13
#define SCORES_INTRO 14
#define SCORES_VIEW 15
#define SCORES_ADD 16
#define SCORES_OUTRO 17
// ---------------------------------------------------------------------------------------------
#define MAX_PARTICLE_SNAKES 20
#define MAX_PARTICLE_SNAKE_SPEED 6
#define MIN_PARTICLE_SNAKE_SPEED 2
#define MAX_PARTICLE_SNAKE_LENGTH 15
#define MIN_PARTICLE_SNAKE_LENGTH 5
// ---------------------------------------------------------------------------------------------
#define MAX_SCORES 10
// ---------------------------------------------------------------------------------------------
#define MUSIC_TITLE 0
#define MUSIC_GAME 1
// ---------------------------------------------------------------------------------------------
#define SAMPLE_OPTION 0
#define SAMPLE_SELECT 1
#define SAMPLE_SNAKE_INPUT 2
#define SAMPLE_SNAKE_GROWING 3
#define SAMPLE_SNAKE_DYING 4
#define SAMPLE_RAT_MOVING 5
#define SAMPLE_RAT_DYING 6
// ---------------------------------------------------------------------------------------------
#define RATS_PER_LEVEL 10
#define RATE_BASE_SCORE 10
#define DEFAULT_LIVES 5
// ---------------------------------------------------------------------------------------------
#define EASY_MODE 1
#define NORMAL_MODE 2
#define HARD_MODE 3
//==============================================================================================

//==============================================================================================
// Game structures.
// ---------------------------------------------------------------------------------------------
typedef struct GS_SETTINGS
{
    int nDisplayWidth;    // The width (in pixels) of the display area.
    int nDisplayHeight;   // The height (in pixels) of the display area.
    int nColorDepth;      // The color depth of the display (8, 16, 24 or 32).
    BOOL bWindowedMode;   // Whether game is in fullscreen or windowed mode.
    BOOL bEnableVSync;    // Whether to syncronize rendering with refresh rate.
    BOOL bEnableAliasing; // Whether to enable anti-aliasing or not.
    float fFrameCap;      // Set the framerate cap (0.0f for uncapped)
    int nMusicVolume;     // The volume of the music (0-255).
    int nEffectsVolume;   // The volume of the sound effects (0-255).
} GS_Settings;
// ---------------------------------------------------------------------------------------------
typedef struct GS_HISCORES
{
    char szName[11];
    char szLevel[8];
    char szMode[8];
    long lScore;
} GS_Hiscores;
// ---------------------------------------------------------------------------------------------
typedef struct PARTICLE_SNAKE
{
    float x;
    float y;
    float speed;
    int direction;
    float intervalCounter;
    float directionInterval;
    int length;
    GS_OGLColor color;
} ParticleSnake;
//==============================================================================================

////////////////////////////////////////////////////////////////////////////////////////////////
// Class Definition. ///////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

class GS_Snake : public GS_Application
{

private:
    GS_Settings m_gsSettings; // Option settings.

    GS_Keyboard m_gsKeyboard;     // Keyboard object.
    GS_Mouse m_gsMouse;           // Mouse object.
    GS_Controller m_gsController; // Controller object.

    GS_OGLDisplay m_gsDisplay; // OpenGL display object.

#ifdef GS_USE_SDL_MIXER
    GS_SDLMixerSound m_gsSound;
#else
    GS_FmodSound m_gsSound;
#endif

    GS_Timer m_gsTimer; // Timer object.

    GS_OGLSprite m_gsBackSprite;   // Sprite used for the game background.
    GS_OGLSprite m_gsCursorSprite; // Sprite used for the mouse cursor.
    // GS_OGLSprite m_gsTileSprite;   // Sprite used for tile background.
    GS_OGLSprite m_gsTitleSprite; // Sprite used for the title.

    GS_OGLTexture m_gsFontTexture; // Texture used to create the font end menu.
    GS_OGLFont m_gsFont;           // Game font.
    GS_OGLMenu m_gsMenu;           // Game menu.

    GS_OGLCollide m_gsCollide; // Object for collision detection.

    RECT m_rcScreen;         // Screen coordinates for rendering tiles.
    RECT m_rcPlayArea;       // Coordinates for the play area
    RECT m_rcScoreArea;      // Coordinates for the score area
    bool m_bKeepAspectRatio; // Whether to keep the aspect ratio for up/down scaling

    int m_nGameProgress; // Keeps track of the game progress.
    int m_nPrevProgress; // Previous stage.
    int m_nNextProgress; // Next stage.

    BOOL m_bIsInitialized;    // Has a method been initialized?
    BOOL m_bWasKeyReleased;   // Whether a key has been released.
    BOOL m_bWasMouseReleased; // Whether the left mouse button was released.
    int m_nOldMouseX;         // Previous mouse x coordinate.
    int m_nOldMouseY;         // Previous mouse y coordinate.
    int m_nOptionSelected;    // Which menu option was selected.
    int m_nCounter;           // Used for counting throughout the game.

    float m_fInterval; // Used for game timing.
    float m_fRotate;   // Used for rotation effects.
    float m_fScale;    // Used for scale effects.
    float m_fAlpha;    // Transparency for fade effects.

    GS_OGLColor m_gsMenuTextColor; // Normal menu text color.
    GS_OGLColor m_gsMenuHighColor; // Menu highlight text color.

    GS_OGLSpriteEx m_gsScoreBackground; // Create a sprite for creating a backckgroun for displaying the score

    GS_OGLParticle m_snakeParticle;
    ParticleSnake m_particleSnake[MAX_PARTICLE_SNAKES];

    Snake m_snake;
    Rat m_rat;
    Level m_level;

    int m_gameMode;
    int m_currentWorld;
    int m_currentLevel;
    int m_ratsEaten;
    int m_lives;

    int m_defaultRatState;

    GS_Hiscores m_gsHiscores[MAX_SCORES]; // Hiscores.

    long m_lScore;     // Keeps track of the game score.
    int m_nScoreIndex; // Keeps track of the last score.

protected:
    // Methods that override base class methods.
    BOOL GameInit();
    BOOL GameShutdown();
    BOOL GameRelease();
    BOOL GameRestore();
    BOOL GameLoop();
    void OnChangeMode();

public:
    // The constuctor & destructor.
    GS_Snake();
    ~GS_Snake();

    // Message procedure that override the base class message procedure.
    LRESULT MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    // Game setup methods.
    BOOL GameSetup();

    // Game progress methods.
    BOOL GameIntro();
    BOOL GameOutro();

    BOOL TitleIntro();
    BOOL TitleScreen();
    BOOL TitleOutro();

    BOOL OptionIntro();
    BOOL OptionScreen();
    BOOL OptionOutro();

    BOOL PlayIntro();
    BOOL PlayGame();
    BOOL PlayUpdate();
    BOOL PlayPause();
    BOOL PlayExit();
    BOOL PlayOutro();

    BOOL ScoresIntro();
    BOOL ScoresView();
    BOOL ScoresAdd();
    BOOL ScoresOutro();

    // Methods for loading and saving data.
    BOOL LoadSettings();
    BOOL SaveSettings();
    BOOL LoadHiscores();
    BOOL SaveHiscores();

    // Helper methods.
    void RenderBackground(float fAlpha = 1.0f);
    void RenderScore(float fAlpha = 1.0f);
    void RenderCursor(float fAlpha = 1.0f);
    void RenderFrameRate(float fAlpha = 1.0f);
    void SetRenderScaling(int nWidth, int nHeight, bool bKeepAspect);
    float GetActionInterval(float fActionsPerSecond);
};

////////////////////////////////////////////////////////////////////////////////////////////////

#endif
