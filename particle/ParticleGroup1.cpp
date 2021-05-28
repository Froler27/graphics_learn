#include <iomanip>

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
#include <osgParticle/FireEffect>

#include <osg/Geometry>
#include <osg/Texture2D>
#include <osg/Billboard>
#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>

#include <osgDB/ReadFile>

#include "ParticleGroup1.h"


CParticleGroup1::CParticleGroup1(int iId)
{
	if (iId == 0)
		init();
	else if (iId == 1)
		init1();
	else
		init2();
}


CParticleGroup1::~CParticleGroup1()
{
}

void CParticleGroup1::init()
{
	setParticle1();
	setParticle2();
}

void CParticleGroup1::init1()
{
	this->addChild(createSequence());
}

void CParticleGroup1::init2()
{
	m_rpFire = new osgParticle::FireEffect;
	m_rpFire->setWind(osg::Vec3(0, 100, 0));
	m_rpFire->setScale(50);
	m_rpFire->setIntensity(3);
	osg::PositionAttitudeTransform* pTrans = new osg::PositionAttitudeTransform;
	//pTrans->setAttitude(osg::Quat(osg::PI_2, osg::Z_AXIS));
	pTrans->addChild(m_rpFire);
	this->addChild(pTrans);
}

void CParticleGroup1::setParticle1()
{
	osgParticle::Particle particleTemplate;//��������ϵͳģ��
	particleTemplate.setLifeTime(3);//������������
	particleTemplate.setSizeRange(osgParticle::rangef(0.5f, 1.f));//�������Ӵ�С�仯��Χ
	particleTemplate.setAlphaRange(osgParticle::rangef(1.f, 1.f));//��������Alpha�仯��Χ
	particleTemplate.setColorRange(osgParticle::rangev4(
		osg::Vec4(1.f, 0.5f, 0.3f, 1.f)
		, osg::Vec4(0.f, 0.7f, 1.f, 0.f)
	));//����������ɫ�仯��Χ
	particleTemplate.setRadius(0.5f);//���ð뾶
	particleTemplate.setMass(2.f);//��������

	osg::ref_ptr<osgParticle::ParticleSystem> rpPS = new osgParticle::ParticleSystem;//��������ϵͳ
	rpPS->setDefaultAttributes("Images/particle.rgb", false, false);//���ò��ʣ��Ƿ�������ӣ��Ƿ���ӹ���
	rpPS->setDefaultParticleTemplate(particleTemplate);//����ģ��


	osg::ref_ptr<osgParticle::ModularEmitter> rpEmitter = new osgParticle::ModularEmitter;//�������ӷ��������������������������ͷ�������
	rpEmitter->setParticleSystem(rpPS);//��������ϵͳ
	osg::ref_ptr<osgParticle::RandomRateCounter> rpCounter = new osgParticle::RandomRateCounter;//�������ӷ������ͼ�����������ÿһ֡���ӵ�������Ŀ
	rpCounter->setRateRange(50.f, 100.f);//����ÿ����ӵ����ӵĸ���
	rpEmitter->setCounter(rpCounter);//����������
	osg::ref_ptr<osgParticle::SectorPlacer> rpPlacer = new osgParticle::SectorPlacer;
	rpPlacer->setCenter(osg::Vec3(0.f, 0.f, 0.f));//����λ��
	rpPlacer->setRadiusRange(4, 5);
	rpPlacer->setPhiRange(0, 2 * osg::PI);
	rpEmitter->setPlacer(rpPlacer);//�����������
	osg::ref_ptr<osgParticle::RadialShooter> rpShooter = new osgParticle::RadialShooter;//�������ȷ�����
	rpShooter->setInitialSpeedRange(0, 10);//���÷������ٶȱ仯��Χ
											 //rpShooter->setInitialRotationalSpeedRange(osg::Vec3(0, 0, 0), osg::Vec3(1, 1, 1));
	rpShooter->setThetaRange(0, 0);
	rpShooter->setPhiRange(0, 0);
	rpEmitter->setShooter(rpShooter);//�������ȷ�����
	this->addChild(rpEmitter);


	osg::ref_ptr<osgParticle::ModularProgram> rpProgram = new osgParticle::ModularProgram;//������׼��������󣬿������������������еĸ���
	rpProgram->setParticleSystem(rpPS);//��������ϵͳ
	osg::ref_ptr<osgParticle::AccelOperator> rpAO = new osgParticle::AccelOperator;//��������ģ�����
	rpAO->setToGravity();//�����������ٶȣ�Ĭ��Ϊ9.80665f
						 //rpProgram->addOperator(rpAO);//��������
	osg::ref_ptr<osgParticle::FluidFrictionOperator> rpFFO = new osgParticle::FluidFrictionOperator;//������������ģ��
	rpFFO->setFluidToAir();//���ÿ������ԣ�FluidViscosityΪ1.8e-5f��FluidDensityΪ1.2929f
	//rpProgram->addOperator(rpFFO);//������������
	VortexOperator *op2 = new VortexOperator;
	op2->setCenter(osg::Vec3(8, 0, 0));
	rpProgram->addOperator(op2);
	this->addChild(rpProgram);

	osg::ref_ptr<osgParticle::ParticleSystemUpdater> rpPSU = new osgParticle::ParticleSystemUpdater;//��Ӹ�������ʵ��ÿ֡���ӵĹ���
	rpPSU->addParticleSystem(rpPS);//��������ϵͳ

	osg::ref_ptr<osg::Geode> rpGeode = new osg::Geode;
	rpGeode->addDrawable(rpPS);

	this->addChild(rpGeode);
	this->addChild(rpPSU);
}

