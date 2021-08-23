find_package(OpenGL REQUIRED)

include_directories(${OptiX_INCLUDE})

slang_compile_and_embed(embedded_ptx_code ${CMAKE_CURRENT_SOURCE_DIR}/devicePrograms.slang)

add_executable(ex09_shadowRays
  ${embedded_ptx_code}
  optix7.h
  CUDABuffer.h
  SampleRenderer.h
  SampleRenderer.cpp
  Model.cpp
  main.cpp
  )

target_link_libraries(ex09_shadowRays
  # optix dependencies, for rendering
  ${optix_LIBRARY}
  ${CUDA_LIBRARIES}
  ${CUDA_CUDA_LIBRARY}
  # glfw and opengl, for display
  glfWindow
  glfw
  ${OPENGL_gl_LIBRARY}
  )
