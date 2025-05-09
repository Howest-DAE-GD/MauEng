#include "PCH.h"

#include "DebugDrawingDemo.h"
#include <iostream>

#include "GameTime.h"
#include <glm/glm.hpp>

namespace MauGam
{
	DebugDrawingScene::DebugDrawingScene()
	{
		ME_PROFILE_FUNCTION()

		m_CameraManager.GetActiveCamera().SetPosition(glm::vec3{ 0.f, 2, 4 });
		m_CameraManager.GetActiveCamera().SetFOV(60.f);

		m_CameraManager.GetActiveCamera().Focus({ 0,0,1 });

		// Setup input
		auto& input{ INPUT_MANAGER };
		input.BindAction("MoveUp", MauEng::KeyInfo{ SDLK_UP, MauEng::KeyInfo::ActionType::Held });
		input.BindAction("MoveLeft", MauEng::KeyInfo{ SDLK_LEFT, MauEng::KeyInfo::ActionType::Held });
		input.BindAction("MoveRight", MauEng::KeyInfo{ SDLK_RIGHT, MauEng::KeyInfo::ActionType::Held });
		input.BindAction("MoveDown", MauEng::KeyInfo{ SDLK_DOWN, MauEng::KeyInfo::ActionType::Held });

		input.BindAction("RotUp", MauEng::KeyInfo{ SDLK_I, MauEng::KeyInfo::ActionType::Held });
		input.BindAction("RotLeft", MauEng::KeyInfo{ SDLK_J, MauEng::KeyInfo::ActionType::Held });
		input.BindAction("RotRight", MauEng::KeyInfo{ SDLK_L, MauEng::KeyInfo::ActionType::Held });
		input.BindAction("RotDown", MauEng::KeyInfo{ SDLK_K, MauEng::KeyInfo::ActionType::Held });


		input.BindAction("Rotate", MauEng::MouseInfo{ {},   MauEng::MouseInfo::ActionType::Moved });
	}

	void DebugDrawingScene::OnLoad()
	{
		ME_PROFILE_FUNCTION()

			Scene::OnLoad();

		LOGGER.Log(MauCor::LogPriority::Info, MauCor::LogCategory::Game, "Demo Scene Loaded! ");
	}

	void DebugDrawingScene::Tick()
	{
		ME_PROFILE_FUNCTION()

		Scene::Tick();

		auto const& input{ INPUT_MANAGER };

		auto constexpr movementSpeed{ 20.f };
		if (input.IsActionExecuted("MoveUp"))
		{
			m_CameraManager.GetActiveCamera().Translate({ 0.f, 0.f, movementSpeed * TIME.ElapsedSec() });
		}
		if (input.IsActionExecuted("MoveDown"))
		{
			m_CameraManager.GetActiveCamera().Translate({ 0.f, 0.f, -movementSpeed * TIME.ElapsedSec() });
		}
		if (input.IsActionExecuted("MoveLeft"))
		{
			m_CameraManager.GetActiveCamera().Translate({ -movementSpeed * TIME.ElapsedSec(), 0.f, 0.f });
		}
		if (input.IsActionExecuted("MoveRight"))
		{
			m_CameraManager.GetActiveCamera().Translate({ movementSpeed * TIME.ElapsedSec(), 0.f, 0.f });
		}

		float constexpr keyboardRotSpeed{ 10 };
		if (input.IsActionExecuted("RotLeft"))
		{
			float const rot{ -keyboardRotSpeed * TIME.ElapsedSec() * 3 };
			m_CameraManager.GetActiveCamera().RotateX(rot);
		}
		if (input.IsActionExecuted("RotRight"))
		{
			float const rot{ keyboardRotSpeed * TIME.ElapsedSec() * 3 };
			m_CameraManager.GetActiveCamera().RotateX(rot);
		}
		if (input.IsActionExecuted("RotUp"))
		{
			float const rot{ keyboardRotSpeed * TIME.ElapsedSec() };
			m_CameraManager.GetActiveCamera().RotateY(rot);
		}
		if (input.IsActionExecuted("RotDown"))
		{
			float const rot{ -keyboardRotSpeed * TIME.ElapsedSec() };
			m_CameraManager.GetActiveCamera().RotateY(rot);
		}

		float constexpr mouseRotSpeed{ 60 };
		if (input.IsActionExecuted("Rotate"))
		{
			auto const mouseMovement{ input.GetDeltaMouseMovement() };
			float const rot{ mouseRotSpeed * TIME.ElapsedSec() };

			m_CameraManager.GetActiveCamera().RotateX(mouseMovement.first * rot);
			m_CameraManager.GetActiveCamera().RotateY(-mouseMovement.second * rot);
		}
	}

