/**************************************************************************************************
* @file RadarPowerZone.h
* @note 雷达威力区类
* @author 00278
* @data 2020-8-12
**************************************************************************************************/
#include <osgDB/WriteFile>
#include <osg/LineWidth>
#include <osg/CullFace>
#include <osg/Depth>
#include <osg/BlendFunc>
#include <osg/LineWidth>
#include <osg/LineStipple>
#include <osg/Hint>
#include <osg/PolygonMode>
#include <osg/Material>
#include <osgUtil/IntersectVisitor>
#include <osgUtil/LineSegmentIntersector>
#include <osg/LineSegment>
#include <osgEarth/ElevationQuery>

#include <FreeXCommonUtils/MathCommonUtil.h>
#include <FreeXEffectObjectUtil/EffectObject.h>
#include <FreeXGeoUtil/CoordConverter.h>

#include "RadarPowerZone.h"
#include "RadarEffect.h"

#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/Nef_polyhedron_3.h>
#include <CGAL/Polyhedron_incremental_builder_3.h>
#include <CGAL/IO/Polyhedron_iostream.h>

#include <osg/PositionAttitudeTransform>

typedef CGAL::Exact_predicates_exact_constructions_kernel Kernel;
typedef CGAL::Polyhedron_3<Kernel>  Polyhedron;
typedef CGAL::Nef_polyhedron_3<Kernel> Nef_polyhedron;
typedef Polyhedron::HalfedgeDS HalfedgeDS;
typedef HalfedgeDS::Vertex   CGALVertex;
typedef CGALVertex::Point CGALPoint;
typedef Polyhedron::Point_iterator Point_iterator;
typedef Polyhedron::Facet_iterator Facet_iterator;
typedef Polyhedron::Halfedge_around_facet_circulator Halfedge_facet_circulator;

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
void CgalPolyhedron<HDS>::operator()(HDS& hds)
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

void mesh2polyhedron(VFMesh &mesh, Polyhedron &P)
{
	CgalPolyhedron<HalfedgeDS> builder(mesh);
	P.delegate(builder);
}

void polyhedron2mesh(VFMesh &mesh, Polyhedron &P)
{
	mesh.facelist.clear();
	mesh.pointlist.clear();
	//输出
	for (Point_iterator k = P.points_begin(); k != P.points_end(); ++k) {
		mesh.pointlist.push_back(*k);
		//std::cout << "v " << *k << std::endl;
		//输出所有的点信息
	}

	std::vector<int> face;
	for (Facet_iterator i = P.facets_begin(); i != P.facets_end(); ++i)
	{
		Halfedge_facet_circulator j = i->facet_begin();
		CGAL_assertion(CGAL::circulator_size(j) >= 3);

		//std::cout << "f ";
		face.clear();
		do
		{
			face.push_back(distance(P.vertices_begin(), j->vertex()));
			//std::cout << 1 + distance(P.vertices_begin(), j->vertex()) << " ";
		} while (++j != i->facet_begin());

		if (face.size() >= 3)
		{
			mesh.facelist.push_back(face);
		}
		//std::cout << std::endl;
	}
}

