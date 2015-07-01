/*____________________________________________________________________
|
| File: terraingrid.cpp
|
| Description: Functions to create and manipulate a terrain grid.
|
| Functions: TerrainGrid_Init_BMPFile
|             Create_Object_LOD_Normal
|             Set_TexCoords_LOD_Normal
|            TerrainGrid_Free
|            TerrainGrid_Get_Height
|
| (C) Copyright 2013 Abonvita Software LLC.
| Licensed under the GX Toolkit License, Version 1.0.
|___________________________________________________________________*/

/*___________________
|
| Include Files
|__________________*/

#include <math.h>

#include "dp.h"

#include "terraingrid.h"

/*___________________
|
| Constants
|__________________*/

#define MAX_TEXTURE_VARIATIONS  6

#define SAND  TerrainGrid_TYPE_SAND
#define GRASS TerrainGrid_TYPE_GRASS
#define ROCK  TerrainGrid_TYPE_ROCK
#define SNOW  TerrainGrid_TYPE_SNOW

#define TEXTURE_FILE_SIZE 1024
#define TEXTURE_TILE_SIZE 64

#define DETAIL_TILE 240

/*___________________
|
| Function prototypes
|__________________*/

static int Create_Object_LOD_Normal (
  TerrainGrid *tg,
  float       *heightmap ); // bytemap height data array
static void Set_TexCoords_LOD_Normal (
  TerrainGrid *tg,
  float        sand_elevation,    // below this line is sand, above is grass
  float        rock_elevation,    // below this line is grass, above is rock
  float        snow_elevation );  // above this line is snow

/*___________________
|
| Global variables
|__________________*/

static char debug_str [256];

/*____________________________________________________________________
|
| Function: TerrainGrid_Init_BMPFile
|
| Input: Called from ____
| Output: Creates a terraingrid 3D object and returns a pointer to it.
|
| Notes: The lower left corner of the grid will be positioned at the 
|   local coordinate origin (0,0,0).  
|___________________________________________________________________*/
     
