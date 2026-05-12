#include "pch.h"
#include "Game.h"
#include "utils.h"
#include "LevelGenerator.h"
#include "Texture.h"

#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <string>

// ---------------------------------------------------------------
//  CONSTRUCTOR / DESTRUCTOR
// ---------------------------------------------------------------
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
	delete m_Laser2;
}

// ---------------------------------------------------------------
//  INITIALIZE & CLEANUP
// ---------------------------------------------------------------
void Game::Initialize()
{
	Color4f white{ 1.f, 1.f, 1.f, 1.f };
	Color4f yellow{ 1.f, 0.9f, 0.1f, 1.f };
	Color4f cyan{ 0.f, 0.85f, 1.f, 1.f };

	if (m_pScoreLabel == nullptr) m_pScoreLabel = new Texture("SCORE", "TypoDigit.otf", 22, white);
	if (m_pLevelLabel == nullptr) m_pLevelLabel = new Texture("LEVEL", "TypoDigit.otf", 22, white);
	if (m_pTimeLabel == nullptr) m_pTimeLabel = new Texture("TIME", "TypoDigit.otf", 22, white);

	if (m_pTitleLabel == nullptr) m_pTitleLabel = new Texture("THE LASER", "TypoDigit.otf", 80, cyan);
	if (m_pStartLabel == nullptr) m_pStartLabel = new Texture("START", "TypoDigit.otf", 38, white);
	if (m_pControlsText == nullptr) m_pControlsText = new Texture("LEFT CLICK: rotate left    RIGHT CLICK: rotate right", "TypoDigit.otf", 22, white);
	if (m_pScoringText == nullptr) m_pScoringText = new Texture("Solve faster to score more points  (max 100 per level)", "TypoDigit.otf", 22, yellow);
	if (m_pRestartText == nullptr) m_pRestartText = new Texture("RESTART", "TypoDigit.otf", 38, white);

	LoadLeaderboard();
	BuildLeaderboardTextures();

	UpdateTimerDigits(60);
	UpdateScoreDigits(0);
	UpdateLevelCounterDigits(1);
}

void Game::Cleanup()
{
	for (Digit* d : m_TimerDigits)           delete d;
	for (Digit* d : m_ScoreDigits)           delete d;
	for (Digit* d : m_LevelCounterDigits)    delete d;
	for (Texture* t : m_LeaderboardTextures) delete t;

	m_TimerDigits.clear();
	m_ScoreDigits.clear();
	m_LevelCounterDigits.clear();
	m_LeaderboardTextures.clear();

	delete m_pScoreLabel;        m_pScoreLabel = nullptr;
	delete m_pLevelLabel;        m_pLevelLabel = nullptr;
	delete m_pTimeLabel;         m_pTimeLabel = nullptr;
	delete m_pTitleLabel;        m_pTitleLabel = nullptr;
	delete m_pStartLabel;        m_pStartLabel = nullptr;
	delete m_pControlsText;      m_pControlsText = nullptr;
	delete m_pScoringText;       m_pScoringText = nullptr;
	delete m_pGameOverTitle;     m_pGameOverTitle = nullptr;
	delete m_pGameOverScoreText; m_pGameOverScoreText = nullptr;
	delete m_pGameOverLevelText; m_pGameOverLevelText = nullptr;
	delete m_pRestartText;       m_pRestartText = nullptr;
	delete m_pLeaderboardTitle;  m_pLeaderboardTitle = nullptr;
}