	void DebugDrawingScene::OnRender() const
	{
		Scene::OnRender();

		DrawDebug();
	}

	void DebugDrawingScene::DrawDebug() const
	{
		ME_PROFILE_FUNCTION()

		// Config
		bool constexpr DRAW_LINES{ false };
		bool constexpr DRAW_RECTS{ false };
		bool constexpr DRAW_TRIANGLES{ false };
		bool constexpr DRAW_ARROWS{ false };
		bool constexpr DRAW_CIRCLES{ false };
		bool constexpr DRAW_SPHERES{ true };
		bool constexpr DRAW_CYL{ false };
		bool constexpr DRAW_POLY{ false };

		// Demo debug drawing tests
		if constexpr (DRAW_LINES)
		{
			DEBUG_RENDERER.DrawLine({ 0, 0,0 }, { 100, 10, 10 }, { 0, 0, 0 });
			DEBUG_RENDERER.DrawLine({ 0, 0,0 }, { 100, 10, 10 }, { 90, 0, 0 }, { 0, 1, 0 });
			DEBUG_RENDERER.DrawLine({ 0, 0,0 }, { 100, 10, 10 }, { 180, 0, 0 }, { 0, 1, 0 });
			DEBUG_RENDERER.DrawLine({ 0, 0,0 }, { 100, 10, 10 }, { 0, 90, 0 }, { 0, 0, 1 });
			DEBUG_RENDERER.DrawLine({ 0, 0,0 }, { 100, 10, 10 }, { 0, 180, 0 }, { 0, 0, 1 });
			DEBUG_RENDERER.DrawLine({ 0, 0,0 }, { 100, 10, 10 }, { 0, 0, 90 }, { 0, 1, 1 });
			DEBUG_RENDERER.DrawLine({ 0, 0,0 }, { 100, 10, 10 }, { 0, 0, 180 }, { 0, 1, 1 });

			static float constexpr ROT_SPEED{ 10.f };
			static float lineRot{};
			DEBUG_RENDERER.DrawLine({ 0, 0,0 }, { 100, 0, 0 }, { lineRot, lineRot, lineRot }, { 1, 1, 1 });

			DEBUG_RENDERER.DrawLine({ 0, 0,0 }, { 10, 0, 0 }, { 0, lineRot, 0 }, { 1, 1, 0 });
			DEBUG_RENDERER.DrawLine({ 0, 0,0 }, { 10, 0, 0 }, { 0, lineRot, 0 }, { 1, 1, 1 }, true, { 100, 100, 100 });
			DEBUG_RENDERER.DrawLine({ 0, 0,0 }, { 10, 0, 0 }, { 0, 0, lineRot }, { 1, 1, 0 });

			lineRot += ROT_SPEED * TIME.ElapsedSec();
		}

		if constexpr (DRAW_RECTS)
		{
			static float constexpr CUBE_ROT_SPEED{ 10.f };
			static float cubeRot{};

			DEBUG_RENDERER.DrawRect({ 0, 0, 0 }, { 20, 30 }, { 0, 0, cubeRot }, { 1, 0, 0 });
			DEBUG_RENDERER.DrawRect({ 0, 0, 0 }, { 20, 30 }, { 0, 0, 0 }, { 1, 1, 1 });
			DEBUG_RENDERER.DrawCube({ 20, 20, 20 }, { 69, 20, 20 }, { 0, 0, 0 }, { 1, 1, 1 });
			DEBUG_RENDERER.DrawCube({ 20, 20, 20 }, { 69, 20, 20 }, { 0, cubeRot, 0 });

			DEBUG_RENDERER.DrawRect({ 0, 0, 0 }, { 20, 30 }, { 0, 0, cubeRot }, { 0, 1, 1 }, true, { 20, 20, 20 });
			DEBUG_RENDERER.DrawCube({ 20, 20, 20 }, { 69, 20, 20 }, { 0, cubeRot, 0 }, { 1, 1,0 }, true, { 0, 0, 0 });


			cubeRot += CUBE_ROT_SPEED * TIME.ElapsedSec();
		}

		if constexpr (DRAW_TRIANGLES)
		{
			static float constexpr TRIANGLE_ROT_SPEED{ 10.f };
			static float triRot{};

			DEBUG_RENDERER.DrawTriangle({ 0,0,0 }, { 10, 10, 10 }, { 20, 10, 16 });
			DEBUG_RENDERER.DrawTriangle({ 0,0,0 }, { 10, 10, 10 }, { 20, 10, 16 }, { 0, triRot, triRot }, { 1, 1, 1 });

			DEBUG_RENDERER.DrawTriangle({ 0,0,0 }, { 10, 10, 10 }, { 20, 10, 16 }, { 0, triRot, triRot }, { 1, 1, 1 }, true, { 25,25, 0 });


			triRot += TRIANGLE_ROT_SPEED * TIME.ElapsedSec();
		}

		if constexpr (DRAW_ARROWS)
		{
			static float constexpr ARROW_ROT_SPEED{ 10.f };
			static float arrRot{};

			DEBUG_RENDERER.DrawArrow({}, { 2, 3, 3 });
			DEBUG_RENDERER.DrawArrow({}, { 2, 3, 3 }, { 0, arrRot, 0 }, { 1, 1, 1 });

			DEBUG_RENDERER.DrawArrow({}, { 0, 0, 3 });
			DEBUG_RENDERER.DrawArrow({}, { 0, 0, 3 }, { arrRot,0 , 0 }, { 1, 1, 1 });

			DEBUG_RENDERER.DrawArrow({}, { 0, 0, 3 }, { arrRot,0 , 0 }, { 1, 1, 1 }, 1, true, { 20, 20, 0 });

			arrRot += ARROW_ROT_SPEED * TIME.ElapsedSec();
		}

		if constexpr (DRAW_CIRCLES)
		{
			static float constexpr CIRCLE_ROT_SPEED{ 10.f };
			static float circleRot{};

			DEBUG_RENDERER.DrawCircle({}, 20);
			DEBUG_RENDERER.DrawCircle({}, 20, { circleRot, 0, 0 }, { 1, 1, 1 });

			DEBUG_RENDERER.DrawEllipse({}, { 100, 10 }, {}, { 0, 1, 0 });
			DEBUG_RENDERER.DrawEllipse({}, { 100, 10 }, { circleRot }, { 1, 1, 1 });

			DEBUG_RENDERER.DrawEllipse({}, { 100, 10 }, { circleRot }, { 1, 1, 1 }, 24, true, { 20, 20, 0 });

			circleRot += CIRCLE_ROT_SPEED * TIME.ElapsedSec();
		}

		if constexpr (DRAW_SPHERES)
		{
			static float constexpr SPHERE_ROT_SPEED{ 10.f };
			static float sphereRot{};

			DEBUG_RENDERER.DrawSphere({}, 20.f);
			DEBUG_RENDERER.DrawSphere({}, 20.f, sphereRot, { 0, 0,1 });

			DEBUG_RENDERER.DrawSphere({ -10, -10, -10 }, 20.f, {}, { 1, 1, 0 });
			DEBUG_RENDERER.DrawSphere({ -10, -10, -10 }, 20.f, sphereRot, { 1,1,1 });

			DEBUG_RENDERER.DrawSphereComplex({ 20,20,20 }, 20.f, { }, { 1, 0, 0 }, 24, 6);
			DEBUG_RENDERER.DrawSphereComplex({ 20,20,20 }, 20.f, { sphereRot }, { 1, 1, 1 }, 24, 6);

			DEBUG_RENDERER.DrawEllipsoid({ -30, -30, 0 }, { 10, 20, 20 }, {});
			DEBUG_RENDERER.DrawEllipsoid({ -30, -30, 0 }, { 10, 20, 20 }, { sphereRot }, { 1, 1, 1 });

			DEBUG_RENDERER.DrawEllipsoidComplex({ 0, 0, 50 }, { 20, 50, 20 }, {}, { 1, 0, 0 }, 100, 30);
			DEBUG_RENDERER.DrawEllipsoidComplex({ 0, 0, 50 }, { 20, 50, 20 }, { sphereRot }, { 1, 1, 1 }, 100, 30);

			DEBUG_RENDERER.DrawSphere({ 20, 20, 20 }, 1, {}, { .5, .5, 1 });

			DEBUG_RENDERER.DrawEllipsoidComplex({ 0, 0, -50 }, { 20, 50, 20 }, {}, { 1, 0, 1 }, 100, 30, false);
			DEBUG_RENDERER.DrawEllipsoidComplex({ 0, 0, -50 }, { 20, 50, 20 }, {}, { 1, 1, 1 }, 100, 30, true);
			DEBUG_RENDERER.DrawEllipsoidComplex({ 0, 0, -50 }, { 20, 50, 20 }, {}, { 1, 1, 1 }, 100, 30, true, { 20, 20, 20 });
			DEBUG_RENDERER.DrawEllipsoidComplex({ 0, 0, -50 }, { 20, 50, 20 }, { 0, sphereRot }, { 1, 1, 0 }, 100, 30, true, { 20, 20, 20 });


			sphereRot += SPHERE_ROT_SPEED * TIME.ElapsedSec();
		}

		if constexpr (DRAW_CYL)
		{
			static float constexpr CYL_ROT_SPEED{ 10.f };
			static float cylRot{};

			DEBUG_RENDERER.DrawCylinder({ -10, 0, -10 }, { 20, 100, 20 });
			DEBUG_RENDERER.DrawCylinder({ -10, 0, -10 }, { 20, 100, 20 }, { cylRot, cylRot, cylRot }, { 1, 1, 1 });
			DEBUG_RENDERER.DrawCylinder({ -10, 0, -10 }, { 20, 100, 20 }, { cylRot, cylRot, cylRot }, { 0, 1, 0 }, 24, true, { 20, 20, 20 });
			DEBUG_RENDERER.DrawCylinder({ -10, 0, -10 }, { 20, 100, 20 }, {}, { 0, 1, 1 }, 24, true, { 20, 20, 20 });

			cylRot += CYL_ROT_SPEED * TIME.ElapsedSec();
		}

		if constexpr (DRAW_POLY)
		{
			static float constexpr POL_ROT_SPEED{ 10.f };
			static float polRot{};

			DEBUG_RENDERER.DrawSphere({}, 1);

			DEBUG_RENDERER.DrawPolygon({ {20, 0, 0}, { 30, 0, 0 }, {30, 10, 0}, {20, 10, 0} });
			DEBUG_RENDERER.DrawPolygon({ {20, 0, 0}, { 30, 0,0 }, {30, 10, 0}, {20, 10, 0} }, { 0, polRot }, { 1, 1, 1 });

			DEBUG_RENDERER.DrawPolygon({ {20, 0, 0}, { 30, 0, 0  }, {30, 10, 0 }, {20, 10, 0} }, { 0, polRot }, { 1, 1, 1 }, true);


			polRot += POL_ROT_SPEED * TIME.ElapsedSec();
		}
	}
}
