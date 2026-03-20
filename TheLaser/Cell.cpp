#include "pch.h"
#include "Cell.h"
#include "utils.h"`

Cell::Cell(const Vector2f& position, const float cellSize)
	: m_Boundaries{ position.x - cellSize / 2, position.y - cellSize / 2, cellSize, cellSize }
{	
}

void Cell::Draw(const Vector2f& offset) const
{
	utils::DrawRect(Rectf{
		m_Boundaries.left + offset.x,
		m_Boundaries.bottom + offset.y,
		m_Boundaries.width,
		m_Boundaries.height
	});
}


