#pragma once

#include <vector>

#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/Nef_polyhedron_3.h>
#include <CGAL/Polyhedron_incremental_builder_3.h>
#include <CGAL/IO/Polyhedron_iostream.h>

#include <osg/Geode>

class CCgal3DObject
{
public:
	typedef CGAL::Exact_predicates_exact_constructions_kernel Kernel;
	typedef CGAL::Polyhedron_3<Kernel>  Polyhedron;
	typedef CGAL::Nef_polyhedron_3<Kernel> Nef_polyhedron;
	typedef Polyhedron::HalfedgeDS HalfedgeDS;
	typedef HalfedgeDS::Vertex   CGALVertex;
	typedef CGALVertex::Point CGALPoint;
	typedef Polyhedron::Point_iterator Point_iterator;
	typedef Polyhedron::Facet_iterator Facet_iterator;
	typedef Polyhedron::Halfedge_around_facet_circulator Halfedge_facet_circulator;

public:
	struct VFMesh {
		std::vector<CGALPoint> pointlist;//点;
		std::vector<std::vector<int>> facelist;//面，保存面的点的索引;
	};

	template <class HDS>
	class CgalPolyhedron : public CGAL::Modifier_base<HDS>
	{
	public:
		CgalPolyhedron(VFMesh &mesh) : m_mesh(mesh) {}
		void operator()(HDS& hds);
	private:
		VFMesh m_mesh;
	};

	template <class HDS>
	void CCgal3DObject::CgalPolyhedron<HDS>::operator()(HDS& hds)
	{
		CGAL::Polyhedron_incremental_builder_3<HDS> B(hds, true);
		int pointsize = m_mesh.pointlist.size();
		int facessize = m_mesh.facelist.size();
		B.begin_surface(pointsize, facessize, 0);
		for (int i = 0; i < pointsize; i++)
		{
			B.add_vertex(CGALPoint(m_mesh.pointlist[i].x(), m_mesh.pointlist[i].y(), m_mesh.pointlist[i].z()));
		}
		for (int i = 0; i < facessize; i++)
		{
			B.begin_facet();
			for (int j = 0; j < m_mesh.facelist[i].size(); j++)
			{
				B.add_vertex_to_facet(m_mesh.facelist[i][j]);
			}
			B.end_facet();
		}
		B.end_surface();
	}

public:
	CCgal3DObject();
	~CCgal3DObject();

	void mesh2polyhedron(VFMesh& mesh, Polyhedron& P);
	void polyhedron2mesh(Polyhedron& P, VFMesh& mesh);

	void geode2Mash(osg::Geode* pSphere, VFMesh& mesh, int iHint);
	osg::Geometry* mash2Geom(VFMesh& mesh, bool bIsLine);
};

