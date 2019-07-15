#include "RigidBody.h"

void RigidBody::Init()
{
}

RigidBody::RigidBody(vector<XMFLOAT3> vertices)
{
	Init();
	//Count the points of the incoming list
	unsigned int vertCount = vertices.size();

	//If there are none just return, we have no information to create the BS from
	if (vertCount == 0)
		return;

	//Max and min as the first vector of the list
	maxLocal = minLocal = vertices[0];

	//Get the max and min out of the list
	for (unsigned int i = 1; i < vertCount; ++i)
	{
		if (maxLocal.x < vertices[i].x) maxLocal.x = vertices[i].x;
		else if (minLocal.x > vertices[i].x) minLocal.x = vertices[i].x;

		if (maxLocal.y < vertices[i].y) maxLocal.y = vertices[i].y;
		else if (minLocal.y > vertices[i].y) minLocal.y = vertices[i].y;

		if (maxLocal.z < vertices[i].z) maxLocal.z = vertices[i].z;
		else if (minLocal.z > vertices[i].z) minLocal.z = vertices[i].z;
	}

	//with model matrix being the identity, local and global are the same
	minGlobal = minLocal;
	maxGlobal = maxLocal;

	XMVECTOR minLoc = XMLoadFloat3(&minLocal);
	XMVECTOR maxLoc = XMLoadFloat3(&minLocal);

	//with the max and the min we calculate the center
	XMStoreFloat3(&centerLocal, (maxLoc + minLoc) / 2.0f);

	//we calculate the distance between min and max vectors
	XMStoreFloat3(&halfWidth, (maxLoc - minLoc) / 2.0f);

	//Get the distance between the center and either the min or the max
	XMStoreFloat(&radius, XMVector3Length(XMLoadFloat3(&halfWidth)));
}


RigidBody::~RigidBody()
{
}
