#pragma once
class Mirror
{
public:

	Mirror(const Rectf& cellBound);
	~Mirror() = default;

	Vector2f GetFirstPoint() const;
	Vector2f GetSecondPoint() const;

	void Draw() const;

private:

	int m_Index{};
	Vector2f m_FirstPoint{};
	Vector2f m_SecondPoint{};


};

