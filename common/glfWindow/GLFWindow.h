#pragma once

#include "glm/glm.hpp"
#include "glm/gtx/string_cast.hpp"
#include <string>
#include <algorithm> 
#include <iostream> 

// glfw framework
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

/*! \namespace osc - Optix Siggraph Course */
namespace osc {

  struct GLFWindow {
    GLFWindow(const std::string &title);
    ~GLFWindow();

    /*! put pixels on the screen ... */
    virtual void draw()
    { /* empty - to be subclassed by user */ }

    /*! callback that window got resized */
    virtual void resize(const glm::ivec2 &newSize)
    { /* empty - to be subclassed by user */ }

    virtual void key(int key, int mods)
    {}
    
    /*! callback that window got resized */
    virtual void mouseMotion(const glm::ivec2 &newPos)
    {}
    
    /*! callback that window got resized */
    virtual void mouseButton(int button, int action, int mods)
    {}

    inline glm::ivec2 getMousePos() const
    {
      double x,y;
      glfwGetCursorPos(handle,&x,&y);
      return glm::ivec2((int)x, (int)y);
    }
    
    /*! re-render the frame - typically part of draw(), but we keep
      this a separate function so render() can focus on optix
      rendering, and now have to deal with opengl pixel copies
      etc */
    virtual void render() 
    { /* empty - to be subclassed by user */ }

    /*! opens the actual window, and runs the window's events to
      completion. This function will only return once the window
      gets closed */
    void run();

    /*! the glfw window handle */
    GLFWwindow *handle { nullptr };
  };


  struct CameraFrame {
    CameraFrame(const float worldScale)
      : motionSpeed(worldScale)
    {
    }
    
    glm::vec3 getPOI() const
    { 
      return position - poiDistance * frame[2];//[2];
    }
      
    /*! re-compute all orientation related fields from given
      'user-style' camera parameters */
    void setOrientation(/* camera origin    : */const glm::vec3 &origin,
                        /* point of interest: */const glm::vec3 &interest,
                        /* up-vector        : */const glm::vec3 &up)
    {
      position = origin;
      upVector = up;
      frame[2] //[2]
        = (interest==origin)
        ? glm::vec3(0,0,1)
        : /* negative because we use NEGATIZE z axis */ - glm::normalize(interest - origin);
      frame[0] = glm::cross(up,frame[2]);
      if (glm::dot(frame[0],frame[0]) < 1e-8f)
        frame[0] = glm::vec3(0,1,0);
      else
        frame[0] = glm::normalize(frame[0]);
      // frame[0]
      //   = (fabs(dot(up,frame[2])) < 1e-6f)
      //   ? glm::vec3(0,1,0)
      //   : normalize(cross(up,frame[2]));
      frame[1] = glm::normalize(glm::cross(frame[2],frame[0]));
      poiDistance = glm::length(interest-origin);
      forceUpFrame();
    }
      
    /*! tilt the frame around the z axis such that the y axis is "facing upwards" */
    void forceUpFrame()
    {
      // frame[2] remains unchanged
      if (fabsf(glm::dot(frame[2],upVector)) < 1e-6f)
        // looking along upvector; not much we can do here ...
        return;
      frame[0] = glm::normalize(glm::cross(upVector,frame[2]));
      frame[1] = glm::normalize(glm::cross(frame[2],frame[0]));
      modified = true;
    }

    void setUpVector(const glm::vec3 &up)
    { upVector = up; forceUpFrame(); }

    inline float computeStableEpsilon(float f) const
    {
      return abs(f) * float(1./(1<<21));
    }
                               
    inline float computeStableEpsilon(const glm::vec3 v) const
    {
      return std::max(std::max(computeStableEpsilon(v.x),
                     computeStableEpsilon(v.y)),
                 computeStableEpsilon(v.z));
    }

    inline glm::vec3 get_from() const { return position; }
    inline glm::vec3 get_at() const { return getPOI(); }
    inline glm::vec3 get_up() const { return upVector; }
      
    glm::mat3      frame         { 1.f };
    glm::vec3         position      { 0,-1,0 };
    /*! distance to the 'point of interst' (poi); e.g., the point we
      will rotate around */
    float         poiDistance   { 1.f };
    glm::vec3         upVector      { 0,1,0 };
    /* if set to true, any change to the frame will always use to
       upVector to 'force' the frame back upwards; if set to false,
       the upVector will be ignored */
    bool          forceUp       { true };

