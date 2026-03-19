#include "pch.h"
#include "Grid.h"

#include "utils.h"

Grid::Grid(int rows, int cols, float cellSize)
	: m_Rows{ rows }
	, m_Columns{ cols }
	, m_CellSize{ cellSize }
{
}

int Grid::GetIndex(int row, int col) const
{
	return row * m_Columns + col;
}

int Grid::GetRows() const
{
	return m_Rows;
}

int Grid::GetColumns() const
{
	return m_Columns;
}

Vector2f Grid::GetCellCenter(int row, int col) const
{
	return Vector2f{ col * m_CellSize + m_CellSize / 2, row * m_CellSize + m_CellSize / 2 };
}

void Grid::Draw(const Vector2f& centerPos) const
{
	Color4f white{ 1.f, 1.f, 1.f, 1.f };

	utils::SetColor(white);

	for(int indexRow = 0; indexRow < m_Rows; ++indexRow)
	{
		for(int indexCol = 0; indexCol < m_Columns; ++indexCol)
		{
			float left{ centerPos.x - (m_Columns * m_CellSize) / 2 + indexCol * m_CellSize };
			float bottom{ centerPos.y - (m_Rows * m_CellSize) / 2 + indexRow * m_CellSize };
			utils::DrawRect(left, bottom, m_CellSize, m_CellSize);
		}
	}
}

