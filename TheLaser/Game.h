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
	explicit Game(const Window& window);
	Game(const Game& other) = delete;
	Game& operator=(const Game& other) = delete;
	Game(Game&& other) = delete;
	Game& operator=(Game&& other) = delete;
	~Game();

	void Update(float elapsedSec) override;
	void Draw() const override;

	void ProcessKeyDownEvent(const SDL_KeyboardEvent& e) override;
	void ProcessKeyUpEvent(const SDL_KeyboardEvent& e) override;
	void ProcessMouseMotionEvent(const SDL_MouseMotionEvent& e) override;
	void ProcessMouseDownEvent(const SDL_MouseButtonEvent& e) override;
	void ProcessMouseUpEvent(const SDL_MouseButtonEvent& e) override;

private:

	void Initialize();
	void Cleanup();
	void ClearBackground() const;

	enum class GameState
	{
		StartScreen,
		Playing,
		Victory,
		GameOver
	};

	GameState m_GameState{ GameState::StartScreen };
	StartingPosition m_StartPosition{};

	float m_GameTimer{ 60.0f };
	float m_VictoryTimer{ 0.0f };
	const float m_MaxVictoryTime{ 0.5f };
	const float m_TimerIncrement{ 5.0f };

	float m_LevelTimeElapsed{ 0.0f };
	int m_Score{ 0 };
	int m_LevelsSolved{ 0 };

	float m_RedBorderPulse{ 0.0f };

	const Vector2f m_Center{ BaseGame::GetViewPort().width * 0.5f, BaseGame::GetViewPort().height * 0.5f };
	Grid* m_Grid{ new Grid{ 12, 12, 40.f, m_Center } };

	std::vector<Vector2f> m_BoundaryPoints{
		Vector2f{0.f, 0.f},
		Vector2f{BaseGame::GetViewPort().width, 0.f},
		Vector2f{BaseGame::GetViewPort().width, BaseGame::GetViewPort().height},
		Vector2f{0.f, BaseGame::GetViewPort().height}
	};

	// --- LASER 1 (red, always active) ---
	Laser* m_Laser{ new Laser() };
	Vector2f m_LaserStartPoint{};
	Vector2f m_LaserDirection{ 1.f, 0.f };

	// --- LASER 2 (blue, active from level 5) ---
	Laser* m_Laser2{ new Laser() };
	Vector2f m_Laser2StartPoint{};
	Vector2f m_Laser2Direction{ 0.f, -1.f };
	int m_Laser2StartCol{ 0 };
	int m_Laser2StartRow{ 0 };
	bool m_DualLaserMode{ false };

	LevelGenerator m_LevelGenerator{};

	Digit m_TextureHolder{ 0, Digit::Mode::Bright, Vector2f{ -100.0f, -100.0f } };

	std::vector<Digit*> m_TimerDigits{};
	int m_DisplayedSeconds{ -1 };

	std::vector<Digit*> m_ScoreDigits{};
	int m_DisplayedScore{ -1 };

	std::vector<Digit*> m_LevelCounterDigits{};
	int m_DisplayedLevels{ -1 };

	Rectf m_TimerBoxRect{};

	Texture* m_pScoreLabel{ nullptr };
	Texture* m_pLevelLabel{ nullptr };
	Texture* m_pTimeLabel{ nullptr };

	Texture* m_pTitleLabel{ nullptr };
	Texture* m_pStartLabel{ nullptr };
	Texture* m_pControlsText{ nullptr };
	Texture* m_pScoringText{ nullptr };

	Texture* m_pGameOverTitle{ nullptr };
	Texture* m_pGameOverScoreText{ nullptr };
	Texture* m_pGameOverLevelText{ nullptr };
	Texture* m_pRestartText{ nullptr };

	Texture* m_pLeaderboardTitle{ nullptr };
	std::vector<Texture*> m_LeaderboardTextures{};
	std::vector<int> m_Leaderboard{};

	// --- LOGIC ---
	void ResetGameStats();
	void SetupNewLevel();
	void StartGame();
	void Restart();
	void ChooseRandomStartPosition();
	int  FindBestLaser2Col(int entryRow, int dirY) const; // picks col least crossed by path 1

	// Returns true if the laser hit the expected receiver type
	bool CalculateLaserPath(Laser* laser, const Vector2f& firstPoint, const Vector2f& laserDirection, MirrorType expectedReceiver);

	// --- RENDER ---
	void DrawLaserEmitter(const Vector2f& startPoint, const Vector2f& direction, const Color4f& color) const;
	void DrawHUD() const;
	void DrawStartScreen() const;
	void DrawRedBorder() const;
	void DrawGameOver() const;
	Rectf GetStartBtnRect() const;
	Rectf GetRestartBtnRect() const;

	// --- HUD UPDATE ---
	void UpdateHUD();
	void UpdateTimerDigits(int seconds);
	void UpdateScoreDigits(int score);
	void UpdateLevelCounterDigits(int levels);

	// --- LEADERBOARD ---
	void BuildLeaderboardTextures();
	void SaveLeaderboard() const;
	void LoadLeaderboard();
};