// ---------------------------------------------------------------
//  UPDATE
// ---------------------------------------------------------------
void Game::Update(float elapsedSec)
{
	m_Grid->Update(elapsedSec);

	if (m_GameState == GameState::StartScreen) return;

	if (m_GameState == GameState::Playing)
	{
		m_GameTimer -= elapsedSec;
		m_LevelTimeElapsed += elapsedSec;

		// Red border pulse when <= 15s
		if (m_GameTimer <= 15.0f)
		{
			float speed{ 2.0f + (15.0f - m_GameTimer) * 0.4f };
			m_RedBorderPulse += elapsedSec * speed;
			if (m_RedBorderPulse > 6.2831853f) m_RedBorderPulse -= 6.2831853f;
		}
		else
		{
			m_RedBorderPulse = 0.0f;
		}

		if (m_GameTimer <= 0.0f)
		{
			m_GameTimer = 0.0f;
			m_GameState = GameState::GameOver;

			m_Leaderboard.push_back(m_Score);
			std::sort(m_Leaderboard.begin(), m_Leaderboard.end(), std::greater<int>());
			if (m_Leaderboard.size() > 3) m_Leaderboard.resize(3);
			SaveLeaderboard();
			BuildLeaderboardTextures();

			Color4f red{ 1.0f, 0.2f, 0.2f, 1.0f };
			Color4f white{ 1.0f, 1.0f, 1.0f, 1.0f };

			delete m_pGameOverTitle;
			m_pGameOverTitle = new Texture("GAME OVER", "TypoDigit.otf", 72, red);

			delete m_pGameOverScoreText;
			m_pGameOverScoreText = new Texture("SCORE: " + std::to_string(m_Score), "TypoDigit.otf", 32, white);

			delete m_pGameOverLevelText;
			m_pGameOverLevelText = new Texture("LEVELS CLEARED: " + std::to_string(m_LevelsSolved), "TypoDigit.otf", 28, white);
		}

		// Calculate laser paths and check victory
		bool laser1Hit{ CalculateLaserPath(m_Laser,  m_LaserStartPoint,  m_LaserDirection,  MirrorType::Receiver) };
		bool laser2Hit{ true };

		if (m_DualLaserMode)
		{
			laser2Hit = CalculateLaserPath(m_Laser2, m_Laser2StartPoint, m_Laser2Direction, MirrorType::ReceiverBlue);
		}

		// Victory: both lasers must hit their receiver, and no mirror is still spinning
		if (laser1Hit && laser2Hit && !m_Grid->IsAnyMirrorRotating())
		{
			m_GameState = GameState::Victory;
			m_GameTimer += m_TimerIncrement;

			int pointsEarned{ std::max(10, 100 - static_cast<int>(m_LevelTimeElapsed * 5.0f)) };
			m_Score += pointsEarned;
		}
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

// ---------------------------------------------------------------
//  DRAW
// ---------------------------------------------------------------
void Game::Draw() const
{
	ClearBackground();

	if (m_GameState == GameState::StartScreen)
	{
		DrawStartScreen();
		return;
	}

	if (m_GameState == GameState::GameOver)
	{
		DrawGameOver();
		return;
	}

	// Playing / Victory
	m_Grid->Draw();

	// Laser 1: red
	m_Laser->Draw(Color4f{ 1.f, 0.1f, 0.1f, 1.f });
	DrawLaserEmitter(m_LaserStartPoint, m_LaserDirection, Color4f{ 0.9f, 0.1f, 0.1f, 1.f });

	// Laser 2: blue (only in dual laser mode)
	if (m_DualLaserMode)
	{
		m_Laser2->Draw(Color4f{ 0.15f, 0.45f, 1.f, 1.f });
		DrawLaserEmitter(m_Laser2StartPoint, m_Laser2Direction, Color4f{ 0.1f, 0.35f, 0.9f, 1.f });
	}

	// Red border drawn before HUD so HUD stays on top
	if (m_GameTimer <= 15.0f) DrawRedBorder();

	DrawHUD();
}

// ---------------------------------------------------------------
//  BUTTON RECT HELPERS
// ---------------------------------------------------------------
Rectf Game::GetStartBtnRect() const
{
	float midX{ GetViewPort().width / 2.0f };
	float midY{ GetViewPort().height / 2.0f };
	float btnW{ 240.0f };
	float btnH{ 72.0f };
	return Rectf{ midX - btnW / 2.0f, midY - btnH / 2.0f - 60.0f, btnW, btnH };
}

Rectf Game::GetRestartBtnRect() const
{
	float midX{ GetViewPort().width / 2.0f };
	float h{ GetViewPort().height };
	float btnW{ 260.0f };
	float btnH{ 72.0f };
	return Rectf{ midX - btnW / 2.0f, h * 0.10f, btnW, btnH };
}

// ---------------------------------------------------------------
//  DRAW: START SCREEN
// ---------------------------------------------------------------
void Game::DrawStartScreen() const
{
	float w{ GetViewPort().width };
	float h{ GetViewPort().height };
	float midX{ w / 2.0f };

	m_Grid->Draw();
	utils::SetColor(Color4f{ 0.0f, 0.0f, 0.0f, 0.75f });
	utils::FillRect(Rectf{ 0.f, 0.f, w, h });

	DrawHUD();

	float titleX{ midX - m_pTitleLabel->GetWidth() / 2.0f };
	m_pTitleLabel->Draw(Vector2f{ titleX, h * 0.70f }, Rectf{});

	utils::SetColor(Color4f{ 0.0f, 0.85f, 1.0f, 0.6f });
	utils::DrawLine(Vector2f{ midX - 220.f, h * 0.67f }, Vector2f{ midX + 220.f, h * 0.67f }, 1.5f);

	float ctrlX{ midX - m_pControlsText->GetWidth() / 2.0f };
	m_pControlsText->Draw(Vector2f{ ctrlX, h * 0.55f }, Rectf{});

	float scorX{ midX - m_pScoringText->GetWidth() / 2.0f };
	m_pScoringText->Draw(Vector2f{ scorX, h * 0.48f }, Rectf{});

	Rectf btn{ GetStartBtnRect() };
	utils::SetColor(Color4f{ 0.08f, 0.55f, 0.08f, 1.0f });
	utils::FillRect(btn);
	utils::SetColor(Color4f{ 0.2f, 1.0f, 0.2f, 1.0f });
	utils::DrawRect(btn, 3.f);

	float startTxtX{ midX - m_pStartLabel->GetWidth() / 2.0f };
	float startTxtY{ btn.bottom + btn.height / 2.0f - m_pStartLabel->GetHeight() / 2.0f };
	m_pStartLabel->Draw(Vector2f{ startTxtX, startTxtY }, Rectf{});
}

// ---------------------------------------------------------------
//  DRAW: HUD
// ---------------------------------------------------------------
void Game::DrawHUD() const
{
	float w{ GetViewPort().width };
	float h{ GetViewPort().height };
	float lblH{ m_pScoreLabel->GetHeight() };
	const float margin{ 12.0f };

	// TIMER (center)
	const float pad{ 10.0f };
	Rectf timerBox
	{
		m_TimerBoxRect.left - pad,
		m_TimerBoxRect.bottom - pad,
		m_TimerBoxRect.width + pad * 2.0f,
		m_TimerBoxRect.height + pad * 2.0f
	};

	utils::SetColor(Color4f{ 0.12f, 0.03f, 0.03f, 1.0f });
	utils::FillRect(timerBox);
	utils::SetColor(Color4f{ 0.85f, 0.12f, 0.12f, 1.0f });
	utils::DrawRect(timerBox, 2.f);

	float timeLabelX{ timerBox.left + timerBox.width / 2.0f - m_pTimeLabel->GetWidth() / 2.0f };
	float timeLabelY{ timerBox.bottom + timerBox.height + 4.0f };
	m_pTimeLabel->Draw(Vector2f{ timeLabelX, timeLabelY }, Rectf{});
	for (Digit* d : m_TimerDigits) d->Draw();

	// SCORE (top left)
	float scoreLabelY{ h - lblH - margin };
	m_pScoreLabel->Draw(Vector2f{ 20.0f, scoreLabelY }, Rectf{});
	for (Digit* d : m_ScoreDigits) d->Draw();

	// LEVEL (top right)
	float lvlLabelX{ w - m_pLevelLabel->GetWidth() - 20.0f };
	float lvlLabelY{ h - lblH - margin };
	m_pLevelLabel->Draw(Vector2f{ lvlLabelX, lvlLabelY }, Rectf{});
	for (Digit* d : m_LevelCounterDigits) d->Draw();
}

// ---------------------------------------------------------------
//  DRAW: RED BORDER
// ---------------------------------------------------------------
void Game::DrawRedBorder() const
{
	float w{ GetViewPort().width };
	float h{ GetViewPort().height };
	float thickness{ 12.0f };
	float alpha{ 0.45f + 0.55f * sinf(m_RedBorderPulse) };

	utils::SetColor(Color4f{ 1.0f, 0.05f, 0.05f, alpha });
	utils::FillRect(Rectf{ 0.f,           h - thickness, w,         thickness });
	utils::FillRect(Rectf{ 0.f,           0.f,           w,         thickness });
	utils::FillRect(Rectf{ 0.f,           0.f,           thickness, h });
	utils::FillRect(Rectf{ w - thickness, 0.f,           thickness, h });
}

// ---------------------------------------------------------------
//  DRAW: GAME OVER
// ---------------------------------------------------------------
void Game::DrawGameOver() const
{
	float w{ GetViewPort().width };
	float h{ GetViewPort().height };
	float midX{ w / 2.0f };

	if (m_pGameOverTitle != nullptr)
		m_pGameOverTitle->Draw(Vector2f{ midX - m_pGameOverTitle->GetWidth() / 2.0f, h * 0.80f }, Rectf{});

	if (m_pGameOverScoreText != nullptr)
		m_pGameOverScoreText->Draw(Vector2f{ midX - m_pGameOverScoreText->GetWidth() / 2.0f, h * 0.66f }, Rectf{});

	if (m_pGameOverLevelText != nullptr)
		m_pGameOverLevelText->Draw(Vector2f{ midX - m_pGameOverLevelText->GetWidth() / 2.0f, h * 0.59f }, Rectf{});

	utils::SetColor(Color4f{ 0.6f, 0.6f, 0.6f, 0.7f });
	utils::DrawLine(Vector2f{ midX - 180.f, h * 0.535f }, Vector2f{ midX + 180.f, h * 0.535f }, 1.5f);

	if (m_pLeaderboardTitle != nullptr)
		m_pLeaderboardTitle->Draw(Vector2f{ midX - m_pLeaderboardTitle->GetWidth() / 2.0f, h * 0.50f }, Rectf{});

	float entryY{ h * 0.44f };
	for (Texture* entry : m_LeaderboardTextures)
	{
		entry->Draw(Vector2f{ midX - entry->GetWidth() / 2.0f, entryY }, Rectf{});
		entryY -= 38.0f;
	}

	Rectf btn{ GetRestartBtnRect() };
	utils::SetColor(Color4f{ 0.08f, 0.55f, 0.08f, 1.0f });
	utils::FillRect(btn);
	utils::SetColor(Color4f{ 0.2f, 1.0f, 0.2f, 1.0f });
	utils::DrawRect(btn, 3.f);

	if (m_pRestartText != nullptr)
	{
		float txtX{ midX - m_pRestartText->GetWidth() / 2.0f };
		float txtY{ btn.bottom + btn.height / 2.0f - m_pRestartText->GetHeight() / 2.0f };
		m_pRestartText->Draw(Vector2f{ txtX, txtY }, Rectf{});
	}
}

// ---------------------------------------------------------------
//  INPUT
// ---------------------------------------------------------------
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
	Vector2f mousePos{ static_cast<float>(e.x), static_cast<float>(e.y) };

	if (m_GameState == GameState::StartScreen)
	{
		if (e.button == SDL_BUTTON_LEFT)
		{
			Rectf btn{ GetStartBtnRect() };
			bool inX{ mousePos.x >= btn.left && mousePos.x <= btn.left + btn.width };
			bool inY{ mousePos.y >= btn.bottom && mousePos.y <= btn.bottom + btn.height };
			if (inX && inY) StartGame();
		}
		return;
	}

	if (m_GameState == GameState::GameOver)
	{
		if (e.button == SDL_BUTTON_LEFT)
		{
			Rectf btn{ GetRestartBtnRect() };
			bool inX{ mousePos.x >= btn.left && mousePos.x <= btn.left + btn.width };
			bool inY{ mousePos.y >= btn.bottom && mousePos.y <= btn.bottom + btn.height };
			if (inX && inY) Restart();
		}
		return;
	}

	if (m_GameState == GameState::Playing)
	{
		if (e.button == SDL_BUTTON_LEFT)
			m_Grid->RotateMirrorAt(mousePos, m_Center, +1);
		else if (e.button == SDL_BUTTON_RIGHT)
			m_Grid->RotateMirrorAt(mousePos, m_Center, -1);
	}
}

void Game::ProcessMouseUpEvent(const SDL_MouseButtonEvent& e) {}

void Game::ClearBackground() const
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

// ---------------------------------------------------------------
//  GAME FLOW
// ---------------------------------------------------------------
void Game::StartGame()
{
	ResetGameStats();
	SetupNewLevel();
}

void Game::Restart()
{
	m_GameState = GameState::StartScreen;
	ResetGameStats();
	UpdateTimerDigits(60);
	UpdateScoreDigits(0);
	UpdateLevelCounterDigits(1);
}

void Game::ResetGameStats()
{
	m_GameTimer = 60.0f;
	m_Score = 0;
	m_LevelsSolved = 0;
	m_RedBorderPulse = 0.0f;
	m_DualLaserMode = false;
	m_DisplayedSeconds = -1;
	m_DisplayedScore = -1;
	m_DisplayedLevels = -1;
}

void Game::SetupNewLevel()
{
	m_GameState = GameState::Playing;
	m_VictoryTimer = 0.0f;
	m_LevelTimeElapsed = 0.0f;

	m_DualLaserMode = (m_LevelsSolved >= 4);

	ChooseRandomStartPosition();

	m_Laser->ClearPath();
	m_Laser->AddPoint(m_LaserStartPoint);

	int mirrorCount{};
	int mirrors2{};

	if (m_DualLaserMode)
	{
		// From level 5: both start at 4, increase by 1 every level, cap at 10
		int shared{ 4 + (m_LevelsSolved - 4) };
		if (shared > 10) shared = 10;
		mirrorCount = shared;
		mirrors2 = shared;
	}
	else
	{
		// Levels 1-4: start at 4, increase by 1 each level
		mirrorCount = 4 + m_LevelsSolved;
		mirrors2 = 0;
	}

	// Generate path 1 (no decoys yet)
	m_LevelGenerator.GeneratePath(m_Grid, m_StartPosition, m_LaserDirection, mirrorCount);

	if (m_DualLaserMode)
	{
		// After path 1 is known, pick the column/row for laser 2 that
		// is least crossed by path 1 so GenerateSecondPath has room.
		int dirX{ static_cast<int>(m_Laser2Direction.x) };
		int dirY{ static_cast<int>(m_Laser2Direction.y) };

		if (dirY != 0) // laser 2 enters vertically -> pick best column
		{
			m_Laser2StartCol = FindBestLaser2Col(m_Laser2StartRow, dirY);
			float cellSize{ m_Grid->GetCellSize() };
			float offset{ (dirY < 0) ? cellSize : -cellSize };
			m_Laser2StartPoint = Vector2f{
				m_Grid->GetCellCenter(m_Laser2StartCol, m_Laser2StartRow).x,
				m_Grid->GetCellCenter(m_Laser2StartCol, m_Laser2StartRow).y + offset
			};
		}

		m_Laser2->ClearPath();
		m_Laser2->AddPoint(m_Laser2StartPoint);

		m_LevelGenerator.GenerateSecondPath(m_Grid, m_Laser2StartCol, m_Laser2StartRow, dirX, dirY, mirrors2);
	}

	// Decoys placed LAST so they avoid both path 1 and path 2 beam cells
	m_LevelGenerator.PlaceDecoys(m_Grid, 7);
}

// ---------------------------------------------------------------
//  START POSITIONS
// ---------------------------------------------------------------
void Game::ChooseRandomStartPosition()
{
	int choose{ rand() % 4 };
	m_StartPosition = static_cast<StartingPosition>(choose);

	float cellSize{ m_Grid->GetCellSize() };
	int cols{ m_Grid->GetColumns() };
	int rows{ m_Grid->GetRows() };

	switch (m_StartPosition)
	{
	case StartingPosition::BottomLeft:
		m_LaserStartPoint = Vector2f{ m_Grid->GetCellCenter(0, 0).x - cellSize, m_Grid->GetCellCenter(0, 0).y };
		m_LaserDirection = Vector2f{ 1.f, 0.f };
		// Laser 2: enters from top center going down
		m_Laser2StartCol = cols / 2;
		m_Laser2StartRow = rows - 1;
		m_Laser2StartPoint = Vector2f{ m_Grid->GetCellCenter(m_Laser2StartCol, m_Laser2StartRow).x, m_Grid->GetCellCenter(m_Laser2StartCol, m_Laser2StartRow).y + cellSize };
		m_Laser2Direction = Vector2f{ 0.f, -1.f };
		break;
	case StartingPosition::BottomRight:
		m_LaserStartPoint = Vector2f{ m_Grid->GetCellCenter(cols - 1, 0).x + cellSize, m_Grid->GetCellCenter(cols - 1, 0).y };
		m_LaserDirection = Vector2f{ -1.f, 0.f };
		// Laser 2: enters from top center going down
		m_Laser2StartCol = cols / 2;
		m_Laser2StartRow = rows - 1;
		m_Laser2StartPoint = Vector2f{ m_Grid->GetCellCenter(m_Laser2StartCol, m_Laser2StartRow).x, m_Grid->GetCellCenter(m_Laser2StartCol, m_Laser2StartRow).y + cellSize };
		m_Laser2Direction = Vector2f{ 0.f, -1.f };
		break;
	case StartingPosition::TopLeft:
		m_LaserStartPoint = Vector2f{ m_Grid->GetCellCenter(0, rows - 1).x - cellSize, m_Grid->GetCellCenter(0, rows - 1).y };
		m_LaserDirection = Vector2f{ 1.f, 0.f };
		// Laser 2: enters from bottom center going up
		m_Laser2StartCol = cols / 2;
		m_Laser2StartRow = 0;
		m_Laser2StartPoint = Vector2f{ m_Grid->GetCellCenter(m_Laser2StartCol, m_Laser2StartRow).x, m_Grid->GetCellCenter(m_Laser2StartCol, m_Laser2StartRow).y - cellSize };
		m_Laser2Direction = Vector2f{ 0.f, 1.f };
		break;
	case StartingPosition::TopRight:
		m_LaserStartPoint = Vector2f{ m_Grid->GetCellCenter(cols - 1, rows - 1).x + cellSize, m_Grid->GetCellCenter(cols - 1, rows - 1).y };
		m_LaserDirection = Vector2f{ -1.f, 0.f };
		// Laser 2: enters from bottom center going up
		m_Laser2StartCol = cols / 2;
		m_Laser2StartRow = 0;
		m_Laser2StartPoint = Vector2f{ m_Grid->GetCellCenter(m_Laser2StartCol, m_Laser2StartRow).x, m_Grid->GetCellCenter(m_Laser2StartCol, m_Laser2StartRow).y - cellSize };
		m_Laser2Direction = Vector2f{ 0.f, 1.f };
		break;
	}
}

// Scans all columns in the laser2 entry row and returns the one whose entire
// column has the fewest path-1 beam cells - giving GenerateSecondPath the most room.
int Game::FindBestLaser2Col(int entryRow, int dirY) const
{
	const std::vector<bool>& path1 = m_LevelGenerator.GetPath1Cells();
	int cols = m_Grid->GetColumns();
	int rows = m_Grid->GetRows();
	int totalCells = cols * rows;

	if (static_cast<int>(path1.size()) != totalCells)
		return cols / 2; // fallback if path1 not ready

	int bestCol = cols / 2;
	int bestScore = INT_MAX;

	for (int col = 0; col < cols; ++col)
	{
		int score = 0;
		// Count path-1 cells in this entire column
		for (int row = 0; row < rows; ++row)
			if (path1[row * cols + col]) ++score;

		// Prefer the column whose entry cell is not on path 1
		if (path1[entryRow * cols + col]) score += 100;

		if (score < bestScore)
		{
			bestScore = score;
			bestCol = col;
		}
	}
	return bestCol;
}
bool Game::CalculateLaserPath(Laser* laser, const Vector2f& firstPoint, const Vector2f& laserDirection, MirrorType expectedReceiver)
{
	laser->ClearPath();
	laser->AddPoint(firstPoint);

	const float step{ 10.0f };
	const int   maxBounces{ 64 }; // safety cap - prevents infinite loop crash
	int         bounces{ 0 };

	Vector2f currentLaserDirection{ laserDirection };
	Vector2f currentFirstPoint{ firstPoint };
	Vector2f secondPoint{ firstPoint };
	utils::HitInfo hitInformation{};
	bool hitReceiver{ false };

	while (!utils::Raycast(m_BoundaryPoints, currentFirstPoint, secondPoint, hitInformation)
		&& bounces < maxBounces)
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
					MirrorType hitType{ currentCell->GetMirrorType() };

					// Any receiver stops the laser; count as win only if it's the right one
					if (hitType == MirrorType::Receiver || hitType == MirrorType::ReceiverBlue)
					{
						laser->AddPoint(hitInformation.intersectPoint);
						hitReceiver = (hitType == expectedReceiver);
						break;
					}

					// One-sided reflection check
					Vector2f frontNormal{ currentCell->GetMirrorFrontNormal() };
					float dot{ currentLaserDirection.x * frontNormal.x
							 + currentLaserDirection.y * frontNormal.y };

					if (dot < 0.0f)
					{
						laser->AddPoint(hitInformation.intersectPoint);
						currentLaserDirection = currentLaserDirection.Reflect(hitInformation.normal);
						currentFirstPoint.x = hitInformation.intersectPoint.x + (currentLaserDirection.x * 3.0f);
						currentFirstPoint.y = hitInformation.intersectPoint.y + (currentLaserDirection.y * 3.0f);
						secondPoint = currentFirstPoint;
						++bounces;
					}
					else
					{
						laser->AddPoint(hitInformation.intersectPoint);
						break;
					}
				}
			}
		}
	}

	if (!hitReceiver) laser->AddPoint(hitInformation.intersectPoint);

	return hitReceiver;
}

