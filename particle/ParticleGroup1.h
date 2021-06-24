#pragma once

#include <osg/Group>
#include <osg/Sequence>

#include <osgGA/GUIEventHandler>

#include <osgParticle/FireEffect>
#include <osgParticle/Operator>

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
	void setParticle1();
	void setParticle2();

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

class VortexOperator : public osgParticle::Operator 
{
public:
	VortexOperator()
		: osgParticle::Operator(), center_(0, 0, 0), axis_(0, 0, 1), intensity_(0.1f) {}

	VortexOperator(const VortexOperator &copy, const osg::CopyOp &copyop = osg::CopyOp::SHALLOW_COPY)
		: osgParticle::Operator(copy, copyop), center_(copy.center_), axis_(copy.axis_), intensity_(copy.intensity_) {}

	META_Object(osgParticle, VortexOperator);

	void setCenter(const osg::Vec3 &c)
	{
		center_ = c;
	}

	void setAxis(const osg::Vec3 &a)
	{
		axis_ = a / a.length();
	}

	// this method is called by ModularProgram before applying
	// operators on the particle set via the operate() method.    
	void beginOperate(osgParticle::Program *prg)
	{
		// we have to check whether the reference frame is RELATIVE_RF to parents
		// or it's absolute; in the first case, we must transform the vectors
		// from local to world space.
		if (prg->getReferenceFrame() == osgParticle::Program::RELATIVE_RF) {
			// transform the center point (full transformation)
			xf_center_ = prg->transformLocalToWorld(center_);
			// transform the axis vector (only rotation and scale)
			xf_axis_ = prg->rotateLocalToWorld(axis_);
		}
		else {
			xf_center_ = center_;
			xf_axis_ = axis_;
		}
	}

	// apply a vortex-like acceleration. This code is not optimized,
	// it's here only for demonstration purposes.
	void operate(osgParticle::Particle *P, double dt)
	{
		float l = xf_axis_ * (P->getPosition() - xf_center_);
		osg::Vec3 lc = xf_center_ + xf_axis_ * l;
		osg::Vec3 R = P->getPosition() - lc;
		osg::Vec3 v = (R ^ xf_axis_) * P->getMassInv() * intensity_;

		// compute new position
		osg::Vec3 newpos = P->getPosition() + v * dt;

		// update the position of the particle without modifying its
		// velocity vector (this is unusual, normally you should call
		// the Particle::setVelocity() or Particle::addVelocity()
		// methods).
		P->setPosition(newpos);
	}

protected:
	virtual ~VortexOperator() {}

private:
	osg::Vec3 center_;
	osg::Vec3 xf_center_;
	osg::Vec3 axis_;
	osg::Vec3 xf_axis_;
	float intensity_;
};