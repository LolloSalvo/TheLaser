#include "pch.h"
#include "Grid.h"

#include "utils.h"

Grid::Grid(int rows, int cols, float cellSize)
	: m_Rows{ rows }
	, m_Columns{ cols }
	, m_CellSize{ cellSize }
{
	for (int indexRow{}; indexRow < m_Rows; ++indexRow)
	{
		for (int indexCol{}; indexCol < m_Columns; ++indexCol)
		{
			m_pCells.push_back(new Cell{ GetCellCenter(indexRow, indexCol), m_CellSize });
		}
	}
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

int Grid::GetRowFromIndex(int index) const
{
	return index / m_Columns;
}

int Grid::GetColFromIndex(int index) const
{
	return index % m_Columns;
}

Vector2f Grid::GetCellCenter(int row, int col) const
{
	return Vector2f{ col * m_CellSize + m_CellSize / 2, row * m_CellSize + m_CellSize / 2 };
}

void Grid::Draw(const Vector2f& centerPos) const
{
	float totalWidth = m_Columns * m_CellSize;
	float totalHeight = m_Rows * m_CellSize;

	const Vector2f offset{ centerPos.x - (totalWidth / 2.0f), centerPos.y - (totalHeight / 2.0f)};

	Color4f white{ 1.f, 1.f, 1.f, 1.f };

	utils::SetColor(white);

	
	for(Cell* cell : m_pCells)
	{
		cell->Draw(offset);
	}
}

int Grid::GetCellIndexFromPosition(const Vector2f& position, const Vector2f& centerPos) const
{

	float totalWidth = m_Columns * m_CellSize;
	float totalHeight = m_Rows * m_CellSize;

	const Vector2f offset{ centerPos.x - (totalWidth / 2.0f), centerPos.y - (totalHeight / 2.0f) };
	
	int col = static_cast<int>((position.x - offset.x) / m_CellSize);
	int row = static_cast<int>((position.y - offset.y) / m_CellSize);
	
	if (col < 0 || col >= m_Columns || row < 0 || row >= m_Rows)
	{
		return -1; // Out of bounds
	}
	return GetIndex(row, col);
	
}

