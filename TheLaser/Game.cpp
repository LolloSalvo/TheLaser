#include "pch.h"
#include "Game.h"
#include "utils.h"
#include "LevelGenerator.h" 
#include "Texture.h"

#include <iostream>
#include <vector>
#include <algorithm>

Game::Game(const Window& window)
	: BaseGame{ window }
{
	Initialize();
}

Game::~Game()
{
	Cleanup();
	delete m_Grid;
	delete m_Laser;
}

void Game::Initialize()
{
	Color4f white{ 1.f, 1.f, 1.f, 1.f };
	if (m_pScoreLabel == nullptr) m_pScoreLabel = new Texture("SCORE:", "TypoDigit.otf", 20, white);
	if (m_pLevelLabel == nullptr) m_pLevelLabel = new Texture("LEVEL:", "TypoDigit.otf", 20, white);
	if (m_pTimeLabel == nullptr) m_pTimeLabel = new Texture("TIME", "TypoDigit.otf", 16, white);

	ResetGameStats();
	SetupNewLevel();
}

void Game::Cleanup()
{
	for (Digit* currentDigit : m_TimerDigits) delete currentDigit;
	m_TimerDigits.clear();

	for (Digit* currentDigit : m_ScoreDigits) delete currentDigit;
	m_ScoreDigits.clear();

	for (Digit* currentDigit : m_LevelCounterDigits) delete currentDigit;
	m_LevelCounterDigits.clear();

	delete m_pScoreLabel;
	delete m_pLevelLabel;
	delete m_pTimeLabel;
	delete m_pGameOverTitle;
	delete m_pGameOverScoreText;
	delete m_pGameOverLevelText;
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

			Color4f red{ 1.0f, 0.2f, 0.2f, 1.0f };
			Color4f white{ 1.0f, 1.0f, 1.0f, 1.0f };

			delete m_pGameOverTitle;
			m_pGameOverTitle = new Texture("GAME OVER", "TypoDigit.otf", 64, red);

			delete m_pGameOverScoreText;
			m_pGameOverScoreText = new Texture("FINAL SCORE: " + std::to_string(m_Score), "TypoDigit.otf", 32, white);

			delete m_pGameOverLevelText;
			m_pGameOverLevelText = new Texture("LEVELS CLEARED: " + std::to_string(m_LevelsSolved), "TypoDigit.otf", 32, white);
		}

		CalculateLaserPath(m_LaserStartPoint, m_LaserDirection);
	}
	else if (m_GameState == GameState::Victory)
	{
		m_VictoryTimer += elapsedSec;

		if (m_VictoryTimer >= m_MaxVictoryTime)
		{
			m_LevelsSolved++;
			SetupNewLevel();
		}
	}

	UpdateHUD();
}

void Game::Draw() const
{
	ClearBackground();

	if (m_GameState == GameState::GameOver)
	{
		DrawGameOver();
		return;
	}

	m_Grid->Draw();
	m_Laser->Draw();
	DrawLaserEmitter();

	m_pScoreLabel->Draw(Vector2f{ 20.0f, GetViewPort().height - 30.0f }, Rectf{});

	float startXLabel{ GetViewPort().width - m_pLevelLabel->GetWidth() - 20.0f };
	m_pLevelLabel->Draw(Vector2f{ startXLabel, GetViewPort().height - 30.0f }, Rectf{});

	// --- Timer background box ---
	const float pad{ 10.0f };
	Rectf timerBox
	{
		m_TimerBoxRect.left - pad,
		m_TimerBoxRect.bottom - pad,
		m_TimerBoxRect.width + pad * 2.0f,
		m_TimerBoxRect.height + pad * 2.0f
	};

	// Dark red fill
	utils::SetColor(Color4f{ 0.12f, 0.03f, 0.03f, 1.0f });
	utils::FillRect(timerBox);

	// Bright red border
	utils::SetColor(Color4f{ 0.85f, 0.12f, 0.12f, 1.0f });
	utils::DrawRect(timerBox, 3.f);

	// "TIME" label centered above the box (y-up: higher y = visually higher)
	float timeLabelX{ timerBox.left + timerBox.width / 2.0f - m_pTimeLabel->GetWidth() / 2.0f };
	float timeLabelY{ timerBox.bottom + timerBox.height + 6.0f };
	m_pTimeLabel->Draw(Vector2f{ timeLabelX, timeLabelY }, Rectf{});

	for (Digit* currentDigit : m_TimerDigits) currentDigit->Draw();
	for (Digit* currentDigit : m_ScoreDigits) currentDigit->Draw();
	for (Digit* currentDigit : m_LevelCounterDigits) currentDigit->Draw();
}