// ---------------------------------------------------------------
//  LASER EMITTER (colored triangle)
// ---------------------------------------------------------------
void Game::DrawLaserEmitter(const Vector2f& startPoint, const Vector2f& direction, const Color4f& color) const
{
	float height{ 32.0f };
	float baseHalfWidth{ 12.0f };

	Vector2f perp{ -direction.y, direction.x };
	Vector2f apex{ startPoint };
	Vector2f baseCenter{ apex.x - direction.x * height, apex.y - direction.y * height };

	Vector2f v2{ baseCenter.x + perp.x * baseHalfWidth, baseCenter.y + perp.y * baseHalfWidth };
	Vector2f v3{ baseCenter.x - perp.x * baseHalfWidth, baseCenter.y - perp.y * baseHalfWidth };

	std::vector<Vector2f> tri{ apex, v2, v3 };

	utils::SetColor(color);
	utils::FillPolygon(tri);
	utils::SetColor(Color4f{ 1.0f, 1.0f, 1.0f, 1.0f });
	utils::DrawPolygon(tri, true);
}

// ---------------------------------------------------------------
//  HUD UPDATE
// ---------------------------------------------------------------
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
	for (Digit* d : m_TimerDigits) delete d;
	m_TimerDigits.clear();

	std::string s{ std::to_string(seconds) };
	const float renderedWidth{ 50.0f };
	const float digitHeight{ 60.0f };
	const float spacing{ 45.0f };

	float totalWidth{ (static_cast<float>(s.length()) - 1.0f) * spacing + renderedWidth };
	float startX{ (GetViewPort().width / 2.0f) - (totalWidth / 2.0f) };
	float startY{ GetViewPort().height - digitHeight - 80.0f };

	m_TimerBoxRect = Rectf{ startX, startY, totalWidth, digitHeight };

	for (size_t i{ 0 }; i < s.length(); ++i)
	{
		int v{ s[i] - '0' };
		m_TimerDigits.push_back(new Digit{ v, Digit::Mode::Bright, Vector2f{ startX + static_cast<float>(i) * spacing, startY } });
	}
}

