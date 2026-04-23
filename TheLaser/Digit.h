#pragma once
#include "Texture.h"

class Digit
{
public:
	
	enum class Mode
	{
		Dark,
		Bright
	};

	Digit(int number, Mode mode, Vector2f position);
	Digit(int number, Mode mode, Vector2f position, Color4f color );

	void Increase();
	void Decrease();

	void SetValue(const int number);

	void Draw() const;

	~Digit();

private:

	int m_Number{};
	Vector2f m_Position{};
	Color4f m_Color{0.f, 1.f, 0.f, 1.f};

	Mode m_Mode{ Mode::Bright };

	static Texture* m_SpriteSheet;
	static int m_NumInstances;

	Rectf GetFrameRect() const;
};

