#include "pch.h"
#include "Cell.h"
#include "utils.h"

Cell::Cell(int index, const Vector2f& position, const float cellSize)
	: m_Index{ index }
	, m_Boundaries{ position.x, position.y, cellSize, cellSize }
	, m_pMirror{nullptr}
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
	m_pMirror = mirror;
}

bool Cell::HasMirror() const
{
	if (m_pMirror != nullptr)
	{
		return true;
	}
	return false;
}

bool Cell::GetMirrorPoint(Vector2f& p1Out, Vector2f& p2Out)
{
	if (this->HasMirror())
	{
		p1Out = m_pMirror->GetFirstPoint();
		p2Out = m_pMirror->GetSecondPoint();

		return true;
	}

	return false;
}


