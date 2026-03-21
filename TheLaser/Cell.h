#pragma once
#include "Mirror.h"

class Cell
{
public:

	Cell(const Vector2f& position, const float cellSize);
	~Cell() = default;

	void Draw(const Vector2f& offset) const;

	Rectf GetBoundaries() const;

	void SetMirror(Mirror* mirror);

private:

	Rectf m_Boundaries{};

	Mirror* m_pMirror{};

};