    /*! multiplier how fast the camera should move in world space
      for each unit of "user specifeid motion" (ie, pixel
      count). Initial value typically should depend on the world
      size, but can also be adjusted. This is actually something
      that should be more part of the manipulator widget(s), but
      since that same value is shared by multiple such widgets
      it's easiest to attach it to the camera here ...*/
    float         motionSpeed   { 1.f };
    
    /*! gets set to true every time a manipulator changes the camera
      values */
    bool          modified      { true };
  };



  // ------------------------------------------------------------------
  /*! abstract base class that allows to manipulate a renderable
    camera */
  struct CameraFrameManip {
    CameraFrameManip(CameraFrame *cameraFrame)
      : cameraFrame(cameraFrame)
    {}
    
    /*! this gets called when the user presses a key on the keyboard ... */
    virtual void key(int key, int mods)
    {
      CameraFrame &fc = *cameraFrame;
      
      switch(key) {
      case '+':
      case '=':
        fc.motionSpeed *= 1.5f;
        std::cout << "# viewer: new motion speed is " << fc.motionSpeed << std::endl;
        break;
      case '-':
      case '_':
        fc.motionSpeed /= 1.5f;
        std::cout << "# viewer: new motion speed is " << fc.motionSpeed << std::endl;
        break;
      case 'C':
        std::cout << "(C)urrent camera:" << std::endl;
        std::cout << "- from :" << glm::to_string(fc.position) << std::endl;
        std::cout << "- poi  :" << glm::to_string(fc.getPOI()) << std::endl;
        std::cout << "- upVec:" << glm::to_string(fc.upVector) << std::endl; 
        std::cout << "- frame:" << glm::to_string(fc.frame) << std::endl;
        break;
      case 'x':
      case 'X':
        fc.setUpVector(fc.upVector==glm::vec3(1,0,0)?glm::vec3(-1,0,0):glm::vec3(1,0,0));
        break;
      case 'y':
      case 'Y':
        fc.setUpVector(fc.upVector==glm::vec3(0,1,0)?glm::vec3(0,-1,0):glm::vec3(0,1,0));
        break;
      case 'z':
      case 'Z':
        fc.setUpVector(fc.upVector==glm::vec3(0,0,1)?glm::vec3(0,0,-1):glm::vec3(0,0,1));
        break;
      default:
        break;
      }
    }

    virtual void strafe(const glm::vec3 &howMuch)
    {
      cameraFrame->position += howMuch;
      cameraFrame->modified =  true;
    }
    /*! strafe, in screen space */
    virtual void strafe(const glm::vec2 &howMuch)
    {
      strafe(+howMuch.x*cameraFrame->frame[0]
             -howMuch.y*cameraFrame->frame[1]);
    }

    virtual void move(const float step) = 0;
    virtual void rotate(const float dx, const float dy) = 0;
    
    // /*! this gets called when the user presses a key on the keyboard ... */
    // virtual void special(int key, const glm::ivec2 &where) { };
    
    /*! mouse got dragged with left button pressedn, by 'delta'
      pixels, at last position where */
    virtual void mouseDragLeft  (const glm::vec2 &delta)
    {
      rotate(delta.x * degrees_per_drag_fraction,
             delta.y * degrees_per_drag_fraction);
    }
    
    /*! mouse got dragged with left button pressedn, by 'delta'
      pixels, at last position where */
    virtual void mouseDragMiddle(const glm::vec2 &delta)
    {
      strafe(delta*pixels_per_move*cameraFrame->motionSpeed);
    }

    /*! mouse got dragged with left button pressedn, by 'delta'
      pixels, at last position where */
    virtual void mouseDragRight (const glm::vec2 &delta)
    {
      move(delta.y*pixels_per_move*cameraFrame->motionSpeed);
    }

    // /*! mouse button got either pressed or released at given location */
    // virtual void mouseButtonLeft  (const glm::ivec2 &where, bool pressed) {}
      
    // /*! mouse button got either pressed or released at given location */
    // virtual void mouseButtonMiddle(const glm::ivec2 &where, bool pressed) {}
      
