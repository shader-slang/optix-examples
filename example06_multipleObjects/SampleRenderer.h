#pragma once

#include <iostream>
#ifndef TERMINAL_COLORS
#define TERMINAL_COLORS
#define TERMINAL_RED "\033[1;31m"
#define TERMINAL_GREEN "\033[1;32m"
#define TERMINAL_YELLOW "\033[1;33m"
#define TERMINAL_BLUE "\033[1;34m"
#define TERMINAL_RESET "\033[0m"
#define TERMINAL_DEFAULT TERMINAL_RESET
#define TERMINAL_BOLD "\033[1;1m"
#endif
#ifndef PRINT
# define PRINT(var) std::cout << #var << "=" << var << std::endl;
# define PING std::cout << __FILE__ << "::" << __LINE__ << ": " << __FUNCTION__ << std::endl;
#endif

// our own classes, partly shared between host and device
#include "CUDABuffer.h"
#include "LaunchParams.h"
#include "glm/glm.hpp"

/*! \namespace osc - Optix Siggraph Course */
namespace osc {

  struct Camera {
    /*! camera position - *from* where we are looking */
    glm::vec3 from;
    /*! which point we are looking *at* */
    glm::vec3 at;
    /*! general up-vector */
    glm::vec3 up;
  };
  
  /*! a simple indexed triangle mesh that our sample renderer will
      render */
  struct TriangleMesh {
    /*! add a unit cube (subject to given xfm matrix) to the current
        triangleMesh */
    void addUnitCube(const glm::mat4x3 &xfm);
    
    //! add aligned cube aith front-lower-left corner and size
    void addCube(const glm::vec3 &center, const glm::vec3 &size);

    std::vector<glm::vec3> vertex;
    std::vector<glm::ivec3> index;
    glm::vec3 color;
  };
  
  /*! a sample OptiX-7 renderer that demonstrates how to set up
      context, module, programs, pipeline, SBT, etc, and perform a
      valid launch that renders some pixel (using a simple test
      pattern, in this case */
  class SampleRenderer
  {
    // ------------------------------------------------------------------
    // publicly accessible interface
    // ------------------------------------------------------------------
  public:
    /*! constructor - performs all setup, including initializing
      optix, creates module, pipeline, programs, SBT, etc. */
    SampleRenderer(const std::vector<TriangleMesh> &meshes);

    /*! render one frame */
    void render();

    /*! resize frame buffer to given resolution */
    void resize(const glm::ivec2 &newSize);

    /*! download the rendered color buffer */
    void downloadPixels(uint32_t h_pixels[]);

    /*! set camera to render with */
    void setCamera(const Camera &camera);
  protected:
    // ------------------------------------------------------------------
    // internal helper functions
    // ------------------------------------------------------------------

    /*! helper function that initializes optix and checks for errors */
    void initOptix();
  
    /*! creates and configures a optix device context (in this simple
      example, only for the primary GPU device) */
    void createContext();

    /*! creates the module that contains all the programs we are going
      to use. in this simple example, we use a single module from a
      single .cu file, using a single embedded ptx string */
    void createModule();
    
    /*! does all setup for the raygen program(s) we are going to use */
    void createRaygenPrograms();
    
    /*! does all setup for the miss program(s) we are going to use */
    void createMissPrograms();
    
    /*! does all setup for the hitgroup program(s) we are going to use */
    void createHitgroupPrograms();

    /*! assembles the full pipeline of all programs */
    void createPipeline();

    /*! constructs the shader binding table */
    void buildSBT();

    /*! build an acceleration structure for the given triangle mesh */
    OptixTraversableHandle buildAccel();

  protected:
    /*! @{ CUDA device context and stream that optix pipeline will run
        on, as well as device properties for this device */
    CUcontext          cudaContext;
    CUstream           stream;
    cudaDeviceProp     deviceProps;
    /*! @} */

    //! the optix context that our pipeline will run in.
    OptixDeviceContext optixContext;

    /*! @{ the pipeline we're building */
    OptixPipeline               pipeline;
    OptixPipelineCompileOptions pipelineCompileOptions = {};
    OptixPipelineLinkOptions    pipelineLinkOptions = {};
    /*! @} */

    /*! @{ the module that contains out device programs */
    OptixModule                 module;
    OptixModuleCompileOptions   moduleCompileOptions = {};
    /* @} */

    /*! vector of all our program(group)s, and the SBT built around
        them */
    std::vector<OptixProgramGroup> raygenPGs;
    CUDABuffer raygenRecordsBuffer;
    std::vector<OptixProgramGroup> missPGs;
    CUDABuffer missRecordsBuffer;
    std::vector<OptixProgramGroup> hitgroupPGs;
    CUDABuffer hitgroupRecordsBuffer;
    OptixShaderBindingTable sbt = {};

    /*! @{ our launch parameters, on the host, and the buffer to store
        them on the device */
    LaunchParams launchParams;
    CUDABuffer   launchParamsBuffer;
    /*! @} */

    CUDABuffer colorBuffer;

    /*! the camera we are to render with. */
    Camera lastSetCamera;
    
    /*! the model we are going to trace rays against */
    std::vector<TriangleMesh> meshes;
    /*! one buffer per input mesh */
    std::vector<CUDABuffer> vertexBuffer;
    /*! one buffer per input mesh */
    std::vector<CUDABuffer> indexBuffer;
    //! buffer that keeps the (final, compacted) accel structure
    CUDABuffer asBuffer;
  };

} // ::osc
