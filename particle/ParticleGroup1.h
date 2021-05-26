#pragma once

#include <osg/Group>
#include <osg/Sequence>

#include <osgGA/GUIEventHandler>

#include <osgParticle/FireEffect>

#include <osgViewer/View>

/**
@brief : 创建模拟爆炸节点
*/
class CParticleGroup1 :
	public osg::Group
{
public:
	CParticleGroup1(int iId = 0);
	~CParticleGroup1();

	void init();
	void init1();
	void init2();

	osg::Node* createBillboard(osg::Image* pImage);
	osg::Sequence* createSequence();
	void setPosition(const osg::Vec3& v3Pos) { m_rpFire->setPosition(v3Pos); }

private:
	osg::ref_ptr<osgParticle::FireEffect> m_rpFire;
};

class CMoveFly : public osg::NodeCallback
{
public:
	void operator()(osg::Node* node, osg::NodeVisitor* nv)
	{
		osg::PositionAttitudeTransform* pTrans = static_cast<osg::PositionAttitudeTransform*>(node);
		osg::Vec3 v3Pos = pTrans->getPosition();
		v3Pos += osg::Vec3(0.1f, 0, 0);
		pTrans->setPosition(v3Pos);
	}
};

class CFirePosCallback : public osgGA::GUIEventHandler
{
public:
	CFirePosCallback(osg::PositionAttitudeTransform* pTrans, CParticleGroup1* pFire)
		: m_rpFire(pFire)
		, m_rpTrans(pTrans)
	{
	}

	bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
	{
		osgViewer::View* pView = dynamic_cast<osgViewer::View*>(&aa);
		switch (ea.getEventType())
		{
		case osgGA::GUIEventAdapter::FRAME:
		{
			osg::Vec3 v3Pos = m_rpTrans->getPosition();
			m_rpFire->setPosition(v3Pos);
		}
		default:
			break;
		}
		return false;
	}

private:
	osg::ref_ptr<CParticleGroup1> m_rpFire;
	osg::ref_ptr<osg::PositionAttitudeTransform> m_rpTrans;
};