TerrainGrid *TerrainGrid_Init_BMPFile (
  char  *filename,          // greyscale image to create map from
  int    spacing,           // distance in feet between points in the grid
  float  base_height,       // blackest pixel = this height
  float  height_scale,      // amount to scale each height value by
  float  sand_elevation,    // below this line is sand,
  float  rock_elevation,    // below this line is grass, above is rock
  float  snow_elevation )   // above this line is snow
{
  int i, x, z, dx, dy;
  int error;
  float *heightmap;
  byte *image, *bytemap, *byte_data, blackest_pixel;
  gxState state;
  gxBound box;
  gxPage vpage;
  TerrainGrid *tg = NULL;

/*____________________________________________________________________
|
| Init variables
|___________________________________________________________________*/

  bytemap   = NULL;
  heightmap = NULL;
  error     = FALSE;

/*____________________________________________________________________
|
| Read in the BMP file
|___________________________________________________________________*/

  // Get dimensions of the file
  if (NOT gxGetBMPFileDimensions (filename, &dx, &dy, NULL)) {
    DEBUG_ERROR ("TerrainGrid_Init_BMPFile(): ERROR opening file")
    error = TRUE;
  }
  else {
    // Dimensions must be square, >= 3 and odd
    if ((dx != dy) OR (dx < 3) OR ((dx % 2) == 0)) {
      sprintf (debug_str, "TerrainGrid_Init_BMPFile(): ERROR file dimensions (%d x %d) not correct", dx, dy);
      DEBUG_WRITE (debug_str)
      DEBUG_ERROR (" - dimensions must be square, >= 3, and odd")
      error = TRUE;
    }
    else {
      // Create a virtual page
      if (NOT gxCreateVirtualPage (dx, dy, gxHINT_CREATE_IN_SYSTEM_MEMORY, &vpage)) {
        DEBUG_ERROR ("TerrainGrid_Init_BMPFile(): ERROR creating a virtual page")
        error = TRUE;
      }
      else {
        // Set access to virtual page
        gxSaveState (&state);
        gxSetActivePage (vpage);
        // Load data from BMP file
        gxReadBMPFile (filename, 1);
        // Build a bytemap array
        box.x = 0;
        box.y = 0;
        box.w = dx;
        box.h = dy;
        image = gxCreateImage (box);
        if (image == NULL) {
          DEBUG_ERROR ("TerrainGrid_Init_BMPFile(): ERROR allocating memory")
          error = TRUE;
        }
        else {
          bytemap = gxCreateBytemap (image);  // a 2D array of bytes
          free (image);
          if (bytemap == NULL) {
            DEBUG_ERROR ("TerrainGrid_Init_BMPFile(): ERROR allocating memory")
            error = TRUE;
          }
          else {
            byte_data = bytemap + (2 * sizeof(unsigned)); // first two integers are width, height, rest is data
            // Find darkest (base height) pixel
            blackest_pixel = 255;
            for (i=0; i<dx*dy; i++)
              if (byte_data[i] < blackest_pixel)
                blackest_pixel = byte_data[i];
          }
        }
        // Reset active page
        gxRestoreState (&state);
        // Free memory 
        gxFreeVirtualPage (vpage);
      }
    }
  }

/*____________________________________________________________________
|
| Create a height map from the bytemap - start at the bottom-left of the
|   bytemap and work right and up
|___________________________________________________________________*/

  if (NOT error) {
    // Allocate memory for the heightmap
    heightmap = (float *) calloc (dx * dx, sizeof(float));
    if (heightmap == NULL) {
      DEBUG_ERROR ("TerrainGrid_Init_BMPFile(): ERROR allocating memory")
      error = TRUE;
    }
    else {
      i = 0;
      // Compute height values
      for (z=dx-1; z>= 0; z--)
        for (x=0; x<dx; x++)
          heightmap[i++] = base_height + (float)(byte_data[z*dx+x]-blackest_pixel) * height_scale;
    }
  }

  // Free memory for bytemap - not needed anymore
  if (bytemap) {
    free (bytemap);
    bytemap = NULL;
  }

/*____________________________________________________________________
|
| Create toplevel grid struct
|___________________________________________________________________*/

  if (NOT error) {

    // Allocate memory for grid struct
    tg = (TerrainGrid *) calloc (1, sizeof(TerrainGrid));
    if (tg == NULL)
      error = TRUE;
    else {
      // Init variables in the struct and allocate memory as needed
      tg->dimension       = dx - 1;
      tg->spacing         = spacing;
      tg->length_diagonal = (float)(tg->spacing * tg->spacing + tg->spacing * tg->spacing);
      tg->terrain_in_square = (byte *) calloc (tg->dimension * tg->dimension, sizeof(byte));
      if (tg->terrain_in_square == NULL)
        error = TRUE;
    }
  }

/*____________________________________________________________________
|
| Create normal-LOD object for this grid
|___________________________________________________________________*/
             
  if (NOT error) {
    // Create an object for the normal LOD
    if (NOT Create_Object_LOD_Normal (tg, heightmap))
      error = TRUE;
    else
      // Set the texture coords 
      Set_TexCoords_LOD_Normal (tg, sand_elevation, rock_elevation, snow_elevation);
  }

  // Free memory for heightmap - not needed anymore
  if (heightmap) {
    free (heightmap);
    heightmap = NULL;
  }

/*____________________________________________________________________
|
| If any error, release all memory
|___________________________________________________________________*/

  if (error) {
    if (tg) {
      if (tg->obj_normal_lod) 
        gx3d_FreeObject (tg->obj_normal_lod);
      free (tg);
      tg = NULL;
    }
  }

  return (tg);
}

/*____________________________________________________________________
|
| Function: Create_Object_LOD_Normal
|
| Input: Called from TerrainGrid_Init()
| Output: Creates the normal LOD 3D object for the terrain grid.  Returns
|   true on success, else false on any error.
|___________________________________________________________________*/
     
