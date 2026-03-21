#include "pch.h"
#include "Game.h"
#include "utils.h"

#include <iostream>

Game::Game( const Window& window ) 
	:BaseGame{ window }
{
	Initialize();
}

Game::~Game( )
{
	Cleanup( );
	delete m_Grid;
}

void Game::Initialize( )
{
	m_LaserStartPoint = Vector2f{ m_Grid->GetCellCenter(0, 0).x - m_Grid->GetCellSize() / 2, m_Grid->GetCellCenter(0, 0).y + m_Grid->GetCellSize() / 2 };
	m_Laser->AddPoint(m_LaserStartPoint);

	m_Grid->AddMirror(3, 0);
	m_Grid->AddMirror(3, 7);
	m_Grid->AddMirror(7, 7);
	m_Grid->AddMirror(7, 9);
}

void Game::Cleanup( )
{
}

void Game::Update( float elapsedSec )
{
	CalculateLaserPath(m_LaserStartPoint, m_LaserDirection);
}

void Game::Draw( ) const
{
	ClearBackground( );
	m_Grid->Draw();

	m_Laser->Draw();
}

void Game::ProcessKeyDownEvent( const SDL_KeyboardEvent & e )
{
	//std::cout << "KEYDOWN event: " << e.keysym.sym << std::endl;
}

void Game::ProcessKeyUpEvent( const SDL_KeyboardEvent& e )
{
	//std::cout << "KEYUP event: " << e.keysym.sym << std::endl;
	//switch ( e.keysym.sym )
	//{
	//case SDLK_LEFT:
	//	//std::cout << "Left arrow key released\n";
	//	break;
	//case SDLK_RIGHT:
	//	//std::cout << "`Right arrow key released\n";
	//	break;
	//case SDLK_1:
	//case SDLK_KP_1:
	//	//std::cout << "Key 1 released\n";
	//	break;
	//}
}

void Game::ProcessMouseMotionEvent( const SDL_MouseMotionEvent& e )
{
	//std::cout << "MOUSEMOTION event: " << e.x << ", " << e.y << std::endl;
}

void Game::ProcessMouseDownEvent( const SDL_MouseButtonEvent& e )
{

	switch (e.button)
	{
	case SDL_BUTTON_LEFT:
		
		std::cout << m_Grid->GetCellIndexFromPosition(Vector2f{ static_cast<float>(e.x), static_cast<float>(e.y) }, m_Center) << std::endl;
		break;
	}
	
}

void Game::ProcessMouseUpEvent( const SDL_MouseButtonEvent& e )
{
	//std::cout << "MOUSEBUTTONUP event: ";
	//switch ( e.button )
	//{
	//case SDL_BUTTON_LEFT:
	//	std::cout << " left button " << std::endl;
	//	break;
	//case SDL_BUTTON_RIGHT:
	//	std::cout << " right button " << std::endl;
	//	break;
	//case SDL_BUTTON_MIDDLE:
	//	std::cout << " middle button " << std::endl;
	//	break;
	//}
}

void Game::ClearBackground( ) const
{
	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT );
}

void Game::CalculateLaserPath(const Vector2f& firstPoint, Vector2f& laserDirection)
{
	const float step{ 10.f };

	Vector2f currentFirstPoint{ firstPoint };
	Vector2f secondPoint{ firstPoint };
	utils::HitInfo hitInfo{};

	while (!utils::Raycast(m_BoundaryPoints, currentFirstPoint, secondPoint, hitInfo))
	{
		secondPoint += Vector2f{ step * laserDirection.x, step * laserDirection.y };

		Cell* pCurrentCell{ m_Grid->GetCellFromPosition(secondPoint, m_Center) };
		if (pCurrentCell != nullptr && pCurrentCell->HasMirror())
		{
			Vector2f mirrorFirstPoint{};
			Vector2f mirrorSecondPoint{};

			if (pCurrentCell->GetMirrorPoint(mirrorFirstPoint, mirrorSecondPoint))
			{
				if (utils::Raycast(std::vector<Vector2f>{mirrorFirstPoint, mirrorSecondPoint}, firstPoint, secondPoint, hitInfo))
				{
					m_Laser->AddPoint(hitInfo.intersectPoint);
					laserDirection = laserDirection.Reflect(hitInfo.normal);

					currentFirstPoint = hitInfo.intersectPoint;
					secondPoint = currentFirstPoint;
				}
			}
		}
	}

	m_Laser->AddPoint(hitInfo.intersectPoint);


}
