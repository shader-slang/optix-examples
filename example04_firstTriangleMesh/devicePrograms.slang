struct Camera {
  float3 position;
  float3 direction;
  float3 horizontal;
  float3 vertical;
};

// LaunchParams
RWStructuredBuffer<uint> colorBuffer;
int2 fbSize;
Camera camera;
RaytracingAccelerationStructure traversable;

float3 randomColor(uint i)
{
    int r = i*13*17 + 0x234235;
    int g = i*7*3*5 + 0x773477;
    int b = i*11*19 + 0x223766;
    return float3((r&255)/255.f,
                    (g&255)/255.f,
                    (b&255)/255.f);
}

[shader("closesthit")]
void triangle_radiance(
  inout float3 prd : SV_RayPayload, 
  in float2 barycentrics
) {
  prd = float3(barycentrics.x, barycentrics.y, 0.f);
}

[shader("closesthit")]
void curve_radiance(
  inout float3 prd : SV_RayPayload, 
  in float curve_parameter,
) {
  prd = float3(curve_parameter);
}


[shader("anyhit")]
void anyhit_radiance() 
{/*! for this simple example, this will remain empty */}


[shader("miss")]
void miss_radiance(inout float3 prd : SV_RayPayload)
{
  // set to constant white as background color
  prd = float3(1.f, 1.f, 1.f);
}

//------------------------------------------------------------------------------
// ray gen program - the actual rendering happens in here
//------------------------------------------------------------------------------
[shader("raygeneration")]
void renderFrame()
{
  // compute a test pattern based on pixel ID
  const int ix = DispatchRaysIndex().x;
  const int iy = DispatchRaysIndex().y;

  // our per-ray data for this example. what we initialize it to
  // won't matter, since this value will be overwritten by either
  // the miss or hit program, anyway
  float3 pixelColorPRD = float3(0.f);

  // normalized screen plane position, in [0,1]^2
  const float2 screen = float2(ix+.5f,iy+.5f) * float2(1.f/fbSize.x, 1.f/fbSize.y);
  
  // generate ray direction
  RayDesc ray;
  ray.Origin = camera.position;
  ray.Direction = normalize(camera.direction
                            + (screen.x - 0.5f) * camera.horizontal
                            + (screen.y - 0.5f) * camera.vertical);
  ray.TMin = 0.f;
  ray.TMax = 1e20f;

  TraceRay(
    /*RayTracingAccelerationStructure*/ traversable,
    /*Ray flags*/ RAY_FLAG_NONE,
    /*Instance inclusion mask*/0xff,
    /*hit index*/0,
    /*ray type count*/1,
    /*miss index*/ 0,
    /*ray*/ ray,
    /*payload*/pixelColorPRD
  );

  const int r = int(255.99f*pixelColorPRD.x);
  const int g = int(255.99f*pixelColorPRD.y);
  const int b = int(255.99f*pixelColorPRD.z);

  // convert to 32-bit rgba value (we explicitly set alpha to 0xff
  // to make stb_image_write happy ...
  const uint rgba = 0xff000000
    | (r<<0) | (g<<8) | (b<<16);

  // and write to frame buffer ...
  const uint fbIndex = ix+iy*fbSize.x;
  colorBuffer[fbIndex] = rgba;
}
