#pragma once
#include "Mirror.h"

class Cell
{
public:

	Cell(int index, const Vector2f& position, const float cellSize);
	~Cell() = default;

	void Draw() const;

	Rectf GetBoundaries() const;

	void SetMirror(Mirror* mirror);
	void DeleteMirror();
	void RotateMirror(int direction);
	MirrorType GetMirrorType() const;

	void Update(float elapsedSec);

	bool HasMirror() const;

	bool GetMirrorPoint(Vector2f& p1Out, Vector2f& p2Out);
	Vector2f GetMirrorFrontNormal() const;

	bool IsRotating() const;

private:

	Rectf m_Boundaries{};

	Mirror* m_pMirror{};

	int m_Index{};

};