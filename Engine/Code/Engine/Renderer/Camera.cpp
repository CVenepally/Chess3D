#include "Engine/Renderer/Camera.hpp"
#include "Engine/Window/Window.hpp"
#include "Engine/math/Vec4.hpp"
#include "Engine/Core/EngineCommon.hpp"
//------------------------------------------------------------------------------------------------------------------
void Camera::SetOrthographicView(Vec2 const& bottomLeft, Vec2 const& topRight, float orthoNear, float orthoFar)
{

	m_mode = eMode_Orthographic;
	m_orthographicBottomLeft = bottomLeft;
	m_orthographicTopRight = topRight;
	m_orthographicNear = orthoNear;
	m_orthographicFar = orthoFar;

}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Camera::SetOrthographicView(AABB2 viewportBounds, float orthoNear, float orthoFar)
{
	SetOrthographicView(viewportBounds.m_mins, viewportBounds.m_maxs, orthoNear, orthoFar);
}
//------------------------------------------------------------------------------------------------------------------
void Camera::SetPerspectiveView(float aspect, float fov, float perspectiveNear, float perspectiveFar)
{

	m_mode = eMode_Perspective;
	m_perspectiveAspect = aspect;
	m_perspectiveFov = fov;
	m_perspectiveNear = perspectiveNear;
	m_perspectiveFar = perspectiveFar;

}

//------------------------------------------------------------------------------------------------------------------
void Camera::SetPositionAndOrientation(Vec3 const& position, EulerAngles const& orientation)
{

	m_position = position;
	m_orientation = orientation;

}

//------------------------------------------------------------------------------------------------------------------
void Camera::SetPosition(Vec3 const& position)
{

	m_position = position;

}

//------------------------------------------------------------------------------------------------------------------
Vec3 Camera::GetPosition() const
{
	return m_position;
}

//------------------------------------------------------------------------------------------------------------------
void Camera::SetOrientation(EulerAngles const& orientation)
{

	m_orientation = orientation;

}

//------------------------------------------------------------------------------------------------------------------
EulerAngles Camera::GetOrientation() const
{
	return m_orientation;
}

//------------------------------------------------------------------------------------------------------------------
Mat44 Camera::GetCameraToWorldTransform() const
{
	Mat44 cameraToWorldMatrix = Mat44::MakeTranslation3D(m_position);

	Mat44 orientationMatrix = m_orientation.GetAsMatrix_IFwd_JLeft_KUp();

	cameraToWorldMatrix.Append(orientationMatrix);

	return cameraToWorldMatrix;
}

//------------------------------------------------------------------------------------------------------------------
Mat44 Camera::GetWorldToCameraTransform() const
{

	Mat44 cameraToWorldMatrix = GetCameraToWorldTransform();

	return cameraToWorldMatrix.GetOrthonormalInverse();

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Mat44 Camera::GetWorldToCameraTransformRotationOnly() const
{
	Mat44 worldToCamera = GetWorldToCameraTransform();
	worldToCamera.SetTranslation3D(Vec3(0.f, 0.f, 0.f));

	return worldToCamera;
}

//------------------------------------------------------------------------------------------------------------------
void Camera::SetCameraToRenderTransform(Mat44 const& matrix)
{

	m_cameraToRenderTransform = Mat44(matrix.GetAsFloatArray());

}

//------------------------------------------------------------------------------------------------------------------
Mat44 Camera::GetCameraToRenderTransform() const
{
	return m_cameraToRenderTransform;
}

//------------------------------------------------------------------------------------------------------------------
Mat44 Camera::GetRenderToClipTransform() const
{
	return GetProjectionMatrix();
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Mat44 Camera::GetWorldToClipTransform() const
{
	Mat44 transform = GetRenderToClipTransform();
	transform.Append(GetCameraToRenderTransform());
	transform.Append(GetWorldToCameraTransform());

	return transform;
}

//------------------------------------------------------------------------------------------------------------------
Vec2 Camera::GetOrthographicBottomLeft() const
{
	return m_orthographicBottomLeft;
}

//------------------------------------------------------------------------------------------------------------------
Vec2 Camera::GetOrthographicTopRight() const
{
	return m_orthographicTopRight;
}

//------------------------------------------------------------------------------------------------------------------
void Camera::Translate2D(Vec2 const& translation)
{

	UNUSED(translation);

}

//------------------------------------------------------------------------------------------------------------------
Mat44 Camera::GetOrthographicMatrix() const
{
	return Mat44::MakeOrthoProjection(m_orthographicBottomLeft.x, m_orthographicTopRight.x, m_orthographicBottomLeft.y, m_orthographicTopRight.y, m_orthographicNear, m_orthographicFar);
}

//------------------------------------------------------------------------------------------------------------------
Mat44 Camera::GetPerspectiveMatrix() const
{
	return Mat44::MakePerspectiveProjection(m_perspectiveFov, m_perspectiveAspect, m_perspectiveNear, m_perspectiveFar);
}

//------------------------------------------------------------------------------------------------------------------
Mat44 Camera::GetProjectionMatrix() const
{
	if(m_mode == eMode_Orthographic)
	{
		return GetOrthographicMatrix();
	}

	if(m_mode == eMode_Perspective)
	{
		return GetPerspectiveMatrix();
	}

	return Mat44();

}
