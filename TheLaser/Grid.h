#pragma once
class Grid
{
public:

	Grid(int rows, int cols, float cellSize);
	~Grid() = default;

	int GetIndex(int row, int col) const;

	int GetRows() const;
	int getColumns() const;

	void Draw(const Vector2f& centerPos) const;

private:

	int m_Rows{};
	int m_Columns{};

	float m_CellSize{};

};

