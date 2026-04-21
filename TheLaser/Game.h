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

	// FUNCTIONS
	void Initialize();
	void Cleanup( );
	void ClearBackground( ) const;

	enum class GameState
	{
		Playing,
		Victory,
		GameOver
	};

	GameState m_GameState{};

	StartingPosition m_StartPosition{};

	const Vector2f m_Center{ BaseGame::GetViewPort().width * 0.5f, BaseGame::GetViewPort().height * 0.5f };

	Grid* m_Grid{ new Grid{ 12, 12, 40.f, m_Center } };

	std::vector<Vector2f> m_BoundaryPoints{ 
		Vector2f{0.f, 0.f},
		Vector2f{BaseGame::GetViewPort().width, 0.f},
		Vector2f{BaseGame::GetViewPort().width, BaseGame::GetViewPort().height},
		Vector2f{0.f, BaseGame::GetViewPort().height}
	};

	Vector2f m_LaserDirection{ 1.f, 0.f };
	Vector2f m_LaserStartPoint{};

	Laser* m_Laser{ new Laser() };
	
	LevelGenerator m_LevelGenerator{};

	Digit m_TextureHolder{ 0, Digit::Mode::Bright, Vector2f{ -100.0f, -100.0f } };

	std::vector<Digit*> m_TimerDigits{};

	int m_DisplayedSeconds{ -1 };

	float m_GameTimer{ 60.0f };
	float m_VictoryTimer{ 0.0f };
	const float m_MaxVictoryTime{ 0.5f };
	const float m_TimerIncrement{ 5.0f };

	int m_Score{ 0 };
	float m_LevelTimeElapsed{ 0.0f };
	std::vector<Digit*> m_ScoreDigits{};
	int m_DisplayedScore{ -1 };


	//Functions

	void ChooseRandomStartPosition();
	
	void CalculateLaserPath(const Vector2f& firstPoint, Vector2f& laserDirection);

	void Restart();

	void UpdateTimerDigits(int seconds);

	void DrawLaserEmitter() const;
	
	void UpdateScoreDigits(int score);

};