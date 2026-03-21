#include "pch.h"
#include "Laser.h"
#include "utils.h"

Laser::Laser()
	: m_pLaserPoints{}
{
}

void Laser::Draw() const
{
	const Color4f red{ 1.f, 0.f, 0.f, 1.f };
	utils::SetColor(red);

	for (int index{}; index < m_pLaserPoints.size() - 1; ++index)
	{
		utils::DrawLine(m_pLaserPoints[index], m_pLaserPoints[index + 1]);
	}
}

void Laser::AddPoint(const Vector2f& point)
{
	m_pLaserPoints.push_back(point);
}

void Laser::ClearPath()
{
	m_pLaserPoints.clear();
}
