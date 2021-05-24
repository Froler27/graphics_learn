#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

#include <osg/Node>
#include <osg/Geode>
#include <osg/Group>

#include <osgDB/ReadFile>
#include <osgDB/WriteFile>

#include <osgParticle/ExplosionDebrisEffect>
#include <osgParticle/ExplosionEffect>
#include <osgParticle/SmokeEffect>
#include <osgParticle/FireEffect>

#include <osgUtil/Optimizer>

#include <osgGA/StateSetManipulator>

/**
@brief : ����һ����ļ��νڵ�
@param : [in] dRadius ��İ뾶
@param : [in] iHint ÿ180/iHint������һ���㣬Ĭ��18
@return: һ�����osg::Geometry*
*/
osg::Group* createExplode()
{
	osg::ref_ptr<osg::Group> rpExplode = new osg::Group;
	osg::Vec3 v3Wind(1.f, 0.f, 0.f);//����
	osg::Vec3 v3Pos(0.f, 0.f, -10.f);//λ��
	
	osg::ref_ptr<osgParticle::ExplosionEffect> rpExplosion = new osgParticle::ExplosionEffect(v3Pos, 10.f);//��ըģ�⣬10.fΪ���űȣ�Ĭ��1.f��������
	osg::ref_ptr<osgParticle::ExplosionDebrisEffect> rpExplosionDebri = new osgParticle::ExplosionDebrisEffect(v3Pos, 10.f);//��Ƭģ��
	osg::ref_ptr<osgParticle::SmokeEffect> rpSmoke = new osgParticle::SmokeEffect(v3Pos, 10.f);//��ģ��
	osg::ref_ptr<osgParticle::FireEffect> rpFire = new osgParticle::FireEffect(v3Pos, 10.f);//����ģ��

	rpExplosion->setWind(v3Wind);//���÷���
	rpExplosionDebri->setWind(v3Wind);
	rpSmoke->setWind(v3Wind);
	rpFire->setWind(v3Wind);

	rpExplode->addChild(rpExplosion);
	rpExplode->addChild(rpExplosionDebri);
	rpExplode->addChild(rpSmoke);
	rpExplode->addChild(rpFire);

	return rpExplode.release();
}

int main(int argc, char** argv)
{
	osg::ref_ptr<osgViewer::Viewer> rpViewer = new osgViewer::Viewer();
	osg::ref_ptr<osg::Group> rpRoot = new osg::Group();

	rpRoot->addChild(createExplode());

	osgUtil::Optimizer optimizer;
	optimizer.optimize(rpRoot);

	rpViewer->addEventHandler(new osgGA::StateSetManipulator(rpViewer->getCamera()->getOrCreateStateSet()));//���w�¼�����(��Ҫ��Alt��һ��)
	rpViewer->addEventHandler(new osgViewer::StatsHandler);//���s�¼�����(��Ҫ��Alt��һ��)

	rpViewer->setSceneData(rpRoot);
	rpViewer->realize();
	rpViewer->run();
	return 0;
}