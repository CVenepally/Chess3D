#pragma once

#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Timer.hpp"
#include "Engine/Core/EngineCommon.hpp"

#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Math/Cylinder3D.hpp"
#include "Engine/Math/Sphere.hpp"

#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Window/Window.hpp"
#include "Engine/Core/DevConsole.hpp"

class App;

extern App* g_theApp;
extern Renderer* g_theRenderer;
extern InputSystem* g_inputSystem;
extern AudioSystem* g_theAudioSystem;
extern Window* g_theWindow;
extern BitmapFont* g_gameFont;


constexpr int NUM_PLAYER_ONE_PAWNS = 8;
constexpr int NUM_PLAYER_TWO_PAWNS = 8;

constexpr int NUM_PLAYER_ONE_ROOKS = 2;
constexpr int NUM_PLAYER_TWO_ROOKS = 2;

constexpr int NUM_PLAYER_ONE_KNIGHTS = 2;
constexpr int NUM_PLAYER_TWO_KNIGHTS = 2;

constexpr int NUM_PLAYER_ONE_BISHOPS = 2;
constexpr int NUM_PLAYER_TWO_BISHOPS = 2;

constexpr int NUM_PLAYER_ONE_KINGS = 1;
constexpr int NUM_PLAYER_TWO_KINGS = 1;

constexpr int NUM_PLAYER_ONE_QUEENS = 1;
constexpr int NUM_PLAYER_TWO_QUEENS = 1;