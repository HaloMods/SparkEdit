// LightwaveImporter.cpp: implementation of the CLightwaveImporter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sparkedit.h"
#include "LightwaveImporter.h"
#include "OutputPane.h"
#include "HaloMapFile.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern COutputPane *g_pOutput;
extern CHaloMapFile HaloMap;
extern CString gSE_WorkingDir;

/*-------------------------------------------------------------------
 * Name: CLightwaveImporter()
 * Description:
 *   
 *-----------------------------------------------------------------*/
CLightwaveImporter::CLightwaveImporter()
{
  ZeroMemory(m_CurrentVerts, sizeof(m_CurrentVerts));
  ZeroMemory(m_CurrentIndices, sizeof(m_CurrentIndices));

  m_CurrentVertCount = 0;
  m_CurrentIndicesCount = 0;
  m_CurrentMeshCount = 0;
  m_bFirstPass = TRUE;
  ZeroMemory(&m_Mesh, sizeof(m_Mesh));
}

/*-------------------------------------------------------------------
 * Name: ~CLightwaveImporter()
 * Description:
 *   
 *-----------------------------------------------------------------*/
CLightwaveImporter::~CLightwaveImporter()
{

}

/*-------------------------------------------------------------------
 * Name: OpenObjFile()
 * Description:
 *   
 *-----------------------------------------------------------------*/
void CLightwaveImporter::OpenObjFile(CString path)
{
  CString str;
  BOOL bResult = FALSE;
  int map_index = -1;

  m_ObjFile.Open(path, CFile::modeRead);

  str.Format("Importing mesh file: %s\n", path);
  g_pOutput->PostText(str, LOG_BLUE);
}

/*-------------------------------------------------------------------
 * Name: ParseObjFile()
 * Description:
 *   
 *-----------------------------------------------------------------*/
void CLightwaveImporter::ParseObjFile()
{
  CString str;

  do
  {
    m_ObjFile.ReadString(str);

    if(str.GetLength() == 0)
      break;

    if(str[0] == 'g')
    {
      //save the existing submesh to the (if not the first one)
      if(m_bFirstPass == FALSE)
      {
        UpdateXboxSubmeshList();
        m_CurrentMeshCount++;
      }
      else
      {
        m_bFirstPass = FALSE;
      }

      m_CurrentVertCount = 0;
      m_CurrentIndicesCount = 0;
    }
    else if(str[0] == 'v')
    {
      //add a vertex to the current submesh
      ReadVertex(str, m_CurrentVerts[m_CurrentVertCount]);
      m_CurrentVertCount++;
    }
    else if(str[0] == 'f')
    {
      //add an index to the current submesh
      ReadTriangleIndex(str, m_CurrentIndices[m_CurrentIndicesCount]);
      m_CurrentIndicesCount++;
    }
  }while(str != "");

  UpdateXboxSubmeshList(); //once more for the last submesh
  m_CurrentMeshCount++;
}

/*-------------------------------------------------------------------
 * Name: Cleanup()
 * Description:
 *   
 *-----------------------------------------------------------------*/
void CLightwaveImporter::Cleanup()
{
  int i;

  for(i=0; i<m_CurrentMeshCount; i++)
  {
    if(m_Mesh[i].pVert)
      delete [] m_Mesh[i].pVert;

    if(m_Mesh[i].pIndex)
      delete [] m_Mesh[i].pIndex;
  }

  m_ObjFile.Close();
}

/*-------------------------------------------------------------------
 * Name: UpdateSubmeshList()
 * Description:
 *   
 *-----------------------------------------------------------------*/
void CLightwaveImporter::UpdateXboxSubmeshList()
{
  int i;

  //allocate vert structs, update
  m_Mesh[m_CurrentMeshCount].pVert = new COMPRESSED_BSP_VERT[m_CurrentVertCount];

  m_Mesh[m_CurrentMeshCount].VertCount = m_CurrentVertCount;

  if(m_Mesh[m_CurrentMeshCount].pVert)
  {
    for(i=0; i<m_CurrentVertCount; i++)
    {
      m_Mesh[m_CurrentMeshCount].pVert[i].vertex_k[0] = m_CurrentVerts[i][0];
      m_Mesh[m_CurrentMeshCount].pVert[i].vertex_k[1] = m_CurrentVerts[i][1];
      m_Mesh[m_CurrentMeshCount].pVert[i].vertex_k[2] = m_CurrentVerts[i][2];
    }
  }

  //allocate index structs, update
  m_Mesh[m_CurrentMeshCount].pIndex = new TRI_INDICES[m_CurrentIndicesCount];

  m_Mesh[m_CurrentMeshCount].IndexCount = m_CurrentIndicesCount;

  if(m_Mesh[m_CurrentMeshCount].pIndex)
  {
    for(i=0; i<m_CurrentIndicesCount; i++)
    {
      m_Mesh[m_CurrentMeshCount].pIndex[i].tri_ind[0] = m_CurrentIndices[i][0];
      m_Mesh[m_CurrentMeshCount].pIndex[i].tri_ind[1] = m_CurrentIndices[i][1];
      m_Mesh[m_CurrentMeshCount].pIndex[i].tri_ind[2] = m_CurrentIndices[i][2];
    }
  }
}

