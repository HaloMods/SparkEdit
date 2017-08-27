// Render.h: interface for the CRender class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RENDER_H__6388EF3B_3D8F_49B3_A5F9_C1084C1E9678__INCLUDED_)
#define AFX_RENDER_H__6388EF3B_3D8F_49B3_A5F9_C1084C1E9678__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "timer.h"
#include "HaloStructDefs.h"
#include "XboxModel.h"
#include "PcModel.h"
#include "util.h"
 
typedef enum ENUM_RENDER_MODE
{
  RENDER_POINTS = 0,
  RENDER_TRIS,
  RENDER_FLAT_SHADING,
  RENDER_TEXTURED,
  RENDER_TEXTURED_TRIS,
  RENDER_LIGHTMAP,
  RENDER_LIGHTMAP_BLEND,
  RENDER_DETAIL_MAPPING
}RENDER_MODE;

#define FRAME_RATE_BUFFER_SIZE  10

class CRender : CUtil
{
public:
	void RenderDetailMapping(UINT mesh_index);
	void SetupFog(void);
	void SetActiveMarker(int index);
	void SetSkyboxModel(CPcModel *pPcModel, CXboxModel *pXboxModel);
	void SetModelLOD(MODEL_LOD lod);
	BOOL RenderModel(void);
	void SetActiveRenderModel(int index);
	void RenderTestObject(void);
	float GetFrameRate(void);
	void TimerEnd(void);
	void TimerStart(void);
	BOOL m_bEnableRender;
	void EnableRender(BOOL bEnable);
	BOOL IsInFrustum(BOUNDING_BOX *pBox);
	float GetFrameTime(void);
	CRender();
	virtual ~CRender();
  void SetFindCoord(float x, float y, float z);
	void SetRenderMode(RENDER_MODE mode);
	void Initialize(int argc, TCHAR *argv[]);

  //void RenderXboxMap(BOOL bSelectMode);
	void RenderVisibleBsp(BOOL bSelectMode);  

protected:
	void RenderTexture(UINT mesh_index);
	void RenderPcPoints(UINT mesh_index);
	void RenderPcSubmeshLines(UINT mesh_index);
	void RenderTextureAndLightmaps(UINT mesh_index);
  void RenderPcFlatShadedPolygons(UINT mesh_index);

	void RenderXboxPoints(UINT mesh_index);
	void RenderXboxSubmeshLines(UINT mesh_index);
	void RenderXboxTexturedSubmesh(UINT mesh_index);
  void RenderXboxFlatShadedPolygons(UINT mesh_index);

	void RenderLocation(void);
	void ResetMeshColors(void);
	void SetNextMeshColor(void);
  void DrawOriginVector(void);
  void IdleAnimate(void);

  float m_CamAng;
  CTimer m_RenderTimer;
  float m_RenderTime;

  float m_FrameTimes[FRAME_RATE_BUFFER_SIZE];
  int m_FrameIndex;
  int m_ActiveMarkerIndex;
  MODEL_LOD m_ModelLOD;

  struct STRUCT_POLYCOLOR
  {
    float red;
    float green;
    float blue;
    int color_count;
  }m_PolyColor;

  RENDER_MODE m_RenderMode;
  CPcModel *m_pPcSkybox;
  CXboxModel *m_pXboxSkybox;

  float m_FindCoord[3];
  int m_GreenPulseCount;
  int m_DebugPlaneCounter;
  int m_ActiveRenderModel;
public:
  void RenderLightmap(UINT mesh_index);
};

#endif // !defined(AFX_RENDER_H__6388EF3B_3D8F_49B3_A5F9_C1084C1E9678__INCLUDED_)
