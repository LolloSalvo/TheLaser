#pragma once
#include "BaseGame.h"

#include "Grid.h"
#include "Mirror.h"
#include "Laser.h"
#include "LevelGenerator.h"
#include "Digit.h"

#include <vector>

class Game : public BaseGame
{
public:
	explicit Game( const Window& window );
	Game(const Game& other) = delete;
	Game& operator=(const Game& other) = delete;
	Game( Game&& other) = delete;
	Game& operator=(Game&& other) = delete;
	// http://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rh-override
	~Game();

	void Update( float elapsedSec ) override;
	void Draw( ) const override;

	// Event handling
	void ProcessKeyDownEvent( const SDL_KeyboardEvent& e ) override;
	void ProcessKeyUpEvent( const SDL_KeyboardEvent& e ) override;
	void ProcessMouseMotionEvent( const SDL_MouseMotionEvent& e ) override;
	void ProcessMouseDownEvent( const SDL_MouseButtonEvent& e ) override;
	void ProcessMouseUpEvent( const SDL_MouseButtonEvent& e ) override;

private:

	// --- CORE FUNCTIONS ---
	void Initialize();
	void Cleanup();
	void ClearBackground() const;

	// --- ENUMS ---
	enum class GameState
	{
		Playing,
		Victory,
		GameOver
	};

	// --- GAME STATE & TIMERS ---
	GameState m_GameState{};
	StartingPosition m_StartPosition{};

	float m_GameTimer{ 60.0f };
	float m_VictoryTimer{ 0.0f };
	const float m_MaxVictoryTime{ 0.5f };
	const float m_TimerIncrement{ 5.0f };

	float m_LevelTimeElapsed{ 0.0f };
	int m_Score{ 0 };
	int m_LevelsSolved{ 0 };

	// --- CORE OBJECTS (GRID, LASER, LEVEL) ---
	const Vector2f m_Center{ BaseGame::GetViewPort().width * 0.5f, BaseGame::GetViewPort().height * 0.5f };
	Grid* m_Grid{ new Grid{ 12, 12, 40.f, m_Center } };

	std::vector<Vector2f> m_BoundaryPoints{
		Vector2f{0.f, 0.f},
		Vector2f{BaseGame::GetViewPort().width, 0.f},
		Vector2f{BaseGame::GetViewPort().width, BaseGame::GetViewPort().height},
		Vector2f{0.f, BaseGame::GetViewPort().height}
	};

	Laser* m_Laser{ new Laser() };
	Vector2f m_LaserStartPoint{};
	Vector2f m_LaserDirection{ 1.f, 0.f };

	LevelGenerator m_LevelGenerator{};

	// --- HUD & UI VARIABLES ---
	Digit m_TextureHolder{ 0, Digit::Mode::Bright, Vector2f{ -100.0f, -100.0f } };

	std::vector<Digit*> m_TimerDigits{};
	int m_DisplayedSeconds{ -1 };

	std::vector<Digit*> m_ScoreDigits{};
	int m_DisplayedScore{ -1 };

	std::vector<Digit*> m_LevelCounterDigits{};
	int m_DisplayedLevels{ -1 };

	// --- TEXTURES HUD & GAME OVER ---
	Texture* m_pScoreLabel{ nullptr };
	Texture* m_pLevelLabel{ nullptr };
	Texture* m_pGameOverTitle{ nullptr };
	Texture* m_pGameOverScoreText{ nullptr };
	Texture* m_pGameOverLevelText{ nullptr };

	// --- LOGIC FUNCTIONS ---
	void ResetGameStats();
	void SetupNewLevel();
	void Restart();
	void ChooseRandomStartPosition();
	void CalculateLaserPath(const Vector2f& firstPoint, Vector2f& laserDirection);

	// --- RENDER & HUD FUNCTIONS ---
	void DrawLaserEmitter() const;
	void UpdateHUD();
	void UpdateTimerDigits(int seconds);
	void UpdateScoreDigits(int score);
	void UpdateLevelCounterDigits(int levels);
	void DrawGameOver() const;
};