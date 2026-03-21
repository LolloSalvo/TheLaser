#pragma once
#include "BaseGame.h"

#include "Grid.h"
#include "Mirror.h"
#include "Laser.h"

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

	const Vector2f m_Center{ BaseGame::GetViewPort().width * 0.5f, BaseGame::GetViewPort().height * 0.5f };

	Grid* m_Grid{ new Grid{ 10, 10, 50.f, m_Center } };

	std::vector<Vector2f> m_BoundaryPoints{ 
		Vector2f{0.f, 0.f},
		Vector2f{BaseGame::GetViewPort().width, 0.f},
		Vector2f{BaseGame::GetViewPort().width, BaseGame::GetViewPort().height},
		Vector2f{0.f, BaseGame::GetViewPort().height}
	};

	Vector2f m_LaserDirection{ 1.f, 0.f };
	Vector2f m_LaserStartPoint{};

	void CalculateLaserPath(const Vector2f& firstPoint, Vector2f& laserDirection);

	Laser* m_Laser{ new Laser() };
};