void node2Mash(osg::Geode* pSphere, VFMesh& mesh, int iHint)
{
	osg::Geometry* pSphere1Geom = static_cast<osg::Geometry*>(pSphere->getDrawable(0));
	osg::Vec3Array* pSphere1Arr = static_cast<osg::Vec3Array*>(pSphere1Geom->getVertexArray());
	std::vector<osg::DrawElements*> vecS1DrawEles;
	bool bTag = pSphere1Geom->getDrawElementsList(vecS1DrawEles);
	osg::DrawElementsUInt* pSphere1Ele = static_cast<osg::DrawElementsUInt*>(vecS1DrawEles[0]);
	//osg::Vec3 v3Sphere1Center = pSphere->getWorldMatrices()[0].getTrans();

	int iNumCircle = iHint * 2;
	osg::Vec3 v3Tem =  pSphere1Arr->at(0);//+v3Sphere1Center
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

osg::Geometry* createSphereGeom(double dRadius /*= 1.*/, int iHint /*= 45*/, const osg::Vec3& v3Dist)
{
	osg::ref_ptr<osg::Geometry>  rpGeom = new osg::Geometry;
	osg::ref_ptr<osg::Vec3Array> rpVertexes = new osg::Vec3Array;//顶点数组
	osg::ref_ptr<osg::Vec3Array> rpNormal = new osg::Vec3Array;//法线数组
	osg::ref_ptr<osg::Vec2Array> rpTexCoord = new osg::Vec2Array;//纹理数组

	for (int i = 0; i <= iHint; i++)//从上到下添加点，有等于号是为了之后贴纹理时可以和图片的点一一对应
	{
		for (int j = 0; j <= iHint * 2; j++)//逆时针添加点
		{
			osg::Vec3 vec3VertexT(
				sin(osg::PI*i / iHint)*cos(osg::PI*j / iHint),
				sin(osg::PI*i / iHint)*sin(osg::PI*j / iHint),
				cos(osg::PI*i / iHint));//球面坐标公式
			rpVertexes->push_back(vec3VertexT * dRadius + v3Dist);//添加顶点
			rpNormal->push_back(vec3VertexT);//添加法线
			rpTexCoord->push_back(osg::Vec2(double(j) / 2.0 / iHint
				, 1 - double(i) / iHint));//添加纹理坐标
		}
	}

	osg::ref_ptr<osg::Vec4Array> rpColors = new osg::Vec4Array;
	rpColors->push_back(osg::Vec4(1.0, 1.0, 1.0, 1.0));

	rpGeom->setVertexArray(rpVertexes);
	rpGeom->setNormalArray(rpNormal);
	rpGeom->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);
	rpGeom->setColorArray(rpColors);
	rpGeom->setColorBinding(osg::Geometry::BIND_OVERALL);
	rpGeom->setTexCoordArray(0, rpTexCoord);

	//添加图元
	osg::ref_ptr<osg::DrawElementsUInt> rpFace = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLE_STRIP);
	for (int i = 0; i < iHint; i++) {
		for (int j = 0; j <= iHint * 2; j++) {
			rpFace->push_back(i*(iHint * 2 + 1) + j);
			rpFace->push_back((i + 1)*(iHint * 2 + 1) + j);
		}
	}
	rpGeom->addPrimitiveSet(rpFace);

	return rpGeom.release();
}

osg::Geometry* mash2Geom(VFMesh& mesh, bool bIsLine)
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





CRadarPowerZone::CRadarPowerZone(osg::observer_ptr<FxEffectObject::CFxRadarEnvelopeEle> pEle, osgViewer::View* pView, FxEffectObject::CFxEffectBase* pRadar)
	:CRadarBase()
	,m_rpFace(nullptr)
	,m_rpSurface(nullptr)
	,m_rpSurfaceLine(nullptr)
	, m_rpSideFaceLine(nullptr)
	,m_rpEdgeLine(nullptr)
	,m_rpTopFace(nullptr)
	,m_rpBottomFace(nullptr)
	,m_rpLeftFace(nullptr)
	,m_rpRightFace(nullptr)
	,m_opRadarComEle(pEle)
	,m_opView(pView)
	,m_opRadar(pRadar)
{
	FxEffectObject::CRadarEnvelope* pRadarEffect = static_cast<FxEffectObject::CRadarEnvelope*>(m_opRadar.get());
	m_opFirstObj = pRadarEffect->getFirstObj();
	m_opSecondObj = pRadarEffect->getSecondObj();

	{
		osg::StateSet* pStateSet = this->getOrCreateStateSet();
		pStateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);
		pStateSet->setMode(GL_BLEND, osg::StateAttribute::ON | osg::StateAttribute::PROTECTED);
		pStateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
		//消除深度白色阴影部分
		osg::ref_ptr<osg::Depth> pDepth = new osg::Depth;
		pDepth->setWriteMask(false);
		pStateSet->setAttribute(pDepth.get(), osg::StateAttribute::ON | osg::StateAttribute::PROTECTED);
	}
	init();
}

CRadarPowerZone::~CRadarPowerZone()
{
}