void CParticleGroup1::setParticle2()
{
	osgParticle::Particle particleTemplate;//��������ϵͳģ��
	particleTemplate.setLifeTime(3);//������������
	particleTemplate.setSizeRange(osgParticle::rangef(0.5f, 0.5f));//�������Ӵ�С�仯��Χ
	particleTemplate.setAlphaRange(osgParticle::rangef(1.f, 1.f));//��������Alpha�仯��Χ
	particleTemplate.setColorRange(osgParticle::rangev4(
		osg::Vec4(1.f, 0.5f, 0.3f, 1.f)
		, osg::Vec4(0.f, 0.7f, 1.f, 0.f)
	));//����������ɫ�仯��Χ
	particleTemplate.setRadius(0.5f);//���ð뾶
	particleTemplate.setMass(2.f);//��������

	osg::ref_ptr<osgParticle::ParticleSystem> rpPS = new osgParticle::ParticleSystem;//��������ϵͳ
	rpPS->setDefaultAttributes("Images/particle.rgb", false, false);//���ò��ʣ��Ƿ�������ӣ��Ƿ���ӹ���
	rpPS->setDefaultParticleTemplate(particleTemplate);//����ģ��


	osg::ref_ptr<osgParticle::ModularEmitter> rpEmitter = new osgParticle::ModularEmitter;//�������ӷ��������������������������ͷ�������
	rpEmitter->setParticleSystem(rpPS);//��������ϵͳ
	osg::ref_ptr<osgParticle::RandomRateCounter> rpCounter = new osgParticle::RandomRateCounter;//�������ӷ������ͼ�����������ÿһ֡���ӵ�������Ŀ
	rpCounter->setRateRange(50.f, 100.f);//����ÿ����ӵ����ӵĸ���
	rpEmitter->setCounter(rpCounter);//����������
	osg::ref_ptr<osgParticle::SectorPlacer> rpPlacer = new osgParticle::SectorPlacer;
	rpPlacer->setCenter(osg::Vec3(0.f, 0.f, 0.f));//����λ��
	rpPlacer->setRadiusRange(1, 2.5);
	rpPlacer->setPhiRange(0, 2 * osg::PI);
	rpEmitter->setPlacer(rpPlacer);//�����������
	osg::ref_ptr<osgParticle::RadialShooter> rpShooter = new osgParticle::RadialShooter;//�������ȷ�����
	rpShooter->setInitialSpeedRange(10, 10);//���÷������ٶȱ仯��Χ
											 //rpShooter->setInitialRotationalSpeedRange(osg::Vec3(0, 0, 0), osg::Vec3(1, 1, 1));
	rpShooter->setThetaRange(0, 0);
	rpShooter->setPhiRange(0, 0);
	rpEmitter->setShooter(rpShooter);//�������ȷ�����
	this->addChild(rpEmitter);


	osg::ref_ptr<osgParticle::ModularProgram> rpProgram = new osgParticle::ModularProgram;//������׼��������󣬿������������������еĸ���
	rpProgram->setParticleSystem(rpPS);//��������ϵͳ
	osg::ref_ptr<osgParticle::AccelOperator> rpAO = new osgParticle::AccelOperator;//��������ģ�����
	rpAO->setToGravity();//�����������ٶȣ�Ĭ��Ϊ9.80665f
						 //rpProgram->addOperator(rpAO);//��������
	osg::ref_ptr<osgParticle::FluidFrictionOperator> rpFFO = new osgParticle::FluidFrictionOperator;//������������ģ��
	rpFFO->setFluidToAir();//���ÿ������ԣ�FluidViscosityΪ1.8e-5f��FluidDensityΪ1.2929f
	//rpProgram->addOperator(rpFFO);//������������
	VortexOperator *op2 = new VortexOperator;
	op2->setCenter(osg::Vec3(8, 0, 0));
	//rpProgram->addOperator(op2);
	this->addChild(rpProgram);

	osg::ref_ptr<osgParticle::ParticleSystemUpdater> rpPSU = new osgParticle::ParticleSystemUpdater;//��Ӹ�������ʵ��ÿ֡���ӵĹ���
	rpPSU->addParticleSystem(rpPS);//��������ϵͳ

	osg::ref_ptr<osg::Geode> rpGeode = new osg::Geode;
	rpGeode->addDrawable(rpPS);

	this->addChild(rpGeode);
	this->addChild(rpPSU);
}

