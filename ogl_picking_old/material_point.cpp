/** @file material_point.cpp
 *  @brief implementation of material point and material poin set used in physics simulation
 *
 *	@author Bartlomiej Filipek
 *	@date March 2011
 */

#include "stdafx.h"
#include "material_point.h"

///////////////////////////////////////////////////////////////////////////////
// MaterialPoint
///////////////////////////////////////////////////////////////////////////////

MaterialPoint::AlgorithmType MaterialPoint::s_algType = MaterialPoint::atEuler;

///////////////////////////////////////////////////////////////////////////////
MaterialPoint::MaterialPoint()
{
	m_nextPos = Vec3d(0.0);
	m_pos = Vec3d(0.0);
	m_prevPos = Vec3d(0.0);
	m_nextVel = Vec3d(0.0);
	m_vel = Vec3d(0.0f);
	m_acceleration = Vec3d(0.0);
	m_mass = 1.0;
	m_radius = 0.0;

	m_stepCounter = 0;
}

///////////////////////////////////////////////////////////////////////////////
MaterialPoint::~MaterialPoint()
{

}

///////////////////////////////////////////////////////////////////////////////
void MaterialPoint::PrepareMove(double deltaTime, const Vec3d &force)
{
	m_acceleration = force /m_mass;
	
	if (s_algType == atEuler || m_stepCounter < 2) CalcEuler(deltaTime);
	else CalcVerlet(deltaTime);	
}

///////////////////////////////////////////////////////////////////////////////
void MaterialPoint::PrepareMoveEuler(double deltaTime, const Vec3d &force)
{
	m_acceleration = force / m_mass;

	m_nextVel = m_vel + m_acceleration * deltaTime;
	m_nextPos = m_pos + m_nextVel*deltaTime;
}

///////////////////////////////////////////////////////////////////////////////
void MaterialPoint::UpdateMove()
{
	m_prevPos = m_pos;
	m_pos = m_nextPos;
	m_vel = m_nextVel;

	m_stepCounter++;
}

///////////////////////////////////////////////////////////////////////////////
bool MaterialPoint::RayTest(const Vec3d &start, const Vec3d &end, Vec3d *pt, double *t, double epsilon)
{
	*pt = ClosestPoint(start, end, m_pos, t);
	double len = Distance(*pt, m_pos);

	return len < (m_radius+epsilon);
}

///////////////////////////////////////////////////////////////////////////////
void MaterialPoint::CalcEuler(double deltaTime)
{
	m_nextVel = m_vel + m_acceleration * deltaTime;
	m_nextPos = m_pos + m_nextVel*deltaTime;
}

///////////////////////////////////////////////////////////////////////////////
void MaterialPoint::CalcVerlet(double deltaTime)
{
	m_nextPos = (2.0 * m_pos) - m_prevPos + (m_acceleration * (deltaTime * deltaTime));
	m_nextVel = (m_nextPos - m_prevPos) / (2.0 * deltaTime);
}



///////////////////////////////////////////////////////////////////////////////
// MaterialPointSet
///////////////////////////////////////////////////////////////////////////////
MaterialPointSet::MaterialPointSet(unsigned int count)
	: m_count(count)
	, m_points(count)
	, m_bounds(count)
{
	// default:
	for (unsigned int i = 0; i < count; ++i)
		m_bounds[i] = false;
}

///////////////////////////////////////////////////////////////////////////////
void MaterialPointSet::Update(double deltaTime)
{
	BeforeStep(deltaTime);
	
	for (unsigned int i = 0; i < m_count; ++i)
	{
		if (m_bounds[i] == false)
			m_points[i].PrepareMove(deltaTime, Force(i));
	}

	AfterStep(deltaTime);

	for (unsigned int i = 0; i < m_count; ++i)
	{
		if (m_bounds[i] == false)
			m_points[i].UpdateMove();
	}
}

///////////////////////////////////////////////////////////////////////////////
bool MaterialPointSet::RayTest(const Vec3d &start, const Vec3d &end, unsigned int *id, double *t, double epsilon)
{
	unsigned int pointID = m_count+1;
	bool foundCollision = false;
	double minDistToStart = 10000000.0;
	double dst;
	Vec3d pt;
	for (unsigned int i = 0; i < m_count; ++i)
	{
		if (m_points[i].RayTest(start, end, &pt, t, epsilon))
		{
			dst = Distance(start, pt);
			if (dst < minDistToStart)
			{
				minDistToStart = dst;
				pointID = i;
				foundCollision = true;
			}
		}
	}

	*id = pointID;

	return foundCollision;
}