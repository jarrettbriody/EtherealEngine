#include "ParticleStructs.hlsli"

RWBuffer<uint>							DrawArguments		: register(u0);
RWStructuredBuffer<ParticleDrawInfo>	ParticleDrawList	: register(u1);

[numthreads(1, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
	DrawArguments[0] = ParticleDrawList.IncrementCounter() * 6; //IndexCountPerInstance
	DrawArguments[1] = 1; //InstanceCount
	DrawArguments[2] = 0; //StartIndexLocation
	DrawArguments[3] = 0; //BaseVertexLocation
	DrawArguments[4] = 0; //StartInstanceLocation
}