#pragma once
#include "Cell.h"
#include "Mirror.h"

#include <vector>

class Grid
{
public:

	Grid(int rows, int cols, float cellSize);
	~Grid() = default;

	int GetIndex(int row, int col) const;

	int GetRows() const;
	int GetColumns() const;

	int GetRowFromIndex(int index) const;
	int GetColFromIndex(int index) const;

	Vector2f GetCellCenter(int row, int col) const;

	void Draw(const Vector2f& centerPos) const;

	void AddMirror(Mirror* mirror);

	int GetCellIndexFromPosition(const Vector2f& position, const Vector2f& centerPos) const;

private:

	int m_Rows{};
	int m_Columns{};

	float m_CellSize{};

	std::vector<Cell*> m_pCells{};

	std::vector<Mirror*> m_pMirrors{};
};

