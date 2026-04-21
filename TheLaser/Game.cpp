#include "pch.h"
#include "Game.h"
#include "utils.h"

#include <iostream>
#include <vector>

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
	m_GameState = GameState::Playing;
	m_GameTimer = 60.0f;
	m_VictoryTimer = 0.0f;
	m_DisplayedSeconds = -1;

	ChooseRandomStartPosition();
	m_Laser->AddPoint(m_LaserStartPoint);

	LevelGenerator generator{};
	generator.GeneratePath(m_Grid, m_StartPosition, m_LaserDirection, 15);
	
	/*m_Grid->AddMirror(3, 0);
	m_Grid->AddMirror(3, 7);
	m_Grid->AddMirror(7, 7);
	m_Grid->AddMirror(7, 8);*/
}

void Game::Cleanup( )
{
	for (Digit* currentDigit : m_TimerDigits)
	{
		delete currentDigit;
	}
	m_TimerDigits.clear();
}

void Game::Update( float elapsedSec )
{
	if (m_GameState == GameState::Playing)
	{
		CalculateLaserPath(m_LaserStartPoint, m_LaserDirection);
	}

	int currentSeconds{ static_cast<int>(std::ceil(m_GameTimer)) };

	if (currentSeconds != m_DisplayedSeconds)
	{
		UpdateTimerDigits(currentSeconds);
		m_DisplayedSeconds = currentSeconds;
	}
}

void Game::Draw( ) const
{
	ClearBackground( );
	m_Grid->Draw();

	m_Laser->Draw();

	for (Digit* currentDigit : m_TimerDigits)
	{
		currentDigit->Draw();
	}
}

void Game::ProcessKeyDownEvent( const SDL_KeyboardEvent & e )
{
	switch (e.keysym.sym)
	{
	case SDLK_r:
		
		Restart();
		break;
	}
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

	if (m_GameState == GameState::Playing)
	{
		if (e.button == SDL_BUTTON_LEFT)
		{
			m_Grid->RotateMirrorAt(Vector2f{ static_cast<float>(e.x), static_cast<float>(e.y) }, m_Center);
		}
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

void Game::ChooseRandomStartPosition()
{
	int choose = rand() % 4;

	m_StartPosition = static_cast<StartingPosition>(choose);

	switch (m_StartPosition)
	{
	case StartingPosition::BottomLeft:
		m_LaserStartPoint = Vector2f{ m_Grid->GetCellCenter(0, 0).x - m_Grid->GetCellSize(), m_Grid->GetCellCenter(0, 0).y };
		m_LaserDirection = Vector2f{ 1.f, 0.f };
		break;
	case StartingPosition::BottomRight:
		m_LaserStartPoint = Vector2f{ m_Grid->GetCellCenter(m_Grid->GetColumns() - 1 , 0).x + m_Grid->GetCellSize(), m_Grid->GetCellCenter(m_Grid->GetColumns() - 1, 0).y };
		m_LaserDirection = Vector2f{ -1.f, 0.f };
		break;
	case StartingPosition::TopLeft:
		m_LaserStartPoint = Vector2f{ m_Grid->GetCellCenter(0, m_Grid->GetRows() - 1).x - m_Grid->GetCellSize(), m_Grid->GetCellCenter(0, m_Grid->GetRows() - 1).y };
		m_LaserDirection = Vector2f{ 1.f, 0.f };	
		break;
	case StartingPosition::TopRight:
		m_LaserStartPoint = Vector2f{ m_Grid->GetCellCenter(m_Grid->GetColumns() - 1, m_Grid->GetRows() - 1).x + m_Grid->GetCellSize(), m_Grid->GetCellCenter(m_Grid->GetColumns() - 1, m_Grid->GetRows() - 1).y };
		m_LaserDirection = Vector2f{ -1.f, 0.f };
		break;
	default:
		break;
	}
}

void Game::CalculateLaserPath(const Vector2f& firstPoint, Vector2f& laserDirection)
{
	m_Laser->ClearPath();
	m_Laser->AddPoint(firstPoint);

	const float step = 10.0f;

	Vector2f currentLaserDirection = laserDirection;
	Vector2f currentFirstPoint = firstPoint;
	Vector2f secondPoint = firstPoint;
	utils::HitInfo hitInformation{};

	bool hitReceiver = false;

	while (!utils::Raycast(m_BoundaryPoints, currentFirstPoint, secondPoint, hitInformation))
	{
		secondPoint.x = secondPoint.x + (step * currentLaserDirection.x);
		secondPoint.y = secondPoint.y + (step * currentLaserDirection.y);

		Cell* currentCell = m_Grid->GetCellFromPosition(secondPoint, m_Center);

		if (currentCell != nullptr && currentCell->HasMirror())
		{
			Vector2f mirrorFirstPoint{};
			Vector2f mirrorSecondPoint{};

			if (currentCell->GetMirrorPoint(mirrorFirstPoint, mirrorSecondPoint))
			{
				std::vector<Vector2f> mirrorPoints{ mirrorFirstPoint, mirrorSecondPoint };

				if (utils::Raycast(mirrorPoints, currentFirstPoint, secondPoint, hitInformation))
				{
					m_Laser->AddPoint(hitInformation.intersectPoint);

					if (currentCell->GetMirrorType() == MirrorType::Receiver)
					{
						hitReceiver = true;
						m_GameState = GameState::Victory;

						std::cout << "VICTORY!" << std::endl;
						//Sleep(1000);
						Restart();
						break;
					}

					currentLaserDirection = currentLaserDirection.Reflect(hitInformation.normal);

					currentFirstPoint.x = hitInformation.intersectPoint.x + (currentLaserDirection.x * 3.0f);
					currentFirstPoint.y = hitInformation.intersectPoint.y + (currentLaserDirection.y * 3.0f);
					secondPoint = currentFirstPoint;
				}
			}
		}
	}

	if (!hitReceiver)
	{
		m_Laser->AddPoint(hitInformation.intersectPoint);
	}
}

void Game::Restart()
{
	m_GameState = GameState::Playing;

	ChooseRandomStartPosition();
	m_Laser->AddPoint(m_LaserStartPoint);

	LevelGenerator generator{};
	generator.GeneratePath(m_Grid, m_StartPosition, m_LaserDirection, 15);
}

void Game::UpdateTimerDigits(int seconds)
{
	for (Digit* currentDigit : m_TimerDigits)
	{
		delete currentDigit;
	}
	m_TimerDigits.clear();

	std::string secondsString{ std::to_string(seconds) };
	float spacing{ 40.0f };
	float totalWidth{ secondsString.length() * spacing };

	float startX{ (GetViewPort().width / 2.0f) - (totalWidth / 2.0f) };
	float startY{ GetViewPort().height - 60.0f };

	for (size_t index{ 0 }; index < secondsString.length(); ++index)
	{
		int numberValue{ secondsString[index] - '0' };
		Vector2f digitPosition{ startX + (index * spacing), startY };

		m_TimerDigits.push_back(new Digit{ numberValue, Digit::Mode::Bright, digitPosition });
	}
}

