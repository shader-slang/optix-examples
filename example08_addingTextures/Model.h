#pragma once


#include "glm/glm.hpp"
#include <vector>
#include <string>

/*! \namespace osc - Optix Siggraph Course */
namespace osc {  
  /*! a simple indexed triangle mesh that our sample renderer will
      render */
  struct TriangleMesh {
    std::vector<glm::vec3>  vertex;
    std::vector<glm::vec3>  normal;
    std::vector<glm::vec2>  texcoord;
    std::vector<glm::ivec3> index;

    // material data:
    glm::vec3 diffuse;
    int diffuseTextureID { -1 };
  };

   struct Texture {
    ~Texture()
    { if (pixel) delete[] pixel; }
    
    uint32_t *pixel { nullptr };
    glm::ivec2 resolution { -1 };
  };
  
  struct Model {
    ~Model()
    { 
      for (auto mesh : meshes) delete mesh; 
      for (auto texture : textures) delete texture; 
    }
    
    std::vector<TriangleMesh *> meshes;
    std::vector<Texture *> textures;
    //! bounding box of all vertices in the model
    glm::vec3 boundsMin;
    glm::vec3 boundsMax;
    glm::vec3 boundsCenter;
    glm::vec3 boundsSpan;
  };

  Model *loadOBJ(const std::string &objFile);
}
