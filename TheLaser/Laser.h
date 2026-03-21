#pragma once
#include <vector>

class Laser
{
public:

	enum class StartingPosition
	{
		bottomLeft,
		bottomRight,
		TopLeft,
		TopRight
	};

	Laser();
	~Laser() = default;

	void Draw() const;

	void AddPoint(const Vector2f& point);

	void ClearPath();

private:

	std::vector<Vector2f> m_pLaserPoints{};

};

