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
@brief : ����һ����ļ��νڵ�
@param : [in] dRadius ��İ뾶
@param : [in] iHint ÿ180/iHint������һ���㣬Ĭ��18
@return: һ�����osg::Geometry*
*/
osg::Geometry* createSphereGeom(double dRadius = 1., int iHint = 18)
{
	osg::ref_ptr<osg::Geometry>  rpGeom     = new osg::Geometry;
	osg::ref_ptr<osg::Vec3Array> rpVertexes = new osg::Vec3Array;//��������
	osg::ref_ptr<osg::Vec3Array> rpNormal   = new osg::Vec3Array;//��������
	osg::ref_ptr<osg::Vec2Array> rpTexCoord = new osg::Vec2Array;//��������

	for (int i = 0; i <= iHint; i++)//���ϵ�����ӵ㣬�е��ں���Ϊ��֮��������ʱ���Ժ�ͼƬ�ĵ�һһ��Ӧ
	{
		for (int j = 0; j <= iHint * 2; j++)//��ʱ����ӵ�
		{
			osg::Vec3 vec3VertexT(
				sin(osg::PI*i / iHint)*cos(osg::PI*j / iHint),
				sin(osg::PI*i / iHint)*sin(osg::PI*j / iHint),
				cos(osg::PI*i / iHint));//�������깫ʽ
			rpVertexes->push_back(vec3VertexT * dRadius);//��Ӷ���
			rpNormal->push_back(vec3VertexT);//��ӷ���
			rpTexCoord->push_back(osg::Vec2(double(j) / 2.0 / iHint
				, 1 - double(i) / iHint));//�����������
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

	//���ͼԪ
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
@brief : ����������
@param : [in/out] pNode ��Ľڵ�
@return: 
*/
void setTex(osg::Node* pNode)
{
	osg::ref_ptr<osg::Image> rpImage = osgDB::readImageFile("../star/moon.jpg");
	osg::ref_ptr<osg::Texture2D> rpTexture = new osg::Texture2D();

	rpTexture->setImage(rpImage);
	rpTexture->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);//����S����Ļ���ģʽ
	rpTexture->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);//����R����Ļ���ģʽ
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
	//setTex(rpGeode);//������������

	rpRoot->addChild(rpGeode);
	rpViewer->setSceneData(rpRoot);

	rpViewer->addEventHandler(new osgGA::StateSetManipulator(rpViewer->getCamera()->getOrCreateStateSet()));//���w�¼�����
	rpViewer->addEventHandler(new osgViewer::StatsHandler);//���s�¼�����

	rpViewer->run();
	return 0;
}