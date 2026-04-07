#include <algorithm>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <vector>

#include "point.hh"
#include "scenes/scenes.hh"
#include "utils/image.hh"

#include "schematica/parser.hh"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "ImGuiFileDialog.h"
#include "GLFW/glfw3.h"

int _main()
{

  std::cout << "Rendering scene..." << std::endl;
  size_t terrain_size = 500;
  double scale = 4.0 / terrain_size;
  size_t camera_count = 1;

  // isim::SceneOutput output = isim::minecraft_terrain_scene(
  //   terrain_size, terrain_size, scale, 50, camera_count);
  // isim::SceneOutput output = isim::minecraft_tree();

  // isim::SceneOutput output =
  //   isim::load_schematic("schematics/temple_of_notch.schem", true, 0,
  //                        isim::Point3(71, 67, 77), isim::Point3(72, 78, 106));

  isim::SceneOutput output =
    isim::load_schematic("schematics/dragongamer.schem", true, 0);
  isim::Scene* scene = output.result;
  std::string output_name = output.scene_name;

  std::cout << "Scene " << output_name << " created, rendering..." << std::endl;

  std::vector<isim::Image> image =
    scene->render_all_cameras(isim::Resolution::SmallHD(), true);

  for (size_t i = 0; i < image.size(); i++)
    {
      std::string filename;
      if (image.size() == 1)
        {
          filename = output_name + ".png";
        }
      else
        {
          filename = output_name + "_camera_" + std::to_string(i) + ".png";
        }
      image[i].save(filename.c_str());
      std::cout << "Scene rendered and saved to " << filename << std::endl;
    }

  return 0;
}

int main()
{
  if (!glfwInit())
    return -1;
  
  const char* glsl_version = "#version 150";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac

  // Create window with graphics context
  float main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor()); // Valid on GLFW 3.3+ only
  GLFWwindow* window = glfwCreateWindow((int)(1280 * main_scale), (int)(800 * main_scale), "Minecraft", nullptr, nullptr);

  if (window == nullptr)
      return 1;

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1); // Enable vsync

  
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;

  ImGui::StyleColorsDark();
  
  // Setup scaling
  ImGuiStyle& style = ImGui::GetStyle();
  style.ScaleAllSizes(main_scale);
  style.FontScaleDpi = main_scale;

  // Setup backends
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version); // macOS = 150

  
  bool show_demo_window = true;
  bool show_another_window = false;
  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

  char buf[250] = { 0 };

  // Main loop
  while (!glfwWindowShouldClose(window))
  {
      glfwPollEvents();

      // New frame
      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();

      
      // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
      if (show_demo_window)
          ImGui::ShowDemoWindow(&show_demo_window);

    
      // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
      {
          static std::string mapPath = "not loaded";
          static int seed = 42;
          static int dimensions[2] = { 50, 50 };

          ImGui::Begin("Input");

          ImGui::Text("Welcome to our Minecraft clone !\nYou can choose the size of the map and its seed.\nEnjoy :)");               // Display some text (you can use a format strings too)
          ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state

          ImGui::InputInt2("dimensions", dimensions);
          ImGui::InputInt("seed", &seed);            // Edit 1 float using a slider from 0.0f to 1.0f

          
          ImGui::Text("Loaded map: %s", mapPath.c_str());               // Display some text (you can use a format strings too)

          // open Dialog Simple
       	  if (ImGui::Button("Open File Dialog")) {
      		IGFD::FileDialogConfig config;config.path = ".";
      		ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose map to load (.schem)", ".schem", config);
  		  }

          ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
          ImGui::End();

          
        if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey", ImGuiWindowFlags_NoCollapse, ImVec2(400, 400))) { // => will show a dialog
          if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
            mapPath = ImGuiFileDialog::Instance()->GetFilePathName();
            std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
          }
    
          // close
          ImGuiFileDialog::Instance()->Close();
        }
      }

      // Render

      ImGui::Render();
      int display_w, display_h;
      glfwGetFramebufferSize(window, &display_w, &display_h);
      glViewport(0, 0, display_w, display_h);
      glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
      glClear(GL_COLOR_BUFFER_BIT);
      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

      glfwSwapBuffers(window);
  }

  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}
