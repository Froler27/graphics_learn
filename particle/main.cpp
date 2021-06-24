#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

#include <osg/Node>
#include <osg/Geode>
#include <osg/Group>
#include <osg/PositionAttitudeTransform>

#include <osgDB/ReadFile>
#include <osgDB/WriteFile>

#include <osgUtil/Optimizer>

#include <osgGA/StateSetManipulator>

#include "ExplosionGroup.h"
#include "ParticleGroup1.h"

int main(int argc, char** argv)
{
	osg::ref_ptr<osgViewer::Viewer> rpViewer = new osgViewer::Viewer();
	osg::ref_ptr<osg::Group> rpRoot = new osg::Group();

	rpRoot->addChild(new CExplosionGroup());
	//rpRoot->addChild(new CParticleGroup1());
	//rpRoot->addChild(new CParticleGroup1(1));

	/*osg::Group* pGroup = new osg::Group;
	CParticleGroup1* pFire = new CParticleGroup1(2);
	pGroup->addChild(pFire);
	pGroup->addChild(osgDB::readNodeFile("cessna.osg"));

	osg::PositionAttitudeTransform* pTrans = new osg::PositionAttitudeTransform;
	pTrans->setAttitude(osg::Quat(osg::PI_2, osg::Z_AXIS));
	pTrans->addChild(pGroup);
	rpRoot->addChild(pTrans);

	CMoveFly* pMoveFly = new CMoveFly;
	pTrans->setUpdateCallback(pMoveFly);
	rpViewer->addEventHandler(new CFirePosCallback(pTrans, pFire));*/
	

	osgUtil::Optimizer optimizer;
	optimizer.optimize(rpRoot);//�Ż���������

	rpViewer->addEventHandler(new osgGA::StateSetManipulator(rpViewer->getCamera()->getOrCreateStateSet()));//���w�¼�����(��Ҫ��Alt��һ��)
	rpViewer->addEventHandler(new osgViewer::StatsHandler);//���s�¼�����(��Ҫ��Alt��һ��)

	rpViewer->setSceneData(rpRoot);
	rpViewer->realize();
	rpViewer->run();
	return 0;
}