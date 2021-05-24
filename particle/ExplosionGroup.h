#pragma once

#include <osg/Group>

/**
@brief : 创建模拟爆炸节点
*/
class CExplosionGroup :
	public osg::Group
{
public:
	CExplosionGroup();
	~CExplosionGroup();

	void init();
};

