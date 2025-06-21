#pragma once

struct IntVec2;

struct Vec2
{

public:

	float x = 0.f;
	float y = 0.f;

	static const Vec2 ZERO;
	static const Vec2 ONE;
	static const Vec2 RIGHT;
	static const Vec2 LEFT;
	static const Vec2 UP;
	static const Vec2 DOWN;

public:

	~Vec2() {}
	Vec2() {}
	Vec2(Vec2 const& copyFrom);

	explicit Vec2(float initialX, float initialY);
	explicit Vec2(float initialXY);
	explicit Vec2(IntVec2 intVec);

	static Vec2 const MakeFromPolarRadians(float orientationRadians, float length = 1.f);
	static Vec2 const MakeFromPolarDegrees(float orientationDegrees, float length = 1.f);

	float GetLength() const;
	float GetLengthSquared() const;
	float GetOrientationRadians() const;
	float GetOrientationDegrees() const;

	Vec2 const GetRotated90Degrees() const; // counter clock wise
	Vec2 const GetRotatedMinus90Degrees() const; // clock wise
	Vec2 const GetRotatedRadians(float deltaRadians) const;
	Vec2 const GetRotatedDegrees(float deltaDegrees) const;
	Vec2 const GetClamped(float maxLength) const;
	Vec2 const GetNormalized() const;
	Vec2 const GetReflected(Vec2 const& normalOfSurfaceToReflectOff) const; 

	void SetFromText(char const* textVec2);
	void SetOrientationRadians(float newOrientationRadians);
	void SetOrientationDegrees(float newOrientationDegrees);
	void SetPolarRadians(float newOrientationRadians, float newLength);
	void SetPolarDegrees(float newOrientationDegrees, float newLength);
	void Rotate90Degrees();
	void RotateMinus90Degrees();
	void RotateRadians(float deltaRadians);
	void RotateDegrees(float deltaDegree);
	void SetLength(float newLength);
	void ClampLength(float maxLength);
	void Normalize();
	void Reflect(Vec2 const& normalOfSurfaceToReflectOff); 
	
	float NormalizeAndGetPreviousLength();

	bool		operator==(Vec2 const& compare) const;
	bool		operator!=(Vec2 const& compare) const;

	Vec2 const	operator+(Vec2 const& vecToAdd) const;
	Vec2 const	operator+(int const& numToAdd) const;
	Vec2 const	operator-(Vec2 const& vecToSubtract) const;
	Vec2 const	operator-() const;
	Vec2 const	operator*(float uniformScale) const;
	Vec2 const	operator*(Vec2 const& vecToMultiply) const;
	Vec2 const	operator/(float inverseScale) const;

	void		operator+=(Vec2 const& vecToAdd);
	void		operator-=(Vec2 const& vecToSubtract);
	void		operator*=(const float uniformScale);
	void		operator/=(const float uniformDivisor);
	void		operator=(Vec2 const& copyFrom);

	friend Vec2 const operator*(float uniformScale, Vec2 const& vecToScale);

	// Custom methods

	Vec2 const GetAbsoluteVec() const;



};
