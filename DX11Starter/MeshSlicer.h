#pragma once

using namespace std;

class MeshSlicer 
{
public:
	MeshSlicer();
	~MeshSlicer();
private:
	void TestEdge(); // Adds a vertex to either the left or right side depending on what side of the slicing plane they are on
	void CompareZ();
	void GetVertex();
	void SplitEdge();
	void AddConvexShape();
	void AddPoints();
	void AddEdgeLinks();
};