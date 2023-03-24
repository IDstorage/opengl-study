#pragma once

#include <string>

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_glfw.h"
#include "ImGui/imgui_impl_opengl3.h"

class GLFWwindow;

namespace ids {
	class EditorGUI {
	public:
		static void initialize(GLFWwindow* window, bool docking, const std::string& version = "#version 130");

		static void startNewFrame();
		static void render();
		
		static void release();
	};
}