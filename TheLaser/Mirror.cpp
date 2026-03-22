#include "pch.h"
#include "Mirror.h"
#include "utils.h"

Mirror::Mirror(const Rectf& cellBound)
{
	float offset{ cellBound.width / 10.f };

	if(rand() % 2 == 0)
	{
		m_Type = MirrorType::BackSlash;
		m_FirstPoint = Vector2f{ cellBound.left + offset, cellBound.bottom + offset };
		m_SecondPoint = Vector2f{ cellBound.left + cellBound.width - offset, cellBound.bottom + cellBound.height - offset };
	}
	else
	{
		m_Type = MirrorType::ForwardSlash;
		m_FirstPoint = Vector2f{ cellBound.left + offset, cellBound.bottom + cellBound.height - offset };
		m_SecondPoint = Vector2f{ cellBound.left + cellBound.width - offset, cellBound.bottom + offset };
	}
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

void Mirror::RotateMirror(const Rectf& boundaries)
{
	float offset{ boundaries.width / 10.f };

	if (m_Type == MirrorType::BackSlash)
	{
		m_Type = MirrorType::ForwardSlash;
		m_FirstPoint = Vector2f{ boundaries.left + offset, boundaries.bottom + boundaries.height - offset };
		m_SecondPoint = Vector2f{ boundaries.left + boundaries.width - offset, boundaries.bottom + offset };
	}
	else
	{
		m_Type = MirrorType::BackSlash;
		m_FirstPoint = Vector2f{ boundaries.left + offset, boundaries.bottom + offset };
		m_SecondPoint = Vector2f{ boundaries.left + boundaries.width - offset, boundaries.bottom + boundaries.height - offset };
	}
	
}