void Game::UpdateScoreDigits(int score)
{
	for (Digit* d : m_ScoreDigits) delete d;
	m_ScoreDigits.clear();

	std::string s{ std::to_string(score) };
	const float digitHeight{ 60.0f };
	const float spacing{ 45.0f };

	float startX{ 20.0f };
	float labelH{ m_pScoreLabel ? m_pScoreLabel->GetHeight() : 26.0f };
	float startY{ GetViewPort().height - labelH - digitHeight - 18.0f };

	for (size_t i{ 0 }; i < s.length(); ++i)
	{
		int v{ s[i] - '0' };
		m_ScoreDigits.push_back(new Digit{ v, Digit::Mode::Bright, Vector2f{ startX + static_cast<float>(i) * spacing, startY } });
	}
}

void Game::UpdateLevelCounterDigits(int levels)
{
	for (Digit* d : m_LevelCounterDigits) delete d;
	m_LevelCounterDigits.clear();

	std::string s{ std::to_string(levels) };
	const float renderedWidth{ 50.0f };
	const float digitHeight{ 60.0f };
	const float spacing{ 45.0f };

	float totalWidth{ (static_cast<float>(s.length()) - 1.0f) * spacing + renderedWidth };
	float startX{ GetViewPort().width - totalWidth - 20.0f };
	float labelH{ m_pLevelLabel ? m_pLevelLabel->GetHeight() : 26.0f };
	float startY{ GetViewPort().height - labelH - digitHeight - 18.0f };

	for (size_t i{ 0 }; i < s.length(); ++i)
	{
		int v{ s[i] - '0' };
		m_LevelCounterDigits.push_back(new Digit{ v, Digit::Mode::Bright, Vector2f{ startX + static_cast<float>(i) * spacing, startY } });
	}
}