/*-------------------------------------------------------------------
 * Name: ReadVertex()
 * Description:
 *   
 *-----------------------------------------------------------------*/
void CLightwaveImporter::ReadVertex(CString str, float *pVert)
{
  char *pTemp;
  char junk[10];

  pTemp = str.LockBuffer();
  sscanf(pTemp, "%s %f %f %f", &junk, &pVert[0], &pVert[1], &pVert[2]);
  str.ReleaseBuffer();
}

/*-------------------------------------------------------------------
 * Name: ReadTriangleIndex()
 * Description:
 *   
 *-----------------------------------------------------------------*/
void CLightwaveImporter::ReadTriangleIndex(CString str, USHORT *pIndex)
{
  char *pTemp;
  char junk[10];

  pTemp = str.LockBuffer();
  sscanf(pTemp, "%s %d %d %d", &junk, &pIndex[0], &pIndex[1], &pIndex[2]);
  str.ReleaseBuffer();
}

/*-------------------------------------------------------------------
 * Name: VerifyAgainstLoadedMap()
 * Description:
 *   
 *-----------------------------------------------------------------*/
BOOL CLightwaveImporter::VerifyAgainstLoadedMap()
{

  BOOL bStatus = TRUE;
/*  int m;
  CString str;

  if(HaloMap.m_Bsp2.m_SubMeshCount != m_CurrentMeshCount)
  {
    AfxMessageBox("Imported Mesh Count does not match loaded map.  Import Failed.");
    bStatus = FALSE;
  }
  else
  {
    for(m=0; m<m_CurrentMeshCount; m++)
    {
      if(m_Mesh[m].IndexCount != HaloMap.m_Bsp2.m_pMesh[m].IndexCount)
      {
        str.Format("Imported index count for mesh[%d] does not match loaded map.  Import Failed.", m);
        AfxMessageBox(str);
        bStatus = FALSE;
        break;
      }

      if(m_Mesh[m].VertCount != HaloMap.m_Bsp2.m_pMesh[m].VertCount)
      {
        str.Format("Imported vertex count for mesh[%d] does not match loaded map.  Import Failed.", m);
        AfxMessageBox(str);
        bStatus = FALSE;
        break;
      }
    }    
  }*/
  return(bStatus);
}

/*-------------------------------------------------------------------
 * Name: LoadImportedDataIntoBsp()
 * Description:
 *   
 *-----------------------------------------------------------------*/
void CLightwaveImporter::LoadImportedDataIntoBsp()
{
//  int m, v;
/*
  for(m=0; m<m_CurrentMeshCount; m++)
  {
    for(v=0; v<m_Mesh[m].VertCount; v++)
    {
      HaloMap.m_Bsp2.m_pMesh[m].pVert[v].vertex_k[0] = m_Mesh[m].pVert[v].vertex_k[0];
      HaloMap.m_Bsp2.m_pMesh[m].pVert[v].vertex_k[1] = m_Mesh[m].pVert[v].vertex_k[1];
      HaloMap.m_Bsp2.m_pMesh[m].pVert[v].vertex_k[2] = m_Mesh[m].pVert[v].vertex_k[2];
    }
  }*/
}

/*-------------------------------------------------------------------
 * Name: SaveCollisionVertFormat()
 * Description:
 *   
 *-----------------------------------------------------------------*/
