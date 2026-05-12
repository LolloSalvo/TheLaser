#include "pch.h"
#include "Cell.h"
#include "utils.h"

Cell::Cell(int index, const Vector2f& position, const float cellSize)
	: m_Index{ index }
	, m_Boundaries{ position.x, position.y, cellSize, cellSize }
	, m_pMirror{ nullptr }
{
}

void Cell::Draw() const
{
	Color4f white{ 1.f, 1.f, 1.f, 1.f };
	Color4f blue{ 0.f, 0.f, 1.f, 1.f };

	utils::SetColor(white);
	utils::DrawRect(m_Boundaries);

	utils::SetColor(blue);
	if (this->HasMirror())
	{
		m_pMirror->Draw();
	}
}

Rectf Cell::GetBoundaries() const
{
	return m_Boundaries;
}

void Cell::SetMirror(Mirror* mirror)
{
	// Always delete the old mirror before replacing it to prevent memory corruption
	if (m_pMirror != nullptr)
	{
		delete m_pMirror;
		m_pMirror = nullptr;
	}
	m_pMirror = mirror;
}

void Cell::DeleteMirror()
{
	delete m_pMirror;
	m_pMirror = nullptr;
}

bool Cell::HasMirror() const
{
	return m_pMirror != nullptr;
}

bool Cell::GetMirrorPoint(Vector2f& p1Out, Vector2f& p2Out)
{
	if (m_pMirror == nullptr) return false;
	p1Out = m_pMirror->GetFirstPoint();
	p2Out = m_pMirror->GetSecondPoint();
	return true;
}

void Cell::RotateMirror(int direction)
{
	if (m_pMirror != nullptr)
		m_pMirror->RotateMirror(direction);
}

MirrorType Cell::GetMirrorType() const
{
	if (m_pMirror == nullptr) return MirrorType::Reflector;
	return m_pMirror->GetType();
}

void Cell::Update(float elapsedSeconds)
{
	if (m_pMirror != nullptr)
		m_pMirror->Update(elapsedSeconds);
}

bool Cell::IsRotating() const
{
	if (m_pMirror == nullptr) return false;
	return m_pMirror->IsRotating();
}

Vector2f Cell::GetMirrorFrontNormal() const
{
	if (m_pMirror == nullptr) return Vector2f{ 0.f, 1.f };
	return m_pMirror->GetFrontNormal();
}