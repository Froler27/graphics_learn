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
@brief : 创建一个球的几何节点
@param : [in] dRadius 球的半径
@param : [in] iHint 每180/iHint度设置一个点，默认18
@return: 一个球的osg::Geometry*
*/
osg::Group* createExplode()
{
	osg::ref_ptr<osg::Group> rpExplode = new osg::Group;
	osg::Vec3 v3Wind(1.f, 0.f, 0.f);//风向
	osg::Vec3 v3Pos(0.f, 0.f, -10.f);//位置
	
	osg::ref_ptr<osgParticle::ExplosionEffect> rpExplosion = new osgParticle::ExplosionEffect(v3Pos, 10.f);//爆炸模拟，10.f为缩放比，默认1.f，不缩放
	osg::ref_ptr<osgParticle::ExplosionDebrisEffect> rpExplosionDebri = new osgParticle::ExplosionDebrisEffect(v3Pos, 10.f);//碎片模拟
	osg::ref_ptr<osgParticle::SmokeEffect> rpSmoke = new osgParticle::SmokeEffect(v3Pos, 10.f);//烟模拟
	osg::ref_ptr<osgParticle::FireEffect> rpFire = new osgParticle::FireEffect(v3Pos, 10.f);//火焰模拟

	rpExplosion->setWind(v3Wind);//设置风向
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

	rpViewer->addEventHandler(new osgGA::StateSetManipulator(rpViewer->getCamera()->getOrCreateStateSet()));//添加w事件处理(需要和Alt键一起按)
	rpViewer->addEventHandler(new osgViewer::StatsHandler);//添加s事件处理(需要和Alt键一起按)

	rpViewer->setSceneData(rpRoot);
	rpViewer->realize();
	rpViewer->run();
	return 0;
}