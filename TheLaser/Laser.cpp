#include "pch.h"
#include "Laser.h"
#include "utils.h"

Laser::Laser()
	: m_pLaserPoints{}
{
}

void Laser::Draw() const
{
	Draw(Color4f{ 1.f, 0.f, 0.f, 1.f });
}

void Laser::Draw(const Color4f& color) const
{
	utils::SetColor(color);
	for (int index{}; index < static_cast<int>(m_pLaserPoints.size()) - 1; ++index)
	{
		utils::DrawLine(m_pLaserPoints[index], m_pLaserPoints[index + 1], 2.f);
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