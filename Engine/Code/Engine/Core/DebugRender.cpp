#include "Engine/Core/DebugRender.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/Timer.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/Camera.hpp"

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
struct DebugWorldObject
{
	std::vector<Vertex_PCU> m_verts;
	DebugRenderMode m_mode = DebugRenderMode::USE_DEPTH;
	RasterizerMode m_rasterMode = RasterizerMode::SOLID_CULL_BACK;
	Texture* m_texture = nullptr;
	float m_duration;
	Timer m_timer;

	bool m_isText = false;
	bool m_isBillboarded = false;
	Vec3 m_position;
	Mat44 m_transform;

	Rgba8 m_startColor = Rgba8::WHITE;
	Rgba8 m_endColor = Rgba8::WHITE;
};

struct DebugScreenObject
{
	std::vector<Vertex_PCU> m_verts;
	Timer m_timer;
	std::string m_text;
	float m_duration = 0.f;

	Texture* m_texture = nullptr;

	Rgba8 m_startColor = Rgba8::WHITE;
	Rgba8 m_endColor = Rgba8::WHITE;
};

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
DegbugRenderConfig m_debugRenderConfig;

std::vector<DebugWorldObject> m_debugWorldObjects;
std::vector<DebugWorldObject> m_debugWorldObjectsEveryframe;
std::vector<DebugScreenObject> m_debugScreenObjects;

std::vector<DebugScreenObject> m_debugScreenFiniteMessages;
std::vector<DebugScreenObject> m_debugScreenMessagesEveryFrame;
std::vector<DebugScreenObject> m_debugScreenInfiniteMessages;


