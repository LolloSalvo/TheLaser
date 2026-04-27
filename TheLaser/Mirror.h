#pragma once
#include "utils.h"

enum class MirrorType
{
	Reflector,
	Receiver,
	Splitter,
	OneWay
};

class Mirror
{
private:
	Rectf m_Boundaries{};
	MirrorType m_Type{};
	Vector2f m_FirstPoint{};
	Vector2f m_SecondPoint{};

	float m_CurrentAngle;
	float m_TargetAngle;
	float m_RotationSpeed;

	void CalculatePoints();

public:
	Mirror(const Rectf& cellBoundaries, MirrorType type);

	Vector2f GetFirstPoint() const;
	Vector2f GetSecondPoint() const;
	MirrorType GetType() const;
	Vector2f GetFrontNormal() const;

	void Draw() const;
	void RotateMirror(int direction); // +1 = counter-clockwise, -1 = clockwise
	void Update(float elapsedSeconds);

	bool IsRotating() const;
};