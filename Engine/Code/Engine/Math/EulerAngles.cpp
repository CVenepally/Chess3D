#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Mat44.hpp"

//------------------------------------------------------------------------------------------------------------------
EulerAngles::EulerAngles(float yawDegrees, float pitchDegrees, float rollDegrees)
	: m_yawDegrees(yawDegrees)
	, m_pitchDegrees(pitchDegrees)
	, m_rollDegrees(rollDegrees)
{

}

//------------------------------------------------------------------------------------------------------------------
void EulerAngles::GetAsVectors_IFwd_JLeft_KUp(Vec3& out_fwdIBasis, Vec3& out_leftJBasis, Vec3& out_upKBasis) const
{
	
	Mat44 eulerMatrix;

	eulerMatrix.AppendZRotation(m_yawDegrees);
	eulerMatrix.AppendYRotation(m_pitchDegrees);
	eulerMatrix.AppendXRotation(m_rollDegrees);

	out_fwdIBasis = eulerMatrix.GetIBasis3D();
	out_leftJBasis = eulerMatrix.GetJBasis3D();
	out_upKBasis = eulerMatrix.GetKBasis3D();
}

//------------------------------------------------------------------------------------------------------------------
Mat44 EulerAngles::GetAsMatrix_IFwd_JLeft_KUp() const
{

	Mat44 eulerMatrix;

	eulerMatrix.AppendZRotation(m_yawDegrees);
	eulerMatrix.AppendYRotation(m_pitchDegrees);
	eulerMatrix.AppendXRotation(m_rollDegrees);

	return eulerMatrix;

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void EulerAngles::SetFromText(char const* textEulerAngles)
{

	Strings eulerStrings = SplitStringOnDelimiter(textEulerAngles, ',');

	m_yawDegrees	= static_cast<float>(atof(eulerStrings[0].c_str()));
	m_pitchDegrees	= static_cast<float>(atof(eulerStrings[1].c_str()));
	m_rollDegrees	= static_cast<float>(atof(eulerStrings[2].c_str()));

}

//------------------------------------------------------------------------------------------------------------------
EulerAngles const EulerAngles::operator + (EulerAngles const& anglesToAdd) const
{

	return EulerAngles(m_yawDegrees + anglesToAdd.m_yawDegrees, m_pitchDegrees + anglesToAdd.m_pitchDegrees, m_rollDegrees + anglesToAdd.m_rollDegrees);

}

//------------------------------------------------------------------------------------------------------------------
EulerAngles const EulerAngles::operator-(EulerAngles const& anglesToSubtract) const
{

	return EulerAngles(m_yawDegrees - anglesToSubtract.m_yawDegrees, m_pitchDegrees - anglesToSubtract.m_pitchDegrees, m_rollDegrees - anglesToSubtract.m_rollDegrees);

}

//------------------------------------------------------------------------------------------------------------------
EulerAngles const EulerAngles::operator-() const
{

	return EulerAngles(-m_yawDegrees, -m_pitchDegrees, -m_rollDegrees);

}

//------------------------------------------------------------------------------------------------------------------
EulerAngles const EulerAngles::operator*(float uniformScale) const
{

	return EulerAngles(m_yawDegrees * uniformScale, m_pitchDegrees * uniformScale, m_rollDegrees * uniformScale);

}

//------------------------------------------------------------------------------------------------------------------
EulerAngles const EulerAngles::operator*(EulerAngles const& anglesToMultiply) const
{

	return EulerAngles(m_yawDegrees * anglesToMultiply.m_yawDegrees, m_pitchDegrees * anglesToMultiply.m_pitchDegrees, m_rollDegrees * anglesToMultiply.m_rollDegrees);

}

//------------------------------------------------------------------------------------------------------------------
EulerAngles const EulerAngles::operator/(float inverseScale) const
{

	return EulerAngles(m_yawDegrees / inverseScale, m_pitchDegrees / inverseScale, m_rollDegrees / inverseScale);

}

//------------------------------------------------------------------------------------------------------------------
void EulerAngles::operator+=(EulerAngles const& anglesToAdd)
{

	m_yawDegrees += anglesToAdd.m_yawDegrees;
	m_pitchDegrees += anglesToAdd.m_pitchDegrees;
	m_rollDegrees += anglesToAdd.m_rollDegrees;

}

//------------------------------------------------------------------------------------------------------------------
void EulerAngles::operator-=(EulerAngles const& anglesToSubtract)
{

	m_yawDegrees -= anglesToSubtract.m_yawDegrees;
	m_pitchDegrees -= anglesToSubtract.m_pitchDegrees;
	m_rollDegrees -= anglesToSubtract.m_rollDegrees;

}

//------------------------------------------------------------------------------------------------------------------
void EulerAngles::operator*=(const float uniformScale)
{

	m_yawDegrees *= uniformScale;
	m_pitchDegrees *= uniformScale;
	m_rollDegrees *= uniformScale;

}

//------------------------------------------------------------------------------------------------------------------
void EulerAngles::operator/=(const float uniformDivisor)
{

	m_yawDegrees /= uniformDivisor;
	m_pitchDegrees /= uniformDivisor;
	m_rollDegrees /= uniformDivisor;

}

//------------------------------------------------------------------------------------------------------------------
void EulerAngles::operator=(EulerAngles const& copyFrom)
{

	m_yawDegrees   = copyFrom.m_yawDegrees;
	m_pitchDegrees = copyFrom.m_pitchDegrees;
	m_rollDegrees  = copyFrom.m_rollDegrees;

}

//------------------------------------------------------------------------------------------------------------------
EulerAngles const operator*(float uniformScale, EulerAngles const& anglesToScale)
{

	return EulerAngles(anglesToScale.m_yawDegrees * uniformScale, anglesToScale.m_pitchDegrees * uniformScale, anglesToScale.m_rollDegrees * uniformScale);

}

//------------------------------------------------------------------------------------------------------------------
bool EulerAngles::operator==(EulerAngles const& compare) const
{

	return m_yawDegrees == compare.m_yawDegrees && m_pitchDegrees == compare.m_pitchDegrees && m_rollDegrees == compare.m_rollDegrees;

}

//------------------------------------------------------------------------------------------------------------------
bool EulerAngles::operator!=(EulerAngles const& compare) const
{

	return m_yawDegrees != compare.m_yawDegrees || m_pitchDegrees != compare.m_pitchDegrees || m_rollDegrees != compare.m_rollDegrees;

}
