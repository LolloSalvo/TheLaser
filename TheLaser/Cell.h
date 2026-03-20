#pragma once
class Cell
{
public:

	Cell(const Vector2f& position, const float cellSize);
	~Cell() = default;

	void Draw(const Vector2f& offset) const;

private:

	Rectf m_Boundaries{};

};

