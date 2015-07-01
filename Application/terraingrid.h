/*____________________________________________________________________
|
| File: terraingrid.h
|
| (C) Copyright 2013 Abonvita Software LLC.
| Licensed under the GX Toolkit License, Version 1.0.
|___________________________________________________________________*/

/*___________________
|
| Type definitions
|__________________*/

struct TerrainGrid{
  int         dimension;          // # of vertices in x & z
  int         spacing;            // distance between vertices in feet
  float       length_diagonal;
  byte       *terrain_in_square;  // type of terrain in each grid square
  gx3dObject *obj_normal_lod;     // normal level of detail
//  gx3dObject *obj_low_lod;        // low level of detail
};

/*___________________
|
| Type definitions
|__________________*/

// Terrain types
#define TerrainGrid_TYPE_RIVERBED 1
#define TerrainGrid_TYPE_MUD      2
#define TerrainGrid_TYPE_GRASS    3
#define TerrainGrid_TYPE_DIRT     4
#define TerrainGrid_TYPE_ROCK     5
#define TerrainGrid_TYPE_SNOW     6

/*___________________
|
| Function Prototypes
|__________________*/

// Init a terrain height grid from a BMP file
TerrainGrid *TerrainGrid_Init_BMPFile (
  char  *filename,          // greyscale image to create map from
  int    spacing,           // distance in feet between points in the grid
  float  base_height,       // blackest pixel = this height
  float  height_scale,      // amount to scale each height value by
  float  sand_elevation,    // below this is sand, above is grass
  float  rock_elevation,    // below this is grass, above is rock
  float  snow_elevation );  // below this is rock, above is snow

// Free a terrain grid
void TerrainGrid_Free (TerrainGrid *tg);

// Get the height of a point on the grid
float TerrainGrid_Get_Height (TerrainGrid *tg, float x, float z);
