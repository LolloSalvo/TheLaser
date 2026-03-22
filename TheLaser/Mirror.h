#pragma once
#include "utils.h"

enum class MirrorType
{
	Reflector,
	Receiver,
	Splitter,
	OneWay
};

enum class MirrorOrientation
{
	ForwardSlash,
	BackSlash
};

class Mirror
{
private:
	Rectf m_Boundaries;
	MirrorType m_Type;
	MirrorOrientation m_Orientation;
	Vector2f m_FirstPoint;
	Vector2f m_SecondPoint;

public:
	Mirror(const Rectf& cellBoundaries, MirrorType type);

	Vector2f GetFirstPoint() const;
	Vector2f GetSecondPoint() const;
	MirrorType GetType() const;

	void Draw() const;
	void RotateMirror();
};