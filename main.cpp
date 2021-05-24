#include <math.h>

#include <osg/Geode>
#include <osg/Group>
#include <osg/Vec3>
#include <osg/Geometry>
#include <osg/ref_ptr>
#include <osg/Vec2>
#include <osg/Texture2D>

#include <osgDB/ReadFile>

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

#include <osgGA/StateSetManipulator>

/**
@brief : 创建一个球的几何节点
@param : [in] dRadius 球的半径
@param : [in] iHint 每180/iHint度设置一个点，默认18
@return: 一个球的osg::Geometry*
*/
osg::Geometry* createSphereGeom(double dRadius = 1., int iHint = 18)
{
	osg::ref_ptr<osg::Geometry>  rpGeom     = new osg::Geometry;
	osg::ref_ptr<osg::Vec3Array> rpVertexes = new osg::Vec3Array;//顶点数组
	osg::ref_ptr<osg::Vec3Array> rpNormal   = new osg::Vec3Array;//法线数组
	osg::ref_ptr<osg::Vec2Array> rpTexCoord = new osg::Vec2Array;//纹理数组

	for (int i = 0; i <= iHint; i++)//从上到下添加点，有等于号是为了之后贴纹理时可以和图片的点一一对应
	{
		for (int j = 0; j <= iHint * 2; j++)//逆时针添加点
		{
			osg::Vec3 vec3VertexT(
				sin(osg::PI*i / iHint)*cos(osg::PI*j / iHint),
				sin(osg::PI*i / iHint)*sin(osg::PI*j / iHint),
				cos(osg::PI*i / iHint));//球面坐标公式
			rpVertexes->push_back(vec3VertexT * dRadius);//添加顶点
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

/**
@brief : 给球贴纹理
@param : [in/out] pNode 球的节点
@return: 
*/
void setTex(osg::Node* pNode)
{
	osg::ref_ptr<osg::Image> rpImage = osgDB::readImageFile("../star/moon.jpg");
	osg::ref_ptr<osg::Texture2D> rpTexture = new osg::Texture2D();

	rpTexture->setImage(rpImage);
	rpTexture->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);//设置S方向的环绕模式
	rpTexture->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);//设置R方向的环绕模式
	//rpTextureT->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_NEAREST);
	//rpTextureT->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);

	osg::ref_ptr<osg::StateSet> pState = pNode->getOrCreateStateSet();
	pState->setTextureAttributeAndModes(0, rpTexture, osg::StateAttribute::ON);
}

int main(int argc, char** argv)
{
	osg::ref_ptr<osgViewer::Viewer> rpViewer = new osgViewer::Viewer();
	osg::ref_ptr<osg::Group> rpRoot = new osg::Group();

	osg::ref_ptr<osg::Geode> rpGeode = new osg::Geode;
	rpGeode->addChild(createSphereGeom(1., 45));
	//setTex(rpGeode);//在球上贴纹理

	rpRoot->addChild(rpGeode);
	rpViewer->setSceneData(rpRoot);

	rpViewer->addEventHandler(new osgGA::StateSetManipulator(rpViewer->getCamera()->getOrCreateStateSet()));//添加w事件处理
	rpViewer->addEventHandler(new osgViewer::StatsHandler);//添加s事件处理

	rpViewer->run();
	return 0;
}