void CRadarPowerZone::setPowerFaceColor(osg::Vec4d color)
{
	m_FaceColor = color;
}

void CRadarPowerZone::updateOutLineStyle()
{
	if (m_rpEdgeLine.valid())
	{
		setGeomLineStyle(m_opRadarComEle->outLineStyle(),m_rpEdgeLine);
	}

	if (m_rpSpokeOutlineGeome.valid())
	{
		setGeomLineStyle(m_opRadarComEle->outLineStyle(),m_rpSpokeOutlineGeome);
	}
}

void CRadarPowerZone::setOutLineVisible( bool bVisible )
{
	if (m_rpEdgeLine.valid())
	{
		m_rpEdgeLine->setNodeMask(bVisible);
	}

	if (m_rpSpokeOutlineGeome.valid())
	{
		m_rpSpokeOutlineGeome->setNodeMask(bVisible);
	}
}

void CRadarPowerZone::init()
{
	setCentre(m_opRadarComEle->centre());
	setRadius(m_opRadarComEle->radius());
	setArea(m_opRadarComEle->verticalAngleMax(),
		m_opRadarComEle->verticalAngleMin(),
		m_opRadarComEle->horizontalAngleMax(),
		m_opRadarComEle->horizontalAngleMin());
	setDensityHor(m_opRadarComEle->horSplit());
	setDensityVer(m_opRadarComEle->verSplit());
	
	if (m_opRadarComEle->envelopeFuse())
	{
		addEnvelopFuse();
	}
	else
	{
		calVertices();//计算顶点
		drawSurface();
		drawSurfaceLine();
		drawSideFace();
	}
	
	//edgeLine_drawImplementation();
	//spoke_drawImplementation();
	//setOutLineVisible(m_opRadarComEle->outLineVisible());
}

void CRadarPowerZone::calVertices()
{
	float elevIncr = (m_felevMax - m_felevMin) / m_nDensityHor;
	float azIncr = (m_fazMax - m_fazMin) / m_nDensityVer;
	for (int i = 0; i <= m_nDensityVer; i++)
	{
		float azimuth = osg::DegreesToRadians(m_fazMin + i*azIncr);

		for (int j = 0; j <= m_nDensityHor; j++)
		{
			float elev = osg::DegreesToRadians(m_felevMin + j*elevIncr);

			float x = m_fRadius*cos(azimuth)*cos(elev);
			float y = m_fRadius*cos(azimuth)*sin(elev);
			float z = m_fRadius*sin(azimuth);

			m_vVertexes.push_back(osg::Vec3(x, y, z) + m_vecCenter);
		}
	}
	m_vVertexes.push_back(m_vecCenter);
	if (m_opRadarComEle->terrainMasking())
	{
		//osg::Vec3 v3RadarPos = m_opRadar->getWorldLocationMatrix().getTrans();
		osg::Vec3 v3RadarPos = m_opRadar->getWorldLocationMatrix().getTrans();
		osg::Matrix mtxRot = m_opRadar->getWorldRoateMatrix();
		osg::Matrix mtxRotInv;
		mtxRotInv.invert(mtxRot);
		FxEffectObject::CRadarEnvelope* pRadarEnve = static_cast<FxEffectObject::CRadarEnvelope*>(m_opRadar.get());
		//osgEarth::ElevationQuery eq;
		//osg::Vec3 v3RadarPos = m_opRadar->getWorldMatrices()[0].getTrans();
		for (int i = 0; i < m_vVertexes.size()-1; i++)
		{
			osg::ref_ptr<osgUtil::LineSegmentIntersector> intersector = new osgUtil::LineSegmentIntersector(
				v3RadarPos + m_vecCenter, v3RadarPos + m_vVertexes[i] * mtxRot);
			osgUtil::IntersectionVisitor iv(intersector);
			pRadarEnve->getMapNode()->accept(iv);
			//m_opView->getSceneData()->accept(iv);
			osgUtil::LineSegmentIntersector::Intersections ints = intersector->getIntersections();
			//bool bInster = intersector->containsIntersections();

			if (!ints.empty())
			{
				auto itr = ints.begin();
				m_vVertexes[i] = (itr->getWorldIntersectPoint() - v3RadarPos) * mtxRotInv;
			}
		}
	}
	
}


