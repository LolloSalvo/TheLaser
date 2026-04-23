#include "pch.h"
#include "Grid.h"

#include "utils.h"

Grid::Grid(int cols, int rows, float cellSize, const Vector2f& center)
	: m_Rows{ rows }
	, m_Columns{ cols }
	, m_CellSize{ cellSize }
{
	float totalWidth{ m_Columns * m_CellSize };
	float totalHeight{ m_Rows * m_CellSize };

	float startX{ center.x - (totalWidth / 2) };
	float startY{ center.y - (totalHeight / 2) };

	//const Vector2f offset{ center.x - (totalWidth / 2.0f), center.y - (totalHeight / 2.0f) };

	for (int indexRow{}; indexRow < m_Rows; ++indexRow)
	{
		for (int indexCol{}; indexCol < m_Columns; ++indexCol)
		{
			Vector2f cellPosition{ startX + (indexCol * m_CellSize), startY + (indexRow * m_CellSize) };

			m_pCells.push_back(new Cell{GetIndex(indexCol, indexRow), cellPosition, cellSize });
		}
	}
}

int Grid::GetIndex(int col, int row) const
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

Vector2f Grid::GetCellCenter(int col, int row) const
{
	return Vector2f{ 
		m_pCells[GetIndex(col, row)]->GetBoundaries().left + m_CellSize / 2,
		m_pCells[GetIndex(col, row)]->GetBoundaries().bottom + m_CellSize / 2
	};
	//return Vector2f{ col * m_CellSize + m_CellSize / 2, row * m_CellSize + m_CellSize / 2 };
}

float Grid::GetCellSize() const
{
	return m_CellSize;
}

void Grid::Draw() const
{
	for(Cell* cell : m_pCells)
	{
		cell->Draw();
	}
}

int Grid::GetCellIndexFromPosition(const Vector2f& position, const Vector2f& centerPos) const
{
	float totalWidth{ m_Columns * m_CellSize };
	float totalHeight{ m_Rows * m_CellSize };

	float startX{ centerPos.x - (totalWidth / 2) };
	float startY{ centerPos.y - (totalHeight / 2) };
	
	int col = static_cast<int>((position.x - startX) / m_CellSize);
	int row = static_cast<int>((position.y - startY) / m_CellSize);

	if (col < 0 || col >= m_Columns || row < 0 || row >= m_Rows)
	{
		return -1; // Out of bounds
	}
	return GetIndex(col, row);
	
}

Cell* Grid::GetCellFromPosition(const Vector2f& position, const Vector2f& centerPos) const
{
	float totalWidth{ m_Columns * m_CellSize };
	float totalHeight{ m_Rows * m_CellSize };

	float startX{ centerPos.x - (totalWidth / 2) };
	float startY{ centerPos.y - (totalHeight / 2) };

	int col = static_cast<int>((position.x - startX) / m_CellSize);
	int row = static_cast<int>((position.y - startY) / m_CellSize);

	if (col < 0 || col >= m_Columns || row < 0 || row >= m_Rows)
	{
		return nullptr; // Out of bounds
	}
	return m_pCells[GetIndex(col, row)];
}

Cell* Grid::GetCellFromIndex(int index)
{
	return m_pCells[index];
}

Cell* Grid::GetCellFromIndex(int col, int row)
{
	return m_pCells[GetIndex(col, row)];
}

bool Grid::IsAnyMirrorRotating() const
{
	for (Cell* cell : m_pCells)
	{
		if (cell->HasMirror() && cell->IsRotating())
		{
			return true;
		}
	}
	return false;
}


void Grid::AddMirror(int col, int row)
{
	int cellIndex = GetIndex(col, row);
	Rectf cellBoundaries = m_pCells[cellIndex]->GetBoundaries();
	m_pCells[cellIndex]->SetMirror(new Mirror{ cellBoundaries, MirrorType::Reflector });
}

void Grid::AddReceiver(int col, int row)
{
	int cellIndex = GetIndex(col, row);
	Rectf cellBoundaries = m_pCells[cellIndex]->GetBoundaries();
	m_pCells[cellIndex]->SetMirror(new Mirror{ cellBoundaries, MirrorType::Receiver });
}

void Grid::RemoveMirror(int col, int row)
{
	m_pCells[GetIndex(col, row)]->DeleteMirror();
}

void Grid::AddRemoveMirrorAt(const Vector2f& position, const Vector2f& centerPos)
{
	Cell* currentCell = GetCellFromPosition(position, centerPos);

	if (currentCell != nullptr)
	{
		if (currentCell->HasMirror())
		{
			currentCell->DeleteMirror();
		}
		else
		{
			Rectf cellBoundaries = currentCell->GetBoundaries();
			currentCell->SetMirror(new Mirror{ cellBoundaries, MirrorType::Reflector });
		}
	}
}

void Grid::RotateMirrorAt(const Vector2f& position, const Vector2f& centerPos)
{
	Cell* currentCell = GetCellFromPosition(position, centerPos);

	if (currentCell != nullptr && currentCell->HasMirror())
	{
		currentCell->RotateMirror();
	}
}

void Grid::Update(float elapsedSeconds)
{
	for (Cell* currentCell : m_pCells)
	{
		currentCell->Update(elapsedSeconds);
	}
}


