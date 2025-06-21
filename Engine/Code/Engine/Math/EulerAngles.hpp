#pragma once

//------------------------------------------------------------------------------------------------------------------
struct Vec3;
struct Mat44;
//------------------------------------------------------------------------------------------------------------------
struct EulerAngles
{

public:
	EulerAngles() = default;
	EulerAngles(float yawDegrees, float pitchDegrees, float rollDegrees);

	void	GetAsVectors_IFwd_JLeft_KUp(Vec3& out_fwdIBasis, Vec3& out_leftJBasis, Vec3& out_upKBasis) const;
	Mat44	GetAsMatrix_IFwd_JLeft_KUp() const;

	void	SetFromText(char const* textEulerAngles);

public:

	float m_yawDegrees		= 0.f;
	float m_pitchDegrees	= 0.f;
	float m_rollDegrees		= 0.f;

public:

	bool		operator==(EulerAngles const& compare) const;
	bool		operator!=(EulerAngles const& compare) const;

	EulerAngles const	operator+(EulerAngles const& vecToAdd) const;
	EulerAngles const	operator-(EulerAngles const& vecToSubtract) const;
	EulerAngles const	operator-() const;
	EulerAngles const	operator*(float uniformScale) const;
	EulerAngles const	operator*(EulerAngles const& vecToMultiply) const;
	EulerAngles const	operator/(float inverseScale) const;


	void		operator+=(EulerAngles const& vecToAdd);
	void		operator-=(EulerAngles const& vecToSubtract);
	void		operator*=(const float uniformScale);
	void		operator/=(const float uniformDivisor);
	void		operator=(EulerAngles const& copyFrom);

	friend EulerAngles const operator*(float uniformScale, EulerAngles const& vecToScale);

};