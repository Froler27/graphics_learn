#include <osgParticle/Particle>
#include <osgParticle/ParticleSystem>
#include <osgParticle/ParticleSystemUpdater>
#include <osgParticle/ModularEmitter>
#include <osgParticle/ModularProgram>
#include <osgParticle/RandomRateCounter>
#include <osgParticle/SectorPlacer>
#include <osgParticle/RadialShooter>
#include <osgParticle/AccelOperator>
#include <osgParticle/FluidFrictionOperator>

#include "ParticleGroup1.h"


CParticleGroup1::CParticleGroup1()
{
	init();
}


CParticleGroup1::~CParticleGroup1()
{
}

void CParticleGroup1::init()
{
	osgParticle::Particle particleTemplate;//创建粒子系统模板
	particleTemplate.setLifeTime(2);//设置生命周期
	particleTemplate.setSizeRange(osgParticle::rangef(0.75f, 3.f));//设置粒子大小变化范围
	particleTemplate.setAlphaRange(osgParticle::rangef(0.f, 1.f));//设置粒子Alpha变化范围
	particleTemplate.setColorRange(osgParticle::rangev4(
		osg::Vec4(1.f, 0.5f, 0.3f, 1.f)
		, osg::Vec4(0.f, 0.7f, 1.f, 0.f)
	));//设置粒子颜色变化范围
	particleTemplate.setRadius(0.05f);//设置半径
	particleTemplate.setMass(0.05f);//设置重量

	osg::ref_ptr<osgParticle::ParticleSystem> rpPS = new osgParticle::ParticleSystem;//创建粒子系统
	rpPS->setDefaultAttributes("Images/smoke.rgb", false, false);//设置材质，是否放射粒子，是否添加光照
	rpPS->setDefaultParticleTemplate(particleTemplate);//加入模板

	osg::ref_ptr<osgParticle::ModularEmitter> rpEmitter = new osgParticle::ModularEmitter;//创建粒子放射器（包括计数器、放置器和发射器）
	rpEmitter->setParticleSystem(rpPS);//关联粒子系统

	osg::ref_ptr<osgParticle::RandomRateCounter> rpCounter = new osgParticle::RandomRateCounter;//创建粒子发射器和计数器，调整每一帧增加的粒子数目
	rpCounter->setRateRange(100.f, 100.f);//设置每秒添加的粒子的个数
	rpEmitter->setCounter(rpCounter);//关联计数器

	osg::ref_ptr<osgParticle::PointPlacer> rpPlacer = new osgParticle::PointPlacer;//设置一个点放置器
	rpPlacer->setCenter(osg::Vec3(0.f, 0.f, 0.f));//设置位置
	rpEmitter->setPlacer(rpPlacer);//关联点放置器

	osg::ref_ptr<osgParticle::RadialShooter> rpShooter = new osgParticle::RadialShooter;//创建弧度发射器
	rpShooter->setInitialSpeedRange(100, 0);//设置发射器速度变化范围
	rpEmitter->setShooter(rpShooter);//关联弧度发射器

	this->addChild(rpEmitter);

	osg::ref_ptr<osgParticle::ModularProgram> rpProgram = new osgParticle::ModularProgram;//创建标准编程器对象，控制粒子在生命周期中的更新
	rpProgram->setParticleSystem(rpPS);//关联粒子系统

	osg::ref_ptr<osgParticle::AccelOperator> rpAO = new osgParticle::AccelOperator;//创建重力模拟对象
	rpAO->setToGravity(-1.f);//设置重力加速度，默认为9.80665f
	rpProgram->addOperator(rpAO);//关联重力

	osg::ref_ptr<osgParticle::FluidFrictionOperator> rpFFO = new osgParticle::FluidFrictionOperator;//创建空气阻力模拟
	rpFFO->setFluidToAir();//设置空气属性，FluidViscosity为1.8e-5f，FluidDensity为1.2929f
	rpProgram->addOperator(rpFFO);//关联空气阻力

	this->addChild(rpProgram);

	osg::ref_ptr<osgParticle::ParticleSystemUpdater> rpPSU = new osgParticle::ParticleSystemUpdater;//添加更新器，实现每帧粒子的管理
	rpPSU->addParticleSystem(rpPS);//关联粒子系统

	osg::ref_ptr<osg::Geode> rpGeode = new osg::Geode;
	rpGeode->addDrawable(rpPS);

	this->addChild(rpGeode);
	this->addChild(rpPSU);
}
