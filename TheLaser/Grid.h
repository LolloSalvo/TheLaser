#pragma once
#include "Cell.h"
#include "Mirror.h"

#include <vector>

class Grid
{
public:

	Grid(int rows, int cols, float cellSize, const Vector2f& center);
	~Grid() = default;

	int GetIndex(int col, int row) const;

	int GetRows() const;
	int GetColumns() const;

	float GetCellSize() const;

	int GetRowFromIndex(int index) const;
	int GetColFromIndex(int index) const;

	Vector2f GetCellCenter(int col, int row) const;

	void Draw() const;

	void AddMirror(int col, int row);
	void RemoveMirror(int col, int row);
	void AddRemoveMirrorAt(const Vector2f& position, const Vector2f& centerPos);
	void RotateMirrorAt(const Vector2f& position, const Vector2f& centerPos);

	int GetCellIndexFromPosition(const Vector2f& position, const Vector2f& centerPos) const;

	Cell* GetCellFromPosition(const Vector2f& position, const Vector2f& centerPos) const;

	Cell* GetCellFromIndex(int index);
	Cell* GetCellFromIndex(int col, int row);

private:

	int m_Rows{};
	int m_Columns{};

	float m_CellSize{};

	std::vector<Cell*> m_pCells{};

};