    // /*! mouse button got either pressed or released at given location */
    // virtual void mouseButtonRight (const glm::ivec2 &where, bool pressed) {}
      
  protected:
    CameraFrame *cameraFrame;
    const float kbd_rotate_degrees        {  10.f };
    const float degrees_per_drag_fraction { 150.f };
    const float pixels_per_move           {  10.f };
  };


  struct GLFCameraWindow : public GLFWindow {
    GLFCameraWindow(const std::string &title,
                    const glm::vec3 &camera_from,
                    const glm::vec3 &camera_at,
                    const glm::vec3 &camera_up,
                    const float worldScale)
      : GLFWindow(title),
        cameraFrame(worldScale)
    {
      cameraFrame.setOrientation(camera_from,camera_at,camera_up);
      enableFlyMode();
      enableInspectMode();
    }

    void enableFlyMode();
    void enableInspectMode();
    
    // /*! put pixels on the screen ... */
    // virtual void draw()
    // { /* empty - to be subclassed by user */ }

    // /*! callback that window got resized */
    // virtual void resize(const glm::ivec2 &newSize)
    // { /* empty - to be subclassed by user */ }

    virtual void key(int key, int mods) override
    {
      switch(key) {
      case 'f':
      case 'F':
        std::cout << "Entering 'fly' mode" << std::endl;
        if (flyModeManip) cameraFrameManip = flyModeManip;
        break;
      case 'i':
      case 'I':
        std::cout << "Entering 'inspect' mode" << std::endl;
        if (inspectModeManip) cameraFrameManip = inspectModeManip;
        break;
      default:
        if (cameraFrameManip)
          cameraFrameManip->key(key,mods);
      }
    }
    
    /*! callback that window got resized */
    virtual void mouseMotion(const glm::ivec2 &newPos) override
    {
      glm::ivec2 windowSize;
      glfwGetWindowSize(handle, &windowSize.x, &windowSize.y);
      
      if (isPressed.leftButton && cameraFrameManip)
        cameraFrameManip->mouseDragLeft(glm::vec2(newPos-lastMousePos)/glm::vec2(windowSize));
      if (isPressed.rightButton && cameraFrameManip)
        cameraFrameManip->mouseDragRight(glm::vec2(newPos-lastMousePos)/glm::vec2(windowSize));
      if (isPressed.middleButton && cameraFrameManip)
        cameraFrameManip->mouseDragMiddle(glm::vec2(newPos-lastMousePos)/glm::vec2(windowSize));
      lastMousePos = newPos;
      /* empty - to be subclassed by user */
    }
    
    /*! callback that window got resized */
    virtual void mouseButton(int button, int action, int mods) override
    {
      const bool pressed = (action == GLFW_PRESS);
      switch(button) {
      case GLFW_MOUSE_BUTTON_LEFT:
        isPressed.leftButton = pressed;
        break;
      case GLFW_MOUSE_BUTTON_MIDDLE:
        isPressed.middleButton = pressed;
        break;
      case GLFW_MOUSE_BUTTON_RIGHT:
        isPressed.rightButton = pressed;
        break;
      }
      lastMousePos = getMousePos();
    }

    // /*! mouse got dragged with left button pressedn, by 'delta'
    //   pixels, at last position where */
    // virtual void mouseDragLeft  (const glm::ivec2 &where, const glm::ivec2 &delta) {}

    // /*! mouse got dragged with left button pressedn, by 'delta'
    //   pixels, at last position where */
    // virtual void mouseDragRight (const glm::ivec2 &where, const glm::ivec2 &delta) {}

    // /*! mouse got dragged with left button pressedn, by 'delta'
    //   pixels, at last position where */
    // virtual void mouseDragMiddle(const glm::ivec2 &where, const glm::ivec2 &delta) {}

    
    /*! a (global) pointer to the currently active window, so we can
      route glfw callbacks to the right GLFWindow instance (in this
      simplified library we only allow on window at any time) */
    // static GLFWindow *current;

    struct {
      bool leftButton { false }, middleButton { false }, rightButton { false };
    } isPressed;
    glm::ivec2 lastMousePos = { -1,-1 };

    friend struct CameraFrameManip;

    CameraFrame cameraFrame;
    std::shared_ptr<CameraFrameManip> cameraFrameManip;
    std::shared_ptr<CameraFrameManip> inspectModeManip;
    std::shared_ptr<CameraFrameManip> flyModeManip;
  };