void Game::DrawGameOver() const
{
	float middleX{ GetViewPort().width / 2.0f };
	float middleY{ GetViewPort().height / 2.0f };

	float titleX{ middleX - (m_pGameOverTitle->GetWidth() / 2.0f) };
	m_pGameOverTitle->Draw(Vector2f{ titleX, middleY + 60.0f }, Rectf{});

	float scoreX{ middleX - (m_pGameOverScoreText->GetWidth() / 2.0f) };
	m_pGameOverScoreText->Draw(Vector2f{ scoreX, middleY - 10.0f }, Rectf{});

	float levelX{ middleX - (m_pGameOverLevelText->GetWidth() / 2.0f) };
	m_pGameOverLevelText->Draw(Vector2f{ levelX, middleY - 60.0f }, Rectf{});
}

void Game::ProcessKeyDownEvent(const SDL_KeyboardEvent& e)
{
	if (e.keysym.sym == SDLK_r)
	{
		Restart();
	}
}

void Game::ProcessKeyUpEvent(const SDL_KeyboardEvent& e) {}
void Game::ProcessMouseMotionEvent(const SDL_MouseMotionEvent& e) {}

void Game::ProcessMouseDownEvent(const SDL_MouseButtonEvent& e)
{
	if (m_GameState == GameState::Playing)
	{
		Vector2f mousePos{ static_cast<float>(e.x), static_cast<float>(e.y) };

		if (e.button == SDL_BUTTON_LEFT)
		{
			m_Grid->RotateMirrorAt(mousePos, m_Center, +1); // counter-clockwise
		}
		else if (e.button == SDL_BUTTON_RIGHT)
		{
			m_Grid->RotateMirrorAt(mousePos, m_Center, -1); // clockwise
		}
	}
}

void Game::ProcessMouseUpEvent(const SDL_MouseButtonEvent& e) {}

void Game::ClearBackground() const
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
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
		m_LaserStartPoint = Vector2f{ m_Grid->GetCellCenter(m_Grid->GetColumns() - 1, 0).x + m_Grid->GetCellSize(), m_Grid->GetCellCenter(m_Grid->GetColumns() - 1, 0).y };
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
					if (currentCell->GetMirrorType() == MirrorType::Receiver)
					{
						// Receiver is omnidirectional - always counts
						m_Laser->AddPoint(hitInformation.intersectPoint);
						hitReceiver = true;

						if (!m_Grid->IsAnyMirrorRotating())
						{
							m_GameState = GameState::Victory;
							m_GameTimer += m_TimerIncrement;

							int pointsEarned{ std::max(10, 100 - static_cast<int>(m_LevelTimeElapsed * 5.0f)) };
							m_Score += pointsEarned;
						}
						break;
					}

					Vector2f frontNormal{ currentCell->GetMirrorFrontNormal() };
					float dot{ currentLaserDirection.x * frontNormal.x
							 + currentLaserDirection.y * frontNormal.y };

					if (dot < 0.0f)
					{
						// Front hit -> reflect
						m_Laser->AddPoint(hitInformation.intersectPoint);
						currentLaserDirection = currentLaserDirection.Reflect(hitInformation.normal);
						currentFirstPoint.x = hitInformation.intersectPoint.x + (currentLaserDirection.x * 3.0f);
						currentFirstPoint.y = hitInformation.intersectPoint.y + (currentLaserDirection.y * 3.0f);
						secondPoint = currentFirstPoint;
					}
					else
					{
						// Back hit -> laser is absorbed, stops here
						m_Laser->AddPoint(hitInformation.intersectPoint);
						break;
					}
				}
			}
		}
	}

	if (!hitReceiver) m_Laser->AddPoint(hitInformation.intersectPoint);
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