void CLightwaveImporter::SaveCollisionVertFormat(CString filename)
{
  BSP_COLLISION *pColl;
  BSP_COLL_INDEX *pOutput;
  int c, m,v;
  float vis_vert[3];
  float coll_vert[3];
  UINT coll_offset = 0, coll_count = 0;
  BOOL bSearching;
  CFile *pMapFile = NULL;
  CFile OutFile;
  UINT valid_count = 0;

  //read in collision data for processing
//  HaloMap.GetCollisionInfo(&coll_offset, &coll_count, &pMapFile);

  pColl = new BSP_COLLISION[coll_count];
  pMapFile->Seek(coll_offset, 0);
  pMapFile->Read(pColl, coll_count*sizeof(BSP_COLLISION));

  //allocate output file
  pOutput = new BSP_COLL_INDEX[coll_count];
  ZeroMemory(pOutput, coll_count*sizeof(BSP_COLL_INDEX));

  //figure out the visible-to-bsp mapping
  for(c=0; c<coll_count; c++)
  {
    coll_vert[0] = pColl[c].vert[0];
    coll_vert[1] = pColl[c].vert[1];
    coll_vert[2] = pColl[c].vert[2];

    bSearching = TRUE;
    for(m=0; ((m<m_CurrentMeshCount)&&(bSearching)); m++)
    {
      for(v=0; ((v<m_Mesh[m].VertCount)&&(bSearching)); v++)
      {
        vis_vert[0] = m_Mesh[m].pVert[v].vertex_k[0];
        vis_vert[1] = m_Mesh[m].pVert[v].vertex_k[1];
        vis_vert[2] = m_Mesh[m].pVert[v].vertex_k[2];
        
        if(DoVertsMatch(vis_vert, coll_vert)==TRUE)
        {
          bSearching = FALSE;
          pOutput[valid_count].CollisionIndex = c;
          pOutput[valid_count].VisibleMeshIndex = m;
          pOutput[valid_count].VisibleVertexIndex = v;
          //TRACE("c[%d] m[%d] v[%d] = (%.3f, %.3f, %.3f) (%.3f, %.3f, %.3f)\n", c, m, v, vis_vert[0],vis_vert[1],vis_vert[2],coll_vert[0],coll_vert[1],coll_vert[2]);
          valid_count++;
        }
      }
    }

    if(bSearching)
      TRACE("Failed to find a match %d\n", c);
  }
  //items to save:
  //header: number of collision verts
  //for each collision vert:  current index, (so we can skip bad ones), visible submesh index, visible vertex index

  //save collision indexing info to a file
  OutFile.Open(filename, CFile::modeCreate|CFile::modeWrite);
  OutFile.Write(&valid_count, 4);
  OutFile.Write(pOutput, valid_count*sizeof(BSP_COLL_INDEX));
  OutFile.Close();

  if(pColl)
    delete [] pColl;

  if(pOutput)
    delete [] pOutput;
}

/*-------------------------------------------------------------------
 * Name: DoVertsMatch()
 * Description:
 *   
 *-----------------------------------------------------------------*/
BOOL CLightwaveImporter::DoVertsMatch(float *pV1, float *pV2)
{
  BOOL bMatch = FALSE;

  float d1, d2, d3;

  d1 = fabs(pV1[0] - pV2[0]);
  d2 = fabs(pV1[1] - pV2[1]);
  d3 = fabs(pV1[2] - pV2[2]);


  if((d1 < 0.0001)&&(d2 < 0.0001)&&(d3 < 0.0001))
    bMatch = TRUE;

  return(bMatch);
}

void CLightwaveImporter::UpdateCollisionMesh(void)
{
  CFile CollFile;
  CFile *pMapFile = NULL;
  BSP_COLL_INDEX *pCollIndex = NULL;
  BSP_COLLISION *pColl = NULL;
  UINT coll_index_count;
  UINT coll_offset = 0, coll_count = 0, c;
  CString coll_file_path;

  coll_file_path = gSE_WorkingDir + "\\coll\\bloodgulch.col";

  // read in collision indexing info
  CollFile.Open(coll_file_path, CFile::modeRead);
  CollFile.Read(&coll_index_count, 4);
  pCollIndex = new BSP_COLL_INDEX[coll_index_count];
  CollFile.Read(pCollIndex, coll_index_count*sizeof(BSP_COLL_INDEX));
  CollFile.Close();

  // read in original collision data from Map File
//  HaloMap.GetCollisionInfo(&coll_offset, &coll_count, &pMapFile);

  pColl = new BSP_COLLISION[coll_count];
  pMapFile->Seek(coll_offset, 0);
  pMapFile->Read(pColl, coll_count*sizeof(BSP_COLLISION));

  // Update the existing collision mesh with new verts
  for(c=0; c<coll_index_count; c++)
  {
    pColl[pCollIndex[c].CollisionIndex].vert[0] = m_Mesh[pCollIndex[c].VisibleMeshIndex].pVert[pCollIndex[c].VisibleVertexIndex].vertex_k[0];
    pColl[pCollIndex[c].CollisionIndex].vert[1] = m_Mesh[pCollIndex[c].VisibleMeshIndex].pVert[pCollIndex[c].VisibleVertexIndex].vertex_k[1];
    pColl[pCollIndex[c].CollisionIndex].vert[2] = m_Mesh[pCollIndex[c].VisibleMeshIndex].pVert[pCollIndex[c].VisibleVertexIndex].vertex_k[2];
  }

  // Write out changes to Map File
  pMapFile->Seek(coll_offset, 0);
  pMapFile->Write(pColl, coll_count*sizeof(BSP_COLLISION));

  if(pColl)
    delete [] pColl;

  if(pCollIndex)
    delete [] pCollIndex;
}
