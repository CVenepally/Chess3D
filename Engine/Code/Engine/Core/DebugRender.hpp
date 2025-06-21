#pragma once
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/EngineCommon.hpp"

#include <string>


class Renderer;
class Camera;
struct Vec2;
struct Vec3;
struct AABB2;
struct Rgba8;
struct Mat44;

enum class DebugRenderMode
{
	ALWAYS,
	USE_DEPTH,
	X_RAY
};


struct DegbugRenderConfig
{
	Renderer* m_renderer = nullptr;
	std::string m_fontName = "Font";
};

void DebugRenderSystemStartup(DegbugRenderConfig& config);
void DebugRenderSystemShutdown();

void DebugRenderSetVisible();
void DebugRenderSetHidden();
void DebugRenderClear();

void DebugRenderBeginFrame();
void DebugRenderWorld(Camera const& camera);
void DebugRenderScreen(Camera const& camera);
void DebugRenderEndFrame();

void DebugAddWorldSphere(Vec3 const& center, float radius, float duration, Rgba8 const& startColor = Rgba8::WHITE, Rgba8 const& endColor = Rgba8::WHITE, DebugRenderMode mode = DebugRenderMode::USE_DEPTH);
void DebugAddWorldWireframeSphere(Vec3 const& center, float radius, float duration, Rgba8 const& startColor = Rgba8::WHITE, Rgba8 const& endColor = Rgba8::WHITE, DebugRenderMode mode = DebugRenderMode::USE_DEPTH);

void DebugAddWorldCylinder(Vec3 const& start, float height, float radius, float duration, Rgba8 const& startColor, Rgba8 const& endColor, DebugRenderMode mode);
void DebugAddWorldWireframeCylinder(Vec3 const& start, float height, float radius, float duration, Rgba8 const& startColor, Rgba8 const& endColor, DebugRenderMode mode);

void DebugAddWorldLine(Vec3 const& start, Vec3 const& fwdNormal, float height, float radius, float duration, Rgba8 const& color, DebugRenderMode mode = DebugRenderMode::X_RAY);

void DebugAddWorldArrow(Vec3 const& start, Vec3 const& fwdNormal, float height, float radius, float duration, Rgba8 const& color, DebugRenderMode mode = DebugRenderMode::USE_DEPTH);
void DebugAddWorldWireframeArrow(Vec3 const& start, Vec3 const& fwdNormal, float height, float radius, float duration, Rgba8 const& color, DebugRenderMode mode = DebugRenderMode::USE_DEPTH);

void DebugAddBasis(Mat44 const& transform, float duration, float scale = 1.0f, DebugRenderMode mode = DebugRenderMode::USE_DEPTH);
void DebugAddWorldBasis(float duration, DebugRenderMode mode = DebugRenderMode::USE_DEPTH);

void DebugAddWorldText(std::string const& text, Mat44 const& transform, float textHeight, Vec2 const& alignment, float duration, Rgba8 const& startColor, Rgba8 const& endColor, DebugRenderMode mode = DebugRenderMode::USE_DEPTH);
void DebugAddWorldBillboardText(std::string const& text, Vec3 const& position, float textHeight, Vec2 const& alignment, float duration, Rgba8 const& startColor, Rgba8 const& endColor, DebugRenderMode mode = DebugRenderMode::USE_DEPTH);

void DebugAddScreenText(std::string text, AABB2 textBox, float cellHeight, Vec2 alignment, float duration, Rgba8 color = Rgba8::WHITE);
void DebugAddMessage(std::string text, float duration, Rgba8 color = Rgba8::WHITE);


bool OnDebugRenderClear(EventArgs& args);
bool OnDebugRenderToggle(EventArgs& args);