  // ------------------------------------------------------------------
  /*! camera manipulator with the following traits
      
    - there is a "point of interest" (POI) that the camera rotates
    around.  (we track this via poiDistance, the point is then
    thta distance along the fame's z axis)
      
    - we can restrict the minimum and maximum distance camera can be
    away from this point
      
    - we can specify a max bounding box that this poi can never
    exceed (validPoiBounds).
      
    - we can restrict whether that point can be moved (by using a
    single-point valid poi bounds box
      
    - left drag rotates around the object

    - right drag moved forward, backward (within min/max distance
    bounds)

    - middle drag strafes left/right/up/down (within valid poi
    bounds)
      
  */
  struct InspectModeManip : public CameraFrameManip {

    InspectModeManip(CameraFrame *cameraFrame)
      : CameraFrameManip(cameraFrame)
    {}
      
  private:
    /*! helper function: rotate camera frame by given degrees, then
      make sure the frame, poidistance etc are all properly set,
      the widget gets notified, etc */
    virtual void rotate(const float deg_u, const float deg_v) override
    {
      float rad_u = -glm::pi<float>()/180.f*deg_u;
      float rad_v = -glm::pi<float>()/180.f*deg_v;

      CameraFrame &fc = *cameraFrame;
      
      const glm::vec3 poi  = fc.getPOI();
      fc.frame = glm::mat3_cast(glm::angleAxis(rad_u, fc.frame[1]) * glm::angleAxis(rad_v, fc.frame[0])) * fc.frame;

      if (fc.forceUp) fc.forceUpFrame();

      fc.position = poi + fc.poiDistance * fc.frame[2];
      fc.modified = true;
    }
      
    /*! helper function: move forward/backwards by given multiple of
      motion speed, then make sure the frame, poidistance etc are
      all properly set, the widget gets notified, etc */
    virtual void move(const float step) override
    {
      const glm::vec3 poi = cameraFrame->getPOI();
      // inspectmode can't get 'beyond' the look-at point:
      const float minReqDistance = 0.1f * cameraFrame->motionSpeed;
      cameraFrame->poiDistance   = std::max(minReqDistance,cameraFrame->poiDistance-step);
      cameraFrame->position      = poi + cameraFrame->poiDistance * cameraFrame->frame[2];
      cameraFrame->modified      = true;
    }
  };

  // ------------------------------------------------------------------
  /*! camera manipulator with the following traits

    - left button rotates the camera around the viewer position

    - middle button strafes in camera plane
      
    - right buttton moves forward/backwards
      
  */
  struct FlyModeManip : public CameraFrameManip {

    FlyModeManip(CameraFrame *cameraFrame)
      : CameraFrameManip(cameraFrame)
    {}
      
  private:
    /*! helper function: rotate camera frame by given degrees, then
      make sure the frame, poidistance etc are all properly set,
      the widget gets notified, etc */
    virtual void rotate(const float deg_u, const float deg_v) override
    {
      float rad_u = -glm::pi<float>()/180.f*deg_u;
      float rad_v = -glm::pi<float>()/180.f*deg_v;

      CameraFrame &fc = *cameraFrame;
      
      const glm::vec3 poi  = fc.getPOI();
      fc.frame = glm::mat3_cast(glm::angleAxis(rad_u, fc.frame[1]) * glm::angleAxis(rad_v, fc.frame[0])) * fc.frame;

      if (fc.forceUp) fc.forceUpFrame();

      fc.modified = true;
    }
      
    /*! helper function: move forward/backwards by given multiple of
      motion speed, then make sure the frame, poidistance etc are
      all properly set, the widget gets notified, etc */
    virtual void move(const float step) override
    {
      cameraFrame->position    += step * cameraFrame->frame[2];
      cameraFrame->modified     = true;
    }
  };

  inline void GLFCameraWindow::enableFlyMode()
  {
    flyModeManip     = std::make_shared<FlyModeManip>(&cameraFrame);
    cameraFrameManip = flyModeManip;
  }
  
  inline void GLFCameraWindow::enableInspectMode()
  {
    inspectModeManip = std::make_shared<InspectModeManip>(&cameraFrame);
    cameraFrameManip = inspectModeManip;
  }
    

  
  
} // ::osc