static int Create_Object_LOD_Normal (
  TerrainGrid *tg,
  float       *heightmap )  // bytemap height data array
{
  int i, j, k, n, x, z, count;
  int error;
  float x_pos, z_pos;
  word *attached_polygon;
  gx3dObject *obj;
  gx3dObjectLayer *layer;
  gx3dVector sum;

  debug_WriteConsole ("Creating terraingrid object"); // writes to visual studio output window

/*____________________________________________________________________
|
| Init variables
|___________________________________________________________________*/

  error = FALSE;
  obj   = NULL;

/*____________________________________________________________________
|
| Allocate memory for the object
|___________________________________________________________________*/

  // Create an empty 3d object
  obj = gx3d_CreateObject ();
  if (obj == NULL) 
    error = TRUE;
  else {
    // Init values in the object struct
    obj->vertex_format = gx3d_VERTEXFORMAT_DEFAULT;
    // Create a layer in the object
    layer = gx3d_CreateObjectLayer (obj);
    if (layer == NULL) 
      error = TRUE;
    else {
      // Allocate memory for each part of the object
      layer->num_vertices = tg->dimension * tg->dimension * 4;
      layer->num_polygons = tg->dimension * tg->dimension * 2;
      layer->vertex = (gx3dVector *) calloc (layer->num_vertices, sizeof(gx3dVector));
      if (layer->vertex == NULL)
        error = TRUE;
      layer->vertex_normal = (gx3dVector *) calloc (layer->num_vertices, sizeof(gx3dVector));
      if (layer->vertex_normal == NULL)
        error = TRUE;
      layer->polygon = (gx3dPolygon *) calloc (layer->num_polygons, sizeof(gx3dPolygon));
      if (layer->polygon == NULL)
        error = TRUE;
      layer->polygon_normal = (gx3dVector *) calloc (layer->num_polygons, sizeof(gx3dVector));
      if (layer->polygon_normal == NULL)
        error = TRUE;
      layer->tex_coords[0] = (gx3dUVCoordinate *) calloc (layer->num_vertices, sizeof(gx3dUVCoordinate));
      if (layer->tex_coords[0] == NULL)
        error = TRUE;
      layer->tex_coords_w[0] = (float *) calloc (layer->num_vertices, sizeof(float));
      if (layer->tex_coords_w[0] == NULL)
        error = TRUE;
      layer->tex_coords[1] = (gx3dUVCoordinate *) calloc (layer->num_vertices, sizeof(gx3dUVCoordinate));
      if (layer->tex_coords[1] == NULL)
        error = TRUE;
      // Allocate memory for a temp array
      attached_polygon = (word *) calloc (layer->num_vertices * 6, sizeof(word));
      if (attached_polygon == NULL)
        error = TRUE;
    }
  }

/*____________________________________________________________________
|
| Init the grid with values
|___________________________________________________________________*/

  if (NOT error) {

    // Init vertex array
    i = 0;
    z_pos = 0;
    for (z=0; z<tg->dimension; z++) {
      x_pos = 0;
      for (x=0; x<tg->dimension; x++) {
        // Init bottom left vertex
        layer->vertex[i].x = x_pos;
        layer->vertex[i].y = heightmap[z*(tg->dimension+1)+x];
        layer->vertex[i].z = z_pos;
        i++;
        // Init bottom right vertex
        layer->vertex[i].x = x_pos + tg->spacing;
        layer->vertex[i].y = heightmap[z*(tg->dimension+1)+x+1];
        layer->vertex[i].z = z_pos;
        i++;
        // Init top left vertex
        layer->vertex[i].x = x_pos;
        layer->vertex[i].y = heightmap[(z+1)*(tg->dimension+1)+x];
        layer->vertex[i].z = z_pos + tg->spacing;
        i++;
        // Init top right vertex
        layer->vertex[i].x = x_pos + tg->spacing;
        layer->vertex[i].y = heightmap[(z+1)*(tg->dimension+1)+x+1];
        layer->vertex[i].z = z_pos + tg->spacing;
        i++;
        x_pos += tg->spacing;
      }
      z_pos += tg->spacing;
    }

    // Init polygon array
    i = 0;
    for (z=0; z<tg->dimension; z++) {
      for (x=0; x<tg->dimension; x++) {
        layer->polygon[i].index[0] = z*tg->dimension*4 + (x*4)  ;
        layer->polygon[i].index[1] = z*tg->dimension*4 + (x*4)+3;
        layer->polygon[i].index[2] = z*tg->dimension*4 + (x*4)+1;
        i++;
        layer->polygon[i].index[0] = z*tg->dimension*4 + (x*4)  ;
        layer->polygon[i].index[1] = z*tg->dimension*4 + (x*4)+2;
        layer->polygon[i].index[2] = z*tg->dimension*4 + (x*4)+3;
        i++;
      }
    }

    // Init attached polygon array
    for (i=0; i<layer->num_vertices; i++) {
      k = 0;
      for (j=0; j<layer->num_polygons; j++) {
        for (n=0; n<3; n++) {
          // If any point in this polygon has same x,z as point searching for, then this is an attached polygon
          if ((layer->vertex[layer->polygon[j].index[n]].x == layer->vertex[i].x) AND
              (layer->vertex[layer->polygon[j].index[n]].z == layer->vertex[i].z)) {
            attached_polygon[i*6+k] = j + 1;
            k++;
            break;
          }
        }
      }
    }
    
    // Init polygon normal array
    for (i=0; i<layer->num_polygons; i++) 
      gx3d_SurfaceNormal (&(layer->vertex[layer->polygon[i].index[0]]),
                          &(layer->vertex[layer->polygon[i].index[1]]),
                          &(layer->vertex[layer->polygon[i].index[2]]),
                          &(layer->polygon_normal[i]));
    
    // Init vertex normal array
    for (i=0; i<layer->num_vertices; i++) {
      memset (&sum, 0, sizeof(gx3dVector));
      count = 0;
      for (j=0; j<6; j++) {
        k = attached_polygon[i*6+j];
        if (k) {
          sum.x += layer->polygon_normal[k-1].x;
          sum.y += layer->polygon_normal[k-1].y;
          sum.z += layer->polygon_normal[k-1].z;
          count++;
        }
      }
      if (count) {
        sum.x /= (float)count;
        sum.y /= (float)count;
        sum.z /= (float)count;
        gx3d_NormalizeVector (&sum, &(layer->vertex_normal[i]));
      }
    }

    // Free attached polygon array - don't need it anymore
    if (attached_polygon)
      free (attached_polygon);
  }

/*____________________________________________________________________
|
| On any error release all memory, ese attach object to grid
|___________________________________________________________________*/

  if (error) {
    if (obj) 
      gx3d_FreeObject (obj);
  }
  else
    // Attach object to grid
    tg->obj_normal_lod = obj;

  return (NOT error);
}