void CRadarPowerZone::addEnvelopFuse()
{
	osg::Vec3d v3FirstPos = m_opFirstObj->getWorldLocationMatrix().getTrans();
	osg::Vec3d v3SecondPos = m_opSecondObj->getWorldLocationMatrix().getTrans();
	osg::Vec3d v3Dist = v3SecondPos - v3FirstPos;

	int iHint = m_opRadarComEle->verSplit();
	osg::ref_ptr<osg::Geode> rpGeodeL = new osg::Geode;
	rpGeodeL->addDrawable(createSphereGeom(m_opRadarComEle->radius(), iHint, osg::Vec3d()));
	osg::ref_ptr<osg::Geode> rpGeodeR = new osg::Geode;
	rpGeodeR->addDrawable(createSphereGeom(m_opRadarComEle->radius(), iHint, v3Dist));

	VFMesh mesh1, mesh2;
	node2Mash(rpGeodeL, mesh1, iHint);
	node2Mash(rpGeodeR, mesh2, iHint);

	Polyhedron polyhedron1, polyhedron2;
	mesh2polyhedron(mesh1, polyhedron1);
	mesh2polyhedron(mesh2, polyhedron2);

	Nef_polyhedron nef1(polyhedron1);
	polyhedron1.clear();
	Nef_polyhedron nef2(polyhedron2);
	polyhedron2.clear();

	Nef_polyhedron out;
	out = (nef1 + nef2);

	Polyhedron result;
	out.convert_to_polyhedron(result);
	VFMesh resultmesh;
	polyhedron2mesh(resultmesh, result);

	
	osg::Geometry* pResultLineGeom = mash2Geom(resultmesh, true);
	//pResultLineGeom->setNodeMask(0);
	osg::ref_ptr<osg::Vec4Array> rpLineColors = new osg::Vec4Array;
	rpLineColors->push_back(m_opRadarComEle->surfaceLineColor());
	pResultLineGeom->setColorArray(rpLineColors.get());
	pResultLineGeom->setColorBinding(osg::Geometry::BIND_OVERALL);

	osg::Geometry* pResultFaceGeom = mash2Geom(resultmesh, false);
	//pResultLineGeom->setNodeMask(0);
	osg::ref_ptr<osg::Vec4Array> rpFaceColors = new osg::Vec4Array;
	rpFaceColors->push_back(m_opRadarComEle->surfaceColor());
	pResultFaceGeom->setColorArray(rpFaceColors.get());
	pResultFaceGeom->setColorBinding(osg::Geometry::BIND_OVERALL);

	osg::Geode* pResultLineGeode = new osg::Geode;
	pResultLineGeode->addDrawable(pResultLineGeom);
	osg::Geode* pResultFaceGeode = new osg::Geode;
	pResultFaceGeode->addDrawable(pResultFaceGeom);

	osg::MatrixTransform* pMT = new osg::MatrixTransform;
	osg::Matrix matRotIvt;
	matRotIvt.invert(m_opFirstObj->getWorldRoateMatrix());
	osg::Vec3d v3Tem = v3Dist * matRotIvt;
	osg::Quat quatTem;
	quatTem.makeRotate(v3Dist, v3Tem);
	pMT->setMatrix(osg::Matrix(quatTem));

	pMT->addChild(pResultLineGeode);
	pMT->addChild(pResultFaceGeode);
	this->addChild(pMT);
}

