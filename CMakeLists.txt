cmake_minimum_required(VERSION 3.17)
project(ShaderEvaluator)

set(CMAKE_CXX_STANDARD 20)

find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)

add_executable(ShaderEvaluator main.cpp Shader.cpp Shader.h Camera.cpp Camera.h Mesh.h Mesh.cpp imGui/imgui_impl_glfw.h imGui/imgui_impl_glfw.cpp imGui/imgui_impl_opengl3.h imGui/imgui_impl_opengl3.cpp imGui/imgui.h imGui/imgui.cpp imGui/imgui_demo.cpp imGui/imconfig.h imGui/imgui_draw.cpp imGui/imgui_internal.h imGui/imgui_tables.cpp imGui/imgui_widgets.cpp imGui/imstb_rectpack.h imGui/imstb_textedit.h imGui/imstb_truetype.h)

target_link_libraries(ShaderEvaluator PRIVATE glad::glad glfw)