#include "pch.h"
#include "Mirror.h"
#include "utils.h"

Mirror::Mirror(const Rectf& cellBound)
	: m_FirstPoint{Vector2f{cellBound.left, cellBound.bottom}}
	, m_SecondPoint{Vector2f{cellBound.left + cellBound.width, cellBound.bottom + cellBound.height}}
{
}

Vector2f Mirror::GetFirstPoint() const
{
	return m_FirstPoint;
}

Vector2f Mirror::GetSecondPoint() const
{
	return m_SecondPoint;
}

void Mirror::Draw() const
{
	utils::DrawLine(m_FirstPoint, m_SecondPoint);
}
