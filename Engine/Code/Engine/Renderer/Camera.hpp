#pragma once

#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Math/Mat44.hpp"
//------------------------------------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------------------------------------
class Camera
{

public:

	enum Mode
	{
		eMode_Orthographic,
		eMode_Perspective,
		
		eMode_Count
	};


	void SetOrthographicView(Vec2 const& bottomLeft, Vec2 const& topRight, float orthoNear = 0.f, float orthoFar = 1.f);
	void SetOrthographicView(AABB2 viewportBounds, float orthoNear = 0.f, float orthoFar = 1.f);
	void SetPerspectiveView(float aspect, float fov, float orthoNear, float orthoFar);

	void SetPositionAndOrientation(Vec3 const& position, EulerAngles const& orientation);
	void SetPosition(Vec3 const& position);
	Vec3 GetPosition() const;
	void SetOrientation(EulerAngles const& orientation);
	EulerAngles GetOrientation() const;

	Mat44 GetCameraToWorldTransform() const;
	Mat44 GetWorldToCameraTransform() const;
	Mat44 GetWorldToCameraTransformRotationOnly() const;

	void SetCameraToRenderTransform(Mat44 const& matrix);
	Mat44 GetCameraToRenderTransform() const;

	Mat44 GetRenderToClipTransform() const;
	Mat44 GetWorldToClipTransform() const;

	Vec2 GetOrthographicBottomLeft() const;
	Vec2 GetOrthographicTopRight() const;

	void Translate2D(Vec2 const& translation);
	
	Mat44 GetOrthographicMatrix() const;
	Mat44 GetPerspectiveMatrix() const;
	Mat44 GetProjectionMatrix() const;

public:

	Mode m_mode = eMode_Orthographic;

	Vec3 m_position;
	EulerAngles m_orientation;

	Vec2	m_orthographicBottomLeft;
	Vec2	m_orthographicTopRight;
	float	m_orthographicNear;
	float	m_orthographicFar;

	float	m_perspectiveAspect;
	float	m_perspectiveFov;
	float	m_perspectiveNear;
	float	m_perspectiveFar;

	AABB2	m_viewportBounds;

	Mat44	m_cameraToRenderTransform;

};