void CRadarPowerZone::drawSurface()
{
	if(!m_rpSurface.valid())
	{
		m_rpSurface= new osg::Geometry;
		addDrawable(m_rpSurface.get());
	}
	osg::ref_ptr<osg::Vec3Array> rpVertices = new osg::Vec3Array;
	rpVertices->asVector() = m_vVertexes;
	m_rpSurface->setVertexArray(rpVertices.get());

	osg::ref_ptr<osg::Vec4Array> rpColors = new osg::Vec4Array;
	rpColors->push_back(m_opRadarComEle->surfaceColor());
	m_rpSurface->setColorArray(rpColors.get());
	m_rpSurface->setColorBinding(osg::Geometry::BIND_OVERALL);

	for(int i=0; i<m_nDensityVer; i++)
	{
		osg::ref_ptr<osg::DrawElementsUInt> rpElements = new osg::DrawElementsUInt(GL_TRIANGLE_STRIP);

		for(int k=0; k<= m_nDensityHor; k++)
		{
			rpElements->addElement((m_nDensityHor +1)*i    + k);
			rpElements->addElement((m_nDensityHor +1)*(i+1)+ k);
		}
		m_rpSurface->addPrimitiveSet(rpElements);
	}//m_nDensityVer个PrimitiveSet

	m_rpSurface->getOrCreateStateSet()->setRenderBinDetails(110,"DepthSortedBin");
}

void CRadarPowerZone::drawSurfaceLine()
{
	if (!m_rpSurfaceLine.valid())
	{
		m_rpSurfaceLine = new osg::Geometry;
		addDrawable(m_rpSurfaceLine.get());
	}
	osg::ref_ptr<osg::Vec3Array> rpVecArr = new osg::Vec3Array;
	rpVecArr->asVector() = m_vVertexes;
	m_rpSurfaceLine->setVertexArray(rpVecArr.get());

	osg::ref_ptr<osg::Vec4Array> rpLineColors = new osg::Vec4Array;
	rpLineColors->push_back(m_opRadarComEle->surfaceLineColor());
	m_rpSurfaceLine->setColorArray(rpLineColors.get());
	m_rpSurfaceLine->setColorBinding(osg::Geometry::BIND_OVERALL);

	//绘制竖直线
	for (int i = 0; i < m_nDensityHor; i++)
	{
		osg::ref_ptr<osg::DrawElementsUInt> rpLine = new osg::DrawElementsUInt(GL_LINE_STRIP);

		/*if (m_fazMin > -90 && abs(m_felevMax - m_felevMin) < 360)
		{
			rpLine->addElement(rpVecArr->size() - 1);
		}*/

		for (int j = 0; j <= m_nDensityVer; j++)
		{
			rpLine->addElement(j*(m_nDensityHor + 1) + i);
		}
		
		/*if (m_fazMax < 90 && abs(m_felevMax - m_felevMin) < 360)
		{
			rpLine->addElement(rpVecArr->size() - 1);
		}*/

		m_rpSurfaceLine->addPrimitiveSet(rpLine);
	}
	if (!FxCommonUtils::Equal(m_felevMax - m_felevMin, 360.f, 0.01f))
	{//小于360度就加上一条线
		osg::ref_ptr<osg::DrawElementsUInt> rpLine = new osg::DrawElementsUInt(GL_LINE_STRIP);
		for (int j = 0; j <= m_nDensityVer; j++)
		{
			rpLine->addElement(j*(m_nDensityHor + 1) + m_nDensityHor);
		}
		m_rpSurfaceLine->addPrimitiveSet(rpLine);
	}

	//绘制水平线
	for (int i = 0; i <= m_nDensityVer; i++)
	{
		osg::ref_ptr<osg::DrawElementsUInt> rpElements = new osg::DrawElementsUInt(GL_LINE_STRIP);
		for (int j = 0; j <= m_nDensityHor; j++)
		{
			rpElements->addElement(i*(m_nDensityHor + 1) + j);
		}
		m_rpSurfaceLine->addPrimitiveSet(rpElements);
	}
}

void CRadarPowerZone::drawSideFace()
{
	float fAzimuthRange = osg::maximum<float>(m_fazMin, m_fazMax) - osg::minimum<float>(m_fazMin, m_fazMax);
	float fLevelRange = osg::maximum<float>(m_felevMin, m_felevMax) - osg::minimum<float>(m_felevMin, m_felevMax);
	bool bAzimuth = FxCommonUtils::Equal(fAzimuthRange, 180.f, FLT_EPSILON);
	bool bLevel   = FxCommonUtils::Equal(fLevelRange, 360.f, FLT_EPSILON);
	if (bAzimuth && bLevel)
		return;
	if (!bAzimuth)
	{
		if (! FxCommonUtils::Equal(m_fazMax, 90.f, FLT_EPSILON))
		{
			m_rpTopFace = SideGeometry(Top);
			addDrawable(m_rpTopFace);
		}
		if (! FxCommonUtils::Equal(m_fazMin, -90.f, FLT_EPSILON))
		{
			m_rpBottomFace = SideGeometry(Bottom);
			addDrawable(m_rpBottomFace);
		}
	}
	if (!bLevel)
	{
		m_rpLeftFace = SideGeometry(Left);
		m_rpRightFace = SideGeometry(Right);

		addDrawable(m_rpLeftFace);
		addDrawable(m_rpRightFace);
	}
}

