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

	this->addChild(rpExplosion);
	this->addChild(rpExplosionDebri);
	this->addChild(rpSmoke);
	this->addChild(rpFire);
}
