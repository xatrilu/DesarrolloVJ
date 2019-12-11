#ifndef _P2VEC2_H
#define _P2VEC2_H
#include <math.h>

template<class TYPE>
class p2Vec2
{
public:

	TYPE x, y;

	p2Vec2()
	{}

	p2Vec2(const p2Vec2<TYPE>& v)
	{
		this->x = v.x;
		this->y = v.y;
	}

	p2Vec2(const TYPE& x, const TYPE& y)
	{
		this->x = x;
		this->y = y;
	}

	p2Vec2& create(const TYPE& x, const TYPE& y)
	{
		this->x = x;
		this->y = y;

		return(*this);
	}

	// Math ------------------------------------------------
	p2Vec2 operator -(const p2Vec2& v) const
	{
		p2Vec2 r;

		r.x = x - v.x;
		r.y = y - v.y;

		return(r);
	}

	p2Vec2 operator + (const p2Vec2& v) const
	{
		p2Vec2 r;

		r.x = x + v.x;
		r.y = y + v.y;

		return(r);
	}

	const p2Vec2& operator -=(const p2Vec2& v)
	{
		x -= v.x;
		y -= v.y;

		return(*this);
	}

	const p2Vec2& operator +=(const p2Vec2& v)
	{
		x += v.x;
		y += v.y;

		return(*this);
	}

	bool operator ==(const p2Vec2& v) const
	{
		return (x == v.x && y == v.y);
	}

	bool operator !=(const p2Vec2& v) const
	{
		return (x != v.x || y != v.y);
	}

	// Utils ------------------------------------------------
	bool IsZero() const
	{
		return (x == 0.0f && y == 0.0f);
	}

	p2Vec2& SetToZero()
	{
		x = y = 0.0f;
		return(*this);
	}

	p2Vec2& Negate()
	{
		x = -x;
		y = -y;

		return(*this);
	}

	// Distances ---------------------------------------------
	TYPE DistanceTo(const p2Vec2& v) const
	{
		TYPE fx = x - v.x;
		TYPE fy = y - v.y;

		return sqrtf((fx * fx) + (fy * fy));
	}

	TYPE DistanceNoSqrt(const p2Vec2& v) const
	{
		TYPE fx = x - v.x;
		TYPE fy = y - v.y;

		return (fx * fx) + (fy * fy);
	}

	TYPE DistanceManhattan(const p2Vec2& v) const
	{
		return abs(v.x - x) + abs(v.y - y);
	}
};

typedef p2Vec2<int> iVec2;
typedef p2Vec2<float> fVec2;

#endif // !_P2VEC2_H

