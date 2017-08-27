// SelectionObject.h: interface for the CSelectionObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SELECTIONOBJECT_H__31C98BB3_DDDC_445B_A12E_F2396BC837B0__INCLUDED_)
#define AFX_SELECTIONOBJECT_H__31C98BB3_DDDC_445B_A12E_F2396BC837B0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HaloStructDefs.h"
#include "timer.h"
#include "Util.h"

#define SEL_BOX_SIZE   0.1f
#define SELECTION_COUNT_MAX 200

typedef enum ENUM_EDITOR_MODE
{
  EDITOR_MODE_SELECT,
  EDITOR_MODE_TRANSLATE,
  EDITOR_MODE_ROTATE
}EDITOR_MODE;

typedef struct STRUCT_SELECTION_OBJECT
{
  float loc[3];
  char  name[128];
  BOOL  bSelected;
}SELECTION_OBJECT;

class CSelectionManager : public CUtil
{
public:
	void ToggleEditorMode(void);
	BOOL DeleteActiveSelection(void);
	BOOL IsObjectSelected(void);
	SELECTION_MODE GetActiveSelectionMode(void);
	BOOL DuplicateActiveSelection(void);
	void SetActiveSelectionRotation(float yaw, float pitch, float roll, int mode);
	void SetCurrentEditorMode(EDITOR_MODE mode);
	EDITOR_MODE GetCurrentEditorMode(void);
	int m_ReferenceSelection;
	void GetReferenceCoordinate(float *pCoord);
	void EnableTriggerDraw(BOOL bEnable);
	void RenderBox(BOUNDING_BOX *pBox, float *pClr);
	void Cleanup(void);
	BOOL IsHighDetailEnabled(SELECTION_MODE sel_mode, float cam_dist);
	void Initialize(UINT version);
	void Selection_ProcessPick(void);
	void Selection_InitSelectionTest(double x, double y);
	void Selection_AfterRender(void);
	void Selection_BeforeRender(int i);
	void SetMaxTranslateSpeed(float scale);
	void MoveSelection(float x, float y, float z, float yaw, float pitch, float roll, BOOL bEnableAcc);
	void SetEditCursorMode(float *pMoveVector);
	void SetSelectionMode(SELECTION_MODE sel_mode);
	void PerformSelectionTest(double x, double y);
	void RenderSelectionObjects(void);
	CSelectionManager();
	virtual ~CSelectionManager();

protected:
	void RenderInactiveEditBox(int index, float *pCoord, SELECTION_MODE sel_mode);
	void RenderActiveSelectionBox(int index, float *pCoord, BOOL bSelected, SELECTION_MODE sel_mode);
  UINT GetObjectIndex(GLint hits, GLuint *pSelBuf);
	void DrawMovementCursor(int index, BOOL bEnable, float *pCoord);

  SELECTION_OBJECT *m_pObj;
  UINT m_ActiveCount;
  UINT m_Version;
  float m_ActiveZPlane;

  SELECTION_MODE m_ActiveSelectionMode;
  EDITOR_MODE m_CurrentEditorMode;
  float m_EditMode[3];
  CTimer m_Timer;
  CTimer m_MoveAccTimer;
  float m_AccFactor;
  int m_AccCount;
  float m_AccMax;
  float m_GroupColor[SELECTION_MODE_MAX][3];
  GLuint m_SelBuffer[512];
  BOOL m_bDrawTriggers;
};

#endif // !defined(AFX_SELECTIONOBJECT_H__31C98BB3_DDDC_445B_A12E_F2396BC837B0__INCLUDED_)
