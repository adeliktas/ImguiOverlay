#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <map>
#include <iomanip>      // std::setprecision
#include <chrono>
#include <thread>

#include <X11/Xlib.h>
#include <X11/keysym.h> //XK_Shift_L, see /usr/include/X11/keysymdef.h

//imgui
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
//#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers
#include <unistd.h>  // sleep()
#include <pthread.h> 
#include <stdio.h>   
#include <stdlib.h>  // EXIT_SUCCESS
#include <string.h>  // strerror() 
#include <errno.h>

#include <regex>

bool key_is_pressed(KeySym ks) { //@param ks  like XK_Shift_L, see /usr/include/X11/keysymdef.h
    Display *dpy = XOpenDisplay(":0");
    char keys_return[32];
    XQueryKeymap(dpy, keys_return);
    KeyCode kc2 = XKeysymToKeycode(dpy, ks);
    bool isPressed = !!(keys_return[kc2 >> 3] & (1 << (kc2 & 7)));
    XCloseDisplay(dpy);
    return isPressed;
}

static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

namespace visuals
{
	void StrokeText(int x, int y, ImColor color, std::string str)
	{
		ImDrawList *gui = ImGui::GetBackgroundDrawList();
		ImFont a;
		gui->AddText(ImVec2(x, y - 1), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), str.c_str());
		gui->AddText(ImVec2(x, y + 1), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), str.c_str());
		gui->AddText(ImVec2(x - 1, y), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), str.c_str());
		gui->AddText(ImVec2(x + 1, y), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), str.c_str());
		gui->AddText(ImVec2(x, y), color, str.c_str());
	}

	void FilledRect(int x, int y, int w, int h, ImColor color)
	{
		ImDrawList *gui = ImGui::GetBackgroundDrawList();
		gui->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), color, 0, 0);
	}

	void Line(ImVec2 start, ImVec2 end, ImU32 color, float thickness)
	{
		ImDrawList *gui = ImGui::GetBackgroundDrawList();
		gui->AddLine(start, end, color, thickness);
	}
}

void Crosshair( )
{
	float midX = 1920 / 2;
	float midY = 1080 / 2;

	visuals::Line( { midX, ( midY - 8 ) }, { midX, ( midY - 4 ) }, ImColor( 255, 255, 255 ), 1.f);
	visuals::Line( { ( midX - 8 ), midY }, { ( midX - 4 ), midY }, ImColor( 255, 255, 255 ), 1.f);

	visuals::Line( { midX, ( midY + 8 ) }, { midX, ( midY + 4 ) }, ImColor( 255, 255, 255 ), 1.f);
	visuals::Line( { ( midX + 8 ), midY }, { ( midX + 4 ), midY }, ImColor( 255, 255, 255 ), 1.f);
}

int main(int argc, char *argv[]) {
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3.4);
    
    glfwWindowHint(GLFW_DECORATED, 0); //windowed mode window will have window decorations such as a border, a close widget, etc.
    glfwWindowHint(GLFW_RESIZABLE, 0);
    glfwWindowHint(GLFW_FLOATING, 1); //topmost or always-on-top
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, 1); //framebuffer background will be transparent
    
    //make sure to have right glfw lib or upstream linked in build otherwise GLFW Error 65539: Invalid window attribute 0x0002000D
    glfwWindowHint(GLFW_MOUSE_PASSTHROUGH, 1); //https://www.glfw.org/docs/3.4/group__window.html
    
    glfwWindowHint(GLFW_FOCUSED, 0); //input focus when created
    glfwWindowHint(GLFW_FOCUS_ON_SHOW, 0); //input focus when glfwShowWindow is called
    glfwWindowHint(GLFW_VISIBLE, 1);

    GLFWwindow* window = glfwCreateWindow(1920, 1080, "ImGui GLFW+OpenGL3 Overlay Example", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    ImGui::StyleColorsDark(); //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    
    while (!glfwWindowShouldClose(window)) //while true
    {
        glfwPollEvents();

        ImGuiWindowFlags window_flags = 0;
        window_flags |= ImGuiWindowFlags_NoBackground;
        window_flags |= ImGuiWindowFlags_NoTitleBar;
        window_flags |= ImGuiWindowFlags_NoDecoration;

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

		ImDrawList *gui = ImGui::GetBackgroundDrawList();
		Crosshair();
		gui->AddCircle( ImVec2( 1920 / 2, 1080 / 2 ), 75.f, ImColor( 255, 255, 255 ), 0);
		gui->AddRect({200 , 150}, {400, 300}, ImColor(255, 0, 0, 255), 0, 0, 1);
		std::string TextOne{"TextOne"};
		const char* TextOnePtr = TextOne.c_str();
		int Textlength = TextOne.length();
		gui->AddText({200, 150}, ImColor(255, 255, 255), TextOnePtr, TextOnePtr + Textlength);
		
		ImGui::EndFrame();

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        //glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClearColor(0.0, 0.0, 0.0, 0.0); // transparent background!!!
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }
    
	//glDeleteTextures(1, &texture_id); //map item_texture_id in Core.hpp
	//stbi_image_free(image_data);
	
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
	
	return 1;
}