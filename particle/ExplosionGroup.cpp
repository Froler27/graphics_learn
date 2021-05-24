#include <osgParticle/ExplosionDebrisEffect>
#include <osgParticle/ExplosionEffect>
#include <osgParticle/SmokeEffect>
#include <osgParticle/FireEffect>

#include "ExplosionGroup.h"


CExplosionGroup::CExplosionGroup()
{
	init();
}


CExplosionGroup::~CExplosionGroup()
{
}

void CExplosionGroup::init()
{
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

	this->addChild(rpExplosion);
	this->addChild(rpExplosionDebri);
	this->addChild(rpSmoke);
	this->addChild(rpFire);
}