osg::Geometry* CRadarPowerZone::SideGeometry(ESide side)
{
	osg::ref_ptr<osg::Geometry> rpGeome = new osg::Geometry;
	addDrawable(rpGeome.get());
	float fLevelRange = osg::maximum<float>(m_felevMin, m_felevMax) - osg::minimum<float>(m_felevMin, m_felevMax);
	bool bLevel = FxCommonUtils::Equal(fLevelRange, 360.f, FLT_EPSILON);
	osg::ref_ptr<osg::Vec3Array> rpVertices = new osg::Vec3Array;
	rpVertices->push_back(m_vecCenter);
	if (side == Left)
	{
		for (int i = m_nDensityVer; i >= 0; i--)
		{
			rpVertices->push_back(m_vVertexes.at((m_nDensityHor + 1)*i));
		}
	}
	else if (side == Top)
	{
		for (int j = 0; j <= m_nDensityHor; j++)
		{
			rpVertices->push_back(m_vVertexes.at(m_vVertexes.size() - 2 - j));
		}
	}
	else if (side == Right)
	{
		for (int k = 0; k <= m_nDensityVer; k++)
		{
			rpVertices->push_back(m_vVertexes.at((m_nDensityHor + 1)*(k + 1) - 1));
		}
	}
	else if (side == Bottom)
	{
		for (int n = 0; n <= m_nDensityHor; n++)
		{
			rpVertices->push_back(m_vVertexes.at(n));
		}
	}

	rpGeome->setVertexArray(rpVertices.get());
	rpGeome->addPrimitiveSet(new osg::DrawArrays(GL_TRIANGLE_FAN, 0, rpVertices->size()));

	osg::ref_ptr<osg::DrawElementsUInt> rpLine = new osg::DrawElementsUInt(GL_LINES);
	for (int i = 1; i < rpVertices->size(); i++)
	{
		rpLine->addElement(0);
		rpLine->addElement(i);
	}
	if (bLevel && (side == Top || side == Bottom))
	{
		rpLine->pop_back();
		rpLine->pop_back();
	}
	rpGeome->addPrimitiveSet(rpLine);

	osg::ref_ptr<osg::Vec4Array> rpColors = new osg::Vec4Array;
	rpColors->push_back(m_opRadarComEle->sideFaceColor());
	rpColors->push_back(m_opRadarComEle->sideFaceLineColor());
	rpGeome->setColorArray(rpColors.get());
	rpGeome->setColorBinding(osg::Geometry::BIND_PER_PRIMITIVE_SET);

	return rpGeome.release();
}

void CRadarPowerZone::setPowerFaceVisiable(bool flag)
{
	if(m_rpFace)
	{
		m_rpFace->setNodeMask(1);
	}
	else
	{
		m_rpFace->setNodeMask(0);
	}
}

void CRadarPowerZone::updateRadarColor()
{
	//更新侧面颜色
	changeGeometryColor(m_rpFuseFace, m_opRadarComEle->surfaceColor());//包络面颜色
	changeGeometryColor(m_rpSurface,m_opRadarComEle->surfaceColor());//球面颜色

	changeGeometryColor(m_rpTopFace,m_opRadarComEle->sideFaceColor());//侧面颜色
	changeGeometryColor(m_rpBottomFace,m_opRadarComEle->sideFaceColor());//侧面颜色
	changeGeometryColor(m_rpLeftFace, m_opRadarComEle->sideFaceColor());//侧面颜色
	changeGeometryColor(m_rpRightFace, m_opRadarComEle->sideFaceColor());//侧面颜色

	//更新球面线条颜色
	changeGeometryColor(m_rpSurfaceLine, m_opRadarComEle->surfaceLineColor());//球面线颜色
	changeGeometryColor(m_rpFuseLine, m_opRadarComEle->surfaceLineColor());//包络面线颜色



	//changeGeometryColor(m_rpFace,m_FaceColor);
	//changeGeometryColor(m_rpEdgeLine,m_opRadarComEle->sideFaceLineColor());
}


