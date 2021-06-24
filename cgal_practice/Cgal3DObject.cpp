#include "Cgal3DObject.h"

#include <osg/Geometry>

CCgal3DObject::CCgal3DObject()
{
}


CCgal3DObject::~CCgal3DObject()
{
}

void CCgal3DObject::mesh2polyhedron(VFMesh &mesh, Polyhedron &P)
{
	CgalPolyhedron<HalfedgeDS> builder(mesh);
	P.delegate(builder);
}

void CCgal3DObject::polyhedron2mesh(Polyhedron &P, VFMesh &mesh)
{
	mesh.facelist.clear();
	mesh.pointlist.clear();
	
	for (Point_iterator k = P.points_begin(); k != P.points_end(); ++k) {
		mesh.pointlist.push_back(*k);
	}

	std::vector<int> face;
	for (Facet_iterator i = P.facets_begin(); i != P.facets_end(); ++i)
	{
		Halfedge_facet_circulator j = i->facet_begin();
		CGAL_assertion(CGAL::circulator_size(j) >= 3);

		face.clear();
		do
		{
			face.push_back(distance(P.vertices_begin(), j->vertex()));
		} while (++j != i->facet_begin());

		if (face.size() >= 3)
		{
			mesh.facelist.push_back(face);
		}
	}
}

void CCgal3DObject::geode2Mash(osg::Geode* pSphere, VFMesh& mesh, int iHint)
{
	osg::Geometry* pSphere1Geom = static_cast<osg::Geometry*>(pSphere->getDrawable(0));
	osg::Vec3Array* pSphere1Arr = static_cast<osg::Vec3Array*>(pSphere1Geom->getVertexArray());
	std::vector<osg::DrawElements*> vecS1DrawEles;
	bool bTag = pSphere1Geom->getDrawElementsList(vecS1DrawEles);
	osg::DrawElementsUInt* pSphere1Ele = static_cast<osg::DrawElementsUInt*>(vecS1DrawEles[0]);
	//osg::Vec3 v3Sphere1Center = pSphere->getWorldMatrices()[0].getTrans();

	int iNumCircle = iHint * 2;
	osg::Vec3 v3Tem = pSphere1Arr->at(0);//+v3Sphere1Center
	mesh.pointlist.push_back(CGALPoint(v3Tem.x(), v3Tem.y(), v3Tem.z()));
	for (int j = iNumCircle + 1; j < pSphere1Arr->size() - iNumCircle - 1; j++)
	{
		if ((j + 1) % (iNumCircle + 1) == 0)
			continue;
		v3Tem = pSphere1Arr->at(j);//+v3Sphere1Center
		mesh.pointlist.push_back(CGALPoint(v3Tem.x(), v3Tem.y(), v3Tem.z()));
	}
	v3Tem = pSphere1Arr->at(pSphere1Arr->size() - 1);//+v3Sphere1Center
	mesh.pointlist.push_back(CGALPoint(v3Tem.x(), v3Tem.y(), v3Tem.z()));

	std::vector<int> face;
	for (int j = 1; j < iNumCircle; j++)
	{
		face.clear();
		face.push_back(0);
		face.push_back(j);
		face.push_back(j + 1);
		mesh.facelist.push_back(face);
	}
	face.clear();
	face.push_back(0);
	face.push_back(iNumCircle);
	face.push_back(1);
	mesh.facelist.push_back(face);

	int iVertexNum = iNumCircle*(iNumCircle / 2 - 1) + 2;
	int tt = mesh.pointlist.size();
	for (int i = 0; i < iNumCircle / 2 - 2; i++)
	{
		for (int j = iNumCircle*i + 1; j < iNumCircle*(i + 1); j++)
		{
			face.clear();
			face.push_back(j);
			face.push_back(j + iNumCircle);
			face.push_back(j + 1);
			mesh.facelist.push_back(face);
			face.clear();
			face.push_back(j + 1);
			face.push_back(j + iNumCircle);
			face.push_back(j + 1 + iNumCircle);
			mesh.facelist.push_back(face);
		}
		int k = iNumCircle*(i + 1);
		face.clear();
		face.push_back(k);
		face.push_back(k + iNumCircle);
		face.push_back(iNumCircle*i + 1);
		mesh.facelist.push_back(face);
		face.clear();
		face.push_back(iNumCircle*i + 1);
		face.push_back(k + iNumCircle);
		face.push_back(iNumCircle*(i + 1) + 1);
		mesh.facelist.push_back(face);
	}
	for (int j = iVertexNum - 2; j > iVertexNum - 1 - iNumCircle; j--)
	{
		face.clear();
		face.push_back(iVertexNum - 1);
		face.push_back(j);
		face.push_back(j - 1);
		mesh.facelist.push_back(face);
	}
	face.clear();
	face.push_back(iVertexNum - 1);
	face.push_back(iVertexNum - 1 - iNumCircle);
	face.push_back(iVertexNum - 2);
	mesh.facelist.push_back(face);
	return;
}

osg::Geometry* CCgal3DObject::mash2Geom(VFMesh& mesh, bool bIsLine)
{
	osg::Geometry* pGeom = new osg::Geometry;
	osg::Vec3Array* pVexArr = new osg::Vec3Array;
	for (int i = 0; i < mesh.pointlist.size(); i++)
	{
		double x, y, z;
		x = mesh.pointlist.at(i).x().exact().to_double();
		y = mesh.pointlist.at(i).y().exact().to_double();
		z = mesh.pointlist.at(i).z().exact().to_double();
		pVexArr->push_back(osg::Vec3(x, y, z));
	}
	osg::DrawElementsUInt* pDrawEles;
	if (bIsLine)
	{
		pDrawEles = new osg::DrawElementsUInt(osg::PrimitiveSet::LINES);
		for (int i = 0; i < mesh.facelist.size(); i++)
		{
			std::vector<int> pFace = mesh.facelist.at(i);
			for (int j = 0; j < pFace.size(); j++)
			{
				pDrawEles->push_back(pFace[j]);
				pDrawEles->push_back(pFace[(j + 1) % pFace.size()]);
			}
		}
	}
	else
	{
		pDrawEles = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES);
		for (int i = 0; i < mesh.facelist.size(); i++)
		{
			std::vector<int> pFace = mesh.facelist.at(i);
			for (int j = 0; j < pFace.size(); j++)
			{
				pDrawEles->push_back(pFace[j]);
			}
		}
	}
	pGeom->setVertexArray(pVexArr);
	pGeom->addPrimitiveSet(pDrawEles);
	return pGeom;
}
