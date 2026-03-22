#pragma once
#include <vector>

class Laser
{
public:

	

	Laser();
	~Laser() = default;

	void Draw() const;

	void AddPoint(const Vector2f& point);

	void ClearPath();

private:

	std::vector<Vector2f> m_pLaserPoints{};

};

