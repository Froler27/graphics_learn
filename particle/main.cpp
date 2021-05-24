#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

#include <osg/Node>
#include <osg/Geode>
#include <osg/Group>

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

	//rpRoot->addChild(new CExplosionGroup());
	rpRoot->addChild(new CParticleGroup1());

	osgUtil::Optimizer optimizer;
	optimizer.optimize(rpRoot);//优化场景数据

	rpViewer->addEventHandler(new osgGA::StateSetManipulator(rpViewer->getCamera()->getOrCreateStateSet()));//添加w事件处理(需要和Alt键一起按)
	rpViewer->addEventHandler(new osgViewer::StatsHandler);//添加s事件处理(需要和Alt键一起按)

	rpViewer->setSceneData(rpRoot);
	rpViewer->realize();
	rpViewer->run();
	return 0;
}