void Game::Restart()
{
	ResetGameStats();
	SetupNewLevel();
}

void Game::ResetGameStats()
{
	m_GameTimer = 60.0f;

	m_Score = 0;

	m_LevelsSolved = 0;

	m_DisplayedSeconds = -1;

	m_DisplayedScore = -1;

	m_DisplayedLevels = -1;
}

void Game::SetupNewLevel()
{
	m_GameState = GameState::Playing;

	m_VictoryTimer = 0.0f;
	m_LevelTimeElapsed = 0.0f;

	ChooseRandomStartPosition();

	m_Laser->ClearPath();
	m_Laser->AddPoint(m_LaserStartPoint);

	int mirrorCount{ 5 + m_LevelsSolved };
	if (mirrorCount > 15) mirrorCount = 15;

	m_LevelGenerator.GeneratePath(m_Grid, m_StartPosition, m_LaserDirection, mirrorCount);
}

void Game::UpdateHUD()
{
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

	if (m_LevelsSolved != m_DisplayedLevels)
	{
		UpdateLevelCounterDigits(m_LevelsSolved + 1);
		m_DisplayedLevels = m_LevelsSolved;
	}
}


void Game::UpdateTimerDigits(int seconds)
{
	for (Digit* currentDigit : m_TimerDigits) delete currentDigit;
	m_TimerDigits.clear();

	std::string secondsString{ std::to_string(seconds) };
	const float renderedWidth{ 50.0f };
	const float digitHeight{ 60.0f };
	const float spacing{ 45.0f };

	float totalWidth{ (secondsString.length() - 1) * spacing + renderedWidth };

	float startX{ (GetViewPort().width / 2.0f) - (totalWidth / 2.0f) };
	float startY{ GetViewPort().height - digitHeight - 80.0f };

	m_TimerBoxRect = Rectf{ startX, startY, totalWidth, digitHeight };

	for (size_t index{ 0 }; index < secondsString.length(); ++index)
	{
		int numberValue{ secondsString[index] - '0' };
		Vector2f digitPosition{ startX + (index * 45.0f), startY };
		m_TimerDigits.push_back(new Digit{ numberValue, Digit::Mode::Bright, digitPosition });
	}
}

void Game::UpdateScoreDigits(int score)
{
	for (Digit* currentDigit : m_ScoreDigits) delete currentDigit;
	m_ScoreDigits.clear();

	std::string scoreString{ std::to_string(score) };
	float digitWidth{ 40.0f };
	float digitHeight{ 60.0f };
	float spacing{ digitWidth + 5.0f };

	float startX{ 20.0f };
	float startY{ GetViewPort().height - digitHeight - 40.0f };

	for (size_t index{ 0 }; index < scoreString.length(); ++index)
	{
		int numberValue{ scoreString[index] - '0' };
		Vector2f digitPosition{ startX + (index * spacing), startY };

		m_ScoreDigits.push_back(new Digit{ numberValue, Digit::Mode::Bright, digitPosition });
	}
}

void Game::UpdateLevelCounterDigits(int levels)
{
	for (Digit* currentDigit : m_LevelCounterDigits) delete currentDigit;
	m_LevelCounterDigits.clear();

	std::string levelString{ std::to_string(levels) };
	float digitWidth{ 40.0f };
	float digitHeight{ 60.0f };
	float spacing{ digitWidth + 5.0f };

	float totalWidth{ levelString.length() * spacing };
	float startX{ GetViewPort().width - totalWidth - 20.0f };
	float startY{ GetViewPort().height - digitHeight - 40.0f };

	for (size_t index{ 0 }; index < levelString.length(); ++index)
	{
		int numberValue{ levelString[index] - '0' };
		Vector2f digitPosition{ startX + (index * spacing), startY };

		m_LevelCounterDigits.push_back(new Digit{ numberValue, Digit::Mode::Bright, digitPosition });
	}
}