osg::Node* CParticleGroup1::createBillboard(osg::Image* pImage)
{
	osg::ref_ptr<osg::Geometry> rpGeom = new osg::Geometry;

	osg::ref_ptr<osg::Vec3Array> rpVectices = new osg::Vec3Array;
	rpVectices->push_back(osg::Vec3(-0.2545f, 0.f, -0.216f));
	rpVectices->push_back(osg::Vec3(0.2545f, 0.f, -0.216f));
	rpVectices->push_back(osg::Vec3(0.2545f, 0.f, 0.216f));
	rpVectices->push_back(osg::Vec3(-0.2545f, 0.f, 0.216f));
	rpGeom->setVertexArray(rpVectices);

	osg::ref_ptr<osg::Vec3Array> rpNomals = new osg::Vec3Array;
	rpNomals->push_back(osg::Vec3(1.f, 0.f, 0.f) ^ osg::Vec3(0.f, 0.f, 1.f));
	rpGeom->setNormalArray(rpNomals);
	rpGeom->setNormalBinding(osg::Geometry::BIND_OVERALL);

	osg::ref_ptr<osg::Vec2Array> rpTexCoors = new osg::Vec2Array;
	rpTexCoors->push_back(osg::Vec2(0.f, 0.f));
	rpTexCoors->push_back(osg::Vec2(1.f, 0.f));
	rpTexCoors->push_back(osg::Vec2(1.f, 1.f));
	rpTexCoors->push_back(osg::Vec2(0.f, 1.f));
	rpGeom->setTexCoordArray(0, rpTexCoors);

	rpGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));

	if (pImage)
	{
		osg::ref_ptr<osg::StateSet> rpGeomStateSet = rpGeom->getOrCreateStateSet();
		osg::ref_ptr<osg::Texture2D> rpTexture = new osg::Texture2D;
		rpTexture->setImage(pImage);
		rpGeomStateSet->setTextureAttributeAndModes(0, rpTexture, osg::StateAttribute::ON);
		rpGeomStateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
		rpGeomStateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	}

	osg::ref_ptr<osg::Billboard> rpBillboard1 = new osg::Billboard;
	rpBillboard1->setMode(osg::Billboard::POINT_ROT_EYE);
	rpBillboard1->addDrawable(rpGeom, osg::Vec3(5.f, 0.f, 0.f));

	osg::ref_ptr<osg::Billboard> rpBillboard2 = new osg::Billboard;

	osg::ref_ptr<osg::Group> rpBillboardGroup = new osg::Group;
	rpBillboardGroup->addChild(rpBillboard1);

	return rpBillboardGroup.release();
}

osg::Sequence* CParticleGroup1::createSequence()
{
	osg::ref_ptr<osg::Sequence> rpSeq = new osg::Sequence;
	using Filenames = std::vector<std::string>;
	Filenames filenames;
	
	for (int i = 1; i <= 121; i++)
	{
		std::stringstream buf;
		buf << "../../explosion/Explosion" << std::setw(4) << std::setfill('0') << i << ".png";
		filenames.push_back(buf.str());
	}

	for (Filenames::iterator itr = filenames.begin(); itr != filenames.end(); ++itr)
	{
		osg::Image* pImage = osgDB::readImageFile(*itr);
		if (pImage)
		{
			rpSeq->addChild(createBillboard(pImage));
			rpSeq->setTime(rpSeq->getNumChildren() - 1, 0.1f);
		}
	}

	rpSeq->setInterval(osg::Sequence::LOOP, 0, -1);
	rpSeq->setDuration(0.5f, -1);//���ò����ٶȼ��ظ��Ĵ���
	rpSeq->setMode(osg::Sequence::START);

	return rpSeq.release();
}