void CRadarPowerZone::edgeLine_drawImplementation()
{
	if (!m_rpEdgeLine.valid())
	{
		m_rpEdgeLine = new osg::Geometry;
	}

	// 球面边缘线
	std::vector<osg::Vec3> varray;
	float elevIncr = (m_felevMax - m_felevMin) / m_nDensityVer;
	float azIncr = (m_fazMax - m_fazMin) / m_nDensityHor;
	for (int i = 0; i <= m_nDensityHor; i += m_nDensityHor)
	{
		float az1 = osg::DegreesToRadians(m_fazMin + i*azIncr);

		for (int j = 0; j <= m_nDensityVer; j++)
		{
			float elev = osg::DegreesToRadians(m_felevMin + j*elevIncr);

			float x = m_fRadius*cos(elev)*cos(az1);
			float y = m_fRadius*cos(elev)*sin(az1);
			float z = m_fRadius*sin(elev);

			varray.push_back(osg::Vec3(x, y, z) + m_vecCenter);
		}
	}

	for (int i = 0; i <= m_nDensityVer; i += m_nDensityVer)
	{
		float elev = osg::DegreesToRadians(m_felevMin + i*elevIncr);
		for (int j = 0; j <= m_nDensityHor; j++)
		{
			float az1 = osg::DegreesToRadians(m_fazMin + j*azIncr);
			float x = m_fRadius*cos(elev)*cos(az1);
			float y = m_fRadius*cos(elev)*sin(az1);
			float z = m_fRadius*sin(elev);

			varray.push_back(osg::Vec3(x, y, z) + m_vecCenter);
		}
	}

	osg::ref_ptr<osg::Vec3Array> rpVertices = new osg::Vec3Array;

	int nIndex = 0;
	// 左右边缘
	if (abs(m_fazMax - m_fazMin) < 360)
	{
		for (int i = 0; i < 2; ++i)
		{
			//
			for (int j = 0; j < m_nDensityVer; j++)
			{
				rpVertices->push_back(varray[nIndex + j + i*(m_nDensityVer + 1)]);
				rpVertices->push_back(varray[nIndex + j + i*(m_nDensityVer + 1) + 1]);
			}
		}
	}

	nIndex += 2 * (m_nDensityVer + 1);

	// 上下边缘
	for (int i = 0; i < 2; ++i)
	{
		//上边缘
		for (int j = 0; j < m_nDensityHor; j++)
		{
			rpVertices->push_back(varray[nIndex + j + i*(m_nDensityHor + 1)]);
			rpVertices->push_back(varray[nIndex + j + i*(m_nDensityHor + 1) + 1]);
		}
	}

	osg::ref_ptr<osg::Vec4Array> rpColors = new osg::Vec4Array;
	for (int j = 0; j < rpVertices->size(); j++)
	{
		rpColors->push_back(osg::Vec4d(1, 1, 0, 1));
	}

	m_rpEdgeLine->setVertexArray(rpVertices.get());
	m_rpEdgeLine->setColorArray(rpColors.get());
	m_rpEdgeLine->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

	m_rpEdgeLine->addPrimitiveSet(new osg::DrawArrays(GL_LINES, 0, rpVertices->size()));

	setGeomLineStyle(m_opRadarComEle->outLineStyle(), m_rpEdgeLine);

	addDrawable(m_rpEdgeLine.get());
	//
}

