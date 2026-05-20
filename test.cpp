#include <cstddef>
#include <cstring>
#include <iostream>
#include <vector>

#if defined(__APPLE__)
#  include <OpenGL/gl3.h>
#else
#  include <GL/gl.h>
#endif

#include "scenes/scenes.hh"
#include "utils/image.hh"
#include "utils/resolution.hh"

#include "GLFW/glfw3.h"
#include "ImGuiFileDialog.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

GLuint load_texture(isim::Image& image)
{
  size_t w = image.width();
  size_t h = image.height();

  uint8_t* data = static_cast<uint8_t*>(std::malloc(w * h * 4));

  for (size_t i = 0; i < w * h; i++)
    {
      isim::Color pixel = image.data()[i];
      data[4 * i] = pixel.r;
      data[4 * i + 1] = pixel.g;
      data[4 * i + 2] = pixel.b;
      data[4 * i + 3] = pixel.a;
    }

  GLuint tex;
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE,
               data);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  std::free(data);

  return tex;
}

isim::Image render(size_t width,
                   size_t depth,
                   size_t seed,
                   std::string output_name,
                   std::string input_path,
                   bool perlin,
                   std::string& log,
                   isim::Resolution res)
{
  log = "Rendering scene...";
  std::cout << "Rendering scene..." << '\n';
  double scale = 0.008;
  size_t camera_count = 1;

  isim::SceneOutput output;
  if (perlin)
    output = isim::minecraft_terrain_scene(width, depth, scale, seed, 50,
                                           camera_count);
  else
    output = isim::load_schematic(input_path, true, 0);

  // isim::SceneOutput output = isim::minecraft_tree();

  // isim::SceneOutput output =
  //   isim::load_schematic("schematics/temple_of_notch.schem", true, 0,
  //                        isim::Point3(71, 67, 77), isim::Point3(72, 78, 106));

  isim::Scene* scene = output.result;

  log = "Scene " + output_name + " created, rendering...";
  std::cout << "Scene " << output_name << " created, rendering...";

  std::vector<isim::Image> image = scene->render_all_cameras(res, true);

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
      log = "Scene rendered and saved to " + filename;
      std::cout << "Scene rendered and saved to " << filename;
    }

  delete scene;

  return image[0];
}

