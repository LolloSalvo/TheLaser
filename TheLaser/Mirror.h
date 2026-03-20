#pragma once
class Mirror
{
public:

	Mirror(int index);
	~Mirror() = default;


private:

	int m_Index{};
	Vector2f m_Position{};


};

