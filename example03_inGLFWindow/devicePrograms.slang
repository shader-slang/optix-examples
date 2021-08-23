//------------------------------------------------------------------------------
// All global variables are stored in constant memory, under the 
// "SLANG_globalParams" structure. These parameters are filled in 
// by optix upon optixLaunch via launch parameters. 
// 
// Use the "-dump-intermediates" flag with the slang compiler to see 
// the exact generated layout.  
//------------------------------------------------------------------------------
int frameID;
RWStructuredBuffer<uint> colorBuffer;
int2 fbSize;

//------------------------------------------------------------------------------
// closest hit and anyhit programs for radiance-type rays.
//
// Note eventually we will have to create one pair of those for each
// ray type and each geometry type we want to render; but this
// simple example doesn't use any actual geometries yet, so we only
// create a single, dummy, set of them (we do have to have at least
// one group of them to set up the SBT)
//------------------------------------------------------------------------------

[shader("closesthit")]
void closesthit_radiance() 
{/*! for this simple example, this will remain empty */}

[shader("anyhit")]
void anyhit_radiance() 
{/*! for this simple example, this will remain empty */}

//------------------------------------------------------------------------------
// miss program that gets called for any ray that did not have a
// valid intersection
//
// as with the anyhit/closest hit programs, in this example we only
// need to have _some_ dummy function to set up a valid SBT
// ------------------------------------------------------------------------------

[shader("miss")]
void miss_radiance()
{ /*! for this simple example, this will remain empty */ }


//------------------------------------------------------------------------------
// ray gen program - the actual rendering happens in here
//------------------------------------------------------------------------------
[shader("raygeneration")]
void renderFrame()
{
  // ------------------------------------------------------------------
  // for this example, produce a simple test pattern:
  // ------------------------------------------------------------------

  // compute a test pattern based on pixel ID
  const int ix = DispatchRaysIndex().x;
  const int iy = DispatchRaysIndex().y;

  const int r = ((ix+frameID) % 256);
  const int g = ((iy+frameID) % 256);
  const int b = ((ix+iy+frameID) % 256);

  // convert to 32-bit rgba value (we explicitly set alpha to 0xff
  // to make stb_image_write happy ...
  const uint rgba = 0xff000000
    | (r<<0) | (g<<8) | (b<<16);

  // and write to frame buffer ...
  const uint fbIndex = ix+iy*fbSize.x;
  colorBuffer[fbIndex] = rgba;
}
