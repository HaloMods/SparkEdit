#pragma once

typedef struct STRUCT_VERTEX
{
  float x;
  float y;
  float z;
}VERTEX;

typedef struct STRUCT_PLANE
{
  float norm[3];
  float distance;
}PLANE;

typedef struct STRUCT_TAG_REFERENCE
{
  char tag[4];
  UINT NamePtr;
  UINT unknown;
  UINT TagId;
}TAG_REFERENCE;

typedef struct STRUCT_REFLEXIVE
{
  UINT Count;
  UINT Offset;
  void* unknown;
}REFLEXIVE;

typedef struct
{
  float min[3];
  float max[3];
}BOUNDING_BOX;

typedef struct
{
  float x[2];
  float y[2];
  float z[2];
}BOUNDING_BOX2;
