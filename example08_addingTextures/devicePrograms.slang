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

[shader("closesthit")]
void closesthit_radiance(
  inout float3 prd : SV_RayPayload, 
  in float2 barycentrics,
  uniform float3 color,
  uniform RWStructuredBuffer<float3> vertices,
  uniform RWStructuredBuffer<float3> normals,
  uniform RWStructuredBuffer<float2> texcoords,
  uniform RWStructuredBuffer<int3> indices,
  uniform bool hasTexture,
  uniform Texture2D texture)
{

  const int primID = PrimitiveIndex();
  int3 index = indices[primID];
  const float u = barycentrics.x;
  const float v = barycentrics.y;
  // ------------------------------------------------------------------
  // compute normal, using either shading normal (if avail), or
  // geometry normal (fallback)
  // ------------------------------------------------------------------
  float3 N;
  uint numNormals, normalStride;
  normals.GetDimensions(numNormals, normalStride);
  if (numNormals > 0) {
    N = (1.f-u-v) * normals[index.x]
      +         u * normals[index.y]
      +         v * normals[index.z];
  } else 
  {
    const float3 A     = vertices[index.x];
    const float3 B     = vertices[index.y];
    const float3 C     = vertices[index.z];
    N                  = normalize(cross(B-A,C-A));
  }
  
  // ------------------------------------------------------------------
  // compute diffuse material color, including diffuse texture, if
  // available
  // ------------------------------------------------------------------
  float3 diffuseColor = color;
  uint numTexcoords, texcoordStride;
  texcoords.GetDimensions(numTexcoords, texcoordStride);
  if (hasTexture && numTexcoords > 0) {
    const float2 tc
      = (1.f-u-v) * texcoords[index.x]
      +         u * texcoords[index.y]
      +         v * texcoords[index.z];
    SamplerState temp;
    float4 fromTexture = texture.Sample(temp, tc); 
    diffuseColor *= fromTexture.rgb;
  }
  
  float3 rayDir = WorldRayDirection();
  float cosDN = .2f + .8f*abs(dot(rayDir,N));
  prd = diffuseColor * .2f + cosDN * diffuseColor * .8f;
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