/*____________________________________________________________________
|
| Function: Set_TexCoords_LOD_Normal
|
| Input: Called from TerrainGrid_Init()
| Output: Sets the texture coords for the normal LOD 3D object for the 
|   terrain grid.
|___________________________________________________________________*/
     
static void Set_TexCoords_LOD_Normal (
  TerrainGrid *tg,
  float        sand_elevation,  // below this line is sand (rock)
  float        rock_elevation,  // below this line is grass, above is rock
  float        snow_elevation ) // above this line is snow
{
  int i, x, z, tile;
  float square_height, height[4], step, slice;
  gx3dObjectLayer *layer;

  static struct {
    float u, v;
  } placement [8][4] = {
    { // normal
      { 0, 1 }, // bottom left
      { 1, 1 }, // bottom right
      { 0, 0 }, // top left
      { 1, 0 }  // top right
    },
    { // rotate 90 clockwise
      { 1, 1 }, // bottom left
      { 1, 0 }, // bottom right
      { 0, 1 }, // top left
      { 0, 0 }  // top right
    },
    { // rotate 180 clockwise
      { 0, 1 }, // bottom left
      { 0, 0 }, // bottom right
      { 1, 1 }, // top left
      { 1, 0 }  // top right
    },
    { // rotate 270 clockwise
      { 0, 0 }, // bottom left
      { 1, 0 }, // bottom right
      { 0, 1 }, // top left
      { 1, 1 }  // top right
    },
    { // normal - mirrored
      { 1, 1 }, // bottom left
      { 0, 1 }, // bottom right
      { 1, 0 }, // top left
      { 0, 0 }  // top right
    },
    { // rotate 90 clockwise - mirrored
      { 1, 0 }, // bottom left
      { 1, 1 }, // bottom right
      { 0, 0 }, // top left
      { 0, 1 }  // top right
    },
    { // rotate 180 clockwise - mirrored
      { 0, 0 }, // bottom left
      { 0, 1 }, // bottom right
      { 1, 0 }, // top left
      { 1, 1 }  // top right
    },
    { // rotate 270 clockwise - mirrored
      { 1, 0 }, // bottom left
      { 0, 0 }, // bottom right
      { 1, 1 }, // top left
      { 0, 1 }  // top right
    }
  };

/*____________________________________________________________________
|
| Init variables
|___________________________________________________________________*/
  
  layer = tg->obj_normal_lod->layer;

/*____________________________________________________________________
|
| Init texture coords
|___________________________________________________________________*/

#define NUM_TEXTURE_SLICES  4

// max elevations for these types of terrain
#define ELEVATION_MUD   0
#define ELEVATION_GRASS 2
#define ELEVATION_ROCK  7
#define ELEVATION_SNOW  30

#define ELEVATION_ROCK_SNOW   30
#define ELEVATION_GRASS_ROCK  2
#define ELEVATION_MUD_GRASS   0


#define TEXTURE_SCALE 1
#define RANGE 0

  // Init terrain type for each grid square
    i = 0;
    for (z=0; z<tg->dimension; z++) {
      for (x=0; x<tg->dimension; x++) {
        // Compute the average height of the quad w/ top left corner at vertex x,z
        square_height = (layer->vertex[z*tg->dimension*4 + (x*4)  ].y +
                         layer->vertex[z*tg->dimension*4 + (x*4)+1].y +
                         layer->vertex[z*tg->dimension*4 + (x*4)+2].y +
                         layer->vertex[z*tg->dimension*4 + (x*4)+3].y) / 4;
        if (square_height <= ELEVATION_MUD)
          tg->terrain_in_square[i] = TerrainGrid_TYPE_MUD;
        else if (square_height <= ELEVATION_GRASS)
          tg->terrain_in_square[i] = TerrainGrid_TYPE_GRASS;
        else if (square_height <= ELEVATION_ROCK)
          tg->terrain_in_square[i] = TerrainGrid_TYPE_ROCK;
        else
          tg->terrain_in_square[i] = TerrainGrid_TYPE_SNOW;
        i++;
      }
    }

    float snow_slice, rock_slice, grass_slice, mud_slice;
    
    step = 1 / (float)NUM_TEXTURE_SLICES;
    snow_slice  = step / 2;
    rock_slice  = step / 2 + step;
    grass_slice = step / 2 + step * 2;
    mud_slice   = step / 2 + step * 3;

    // Init texture coordinate array
    random_Init ();
    i = 0;
    for (z=0; z<tg->dimension; z++) {
      for (x=0; x<tg->dimension; x++) {
        
        // Set w coordinate
        int k;
        for (k=0; k<4; k++) {
          height[0] = layer->vertex[z*tg->dimension*4 + (x*4)+k].y;

          if (height[0] > ELEVATION_ROCK_SNOW)
            slice = snow_slice;
          else if (height[0] > ELEVATION_GRASS_ROCK)
            slice = rock_slice;
          else if (height[0] > ELEVATION_MUD_GRASS)
            slice = grass_slice;
          else
            slice = mud_slice;

          layer->tex_coords_w[0][i*4+k] = slice;
        }

        // Set uv coordinates
        for (k=0; k<4; k++)
          height[k] = layer->vertex[z*tg->dimension*4 + (x*4)+k].y;

        // This square all pebble?
        if ((height[0] < ELEVATION_MUD-(RANGE/2)) AND
            (height[1] < ELEVATION_MUD-(RANGE/2)) AND
            (height[2] < ELEVATION_MUD-(RANGE/2)) AND
            (height[3] < ELEVATION_MUD-(RANGE/2))) 
          tile = random_GetInt (0, 7);
        // This square all grass?
        else if ((height[0] > ELEVATION_MUD+(RANGE/2)) AND (height[0] < ELEVATION_GRASS-(RANGE/2)) AND
                 (height[1] > ELEVATION_MUD+(RANGE/2)) AND (height[1] < ELEVATION_GRASS-(RANGE/2)) AND
                 (height[2] > ELEVATION_MUD+(RANGE/2)) AND (height[2] < ELEVATION_GRASS-(RANGE/2)) AND
                 (height[3] > ELEVATION_MUD+(RANGE/2)) AND (height[3] < ELEVATION_GRASS-(RANGE/2)))
          tile = random_GetInt (0, 7);
        // This square all rock?
        else if ((height[0] > ELEVATION_GRASS+(RANGE/2)) AND (height[0] < ELEVATION_ROCK-(RANGE/2)) AND
                 (height[1] > ELEVATION_GRASS+(RANGE/2)) AND (height[1] < ELEVATION_ROCK-(RANGE/2)) AND
                 (height[2] > ELEVATION_GRASS+(RANGE/2)) AND (height[2] < ELEVATION_ROCK-(RANGE/2)) AND
                 (height[3] > ELEVATION_GRASS+(RANGE/2)) AND (height[3] < ELEVATION_ROCK-(RANGE/2)))
          tile = random_GetInt (0, 7);
        // This square all snow?
        else if ((height[0] > ELEVATION_ROCK+(RANGE/2)) AND
                 (height[1] > ELEVATION_ROCK+(RANGE/2)) AND
                 (height[2] > ELEVATION_ROCK+(RANGE/2)) AND
                 (height[3] > ELEVATION_ROCK+(RANGE/2))) 
          tile = random_GetInt (0, 7);
        else 
          tile = random_GetInt (0, 1) * 4;
        
        // Init bottom left vertex
        layer->tex_coords  [0][i*4+0].u = placement[tile][0].u * TEXTURE_SCALE;
        layer->tex_coords  [0][i*4+0].v = placement[tile][0].v * TEXTURE_SCALE;
        // Init bottom right vertex
        layer->tex_coords  [0][i*4+1].u = placement[tile][1].u * TEXTURE_SCALE;
        layer->tex_coords  [0][i*4+1].v = placement[tile][1].v * TEXTURE_SCALE;
        // Init top left vertex
        layer->tex_coords  [0][i*4+2].u = placement[tile][2].u * TEXTURE_SCALE;
        layer->tex_coords  [0][i*4+2].v = placement[tile][2].v * TEXTURE_SCALE;
        // Init top right vertex
        layer->tex_coords  [0][i*4+3].u = placement[tile][3].u * TEXTURE_SCALE;
        layer->tex_coords  [0][i*4+3].v = placement[tile][3].v * TEXTURE_SCALE;

        // Use detail texture for second texture
        // Init bottom left vertex
        layer->tex_coords[1][i*4+0].u = 0;
        layer->tex_coords[1][i*4+0].v = 1;
        // Init bottom right vertex
        layer->tex_coords[1][i*4+1].u = 1;
        layer->tex_coords[1][i*4+1].v = 1;
        // Init top left vertex
        layer->tex_coords[1][i*4+2].u = 0;
        layer->tex_coords[1][i*4+2].v = 0;
        // Init top right vertex
        layer->tex_coords[1][i*4+3].u = 1;
        layer->tex_coords[1][i*4+3].v = 0;
        
        // Incr to next square
        i++;
      }
    }
}