int main()
{
  if (!glfwInit())
    return -1;

  const char* glsl_version = "#version 150";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Required on Mac

  // Create window with graphics context
  float main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(
    glfwGetPrimaryMonitor()); // Valid on GLFW 3.3+ only
  GLFWwindow* window =
    glfwCreateWindow((int)(1280 * main_scale), (int)(800 * main_scale),
                     "Minecraft", nullptr, nullptr);

  if (window == nullptr)
    return 1;

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1); // Enable vsync

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  (void)io;

  ImGui::StyleColorsDark();

  // Setup scaling
  ImGuiStyle& style = ImGui::GetStyle();
  style.ScaleAllSizes(main_scale);
  style.FontScaleDpi = main_scale;

  // Setup backends
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version); // macOS = 150

  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

  // Main loop
  while (!glfwWindowShouldClose(window))
    {
      glfwPollEvents();

      // New frame
      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();

      {
        static int seed = 42;
        static int dimensions[2] = {50, 50};
        static std::string mapPath = "not loaded";
        static bool valid_path = false;
        static char output_name_buf[250] = {0};
        static std::string log;

        // Resolution state and custom inputs
        static int res_choice = 0;
        static int custom_res[2] = {960, 540};
        static isim::Resolution selected_res = isim::Resolution::SmallHD();

        if (ImGui::BeginMainMenuBar())
          {
            if (ImGui::BeginMenu("Render"))
              {
                if (ImGui::BeginMenu("Resolution"))
                  {
                    if (ImGui::MenuItem("SmallHD", NULL, res_choice == 0))
                      {
                        res_choice = 0;
                        selected_res = isim::Resolution::SmallHD();
                      }
                    if (ImGui::MenuItem("HD", NULL, res_choice == 1))
                      {
                        res_choice = 1;
                        selected_res = isim::Resolution::HD();
                      }
                    if (ImGui::MenuItem("FullHD", NULL, res_choice == 2))
                      {
                        res_choice = 2;
                        selected_res = isim::Resolution::FullHD();
                      }
                    if (ImGui::MenuItem("UHD", NULL, res_choice == 3))
                      {
                        res_choice = 3;
                        selected_res = isim::Resolution::UHD();
                      }
                    if (ImGui::MenuItem("Cinema4K", NULL, res_choice == 4))
                      {
                        res_choice = 4;
                        selected_res = isim::Resolution::Cinema4K();
                      }
                    if (ImGui::MenuItem("SmallSquare", NULL, res_choice == 5))
                      {
                        res_choice = 5;
                        selected_res = isim::Resolution::SmallSquare();
                      }
                    if (ImGui::MenuItem("MediumSquare", NULL, res_choice == 6))
                      {
                        res_choice = 6;
                        selected_res = isim::Resolution::MediumSquare();
                      }
                    if (ImGui::MenuItem("LargeSquare", NULL, res_choice == 7))
                      {
                        res_choice = 7;
                        selected_res = isim::Resolution::LargeSquare();
                      }
                    if (ImGui::MenuItem("Custom...", NULL, res_choice == 8))
                      {
                        res_choice = 8;
                        ImGui::OpenPopup("Custom Resolution");
                      }

                    ImGui::EndMenu();
                  }
                ImGui::EndMenu();
              }
            ImGui::EndMainMenuBar();
          }

        // Custom resolution popup
        if (ImGui::BeginPopupModal("Custom Resolution", NULL,
                                   ImGuiWindowFlags_AlwaysAutoResize))
          {
            ImGui::InputInt2("Width x Height", custom_res);
            if (custom_res[0] < 1)
              custom_res[0] = 1;
            if (custom_res[1] < 1)
              custom_res[1] = 1;

            if (ImGui::Button("OK"))
              {
                selected_res = isim::Resolution::Custom((size_t)custom_res[0],
                                                        (size_t)custom_res[1]);
                ImGui::CloseCurrentPopup();
              }
            ImGui::SameLine();
            if (ImGui::Button("Cancel"))
              {
                ImGui::CloseCurrentPopup();
              }
            ImGui::EndPopup();
          }

        static bool show_viewer = false;
        static int img_w = 0, img_h = 0;
        static float zoom = 1.0f;
        static GLuint image_tex;

        if (show_viewer)
          {
            ImGuiIO& io = ImGui::GetIO();

            ImGui::SetNextWindowSize(ImVec2(img_w, img_h),
                                     ImGuiCond_FirstUseEver);

            ImGui::Begin("Image Viewer", &show_viewer);

            if (image_tex)
              {
                // Zoom avec molette
                if (ImGui::IsWindowHovered())
                  zoom += io.MouseWheel * 0.1f;

                zoom = std::max(0.1f, zoom);

                // Taille affichée (ratio conservé)
                float display_w = img_w * zoom;
                float display_h = img_h * zoom;

                ImGui::Image((void*)(intptr_t)image_tex,
                             ImVec2(display_w, display_h));
              }

            ImGui::End();
          }

        ImGui::Begin("Input", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

        ImGui::Text("Welcome to our Minecraft clone !\n\
\n\
You can either:\n\
1. Create a procedural generated map. You can choose the dimensions and the seed.\n\
2. Load a map by providing a .shem file.\n\
Enjoy :)\n\n"); // Display some text (you can use a format strings too)
        ImGui::InputText("Output scene name", output_name_buf, 250);
        ImGui::Text("\n");

        if (ImGui::CollapsingHeader("1. Procedural generated map"))
          {
            ImGui::Text("\n");
            ImGui::InputInt2("dimensions (width x depth)", dimensions);
            ImGui::InputInt("seed", &seed);

            if (ImGui::Button("Create Procedural generated map"))
              {
                std::string output_name(output_name_buf,
                                        std::strlen(output_name_buf));

                if (output_name.empty())
                  {
                    log = "Invalid output name, must not be empty.";
                  }
                else
                  {
                    isim::Image image =
                      render(dimensions[1], dimensions[0], seed, output_name,
                             "", true, log, selected_res);

                    log = "Generated map !";
                    img_w = image.width();
                    img_h = image.height();
                    image_tex = load_texture(image);
                    show_viewer = true;
                  }
              }
            ImGui::Text("\n");
          }

        if (ImGui::CollapsingHeader("2. Loaded map"))
          {
            ImGui::Text("\n");
            ImGui::Text(
              "Loaded map path: %s\n",
              mapPath
                .c_str()); // Display some text (you can use a format strings too)

            // open Dialog Simple
            if (ImGui::Button("Open File Dialog"))
              {
                IGFD::FileDialogConfig config;
                config.path = ".";
                ImGuiFileDialog::Instance()->OpenDialog(
                  "ChooseFileDlgKey", "Choose map to load (.schem)", ".schem",
                  config);
              }

            if (ImGui::Button("Generate loaded map"))
              {
                std::string output_name(output_name_buf,
                                        std::strlen(output_name_buf));

                if (!valid_path)
                  {
                    log = "Invalid file, cannot generate map.";
                  }
                else if (output_name.empty())
                  {
                    log = "Invalid output name, must not be empty.";
                  }
                else
                  {
                    isim::Image image = render(0, 0, 0, output_name, mapPath,
                                               false, log, selected_res);

                    log = "Generated map !";
                    img_w = image.width();
                    img_h = image.height();
                    image_tex = load_texture(image);
                    show_viewer = true;
                  }
              }
            ImGui::Text("\n");
          }

        // ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::Text("\n%s\n", log.c_str());
        ImGui::End();

        if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey",
                                                 ImGuiWindowFlags_NoCollapse,
                                                 ImVec2(400, 400)))
          { // => will show a dialog
            if (ImGuiFileDialog::Instance()->IsOk())
              { // action if OK
                mapPath = ImGuiFileDialog::Instance()->GetFilePathName();
                std::string filePath =
                  ImGuiFileDialog::Instance()->GetCurrentPath();
              }

            valid_path = true;
            // close
            ImGuiFileDialog::Instance()->Close();
          }
      }

      // Render

      ImGui::Render();
      int display_w, display_h;
      glfwGetFramebufferSize(window, &display_w, &display_h);
      glViewport(0, 0, display_w, display_h);
      glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w,
                   clear_color.z * clear_color.w, clear_color.w);
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
