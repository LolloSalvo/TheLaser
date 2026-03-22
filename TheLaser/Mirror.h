#pragma once
class Mirror
{
public:

	Mirror(const Rectf& cellBound);
	~Mirror() = default;

	Vector2f GetFirstPoint() const;
	Vector2f GetSecondPoint() const;

	void Draw() const;

	void RotateMirror(const Rectf& boundaries);


private:

	enum class MirrorType
	{
		ForwardSlash,
		BackSlash
	};

	MirrorType m_Type{};
	int m_Index{};
	Vector2f m_FirstPoint{};
	Vector2f m_SecondPoint{};


};

