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
	osgParticle::Particle particleTemplate;//��������ϵͳģ��
	particleTemplate.setLifeTime(2);//������������
	particleTemplate.setSizeRange(osgParticle::rangef(0.75f, 3.f));//�������Ӵ�С�仯��Χ
	particleTemplate.setAlphaRange(osgParticle::rangef(0.f, 1.f));//��������Alpha�仯��Χ
	particleTemplate.setColorRange(osgParticle::rangev4(
		osg::Vec4(1.f, 0.5f, 0.3f, 1.f)
		, osg::Vec4(0.f, 0.7f, 1.f, 0.f)
	));//����������ɫ�仯��Χ
	particleTemplate.setRadius(0.05f);//���ð뾶
	particleTemplate.setMass(0.05f);//��������

	osg::ref_ptr<osgParticle::ParticleSystem> rpPS = new osgParticle::ParticleSystem;//��������ϵͳ
	rpPS->setDefaultAttributes("Images/smoke.rgb", false, false);//���ò��ʣ��Ƿ�������ӣ��Ƿ���ӹ���
	rpPS->setDefaultParticleTemplate(particleTemplate);//����ģ��

	osg::ref_ptr<osgParticle::ModularEmitter> rpEmitter = new osgParticle::ModularEmitter;//�������ӷ��������������������������ͷ�������
	rpEmitter->setParticleSystem(rpPS);//��������ϵͳ

	osg::ref_ptr<osgParticle::RandomRateCounter> rpCounter = new osgParticle::RandomRateCounter;//�������ӷ������ͼ�����������ÿһ֡���ӵ�������Ŀ
	rpCounter->setRateRange(100.f, 100.f);//����ÿ����ӵ����ӵĸ���
	rpEmitter->setCounter(rpCounter);//����������

	osg::ref_ptr<osgParticle::PointPlacer> rpPlacer = new osgParticle::PointPlacer;//����һ���������
	rpPlacer->setCenter(osg::Vec3(0.f, 0.f, 0.f));//����λ��
	rpEmitter->setPlacer(rpPlacer);//�����������

	osg::ref_ptr<osgParticle::RadialShooter> rpShooter = new osgParticle::RadialShooter;//�������ȷ�����
	rpShooter->setInitialSpeedRange(100, 0);//���÷������ٶȱ仯��Χ
	rpEmitter->setShooter(rpShooter);//�������ȷ�����

	this->addChild(rpEmitter);

	osg::ref_ptr<osgParticle::ModularProgram> rpProgram = new osgParticle::ModularProgram;//������׼��������󣬿������������������еĸ���
	rpProgram->setParticleSystem(rpPS);//��������ϵͳ

	osg::ref_ptr<osgParticle::AccelOperator> rpAO = new osgParticle::AccelOperator;//��������ģ�����
	rpAO->setToGravity(-1.f);//�����������ٶȣ�Ĭ��Ϊ9.80665f
	rpProgram->addOperator(rpAO);//��������

	osg::ref_ptr<osgParticle::FluidFrictionOperator> rpFFO = new osgParticle::FluidFrictionOperator;//������������ģ��
	rpFFO->setFluidToAir();//���ÿ������ԣ�FluidViscosityΪ1.8e-5f��FluidDensityΪ1.2929f
	rpProgram->addOperator(rpFFO);//������������

	this->addChild(rpProgram);

	osg::ref_ptr<osgParticle::ParticleSystemUpdater> rpPSU = new osgParticle::ParticleSystemUpdater;//��Ӹ�������ʵ��ÿ֡���ӵĹ���
	rpPSU->addParticleSystem(rpPS);//��������ϵͳ

	osg::ref_ptr<osg::Geode> rpGeode = new osg::Geode;
	rpGeode->addDrawable(rpPS);

	this->addChild(rpGeode);
	this->addChild(rpPSU);
}