// ---------------------------------------------------------------
//  LEADERBOARD
// ---------------------------------------------------------------
void Game::LoadLeaderboard()
{
	m_Leaderboard.clear();
	std::ifstream file("leaderboard.txt");
	if (!file.is_open()) return;

	int score{};
	while (file >> score && m_Leaderboard.size() < 3)
		m_Leaderboard.push_back(score);

	std::sort(m_Leaderboard.begin(), m_Leaderboard.end(), std::greater<int>());
}

void Game::SaveLeaderboard() const
{
	std::ofstream file("leaderboard.txt");
	if (!file.is_open()) return;
	for (int score : m_Leaderboard)
		file << score << "\n";
}

void Game::BuildLeaderboardTextures()
{
	for (Texture* t : m_LeaderboardTextures) delete t;
	m_LeaderboardTextures.clear();

	delete m_pLeaderboardTitle;
	m_pLeaderboardTitle = new Texture("HIGH SCORES", "TypoDigit.otf", 28, Color4f{ 1.0f, 0.85f, 0.1f, 1.0f });

	Color4f gold{ 1.0f,  0.84f, 0.0f,  1.0f };
	Color4f silver{ 0.75f, 0.75f, 0.75f, 1.0f };
	Color4f bronze{ 0.80f, 0.50f, 0.20f, 1.0f };
	Color4f colors[3]{ gold, silver, bronze };
	std::string medals[3]{ "1st", "2nd", "3rd" };

	for (int i{ 0 }; i < static_cast<int>(m_Leaderboard.size()) && i < 3; ++i)
	{
		std::string text{ medals[i] + "   " + std::to_string(m_Leaderboard[i]) };
		m_LeaderboardTextures.push_back(new Texture(text, "TypoDigit.otf", 26, colors[i]));
	}
}