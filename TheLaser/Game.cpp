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
	ChooseRandomStartPosition();
	//m_LaserStartPoint = Vector2f{ m_Grid->GetCellCenter(0, 0).x - m_Grid->GetCellSize() / 2, m_Grid->GetCellCenter(0, 0).y + m_Grid->GetCellSize() / 2 };
	m_Laser->AddPoint(m_LaserStartPoint);


	CreateMirrorPath(15);
	/*m_Grid->AddMirror(3, 0);
	m_Grid->AddMirror(3, 7);
	m_Grid->AddMirror(7, 7);
	m_Grid->AddMirror(7, 8);*/
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
		m_Grid->RotateMirrorAt(Vector2f{ static_cast<float>(e.x), static_cast<float>(e.y) }, m_Center);
		//m_Grid->AddRemoveMirrorAt(Vector2f{ static_cast<float>(e.x), static_cast<float>(e.y) }, m_Center);
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
	//m_LaserDirection = laserDirection;

	const float step{ 10.f };

	Vector2f currentLaserDirection{ laserDirection };
	Vector2f currentFirstPoint{ firstPoint };
	Vector2f secondPoint{ firstPoint };
	utils::HitInfo hitInfo{};

	while (!utils::Raycast(m_BoundaryPoints, currentFirstPoint, secondPoint, hitInfo))
	{
		secondPoint += Vector2f{ step * currentLaserDirection.x, step * currentLaserDirection.y };

		Cell* pCurrentCell{ m_Grid->GetCellFromPosition(secondPoint, m_Center) };
		if (pCurrentCell != nullptr && pCurrentCell->HasMirror())
		{
			Vector2f mirrorFirstPoint{};
			Vector2f mirrorSecondPoint{};

			if (pCurrentCell->GetMirrorPoint(mirrorFirstPoint, mirrorSecondPoint))
			{
				if (utils::Raycast(std::vector<Vector2f>{mirrorFirstPoint, mirrorSecondPoint}, currentFirstPoint, secondPoint, hitInfo))
				{
					m_Laser->AddPoint(hitInfo.intersectPoint);
					currentLaserDirection = currentLaserDirection.Reflect(hitInfo.normal);

					currentFirstPoint = hitInfo.intersectPoint;
					secondPoint = currentFirstPoint;
				}
			}
		}
	}

	m_Laser->AddPoint(hitInfo.intersectPoint);


}


void Game::CreateMirrorPath(int numMirrors)
{
	int minimumDistance = 2;

	bool pathGenerated = false;
	int totalColumns = m_Grid->GetColumns();
	int totalRows = m_Grid->GetRows();

	while (!pathGenerated)
	{
		for (int row = 0; row < totalRows; ++row)
		{
			for (int column = 0; column < totalColumns; ++column)
			{
				if (m_Grid->GetCellFromIndex(column, row)->HasMirror())
				{
					m_Grid->RemoveMirror(column, row);
				}
			}
		}

		std::vector<bool> visitedCells(totalColumns * totalRows, false);

		int currentColumn = 0;
		int currentRow = 0;

		if (m_StartPosition == StartingPosition::BottomLeft)
		{
			currentColumn = 0;
			currentRow = 0;
		}
		else if (m_StartPosition == StartingPosition::BottomRight)
		{
			currentColumn = totalColumns - 1;
			currentRow = 0;
		}
		else if (m_StartPosition == StartingPosition::TopLeft)
		{
			currentColumn = 0;
			currentRow = totalRows - 1;
		}
		else if (m_StartPosition == StartingPosition::TopRight)
		{
			currentColumn = totalColumns - 1;
			currentRow = totalRows - 1;
		}

		int directionX = static_cast<int>(m_LaserDirection.x);
		int directionY = static_cast<int>(m_LaserDirection.y);

		bool isTrapped = false;

		for (int i = 0; i < numMirrors; ++i)
		{
			int maxDistance = 0;
			int checkColumn = currentColumn + directionX;
			int checkRow = currentRow + directionY;

			while (checkColumn >= 0 && checkColumn < totalColumns && checkRow >= 0 && checkRow < totalRows)
			{
				int cellIndex = checkRow * totalColumns + checkColumn;

				if (visitedCells[cellIndex])
				{
					break;
				}

				maxDistance = maxDistance + 1;
				checkColumn = checkColumn + directionX;
				checkRow = checkRow + directionY;
			}

			if (maxDistance < minimumDistance)
			{
				isTrapped = true;
				break;
			}

			int range = maxDistance - minimumDistance + 1;
			int stepsToTake = minimumDistance + (rand() % range);

			for (int step = 0; step < stepsToTake; ++step)
			{
				int currentIndex = currentRow * totalColumns + currentColumn;
				visitedCells[currentIndex] = true;

				currentColumn = currentColumn + directionX;
				currentRow = currentRow + directionY;
			}

			int finalIndex = currentRow * totalColumns + currentColumn;
			visitedCells[finalIndex] = true;

			m_Grid->AddMirror(currentColumn, currentRow);

			int turnLeftX = -directionY;
			int turnLeftY = directionX;
			int turnRightX = directionY;
			int turnRightY = -directionX;

			bool canTurnLeft = false;
			int leftColumn = currentColumn + turnLeftX;
			int leftRow = currentRow + turnLeftY;

			if (leftColumn >= 0 && leftColumn < totalColumns && leftRow >= 0 && leftRow < totalRows)
			{
				int leftIndex = leftRow * totalColumns + leftColumn;
				if (!visitedCells[leftIndex])
				{
					canTurnLeft = true;
				}
			}

			bool canTurnRight = false;
			int rightColumn = currentColumn + turnRightX;
			int rightRow = currentRow + turnRightY;

			if (rightColumn >= 0 && rightColumn < totalColumns && rightRow >= 0 && rightRow < totalRows)
			{
				int rightIndex = rightRow * totalColumns + rightColumn;
				if (!visitedCells[rightIndex])
				{
					canTurnRight = true;
				}
			}

			if (canTurnLeft && canTurnRight)
			{
				if (rand() % 2 == 0)
				{
					directionX = turnLeftX;
					directionY = turnLeftY;
				}
				else
				{
					directionX = turnRightX;
					directionY = turnRightY;
				}
			}
			else if (canTurnLeft)
			{
				directionX = turnLeftX;
				directionY = turnLeftY;
			}
			else if (canTurnRight)
			{
				directionX = turnRightX;
				directionY = turnRightY;
			}
			else
			{
				isTrapped = true;
				break;
			}
		}

		if (!isTrapped)
		{
			int finalCheckColumn = currentColumn + directionX;
			int finalCheckRow = currentRow + directionY;

			while (finalCheckColumn >= 0 && finalCheckColumn < totalColumns && finalCheckRow >= 0 && finalCheckRow < totalRows)
			{
				int cellIndex = finalCheckRow * totalColumns + finalCheckColumn;

				if (visitedCells[cellIndex])
				{
					isTrapped = true;
					break;
				}

				finalCheckColumn = finalCheckColumn + directionX;
				finalCheckRow = finalCheckRow + directionY;
			}
		}

		if (!isTrapped)
		{
			pathGenerated = true;
		}
	}
}
