#include "MeshHandler.h"

void CMeshHandler::AddMesh(CStaticMesh *m)
{
	s_meshes.Add(m);
}

void CMeshHandler::AddMesh(CAnimatedMesh *m)
{
	a_meshes.Add(m);
}

CStaticMesh *CMeshHandler::GetStaticMesh(const char *name)
{
	for(CNode<CStaticMesh> *m_node = s_meshes.GFDN(); m_node; m_node = m_node->next)
	{
		CStaticMesh *m = m_node->GetData();

		if(strcmp(name, m->name) == 0)
			return m;
	}

	return NULL;
}

CAnimatedMesh *CMeshHandler::GetAnimatedMesh(const char *name)
{
	for(CNode<CAnimatedMesh> *m_node = a_meshes.GFDN(); m_node; m_node = m_node->next)
	{
		CAnimatedMesh *m = m_node->GetData();

		if(strcmp(name, m->name) == 0)
			return m;
	}

	return NULL;
}