/*____________________________________________________________________
|
| Function: TerrainGrid_Free
|
| Input: Called from ____
| Output: Frees the terraingrid object.
|___________________________________________________________________*/

void TerrainGrid_Free (TerrainGrid *tg)
{
  if (tg) {
    if (tg->terrain_in_square)
      free (tg->terrain_in_square);
    if (tg->obj_normal_lod)
      gx3d_FreeObject (tg->obj_normal_lod);
//    if (tg->obj_low_lod)
//      gx3d_FreeObject (tg->obj_low_lod);
    free (tg);
  }
}

/*____________________________________________________________________
|
| Function: TerrainGrid_Get_Height
|
| Input: Called from ____
| Output: Returns the height of a point on the grid or 0 if point is
|   not on the grid.
|___________________________________________________________________*/
 
float TerrainGrid_Get_Height (TerrainGrid *tg, float x, float z)
{
  int row, col, square;
  float zoffset, xoffset, h1, h2, h3, hx, hd, t;
  float height = 0;

  // Verify point is on the grid
  if ((x >= 0) AND (x < (tg->dimension * tg->spacing)) AND
      (z >= 0) AND (z < (tg->dimension * tg->spacing))) {
    // Compute the grid square in
    row = (int)(z / tg->spacing);
    col = (int)(x / tg->spacing);
    square = row * tg->dimension * 4 + col * 4;

    // Compute the offset within the square
    zoffset = z - (row * tg->spacing);
    xoffset = x - (col * tg->spacing);

    // In bottom right triangle?
    if (xoffset > zoffset) {
      h1 = tg->obj_normal_lod->layer->vertex[square + 0].y;
      h2 = tg->obj_normal_lod->layer->vertex[square + 1].y;  // from h1 along x-axis
      h3 = tg->obj_normal_lod->layer->vertex[square + 3].y;  // from h1 along z-axis
    }
    // In top left triangle
    else {
      h1 = tg->obj_normal_lod->layer->vertex[square + 0].y;
      h2 = tg->obj_normal_lod->layer->vertex[square + 2].y;  // from h1 along x-axis
      h3 = tg->obj_normal_lod->layer->vertex[square + 3].y;  // from h1 along z-axis
      t = xoffset;
      xoffset = zoffset;
      zoffset = t;
    }
    hx = h1 + (xoffset / tg->spacing) * (h2-h1);
    hd = h1 + (xoffset / tg->spacing) * (h3-h1);
    height = hx + (zoffset / xoffset) * (hd - hx);
  }

  return (height);
}
