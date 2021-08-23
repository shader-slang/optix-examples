include_directories(${OptiX_INCLUDE})

add_executable(ex01_helloOptix
  optix7.h
  main.cpp
  )
target_link_libraries(ex01_helloOptix
  ${optix_LIBRARY}
  ${CUDA_LIBRARIES}
  ${CUDA_CUDA_LIBRARY}
  )
