#include "pch.h"
#include "Mirror.h"
#include "utils.h"


Mirror::Mirror(const Rectf& cellBoundaries, MirrorType type)
	: m_Boundaries{ cellBoundaries }
	, m_Type{ type }
	, m_RotationSpeed{ 200.0f }
{
	// Random start: either 45deg (/) or 135deg (\)
	int randomOrientation{ rand() % 2 };
	m_CurrentAngle = (randomOrientation == 0) ? 45.0f : 135.0f;
	m_TargetAngle = m_CurrentAngle;

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
		float piConstant{ 3.1415926535f };
		float angleInRadians{ m_CurrentAngle * piConstant / 180.0f };

		// Mirror direction and back normal
		float mirrorDX{ cosf(angleInRadians) };
		float mirrorDY{ sinf(angleInRadians) };
		float backNX{ sinf(angleInRadians) };
		float backNY{ -cosf(angleInRadians) };

		// Half-length of the mirror line
		float halfLen{ sqrtf(
			(m_SecondPoint.x - m_FirstPoint.x) * (m_SecondPoint.x - m_FirstPoint.x) +
			(m_SecondPoint.y - m_FirstPoint.y) * (m_SecondPoint.y - m_FirstPoint.y)
		) / 2.0f };

		float midX{ (m_FirstPoint.x + m_SecondPoint.x) / 2.0f };
		float midY{ (m_FirstPoint.y + m_SecondPoint.y) / 2.0f };
		float domeDepth{ m_Boundaries.width * 0.45f };

		// Build dome polygon: arc from m_FirstPoint -> back -> m_SecondPoint
		const int arcSteps{ 14 };
		std::vector<Vector2f> dome;
		for (int i = 0; i <= arcSteps; ++i)
		{
			float t{ static_cast<float>(i) / arcSteps }; // 0 -> 1
			float a{ piConstant * t };                   // 0 -> PI
			// cos goes 1 -> -1 along mirror, sin goes 0 -> 0 bulging into back
			dome.push_back(Vector2f{
				midX + mirrorDX * halfLen * cosf(a) + backNX * domeDepth * sinf(a),
				midY + mirrorDY * halfLen * cosf(a) + backNY * domeDepth * sinf(a)
				});
		}

		// 1 - solid dark fill
		utils::SetColor(Color4f{ 0.28f, 0.16f, 0.06f, 1.0f });
		utils::FillPolygon(dome);

		// 2 - amber outline
		utils::SetColor(Color4f{ 0.72f, 0.42f, 0.08f, 1.0f });
		utils::DrawPolygon(dome, false);

		// 4 - bright cyan reflective line on top (front face)
		utils::SetColor(Color4f{ 0.0f, 0.8f, 1.0f, 1.0f });
		utils::DrawLine(m_FirstPoint, m_SecondPoint, 3.0f);
	}
	else if (m_Type == MirrorType::Receiver)
	{
		utils::SetColor(Color4f{ 0.0f, 1.0f, 0.0f, 1.0f });
		utils::FillEllipse(
			Vector2f{ m_Boundaries.left + m_Boundaries.width / 2.f,
					  m_Boundaries.bottom + m_Boundaries.height / 2.f },
			m_Boundaries.width / 2.f,
			m_Boundaries.height / 2.f
		);
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
}

void Mirror::RotateMirror(int direction)
{
	// direction: -1 = clockwise (-90deg), +1 = counter-clockwise (+90deg)
	m_TargetAngle += direction * 90.0f;
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

Vector2f Mirror::GetFrontNormal() const
{
	// Mirror line direction: (cos theta, sin theta)
	// Front normal is 90deg CCW from the line direction: (-sin theta, cos theta)
	float piConstant{ 3.1415926535f };
	float angleInRadians{ m_CurrentAngle * piConstant / 180.0f };
	return Vector2f{ -sinf(angleInRadians), cosf(angleInRadians) };
}