bool m_shouldRender = true;

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void DebugRenderSystemStartup(DegbugRenderConfig& config)
{
	m_debugRenderConfig.m_renderer = config.m_renderer;
	m_debugRenderConfig.m_fontName = config.m_fontName;

	SubscribeEventCallbackFunction("debugrenderclear", OnDebugRenderClear);
	SubscribeEventCallbackFunction("debugrendertoggle", OnDebugRenderToggle);

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void DebugRenderSystemShutdown()
{

	UnsubscribeEventCallbackFunction("debugrendertoggle", OnDebugRenderToggle);
	UnsubscribeEventCallbackFunction("debugrenderclear", OnDebugRenderClear);

	m_debugWorldObjects.clear();
	m_debugWorldObjectsEveryframe.clear();
	m_debugScreenObjects.clear();

	m_debugScreenFiniteMessages.clear();
	m_debugScreenMessagesEveryFrame.clear();
	m_debugScreenInfiniteMessages.clear();

	delete m_debugRenderConfig.m_renderer;
	m_debugRenderConfig.m_renderer = nullptr;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void DebugRenderSetVisible()
{
	m_shouldRender = true;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void DebugRenderSetHidden()
{
	m_shouldRender = false;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void DebugRenderClear()
{
	m_debugWorldObjects.clear();
	m_debugWorldObjectsEveryframe.clear();
	m_debugScreenObjects.clear();

	m_debugScreenFiniteMessages.clear();
	m_debugScreenMessagesEveryFrame.clear();
	m_debugScreenInfiniteMessages.clear();
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void DebugRenderBeginFrame()
{
	auto worldObjRemoveStartIterator = std::remove_if(m_debugWorldObjects.begin(), m_debugWorldObjects.end(),
													  [](DebugWorldObject& obj) -> bool { return (obj.m_duration >= 0.f && obj.m_timer.HasPeriodElapsed()); });

	m_debugWorldObjects.erase(worldObjRemoveStartIterator, m_debugWorldObjects.end());


	auto everyFrameWorldObjRemoveStartIterator = std::remove_if(m_debugWorldObjectsEveryframe.begin(), m_debugWorldObjectsEveryframe.end(),
																[](DebugWorldObject& obj) -> bool { return (obj.m_duration >= 0.f && obj.m_timer.HasPeriodElapsed()); });

	m_debugWorldObjectsEveryframe.erase(everyFrameWorldObjRemoveStartIterator, m_debugWorldObjectsEveryframe.end());
	
	auto screenObjRemoveStartIterator = std::remove_if(m_debugScreenObjects.begin(), m_debugScreenObjects.end(),
													   [](DebugScreenObject& obj) -> bool { return  (obj.m_duration >= 0.f && obj.m_timer.HasPeriodElapsed()); });

	m_debugScreenObjects.erase(screenObjRemoveStartIterator, m_debugScreenObjects.end());

	auto frameMessageRemoveStartIterator = std::remove_if(m_debugScreenMessagesEveryFrame.begin(), m_debugScreenMessagesEveryFrame.end(),
														  [](DebugScreenObject& obj) -> bool { return (obj.m_duration == 0.f && obj.m_timer.HasPeriodElapsed()); });

	m_debugScreenMessagesEveryFrame.erase(frameMessageRemoveStartIterator, m_debugScreenMessagesEveryFrame.end());

	auto finiteMessageRemoveStartIterator = std::remove_if(m_debugScreenFiniteMessages.begin(), m_debugScreenFiniteMessages.end(),
														   [](DebugScreenObject& obj) -> bool { return (obj.m_duration >= 0.f && obj.m_timer.HasPeriodElapsed());  });

	m_debugScreenFiniteMessages.erase(finiteMessageRemoveStartIterator, m_debugScreenFiniteMessages.end());


}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void DebugRenderWorldEveryFrame(Camera const& camera)
{
	if(m_shouldRender)
	{

		for(int index = 0; index < static_cast<int>(m_debugWorldObjectsEveryframe.size()); ++index)
		{
			if(m_debugWorldObjectsEveryframe[index].m_duration > 0.f)
			{
				Rgba8 color = m_debugWorldObjectsEveryframe[index].m_startColor.ColorLerp(m_debugWorldObjectsEveryframe[index].m_startColor, m_debugWorldObjectsEveryframe[index].m_endColor, static_cast<float>(m_debugWorldObjectsEveryframe[index].m_timer.GetElapsedFraction()));

				for(size_t vertIndex = 0; vertIndex < m_debugWorldObjectsEveryframe[index].m_verts.size(); ++vertIndex)
				{
					m_debugWorldObjectsEveryframe[index].m_verts[vertIndex].m_color = color;
				}
			}

			if(m_debugWorldObjectsEveryframe[index].m_mode == DebugRenderMode::USE_DEPTH)
			{
				m_debugRenderConfig.m_renderer->SetDepthMode(DepthMode::READ_WRITE_LESS_EQUAL);

				if(!m_debugWorldObjectsEveryframe[index].m_isText)
				{
					m_debugRenderConfig.m_renderer->SetModelConstants();
				}
				else
				{
					Mat44 textTransform;

					if(m_debugWorldObjectsEveryframe[index].m_isBillboarded)
					{
						Mat44 targetTransform = camera.m_orientation.GetAsMatrix_IFwd_JLeft_KUp();

						textTransform = GetBillboardTransform(BillboardType::WORLD_UP_OPPOSING, targetTransform, m_debugWorldObjectsEveryframe[index].m_position);

						textTransform.SetTranslation3D(m_debugWorldObjectsEveryframe[index].m_position);
					}
					else
					{
						textTransform = m_debugWorldObjectsEveryframe[index].m_transform;
					}

					Rgba8 color = Rgba8::StaticColorLerp(m_debugWorldObjectsEveryframe[index].m_startColor, m_debugWorldObjectsEveryframe[index].m_endColor, m_debugWorldObjectsEveryframe[index].m_timer.GetElapsedFraction());

					m_debugRenderConfig.m_renderer->SetModelConstants(textTransform, color);
				}
			}


			if(m_debugWorldObjectsEveryframe[index].m_mode == DebugRenderMode::ALWAYS)
			{
				m_debugRenderConfig.m_renderer->SetDepthMode(DepthMode::DISABLED);

				if(!m_debugWorldObjectsEveryframe[index].m_isText)
				{
					m_debugRenderConfig.m_renderer->SetModelConstants();
				}
				else
				{
					Mat44 textTransform;

					if(m_debugWorldObjectsEveryframe[index].m_isBillboarded)
					{
						Mat44 targetTransform = camera.m_orientation.GetAsMatrix_IFwd_JLeft_KUp();

						textTransform = GetBillboardTransform(BillboardType::WORLD_UP_OPPOSING, targetTransform, m_debugWorldObjectsEveryframe[index].m_position);

						textTransform.SetTranslation3D(m_debugWorldObjectsEveryframe[index].m_position);
					}
					else
					{
						textTransform = m_debugWorldObjectsEveryframe[index].m_transform;
					}

					Rgba8 color = Rgba8::StaticColorLerp(m_debugWorldObjectsEveryframe[index].m_startColor, m_debugWorldObjectsEveryframe[index].m_endColor, m_debugWorldObjectsEveryframe[index].m_timer.GetElapsedFraction());

					m_debugRenderConfig.m_renderer->SetModelConstants(textTransform, color);
				}
			}

			if(m_debugWorldObjectsEveryframe[index].m_mode == DebugRenderMode::X_RAY)
			{
				for(size_t vertIndex = 0; vertIndex < m_debugWorldObjectsEveryframe[index].m_verts.size(); ++vertIndex)
				{
					m_debugWorldObjectsEveryframe[index].m_verts[vertIndex].m_color.a = 100;
				}

				m_debugRenderConfig.m_renderer->SetBlendMode(BlendMode::ALPHA);
				m_debugRenderConfig.m_renderer->SetDepthMode(DepthMode::READ_ONLY_LESS_EQUAL);
				m_debugRenderConfig.m_renderer->BindShader(nullptr);
				m_debugRenderConfig.m_renderer->SetModelConstants();
				m_debugRenderConfig.m_renderer->BindTexture(m_debugWorldObjectsEveryframe[index].m_texture);
				m_debugRenderConfig.m_renderer->DrawVertexArray(m_debugWorldObjectsEveryframe[index].m_verts);

				m_debugRenderConfig.m_renderer->SetBlendMode(BlendMode::OPAQUE);
				m_debugRenderConfig.m_renderer->SetDepthMode(DepthMode::READ_WRITE_LESS_EQUAL);

				for(size_t vertIndex = 0; vertIndex < m_debugWorldObjectsEveryframe[index].m_verts.size(); ++vertIndex)
				{
					m_debugWorldObjectsEveryframe[index].m_verts[vertIndex].m_color.a = 255;
				}

				m_debugRenderConfig.m_renderer->SetModelConstants();
			}

			m_debugRenderConfig.m_renderer->SetRasterizerMode(m_debugWorldObjectsEveryframe[index].m_rasterMode);
			m_debugRenderConfig.m_renderer->SetBlendMode(BlendMode::OPAQUE);
			m_debugRenderConfig.m_renderer->BindShader(nullptr);
			m_debugRenderConfig.m_renderer->BindTexture(m_debugWorldObjectsEveryframe[index].m_texture);
			m_debugRenderConfig.m_renderer->DrawVertexArray(m_debugWorldObjectsEveryframe[index].m_verts);
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void DebugRenderWorld(Camera const& camera)
{

	m_debugRenderConfig.m_renderer->BeginCamera(camera);

	DebugRenderWorldEveryFrame(camera);

	if(m_shouldRender)
	{

		for(int index = 0; index < static_cast<int>(m_debugWorldObjects.size()); ++index)
		{
			if(m_debugWorldObjects[index].m_duration > 0.f)
			{
				Rgba8 color = m_debugWorldObjects[index].m_startColor.ColorLerp(m_debugWorldObjects[index].m_startColor, m_debugWorldObjects[index].m_endColor, static_cast<float>(m_debugWorldObjects[index].m_timer.GetElapsedFraction()));

				for(size_t vertIndex = 0; vertIndex < m_debugWorldObjects[index].m_verts.size(); ++vertIndex)
				{
					m_debugWorldObjects[index].m_verts[vertIndex].m_color = color;
				}
			}

			if(m_debugWorldObjects[index].m_mode == DebugRenderMode::USE_DEPTH)
			{
				m_debugRenderConfig.m_renderer->SetDepthMode(DepthMode::READ_WRITE_LESS_EQUAL);

				if(!m_debugWorldObjects[index].m_isText)
				{
					m_debugRenderConfig.m_renderer->SetModelConstants();
				}
				else
				{
					Mat44 textTransform;

					if(m_debugWorldObjects[index].m_isBillboarded)
					{
						Mat44 targetTransform = camera.m_orientation.GetAsMatrix_IFwd_JLeft_KUp();

						textTransform = GetBillboardTransform(BillboardType::WORLD_UP_OPPOSING, targetTransform, m_debugWorldObjects[index].m_position);

						textTransform.SetTranslation3D(m_debugWorldObjects[index].m_position);
					}
					else
					{
						textTransform = m_debugWorldObjects[index].m_transform;
					}

					Rgba8 color = Rgba8::StaticColorLerp(m_debugWorldObjects[index].m_startColor, m_debugWorldObjects[index].m_endColor, m_debugWorldObjects[index].m_timer.GetElapsedFraction());

					m_debugRenderConfig.m_renderer->SetModelConstants(textTransform, color);
				}
			}	
			
			
			if(m_debugWorldObjects[index].m_mode == DebugRenderMode::ALWAYS)
			{
				m_debugRenderConfig.m_renderer->SetDepthMode(DepthMode::DISABLED);

				if(!m_debugWorldObjects[index].m_isText)
				{
					m_debugRenderConfig.m_renderer->SetModelConstants();
				}
				else
				{
					Mat44 textTransform;

					if(m_debugWorldObjects[index].m_isBillboarded)
					{
						Mat44 targetTransform = camera.m_orientation.GetAsMatrix_IFwd_JLeft_KUp();

						textTransform = GetBillboardTransform(BillboardType::WORLD_UP_OPPOSING, targetTransform, m_debugWorldObjects[index].m_position);

						textTransform.SetTranslation3D(m_debugWorldObjects[index].m_position);
					}
					else
					{
						textTransform = m_debugWorldObjects[index].m_transform;
					}

					Rgba8 color = Rgba8::StaticColorLerp(m_debugWorldObjects[index].m_startColor, m_debugWorldObjects[index].m_endColor, m_debugWorldObjects[index].m_timer.GetElapsedFraction());

					m_debugRenderConfig.m_renderer->SetModelConstants(textTransform, color);
				}
			}

			if(m_debugWorldObjects[index].m_mode == DebugRenderMode::X_RAY)
			{
				for(size_t vertIndex = 0; vertIndex < m_debugWorldObjects[index].m_verts.size(); ++vertIndex)
				{
					m_debugWorldObjects[index].m_verts[vertIndex].m_color.a = 100;
				}

				m_debugRenderConfig.m_renderer->SetBlendMode(BlendMode::ALPHA);
				m_debugRenderConfig.m_renderer->SetDepthMode(DepthMode::READ_ONLY_ALWAYS);
				m_debugRenderConfig.m_renderer->SetRasterizerMode(m_debugWorldObjects[index].m_rasterMode);
				m_debugRenderConfig.m_renderer->BindShader(nullptr);
				m_debugRenderConfig.m_renderer->SetModelConstants();
				m_debugRenderConfig.m_renderer->BindTexture(m_debugWorldObjects[index].m_texture);
				m_debugRenderConfig.m_renderer->DrawVertexArray(m_debugWorldObjects[index].m_verts);

				m_debugRenderConfig.m_renderer->SetBlendMode(BlendMode::OPAQUE);
				m_debugRenderConfig.m_renderer->SetDepthMode(DepthMode::READ_WRITE_LESS_EQUAL);

				for(size_t vertIndex = 0; vertIndex < m_debugWorldObjects[index].m_verts.size(); ++vertIndex)
				{
					m_debugWorldObjects[index].m_verts[vertIndex].m_color.a = 255;
				}

				m_debugRenderConfig.m_renderer->SetModelConstants();

			}

			m_debugRenderConfig.m_renderer->SetRasterizerMode(m_debugWorldObjects[index].m_rasterMode);
			m_debugRenderConfig.m_renderer->SetBlendMode(BlendMode::OPAQUE);
			m_debugRenderConfig.m_renderer->BindShader(nullptr);
			m_debugRenderConfig.m_renderer->BindTexture(m_debugWorldObjects[index].m_texture);
			m_debugRenderConfig.m_renderer->DrawVertexArray(m_debugWorldObjects[index].m_verts);
		}

	}

	m_debugRenderConfig.m_renderer->EndCamera(camera);

}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void DebugRenderScreen(Camera const& camera)
{
	m_debugRenderConfig.m_renderer->BeginCamera(camera);

	int textLineNum = 0;


	if(m_shouldRender)
	{
		for(int index = 0; index < static_cast<int>(m_debugScreenObjects.size()); ++index)
		{
			m_debugRenderConfig.m_renderer->SetModelConstants();
			m_debugRenderConfig.m_renderer->SetBlendMode(BlendMode::OPAQUE);
			m_debugRenderConfig.m_renderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);
			m_debugRenderConfig.m_renderer->SetDepthMode(DepthMode::READ_WRITE_LESS_EQUAL);
			m_debugRenderConfig.m_renderer->BindShader(nullptr);
			m_debugRenderConfig.m_renderer->BindTexture(m_debugScreenObjects[index].m_texture);
			m_debugRenderConfig.m_renderer->DrawVertexArray(m_debugScreenObjects[index].m_verts);
		}

		for(int index = static_cast<int>(m_debugScreenMessagesEveryFrame.size()) - 1; index >= 0; --index)
		{

			Mat44 translation = Mat44::MakeTranslation3D(Vec3(10.f, 780.f - 12.f * textLineNum, 0.f));

			textLineNum += 1;

			m_debugRenderConfig.m_renderer->SetModelConstants(translation);
			m_debugRenderConfig.m_renderer->SetBlendMode(BlendMode::OPAQUE);
			m_debugRenderConfig.m_renderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);
			m_debugRenderConfig.m_renderer->SetDepthMode(DepthMode::READ_WRITE_LESS_EQUAL);
			m_debugRenderConfig.m_renderer->BindShader(nullptr);
			m_debugRenderConfig.m_renderer->BindTexture(m_debugScreenMessagesEveryFrame[index].m_texture);
			m_debugRenderConfig.m_renderer->DrawVertexArray(m_debugScreenMessagesEveryFrame[index].m_verts);
		}

		textLineNum = 0;

		for(int index = static_cast<int>(m_debugScreenInfiniteMessages.size()) - 1; index >= 0.f; --index)
		{

			Mat44 translation = Mat44::MakeTranslation3D(Vec3(10.f, 780.f - (12.f * textLineNum) - (12.f * static_cast<int>(m_debugScreenMessagesEveryFrame.size())), 0.f));

			textLineNum += 1;

			m_debugRenderConfig.m_renderer->SetModelConstants(translation);
			m_debugRenderConfig.m_renderer->SetBlendMode(BlendMode::OPAQUE);
			m_debugRenderConfig.m_renderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);
			m_debugRenderConfig.m_renderer->SetDepthMode(DepthMode::READ_WRITE_LESS_EQUAL);
			m_debugRenderConfig.m_renderer->BindShader(nullptr);
			m_debugRenderConfig.m_renderer->BindTexture(m_debugScreenInfiniteMessages[index].m_texture);
			m_debugRenderConfig.m_renderer->DrawVertexArray(m_debugScreenInfiniteMessages[index].m_verts);
		}

		textLineNum = 0;

		for(int index = static_cast<int>(m_debugScreenFiniteMessages.size()) - 1; index >= 0; --index)
		{
			Mat44 translation = Mat44::MakeTranslation3D(Vec3(10.f, 780.f - (12.f * textLineNum) - (12.f * static_cast<int>(m_debugScreenInfiniteMessages.size())) - (12.f * static_cast<int>(m_debugScreenMessagesEveryFrame.size())), 0.f));

			textLineNum += 1;

			m_debugRenderConfig.m_renderer->SetModelConstants(translation);
			m_debugRenderConfig.m_renderer->SetBlendMode(BlendMode::OPAQUE);
			m_debugRenderConfig.m_renderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);
			m_debugRenderConfig.m_renderer->SetDepthMode(DepthMode::READ_WRITE_LESS_EQUAL);
			m_debugRenderConfig.m_renderer->BindShader(nullptr);
			m_debugRenderConfig.m_renderer->BindTexture(m_debugScreenFiniteMessages[index].m_texture);
			m_debugRenderConfig.m_renderer->DrawVertexArray(m_debugScreenFiniteMessages[index].m_verts);
		}
	}

	m_debugRenderConfig.m_renderer->EndCamera(camera);
}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void DebugRenderEndFrame()
{


}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void DebugAddWorldSphere(Vec3 const& position, float radius, float duration, Rgba8 const& startColor, Rgba8 const& endColor, DebugRenderMode mode)
{
	
	DebugWorldObject debugPointObject;
	debugPointObject.m_mode = mode;
	debugPointObject.m_rasterMode = RasterizerMode::SOLID_CULL_BACK;
	debugPointObject.m_startColor = startColor;
	debugPointObject.m_endColor = endColor;
	debugPointObject.m_duration = duration;
	debugPointObject.m_timer = Timer(duration, &Clock::GetSystemClock());
	debugPointObject.m_timer.Start();

	AddVertsForSphere3D(debugPointObject.m_verts, position, radius, debugPointObject.m_startColor, AABB2::ZERO_TO_ONE, 32, 16);

	if(debugPointObject.m_duration != 0.f)
	{
		m_debugWorldObjects.push_back(debugPointObject);
	}
	else
	{
		m_debugWorldObjectsEveryframe.push_back(debugPointObject);
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void DebugAddWorldWireframeSphere(Vec3 const& center, float radius, float duration, Rgba8 const& startColor, Rgba8 const& endColor, DebugRenderMode mode)
{
	DebugWorldObject debugSphereObject;
	debugSphereObject.m_mode = mode;
	debugSphereObject.m_rasterMode = RasterizerMode::WIREFRAME_CULL_BACK;
	debugSphereObject.m_startColor = startColor;
	debugSphereObject.m_endColor = endColor;
	debugSphereObject.m_duration = duration;
	debugSphereObject.m_timer = Timer(duration, &Clock::GetSystemClock());
	debugSphereObject.m_timer.Start();

	AddVertsForSphere3D(debugSphereObject.m_verts, center, radius, debugSphereObject.m_startColor, AABB2::ZERO_TO_ONE, 32, 16);


	if(debugSphereObject.m_duration != 0.f)
	{
		m_debugWorldObjects.push_back(debugSphereObject);
	}
	else
	{
		m_debugWorldObjectsEveryframe.push_back(debugSphereObject);
	}
}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void DebugAddWorldCylinder(Vec3 const& start, float height, float radius, float duration, Rgba8 const& startColor, Rgba8 const& endColor, DebugRenderMode mode)
{

	DebugWorldObject debugCylinderObject;
	debugCylinderObject.m_mode = mode;
	debugCylinderObject.m_rasterMode = RasterizerMode::SOLID_CULL_BACK;
	debugCylinderObject.m_startColor = startColor;
	debugCylinderObject.m_endColor = endColor;
	debugCylinderObject.m_duration = duration;
	debugCylinderObject.m_timer = Timer(duration, &Clock::GetSystemClock());
	debugCylinderObject.m_timer.Start();

	AddVertsForCylinder3D(debugCylinderObject.m_verts, height, radius, debugCylinderObject.m_startColor, AABB2::ZERO_TO_ONE, 16);

	Vec3 endPosition = start + height * Vec3(0.f, 0.f, 1.f);

	Mat44 lookAtMatrix = GetLookAtTransform(start, endPosition);

	Mat44 translationMatrix = Mat44::MakeTranslation3D(start);

	lookAtMatrix.SetTranslation3D(start);

	TransformVertexArray3D(debugCylinderObject.m_verts, lookAtMatrix);


	if(debugCylinderObject.m_duration != 0.f)
	{
		m_debugWorldObjects.push_back(debugCylinderObject);
	}
	else
	{
		m_debugWorldObjectsEveryframe.push_back(debugCylinderObject);
	}

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void DebugAddWorldWireframeCylinder(Vec3 const& start, float height, float radius, float duration, Rgba8 const& startColor, Rgba8 const& endColor, DebugRenderMode mode)
{
	DebugWorldObject debugCylinderObject;
	debugCylinderObject.m_mode = mode;
	debugCylinderObject.m_rasterMode = RasterizerMode::WIREFRAME_CULL_BACK;
	debugCylinderObject.m_startColor = startColor;
	debugCylinderObject.m_endColor = endColor;
	debugCylinderObject.m_duration = duration;
	debugCylinderObject.m_timer = Timer(duration, &Clock::GetSystemClock());
	debugCylinderObject.m_timer.Start();

	AddVertsForCylinder3D(debugCylinderObject.m_verts, height, radius, debugCylinderObject.m_startColor, AABB2::ZERO_TO_ONE, 16);

	Vec3 endPosition = start + height * Vec3(0.f, 0.f, 1.f);

	Mat44 lookAtMatrix = GetLookAtTransform(start, endPosition);

	Mat44 translationMatrix = Mat44::MakeTranslation3D(start);

	lookAtMatrix.SetTranslation3D(start);

	TransformVertexArray3D(debugCylinderObject.m_verts, lookAtMatrix);


	if(debugCylinderObject.m_duration != 0.f)
	{
		m_debugWorldObjects.push_back(debugCylinderObject);
	}
	else
	{
		m_debugWorldObjectsEveryframe.push_back(debugCylinderObject);
	}

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void DebugAddWorldLine(Vec3 const& start, Vec3 const& fwdNormal, float height, float radius, float duration, Rgba8 const& color, DebugRenderMode mode)
{

	DebugWorldObject debugLineObject;
	debugLineObject.m_mode = mode;
	debugLineObject.m_rasterMode = RasterizerMode::SOLID_CULL_BACK;
	debugLineObject.m_startColor = color;
	debugLineObject.m_endColor = color;
	debugLineObject.m_duration = duration;
	debugLineObject.m_timer = Timer(duration, &Clock::GetSystemClock());
	debugLineObject.m_timer.Start();

	AddVertsForCylinder3D(debugLineObject.m_verts, height, radius, debugLineObject.m_startColor, AABB2::ZERO_TO_ONE, 16);

	Vec3 endPosition = start + height * fwdNormal;

	Mat44 lookAtMatrix = GetLookAtTransform(start, endPosition);

	lookAtMatrix.SetTranslation3D(start);

	TransformVertexArray3D(debugLineObject.m_verts, lookAtMatrix);


	if(debugLineObject.m_duration != 0.f)
	{
		m_debugWorldObjects.push_back(debugLineObject);
	}
	else
	{
		m_debugWorldObjectsEveryframe.push_back(debugLineObject);
	}

}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void DebugAddWorldWireframeArrow(Vec3 const& start, Vec3 const& fwdNormal, float height, float radius, float duration, Rgba8 const& color, DebugRenderMode mode)
{

	DebugWorldObject debugArrowObject;
	debugArrowObject.m_mode = mode;
	debugArrowObject.m_rasterMode = RasterizerMode::WIREFRAME_CULL_BACK;
	debugArrowObject.m_startColor = color;
	debugArrowObject.m_endColor = color;
	debugArrowObject.m_duration = duration;
	debugArrowObject.m_timer = Timer(duration, &Clock::GetSystemClock());
	debugArrowObject.m_timer.Start();

	AddVertsForArrow3D(debugArrowObject.m_verts, start, fwdNormal, height, radius, debugArrowObject.m_startColor, AABB2::ZERO_TO_ONE, 32);


	if(debugArrowObject.m_duration != 0.f)
	{
		m_debugWorldObjects.push_back(debugArrowObject);
	}
	else
	{
		m_debugWorldObjectsEveryframe.push_back(debugArrowObject);
	}

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void DebugAddWorldArrow(Vec3 const& start, Vec3 const& fwdNormal, float height, float radius, float duration, Rgba8 const& color, DebugRenderMode mode)
{
	DebugWorldObject debugArrowObject;
	debugArrowObject.m_mode = mode;
	debugArrowObject.m_rasterMode = RasterizerMode::SOLID_CULL_BACK;
	debugArrowObject.m_startColor = color;
	debugArrowObject.m_endColor = color;
	debugArrowObject.m_duration = duration;
	debugArrowObject.m_timer = Timer(duration, &Clock::GetSystemClock());
	debugArrowObject.m_timer.Start();

	AddVertsForArrow3D(debugArrowObject.m_verts, start, fwdNormal, height, radius, debugArrowObject.m_startColor, AABB2::ZERO_TO_ONE, 32);


	if(debugArrowObject.m_duration != 0.f)
	{
		m_debugWorldObjects.push_back(debugArrowObject);
	}
	else
	{
		m_debugWorldObjectsEveryframe.push_back(debugArrowObject);
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void DebugAddBasis(Mat44 const& transform, float duration, float scale,  DebugRenderMode mode)
{
	Vec3 startPos = transform.GetTranslation3D();
	Vec3 fwdNormal = transform.GetIBasis3D();
	Vec3 leftNormal = transform.GetJBasis3D();
	Vec3 upNormal = transform.GetKBasis3D();

	DebugAddWorldArrow(startPos, fwdNormal, 2.f * scale, 0.1f * scale, duration, Rgba8::RED, mode);
	DebugAddWorldArrow(startPos, leftNormal, 2.f * scale, 0.1f * scale, duration, Rgba8::GREEN, mode);
	DebugAddWorldArrow(startPos, upNormal, 2.f * scale, 0.1f * scale, duration, Rgba8::BLUE, mode);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void DebugAddWorldBasis(float duration, DebugRenderMode mode)
{
	Vec3 startPos = Vec3(0.f, 0.f, 0.f);
	Vec3 fwdNormal = Vec3(1.f, 0.f, 0.f);
	Vec3 leftNormal = Vec3(0.f, 1.f, 0.f);
	Vec3 upNormal = Vec3(0.f, 0.f, 1.f);

	DebugAddWorldArrow(startPos, fwdNormal, 1.5f, 0.15f, duration, Rgba8::RED, mode);
	DebugAddWorldArrow(startPos, leftNormal, 1.5f, 0.15f, duration, Rgba8::GREEN, mode);
	DebugAddWorldArrow(startPos, upNormal, 1.5f, 0.15f, duration, Rgba8::BLUE, mode);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void DebugAddWorldText(std::string const& text, Mat44 const& transform, float textHeight, Vec2 const& alignment, float duration, Rgba8 const& startColor, Rgba8 const& endColor, DebugRenderMode mode)
{
	BitmapFont* textFont = m_debugRenderConfig.m_renderer->CreateOrGetBitmapFontWithFontName("Font");

	DebugWorldObject billboardText;
	billboardText.m_mode = mode;
	billboardText.m_rasterMode = RasterizerMode::SOLID_CULL_NONE;
	billboardText.m_startColor = startColor;
	billboardText.m_endColor = endColor;
	billboardText.m_duration = duration;
	billboardText.m_timer = Timer(duration, &Clock::GetSystemClock());
	billboardText.m_timer.Start();
	billboardText.m_isText = true;
	billboardText.m_isBillboarded = false;
	billboardText.m_transform = transform;
	billboardText.m_texture = &textFont->GetTexture();

	textFont->AddVertsForText3DAtOriginXForward(billboardText.m_verts, textHeight, text, billboardText.m_startColor, 1.f, alignment);

	if(billboardText.m_duration != 0.f)
	{
		m_debugWorldObjects.push_back(billboardText);
	}
	else
	{
		m_debugWorldObjectsEveryframe.push_back(billboardText);
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void DebugAddWorldBillboardText(std::string const& text, Vec3 const& position, float textHeight, Vec2 const& alignment, float duration, Rgba8 const& startColor, Rgba8 const& endColor, DebugRenderMode mode)
{
	BitmapFont* textFont = m_debugRenderConfig.m_renderer->CreateOrGetBitmapFontWithFontName("Font");

	DebugWorldObject billboardText;
	billboardText.m_mode = mode;
	billboardText.m_rasterMode = RasterizerMode::SOLID_CULL_NONE;
	billboardText.m_startColor = startColor;
	billboardText.m_endColor = endColor;
	billboardText.m_duration = duration;
	billboardText.m_timer = Timer(duration, &Clock::GetSystemClock());
	billboardText.m_timer.Start();
	billboardText.m_isText = true;
	billboardText.m_isBillboarded = true;
	billboardText.m_position = position;
	billboardText.m_texture = &textFont->GetTexture();

	textFont->AddVertsForText3DAtOriginXForward(billboardText.m_verts, textHeight, text, billboardText.m_startColor, 1.f, alignment);

	if(billboardText.m_duration != 0.f)
	{
		m_debugWorldObjects.push_back(billboardText);
	}
	else
	{
		m_debugWorldObjectsEveryframe.push_back(billboardText);
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void DebugAddScreenText(std::string text, AABB2 textBox, float cellHeight, Vec2 alignment, float duration, Rgba8 color)
{

	BitmapFont* textFont = m_debugRenderConfig.m_renderer->CreateOrGetBitmapFontWithFontName("Font");

	DebugScreenObject screenText;
	screenText.m_startColor = color;
	screenText.m_endColor = color;
	screenText.m_texture = &textFont->GetTexture();
	screenText.m_duration = duration;
	screenText.m_timer = Timer(duration, &Clock::GetSystemClock());
	screenText.m_timer.Start();

	textFont->AddVertsForTextInBox2D(screenText.m_verts, text, textBox, cellHeight, color, 1.f, alignment, SHRINK_TO_FIT);
	
	m_debugScreenObjects.push_back(screenText);

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void DebugAddMessage(std::string text, float duration, Rgba8 color)
{	
 	BitmapFont* textFont = m_debugRenderConfig.m_renderer->CreateOrGetBitmapFontWithFontName("Font");

	DebugScreenObject screenText;
	screenText.m_startColor = color;
	screenText.m_endColor = color;
	screenText.m_texture = &textFont->GetTexture();
	screenText.m_text = text;
	screenText.m_timer = Timer(duration, &Clock::GetSystemClock());
	screenText.m_duration = duration;
	screenText.m_timer.Start();

	float width = textFont->GetTextWidth(12.f, text);
	AABB2 textBox;
	textBox.m_mins = Vec2(0.f, 0.f);
	textBox.m_maxs = Vec2(textBox.m_mins.x + width, textBox.m_mins.y + 12.f);
	textFont->AddVertsForTextInBox2D(screenText.m_verts, text, textBox, 12.f, color, 1.f);

	if(duration > 0.f)
	{
		m_debugScreenFiniteMessages.push_back(screenText);
	}
	if(duration < 0.f)
	{
		m_debugScreenInfiniteMessages.push_back(screenText);
	}	
	if(duration == 0.f)
	{
		m_debugScreenMessagesEveryFrame.push_back(screenText);
	}

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool OnDebugRenderClear(EventArgs& args)
{
	UNUSED(args);

	DebugRenderClear();

	return false;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool OnDebugRenderToggle(EventArgs& args)
{
	UNUSED(args);

	if(m_shouldRender)
	{
		DebugRenderSetHidden();
	}
	else
	{
		DebugRenderSetVisible();
	}

	return false;
}
