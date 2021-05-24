#pragma once

#include <osg/Group>

/**
@brief : 创建模拟爆炸节点
*/
class CParticleGroup1 :
	public osg::Group
{
public:
	CParticleGroup1();
	~CParticleGroup1();

	void init();
};

