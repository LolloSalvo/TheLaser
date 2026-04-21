#include "pch.h"
#include "Digit.h"

#include "utils.h"

#include <iostream>

Texture* Digit::m_SpriteSheet{};
int Digit::m_NumInstances{ 0 };

Digit::Digit(int number, Mode mode, Vector2f position)
	:m_Mode{mode},
	m_Position{position}
{
	if (number >= 0 && number <= 9)
	{
		m_Number = number;
	}

	if (m_SpriteSheet == nullptr)
	{
		m_SpriteSheet = new Texture{ "digits.png" };
		std::cout << "Larghezza SpriteSheet: " << m_SpriteSheet->GetWidth() << std::endl;
	}

	m_NumInstances++;
}

Digit::Digit(int number, Mode mode, Vector2f position, Color4f color)
	:m_Mode{ mode },
	m_Position{ position },
	m_Color{color}
{
	if (number >= 0 && number <= 9)
	{
		m_Number = number;
	}

	if (m_SpriteSheet == nullptr)
	{
		m_SpriteSheet = new Texture{ "digits.png" };
		std::cout << "Larghezza SpriteSheet: " << m_SpriteSheet->GetWidth() << std::endl;
	}

	m_NumInstances++;
}

Digit::~Digit()
{
	if (m_NumInstances <= 1)
	{
		delete m_SpriteSheet;
	}

	m_NumInstances--;
}

void Digit::SetValue(const int number)
{
	if (number >= 0 && number <= 9)
	{
		m_Number = number;
	}
}

void Digit::Increase()
{
	SetValue(m_Number + 1);
}

void Digit::Decrease()
{
	SetValue(m_Number - 1);
}

Rectf Digit::GetFrameRect() const
{
	if (m_SpriteSheet != nullptr)
	{
		float frameWidth{ m_SpriteSheet->GetWidth() / 11.f };
		float frameHeight{ m_SpriteSheet->GetHeight() / 2.f };

		float yOffset{ frameHeight };

		return Rectf{
			frameWidth * m_Number,
			yOffset,
			frameWidth,
			frameHeight
		};
	}
	return Rectf{};
}

void Digit::Draw() const
{
	Rectf srcRect{ GetFrameRect() };

	float desiredWidth{ 40.0f };
	float desiredHeight{ 60.0f };
	Rectf destRect{ m_Position.x, m_Position.y, desiredWidth, desiredHeight };

	m_SpriteSheet->Draw(destRect, srcRect);
}

void Digit::DrawMeridium(Vector2f position, bool isAM) const
{
	Rectf srcRect{};

	if (isAM)
	{
		srcRect = Rectf( 
			m_SpriteSheet->GetWidth() / 11.f * 10.f,
			m_SpriteSheet->GetHeight() / 4.f * 1.f,
			m_SpriteSheet->GetWidth() / 11.f,
			m_SpriteSheet->GetHeight() / 4.f
		);
	}
	else
	{
		srcRect = Rectf( 
			m_SpriteSheet->GetWidth() / 11.f * 10.f,
			0.f,
			m_SpriteSheet->GetWidth() / 11.f,
			m_SpriteSheet->GetHeight() / 4.f
		);
	}

	m_SpriteSheet->Draw(position, srcRect);
}

