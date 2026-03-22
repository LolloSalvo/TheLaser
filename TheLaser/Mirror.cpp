#include "pch.h"
#include "Mirror.h"
#include "utils.h"

Mirror::Mirror(const Rectf& cellBoundaries, MirrorType type)
	: m_Boundaries{ cellBoundaries }
	, m_Type{ type }
{
	float offset = m_Boundaries.width / 10.0f;
	int randomOrientation = rand() % 2;

	if (randomOrientation == 0)
	{
		m_Orientation = MirrorOrientation::BackSlash;
		m_FirstPoint = Vector2f{ m_Boundaries.left + offset, m_Boundaries.bottom + offset };
		m_SecondPoint = Vector2f{ m_Boundaries.left + m_Boundaries.width - offset, m_Boundaries.bottom + m_Boundaries.height - offset };
	}
	else
	{
		m_Orientation = MirrorOrientation::ForwardSlash;
		m_FirstPoint = Vector2f{ m_Boundaries.left + offset, m_Boundaries.bottom + m_Boundaries.height - offset };
		m_SecondPoint = Vector2f{ m_Boundaries.left + m_Boundaries.width - offset, m_Boundaries.bottom + offset };
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
	float offset = m_Boundaries.width / 10.0f;

	if (m_Orientation == MirrorOrientation::BackSlash)
	{
		m_Orientation = MirrorOrientation::ForwardSlash;
		m_FirstPoint = Vector2f{ m_Boundaries.left + offset, m_Boundaries.bottom + m_Boundaries.height - offset };
		m_SecondPoint = Vector2f{ m_Boundaries.left + m_Boundaries.width - offset, m_Boundaries.bottom + offset };
	}
	else
	{
		m_Orientation = MirrorOrientation::BackSlash;
		m_FirstPoint = Vector2f{ m_Boundaries.left + offset, m_Boundaries.bottom + offset };
		m_SecondPoint = Vector2f{ m_Boundaries.left + m_Boundaries.width - offset, m_Boundaries.bottom + m_Boundaries.height - offset };
	}
}