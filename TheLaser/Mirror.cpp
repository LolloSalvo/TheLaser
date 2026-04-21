#include "pch.h"
#include "Mirror.h"
#include "utils.h"


Mirror::Mirror(const Rectf& cellBoundaries, MirrorType type)
	: m_Boundaries{ cellBoundaries }
	, m_Type{ type }
	, m_RotationSpeed{ 200.0f }
{
	int randomOrientation{ rand() % 2 };

	if (randomOrientation == 0)
	{
		m_Orientation = MirrorOrientation::BackSlash;
		m_CurrentAngle = 135.0f;
		m_TargetAngle = 135.0f;
	}
	else
	{
		m_Orientation = MirrorOrientation::ForwardSlash;
		m_CurrentAngle = 45.0f;
		m_TargetAngle = 45.0f;
	}

	CalculatePoints();
}

void Mirror::CalculatePoints()
{
	float centerX{ m_Boundaries.left + (m_Boundaries.width / 2.0f) };
	float centerY{ m_Boundaries.bottom + (m_Boundaries.height / 2.0f) };

	float offset{ m_Boundaries.width / 10.0f };
	float lineLengthX{ m_Boundaries.width - (offset * 2.0f) };
	float lineLengthY{ m_Boundaries.height - (offset * 2.0f) };
	float radius{ sqrtf((lineLengthX * lineLengthX) + (lineLengthY * lineLengthY)) / 2.0f };

	float piConstant{ 3.1415926535f };
	float angleInRadians{ m_CurrentAngle * piConstant / 180.0f };

	m_FirstPoint.x = centerX - (cos(angleInRadians) * radius);
	m_FirstPoint.y = centerY - (sin(angleInRadians) * radius);

	m_SecondPoint.x = centerX + (cos(angleInRadians) * radius);
	m_SecondPoint.y = centerY + (sin(angleInRadians) * radius);
}

Vector2f Mirror::GetFirstPoint() const
{
	return m_FirstPoint;
}

Vector2f Mirror::GetSecondPoint() const
{
	return m_SecondPoint;
}

MirrorType Mirror::GetType() const
{
	return m_Type;
}

void Mirror::Draw() const
{
	if (m_Type == MirrorType::Reflector)
	{
		utils::SetColor(Color4f{ 0.0f, 0.8f, 1.0f, 1.0f });
		utils::DrawLine(m_FirstPoint, m_SecondPoint, 2.0f);
	}
	else if (m_Type == MirrorType::Receiver)
	{
		utils::SetColor(Color4f{ 0.0f, 1.0f, 0.0f, 1.0f });
	}
	else if (m_Type == MirrorType::Splitter)
	{
		utils::SetColor(Color4f{ 1.0f, 0.0f, 1.0f, 1.0f });
		utils::DrawLine(m_FirstPoint, m_SecondPoint, 2.0f);
	}
	else
	{
		utils::SetColor(Color4f{ 1.0f, 1.0f, 1.0f, 1.0f });
		utils::DrawLine(m_FirstPoint, m_SecondPoint, 2.0f);
	}

	

	if (m_Type == MirrorType::Receiver)
	{
		utils::FillEllipse(Vector2f{ m_Boundaries.left + m_Boundaries.width / 2, m_Boundaries.bottom + m_Boundaries.height / 2 }, m_Boundaries.width / 2, m_Boundaries.height / 2);
	}
}

void Mirror::RotateMirror()
{
	if (m_Orientation == MirrorOrientation::BackSlash)
	{
		m_Orientation = MirrorOrientation::ForwardSlash;
		m_TargetAngle = m_TargetAngle - 90.0f;
	}
	else
	{
		m_Orientation = MirrorOrientation::BackSlash;
		m_TargetAngle = m_TargetAngle + 90.0f;
	}
}

void Mirror::Update(float elapsedSeconds)
{
	if (m_CurrentAngle != m_TargetAngle)
	{
		float angleDifference{ m_TargetAngle - m_CurrentAngle };
		float rotationDirection{ 1.0f };

		if (angleDifference < 0.0f)
		{
			rotationDirection = -1.0f;
		}

		m_CurrentAngle = m_CurrentAngle + (rotationDirection * m_RotationSpeed * elapsedSeconds);

		if ((rotationDirection > 0.0f && m_CurrentAngle > m_TargetAngle) || (rotationDirection < 0.0f && m_CurrentAngle < m_TargetAngle))
		{
			m_CurrentAngle = m_TargetAngle;
		}

		CalculatePoints();
	}
}

bool Mirror::IsRotating() const
{
	return std::abs(m_CurrentAngle - m_TargetAngle) > 0.001f;
}