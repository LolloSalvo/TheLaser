#include "pch.h"
#include "Game.h"
#include "utils.h"

#include <algorithm>

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

void Game::Initialize()
{
	m_GameState = GameState::Playing;
	m_GameTimer = 60.0f;
	m_VictoryTimer = 0.0f;
	m_DisplayedSeconds = -1;

	m_Score = 0;
	m_LevelTimeElapsed = 0.0f;
	m_DisplayedScore = -1;

	ChooseRandomStartPosition();

	m_Laser->ClearPath();
	m_Laser->AddPoint(m_LaserStartPoint);

	LevelGenerator generator{};
	generator.GeneratePath(m_Grid, m_StartPosition, m_LaserDirection, 15);
}

void Game::Cleanup( )
{
	for (Digit* currentDigit : m_TimerDigits)
	{
		delete currentDigit;
	}
	m_TimerDigits.clear();

	for (Digit* currentDigit : m_ScoreDigits)
	{
		delete currentDigit;
	}
	m_ScoreDigits.clear();
}

void Game::Update(float elapsedSec)
{
	m_Grid->Update(elapsedSec);

	if (m_GameState == GameState::Playing)
	{
		m_GameTimer -= elapsedSec;
		m_LevelTimeElapsed += elapsedSec;

		if (m_GameTimer <= 0.0f)
		{
			m_GameTimer = 0.0f;
			m_GameState = GameState::GameOver;
		}

		CalculateLaserPath(m_LaserStartPoint, m_LaserDirection);
	}
	else if (m_GameState == GameState::Victory)
	{
		m_VictoryTimer += elapsedSec;

		if (m_VictoryTimer >= m_MaxVictoryTime)
		{
			m_VictoryTimer = 0.0f;
			m_LevelTimeElapsed = 0.0f;

			ChooseRandomStartPosition();
			m_Laser->ClearPath();
			m_Laser->AddPoint(m_LaserStartPoint);

			LevelGenerator generator{};
			generator.GeneratePath(m_Grid, m_StartPosition, m_LaserDirection, 15);

			m_GameState = GameState::Playing;
		}
	}

	int currentSeconds{ static_cast<int>(std::ceil(m_GameTimer)) };

	if (currentSeconds != m_DisplayedSeconds)
	{
		UpdateTimerDigits(currentSeconds);
		m_DisplayedSeconds = currentSeconds;
	}

	if (m_Score != m_DisplayedScore)
	{
		UpdateScoreDigits(m_Score);
		m_DisplayedScore = m_Score;
	}
}

void Game::Draw() const
{
	ClearBackground();
	m_Grid->Draw();

	m_Laser->Draw();
	DrawLaserEmitter();

	for (Digit* currentDigit : m_TimerDigits)
	{
		currentDigit->Draw();
	}

	for (Digit* currentDigit : m_ScoreDigits)
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

	const float step{ 10.0f };

	Vector2f currentLaserDirection{ laserDirection };
	Vector2f currentFirstPoint{ firstPoint };
	Vector2f secondPoint{ firstPoint };
	utils::HitInfo hitInformation{};

	bool hitReceiver{ false };

	while (!utils::Raycast(m_BoundaryPoints, currentFirstPoint, secondPoint, hitInformation))
	{
		secondPoint.x += (step * currentLaserDirection.x);
		secondPoint.y += (step * currentLaserDirection.y);

		Cell* currentCell{ m_Grid->GetCellFromPosition(secondPoint, m_Center) };

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

						if (!m_Grid->IsAnyMirrorRotating())
						{
							m_GameState = GameState::Victory;
							m_GameTimer += 15.0f;

							// Calcolo punti: 100 base, -5 per ogni secondo trascorso. Minimo 10 punti.
							int pointsEarned{ std::max(10, 100 - static_cast<int>(m_LevelTimeElapsed * 5.0f)) };
							m_Score += pointsEarned;
						}

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

	m_GameTimer = 60.0f;
	m_VictoryTimer = 0.0f;
	m_DisplayedSeconds = -1;


	m_Score = 0;
	m_LevelTimeElapsed = 0.0f;
	m_DisplayedScore = -1;

	ChooseRandomStartPosition();

	m_Laser->ClearPath();
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

	float digitWidth{ 40.0f };
	float digitHeight{ 60.0f };
	float spacing{ digitWidth + 5.0f };

	float totalWidth{ secondsString.length() * spacing };
	float startX{ (GetViewPort().width / 2.0f) - (totalWidth / 2.0f) };
	float startY{ GetViewPort().height - digitHeight - 20.0f };

	for (size_t index{ 0 }; index < secondsString.length(); ++index)
	{
		int numberValue{ secondsString[index] - '0' };
		Vector2f digitPosition{ startX + (index * spacing), startY };

		m_TimerDigits.push_back(new Digit{ numberValue, Digit::Mode::Dark, digitPosition });
	}
}

void Game::DrawLaserEmitter() const
{
	float height{ 32.0f };
	float baseWidth{ 24.0f };
	float baseHalfWidth{ baseWidth / 2.0f };

	Vector2f direction{ m_LaserDirection };
	Vector2f perpendicular{ -direction.y, direction.x };

	Vector2f apexPoint{ m_LaserStartPoint.x, m_LaserStartPoint.y };

	Vector2f baseCenter{ apexPoint.x - (direction.x * height), apexPoint.y - (direction.y * height) };

	Vector2f vertex2{ baseCenter.x + (perpendicular.x * baseHalfWidth), baseCenter.y + (perpendicular.y * baseHalfWidth) };
	Vector2f vertex3{ baseCenter.x - (perpendicular.x * baseHalfWidth), baseCenter.y - (perpendicular.y * baseHalfWidth) };

	std::vector<Vector2f> triangleVertices{ apexPoint, vertex2, vertex3 };

	utils::SetColor(Color4f{ 0.8f, 0.1f, 0.1f, 1.0f });
	utils::FillPolygon(triangleVertices);

	utils::SetColor(Color4f{ 1.0f, 1.0f, 1.0f, 1.0f });
	utils::DrawPolygon(triangleVertices, true);
}

void Game::UpdateScoreDigits(int score)
{
	for (Digit* currentDigit : m_ScoreDigits)
	{
		delete currentDigit;
	}
	m_ScoreDigits.clear();

	std::string scoreString{ std::to_string(score) };

	float digitWidth{ 40.0f };
	float digitHeight{ 60.0f };
	float spacing{ digitWidth + 5.0f };

	float startX{ 20.0f };
	float startY{ GetViewPort().height - digitHeight - 20.0f };

	for (size_t index{ 0 }; index < scoreString.length(); ++index)
	{
		int numberValue{ scoreString[index] - '0' };
		Vector2f digitPosition{ startX + (index * spacing), startY };

		m_ScoreDigits.push_back(new Digit{ numberValue, Digit::Mode::Bright, digitPosition });
	}
}