void CRadarPowerZone::spoke_drawImplementation()
{
	if (!(abs(m_fazMax - m_fazMin) < 360))
	{
		return;
	}

	m_rpSpokeOutlineGeome = new osg::Geometry;
	addDrawable(m_rpSpokeOutlineGeome.get());

	std::vector<osg::Vec3> varray;
	// 四个点即可
	float elevIncr = (m_felevMax - m_felevMin) / m_nDensityVer;
	float azIncr = (m_fazMax - m_fazMin) / m_nDensityHor;
	for (int i = 0; i <= m_nDensityHor; i += m_nDensityHor)
	{
		float az1 = osg::DegreesToRadians(m_fazMin + i*azIncr);

		for (int j = 0; j <= m_nDensityVer; j += m_nDensityVer)
		{
			float elev = osg::DegreesToRadians(m_felevMin + j*elevIncr);

			float x = m_fRadius*cos(elev)*cos(az1);
			float y = m_fRadius*cos(elev)*sin(az1);
			float z = m_fRadius*sin(elev);

			varray.push_back(osg::Vec3(x, y, z) + m_vecCenter);
		}
	}

	// 锥面边缘线
	osg::ref_ptr<osg::Vec3Array> rpVertices = new osg::Vec3Array;
	//左下
	rpVertices->push_back(m_vecCenter);
	rpVertices->push_back(varray[0]);
	//左上
	rpVertices->push_back(m_vecCenter);
	rpVertices->push_back(varray[1]);
	//右上
	rpVertices->push_back(m_vecCenter);
	rpVertices->push_back(varray[2]);
	//右下
	rpVertices->push_back(m_vecCenter);
	rpVertices->push_back(varray[3]);

	osg::ref_ptr<osg::Vec4Array> rpColors = new osg::Vec4Array;
	for (int j = 0; j < rpVertices->size(); j++)
	{
		rpColors->push_back(m_opRadarComEle->outLineStyle().color);
	}

	m_rpSpokeOutlineGeome->setVertexArray(rpVertices.get());
	m_rpSpokeOutlineGeome->setColorArray(rpColors.get());
	m_rpSpokeOutlineGeome->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

	setGeomLineStyle(m_opRadarComEle->outLineStyle(), m_rpSpokeOutlineGeome);

	m_rpSpokeOutlineGeome->addPrimitiveSet(new osg::DrawArrays(GL_LINES, 0, rpVertices->size()));
}

void CRadarPowerZone::setGeomLineStyle(SLineStyle &lineStyle, osg::Geometry *pGemotry)
{
	if (pGemotry == NULL)
	{
		return;
	}
	// 设置点划线
	if (lineStyle.nStipplePattern != 0xFFFF)
	{
		osg::ref_ptr<osg::LineStipple> pStipple = new osg::LineStipple(lineStyle.nStippleFactor, lineStyle.nStipplePattern);
		pGemotry->getOrCreateStateSet()->setAttributeAndModes(pStipple.release(), osg::StateAttribute::ON);
		pGemotry->getOrCreateStateSet()->setMode(GL_LINE_STIPPLE, osg::StateAttribute::ON | osg::StateAttribute::PROTECTED);
	}
	else
	{
		pGemotry->getOrCreateStateSet()->setMode(GL_LINE_STIPPLE, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);
	}

	// 开启/关闭光滑
	pGemotry->getOrCreateStateSet()->setMode(GL_LINE_SMOOTH, osg::StateAttribute::ON | osg::StateAttribute::PROTECTED);
	pGemotry->getOrCreateStateSet()->setAttributeAndModes(new osg::Hint(GL_LINE_SMOOTH_HINT, GL_NICEST), osg::StateAttribute::ON | osg::StateAttribute::PROTECTED);

	pGemotry->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
	// 开启混合
	osg::ref_ptr<osg::BlendFunc> pBlend = new osg::BlendFunc(osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::ONE_MINUS_SRC_ALPHA);
	pGemotry->getOrCreateStateSet()->setAttributeAndModes(pBlend.release(), osg::StateAttribute::ON | osg::StateAttribute::PROTECTED);
	pGemotry->getOrCreateStateSet()->setAttributeAndModes(new osg::LineWidth(lineStyle.fWidth), osg::StateAttribute::ON);

	// 设置颜色
	changeGeometryColor(pGemotry, m_opRadarComEle->outLineStyle().color);
}

