#include "mesh.h"
#include <iostream>
#include <igl/read_triangle_mesh.h>
#include <Eigen/Sparse>


HEdge::HEdge(bool b) {
	mBoundary = b;

	mTwin = nullptr;
	mPrev = nullptr;
	mNext = nullptr;

	mStart = nullptr;
	mFace = nullptr;

	mFlag = false;
	mValid = true;
}

HEdge* HEdge::twin() const {
	return mTwin;
}

HEdge* HEdge::setTwin(HEdge* e) {
	mTwin = e;
	return mTwin;
}

HEdge* HEdge::prev() const {
	return mPrev;
}

HEdge* HEdge::setPrev(HEdge* e) {
	mPrev = e;
	return mPrev;
}

HEdge* HEdge::next() const {
	return mNext;
}

HEdge* HEdge::setNext(HEdge* e) {
	mNext = e;
	return mNext;
}

Vertex* HEdge::start() const {
	return mStart;
}

Vertex* HEdge::setStart(Vertex* v) {
	mStart = v;
	return mStart;
}

Vertex* HEdge::end() const {
	return mNext->start();
}

Face* HEdge::leftFace() const {
	return mFace;
}

Face* HEdge::setFace(Face* f) {
	mFace = f;
	return mFace;
}

bool HEdge::flag() const {
	return mFlag;
}

bool HEdge::setFlag(bool b) {
	mFlag = b;
	return mFlag;
}

bool HEdge::isBoundary() const {
	return mBoundary;
}

bool HEdge::isValid() const {
	return mValid;
}

bool HEdge::setValid(bool b) {
	mValid = b;
	return mValid;
}

OneRingHEdge::OneRingHEdge(const Vertex* v) {
	if (v == nullptr) {
		mStart = nullptr;
		mNext = nullptr;
	} else {
		mStart = v->halfEdge();
		mNext = v->halfEdge();
	}
}

HEdge* OneRingHEdge::nextHEdge() {
	HEdge* ret = mNext;
	if (mNext != nullptr && mNext->prev()->twin() != mStart) {
		mNext = mNext->prev()->twin();
	} else {
		mNext = nullptr;
	}
	return ret;
}

OneRingVertex::OneRingVertex(const Vertex* v): ring(v) {
}

Vertex* OneRingVertex::nextVertex() {
	HEdge* he = ring.nextHEdge();
	return he != nullptr ? he->end() : nullptr;
}

Vertex::Vertex() : mHEdge(nullptr), mFlag(0) {
	mPosition = Eigen::Vector3f::Zero();
	mColor = VCOLOR_BLUE;
	mNormal = Eigen::Vector3f::Zero();
}

Vertex::Vertex(const Eigen::Vector3f& v): mPosition(v), mHEdge(nullptr), mFlag(0) {
	mColor = VCOLOR_BLUE;
	mNormal = Eigen::Vector3f::Zero();
}

Vertex::Vertex(float x, float y, float z): mHEdge(nullptr), mFlag(0) {
	mPosition = Eigen::Vector3f(x, y, z);
	mColor = VCOLOR_BLUE;
	mNormal = Eigen::Vector3f::Zero();
}


const Eigen::Vector3f& Vertex::position() const {
	return mPosition;
}

const Eigen::Vector3f& Vertex::setPosition(const Eigen::Vector3f& p) {
	mPosition = p;
	return mPosition;
}

const Eigen::Vector3f& Vertex::normal() const {
	return mNormal;
}

const Eigen::Vector3f& Vertex::setNormal(const Eigen::Vector3f& n) {
	mNormal = n;
	return mNormal;
}

const Eigen::Vector3f& Vertex::color() const {
	return mColor;
}

const Eigen::Vector3f& Vertex::setColor(const Eigen::Vector3f& c) {
	mColor = c;
	return mColor;
}

HEdge* Vertex::halfEdge() const {
	return mHEdge;
}

HEdge* Vertex::setHalfEdge(HEdge* he) {
	mHEdge = he;
	return mHEdge;
}

int Vertex::index() const {
	return mIndex;
}

int Vertex::setIndex(int i) {
	mIndex = i;
	return mIndex;
}

int Vertex::flag() const {
	return mFlag;
}

int Vertex::setFlag(int f) {
	mFlag = f;
	return mFlag;
}

bool Vertex::isValid() const {
	return mValid;
}

bool Vertex::setValid(bool b) {
	mValid = b;
	return mValid;
}

bool Vertex::isBoundary() const {
	OneRingHEdge ring(this);
	HEdge* curr = nullptr;
	while (curr = ring.nextHEdge()) {
		if (curr->isBoundary()) {
			return true;
		}
	}
	return false;
}

int Vertex::valence() const {
	int count = 0;
	OneRingVertex ring(this);
	Vertex* curr = nullptr;
	while (curr = ring.nextVertex()) {
		++count;
	}
	return count;
}

Face::Face() : mHEdge(nullptr), mValid(true) {
}

HEdge* Face::halfEdge() const {
	return mHEdge;
}

HEdge* Face::setHalfEdge(HEdge* he) {
	mHEdge = he;
	return mHEdge;
}

bool Face::isBoundary() const {
	HEdge* curr = mHEdge;
	do {
		if (curr->twin()->isBoundary()) {
			return true;
		}
		curr = curr->next();
	} while (curr != mHEdge);
	return false;
}

bool Face::isValid() const {
	return mValid;
}

bool Face::setValid(bool b) {
	mValid = b;
	return mValid;
}

Mesh::Mesh() {
	mVertexPosFlag = true;
	mVertexNormalFlag = true;
	mVertexColorFlag = true;
}

Mesh::~Mesh() {
	clear();
}

const std::vector< HEdge* >& Mesh::edges() const {
	return mHEdgeList;
}

const std::vector< HEdge* >& Mesh::boundaryEdges() const {
	return mBHEdgeList;
}

const std::vector< Vertex* >& Mesh::vertices() const {
	return mVertexList;
}

const std::vector< Face* >& Mesh::faces() const {
	return mFaceList;
}


bool Mesh::isVertexPosDirty() const {
	return mVertexPosFlag;
}

void Mesh::setVertexPosDirty(bool b) {
	mVertexPosFlag = b;
}

bool Mesh::isVertexNormalDirty() const {
	return mVertexNormalFlag;
}

void Mesh::setVertexNormalDirty(bool b) {
	mVertexNormalFlag = b;
}

bool Mesh::isVertexColorDirty() const {
	return mVertexColorFlag;
}

void Mesh::setVertexColorDirty(bool b) {
	mVertexColorFlag = b;
}

bool Mesh::loadMeshFile(const std::string filename) {
	// Use libigl to parse the mesh file
	bool iglFlag = igl::read_triangle_mesh(filename, mVertexMat, mFaceMat);
	if (iglFlag) {
		clear();

		// Construct the half-edge data structure.
		int numVertices = mVertexMat.rows();
		int numFaces = mFaceMat.rows();

		// Fill in the vertex list
		for (int vidx = 0; vidx < numVertices; ++vidx) {
			mVertexList.push_back(new Vertex(mVertexMat(vidx, 0),
			                                 mVertexMat(vidx, 1),
			                                 mVertexMat(vidx, 2)));
		}
		// Fill in the face list
		for (int fidx = 0; fidx < numFaces; ++fidx) {
			addFace(mFaceMat(fidx, 0), mFaceMat(fidx, 1), mFaceMat(fidx, 2));
		}

		std::vector< HEdge* > hedgeList;
		for (int i = 0; i < mBHEdgeList.size(); ++i) {
			if (mBHEdgeList[i]->start()) {
				hedgeList.push_back(mBHEdgeList[i]);
			}
			// TODO
		}
		mBHEdgeList = hedgeList;

		for (int i = 0; i < mVertexList.size(); ++i) {
			mVertexList[i]->adjHEdges.clear();
			mVertexList[i]->setIndex(i);
			mVertexList[i]->setFlag(0);
		}
	} else {
		std::cout << __FUNCTION__ << ": mesh file loading failed!\n";
	}
	return iglFlag;
}

static void _setPrevNext(HEdge* e1, HEdge* e2) {
	e1->setNext(e2);
	e2->setPrev(e1);
}

static void _setTwin(HEdge* e1, HEdge* e2) {
	e1->setTwin(e2);
	e2->setTwin(e1);
}

static void _setFace(Face* f, HEdge* e) {
	f->setHalfEdge(e);
	e->setFace(f);
}

void Mesh::addFace(int v1, int v2, int v3) {
	Face* face = new Face();

	HEdge* hedge[3];
	HEdge* bhedge[3]; // Boundary half-edges
	Vertex* vert[3];

	for (int i = 0; i < 3; ++i) {
		hedge[i] = new HEdge();
		bhedge[i] = new HEdge(true);
	}
	vert[0] = mVertexList[v1];
	vert[1] = mVertexList[v2];
	vert[2] = mVertexList[v3];

	// Connect prev-next pointers
	for (int i = 0; i < 3; ++i) {
		_setPrevNext(hedge[i], hedge[(i + 1) % 3]);
		_setPrevNext(bhedge[i], bhedge[(i + 1) % 3]);
	}

	// Connect twin pointers
	_setTwin(hedge[0], bhedge[0]);
	_setTwin(hedge[1], bhedge[2]);
	_setTwin(hedge[2], bhedge[1]);

	// Connect start pointers for bhedge
	bhedge[0]->setStart(vert[1]);
	bhedge[1]->setStart(vert[0]);
	bhedge[2]->setStart(vert[2]);
	for (int i = 0; i < 3; ++i) {
		hedge[i]->setStart(vert[i]);
	}

	// Connect start pointers
	// Connect face-hedge pointers
	for (int i = 0; i < 3; ++i) {
		vert[i]->setHalfEdge(hedge[i]);
		vert[i]->adjHEdges.push_back(hedge[i]);
		_setFace(face, hedge[i]);
	}
	vert[0]->adjHEdges.push_back(bhedge[1]);
	vert[1]->adjHEdges.push_back(bhedge[0]);
	vert[2]->adjHEdges.push_back(bhedge[2]);

	// Merge boundary if needed
	for (int i = 0; i < 3; ++i) {
		Vertex* start = bhedge[i]->start();
		Vertex* end = bhedge[i]->end();

		for (int j = 0; j < end->adjHEdges.size(); ++j) {
			HEdge* curr = end->adjHEdges[j];
			if (curr->isBoundary() && curr->end() == start) {
				_setPrevNext(bhedge[i]->prev(), curr->next());
				_setPrevNext(curr->prev(), bhedge[i]->next());
				_setTwin(bhedge[i]->twin(), curr->twin());
				bhedge[i]->setStart(nullptr); // Mark as unused
				curr->setStart(nullptr); // Mark as unused
				break;
			}
		}
	}

	// Finally add hedges and faces to list
	for (int i = 0; i < 3; ++i) {
		mHEdgeList.push_back(hedge[i]);
		mBHEdgeList.push_back(bhedge[i]);
	}
	mFaceList.push_back(face);
}

Eigen::Vector3f Mesh::initBboxMin() const {
	return (mVertexMat.colwise().minCoeff()).transpose();
}

Eigen::Vector3f Mesh::initBboxMax() const {
	return (mVertexMat.colwise().maxCoeff()).transpose();
}

void Mesh::groupingVertexFlags() {
	// Init to 255
	for (Vertex* vert : mVertexList) {
		if (vert->flag() != 0) {
			vert->setFlag(255);
		}
	}
	// Group handles
	int id = 0;
	std::vector< Vertex* > tmpList;
	for (Vertex* vert : mVertexList) {
		if (vert->flag() == 255) {
			++id;
			vert->setFlag(id);

			// Do search
			tmpList.push_back(vert);
			while (!tmpList.empty()) {
				Vertex* v = tmpList.back();
				tmpList.pop_back();

				OneRingVertex orv = OneRingVertex(v);
				while (Vertex* v2 = orv.nextVertex()) {
					if (v2->flag() == 255) {
						v2->setFlag(id);
						tmpList.push_back(v2);
					}
				}
			}
		}
	}
}

void Mesh::clear() {
	for (int i = 0; i < mHEdgeList.size(); ++i) {
		delete mHEdgeList[i];
	}
	for (int i = 0; i < mBHEdgeList.size(); ++i) {
		delete mBHEdgeList[i];
	}
	for (int i = 0; i < mVertexList.size(); ++i) {
		delete mVertexList[i];
	}
	for (int i = 0; i < mFaceList.size(); ++i) {
		delete mFaceList[i];
	}

	mHEdgeList.clear();
	mBHEdgeList.clear();
	mVertexList.clear();
	mFaceList.clear();
}

std::vector< int > Mesh::collectMeshStats() {
	int V = 0; // # of vertices
	int E = 0; // # of half-edges
	int F = 0; // # of faces
	int B = 0; // # of boundary loops
	int C = 0; // # of connected components
	int G = 0; // # of genus

	/*====== Programming Assignment 0 ======*/

	/**********************************************/
	/*          Insert your code here.            */
	/**********************************************/
	/*


	/* Collect mesh information as listed above.
	/**********************************************/
	// 1. todo double check fred

	V = mVertexList.size();
	E = mBHEdgeList.size() + mHEdgeList.size();
	F = mFaceList.size();
	B = countBoundaryLoops();
	C = countConnectedComponents();
	G = (E / 2 - V - F - B) / 2 + C; //  E / 2  half edges Fred 1. 2020 Oct 4

	/*====== Programming Assignment 0 ======*/

	std::vector< int > stats;
	stats.push_back(V);
	stats.push_back(E);
	stats.push_back(F);
	stats.push_back(B);
	stats.push_back(C);
	stats.push_back(G);
	return stats;
}

int Mesh::countBoundaryLoops() {
	int count = 0;

	/*====== Programming Assignment 0 ======*/

	/**********************************************/
	/*          Insert your code here.            */
	/**********************************************/
	/*
	/* Helper function for Mesh::collectMeshStats()
	/**********************************************/

	/*====== Programming Assignment 0 ======*/


	// 2 fred not sure about this part 
	for (int i = 0; i < mBHEdgeList.size(); ++i) {
		HEdge* bEdge = mBHEdgeList[i];  // initial to unvisited 
		bEdge->setFlag(false);
	}
	for (int i = 0; i < mBHEdgeList.size(); ++i) {
		HEdge* bEdge = mBHEdgeList[i];
		if (bEdge->flag()) {
			// half-edge have been visited, counted in any boundary loops
			continue;
		}
		bEdge->setFlag(true); // visited
		HEdge* anEdge = bEdge->next();
		while (anEdge != bEdge) {
			anEdge->setFlag(true); // visited
			anEdge = anEdge->next();
		}
		// count a boundary loop 
		count += 1;
	}
	return count;
}
   //  3.fred may change to BFS later , change the names and strucutures 
void simpleDFS(HEdge* edge) {
	if (edge->flag()) return;
	edge->setFlag(true); // visited
	// visit the children
	HEdge* anedge = edge->next();
	simpleDFS(anedge);
	// visit neighbors
	anedge = edge->twin()->next();
	while (anedge != edge) {
		simpleDFS(anedge);
		anedge = anedge->twin()->next();
	}
}

int Mesh::countConnectedComponents() {
	int count = 0;

	/*====== Programming Assignment 0 ======*/

	/**********************************************/
	/*          Insert your code here.            */
	/**********************************************/
	/*
	/* Helper function for Mesh::collectMeshStats()
	/* Count the number of connected components of
	/* the mesh. (Hint: use a stack)
	/**********************************************/


	/*====== Programming Assignment 0 ======*/

	return count;
}
// 3. fred DFS, simple DFS 
// why DFS? why not BFS because it would cause great  
void simpleDFS(HEdge* edge) {
	if (edge->flag()) return;
	edge->setFlag(true); // visited
	// visit the children
	HEdge* anedge = edge->next();
	simpleDFS(anedge);
	// visit neighbors
	anedge = edge->twin()->next();
	while (anedge != edge) {
		simpleDFS(anedge);
		anedge = anedge->twin()->next();
	}
}

void Mesh::computeVertexNormals() {
	/*====== Programming Assignment 0 ======*/

	/**********************************************/
	/*          Insert your code here.            */
	/**********************************************/
	/*
	/* Compute per-vertex normal using neighboring
	/* facet information. (Hint: remember using a 
	/* weighting scheme. Plus, do you notice any
	/* disadvantages of your weighting scheme?)
	/**********************************************/

	/*====== Programming Assignment 0 ======*/
	for (int i = 0; i < mVertexList.size(); ++i) {
		//4. computeVertexNormals init normal fred

		Eigen::Vector3f normal = Eigen::Vector3f::Zero();
		// get neighbors by half-edges and collect the positions
		// center + 2 adjacent neighbors should make a face
		// 0 and size - 1 is adjacent
		Vertex* center = mVertexList[i];
		std::vector<Eigen::Vector3f> neighbors;
		HEdge* edge = center->halfEdge();
		neighbors.push_back(edge->end()->position());
		HEdge* anedge = edge->twin()->next();
		while (edge != anedge) {
			neighbors.push_back(anedge->end()->position());
			anedge = anedge->twin()->next();
		}
		// get normal and area of each face and plus together
		double totalWeights = 0.0;
		for (int j = 0; j < neighbors.size() - 1; ++j) {
			double area = triangleArea(center->position(), neighbors[j + 1], neighbors[j]);
			normal += triangleNormal(center->position(), neighbors[j + 1], neighbors[j]) * area;
			totalWeights += area;
		}
		double lastArea = triangleArea(center->position(), neighbors[0], neighbors[neighbors.size() - 1]);
		normal += triangleNormal(center->position(), neighbors[0], neighbors[neighbors.size() - 1]) * lastArea;
		totalWeights += lastArea;
		// average by weights
		normal /= totalWeights;
		normal.normalize();
		// set normal
		center->setNormal(normal);
	}
	// Notify mesh shaders
	setVertexNormalDirty(true);
}


void Mesh::umbrellaSmooth(bool cotangentWeights) {
	/*====== Programming Assignment 1 ======*/

	if (cotangentWeights) {
		/**********************************************/
		/*          Insert your code here.            */
		/**********************************************/
		/*
		/* Step 1: Implement the cotangent weighting 
		/* scheme for explicit mesh smoothing. 
		/*
		/* Hint:
		/* It is advised to double type to store the 
		/* weights to avoid numerical issues.
		/**********************************************/

		// 5 fred double check Oct 4
		int vertexNumber = mVertexList.size();
		Eigen::SparseMatrix<double> Xt(vertexNumber, 3);
		Eigen::SparseMatrix<double> P(vertexNumber, vertexNumber);
		double lambda = 1.0;
		// make sure all 0
		Xt.setZero();
		P.setZero();
		// filling P and Xt
		for (int i = 0; i < vertexNumber; ++i) {
			Vertex* vertex = mVertexList[i];
			// fill Xt with position
			Eigen::Vector3f position = vertex->position();
			Xt.coeffRef(i, 0) = position[0];
			Xt.coeffRef(i, 1) = position[1];
			Xt.coeffRef(i, 2) = position[2];
			// fill P with corresponding weights
			P.coeffRef(i, i) = -1.0;
			// collect neighbors
			std::vector<Vertex*> neighbors;
			HEdge* edge = vertex->halfEdge();
			neighbors.push_back(edge->end());
			HEdge* anedge = edge->twin()->next();
			while (edge != anedge) {
				neighbors.push_back(anedge->end());
				anedge = anedge->twin()->next();
			}
			// collect weights and column-indices
			double totalWeights = 0.0;
			Eigen::Vector3f centerPosition = vertex->position();
			int length = neighbors.size();
			std::vector<double> weights;
			std::vector<int> indices;
			for (int j = 0; j < length; ++j) {
				int j_m1 = j - 1 < 0 ? j - 1 + length : j - 1;
				int j_p1 = j + 1 >= length ? j + 1 - length : j + 1;
				double weight = triangleCot(
					centerPosition, neighbors[j_m1]->position(), neighbors[j]->position()
				) + triangleCot(
					centerPosition, neighbors[j_p1]->position(), neighbors[j]->position()
				);
				totalWeights += weight;
				weights.push_back(weight);
				indices.push_back(neighbors[j]->index());
			}
			// fill P with weights
			for (int j = 0; j < length; ++j) {
				P.coeffRef(i, indices[j]) = weights[j] / totalWeights;
			}
		}

		// do matrix multiplication
		Eigen::SparseMatrix<double> Xt_p1 = Xt + lambda * P * Xt;
		// fill the result back to vertices
		for (int i = 0; i < vertexNumber; ++i) {
			Eigen::Vector3f position;
			position[0] = Xt_p1.coeff(i, 0);
			position[1] = Xt_p1.coeff(i, 1);
			position[2] = Xt_p1.coeff(i, 2);
			mVertexList[i]->setPosition(position);
		}

	} else {
		/**********************************************/
		/*          Insert your code here.            */
		/**********************************************/
		/*
		/* Step 2: Implement the uniform weighting 
		/* scheme for explicit mesh smoothing.
		/**********************************************/

	}

	// 6 fred check the sparsematrix part 
	int vertexNumber = mVertexList.size();
	Eigen::SparseMatrix<double> Xt(vertexNumber, 3);
	Eigen::SparseMatrix<double> P(vertexNumber, vertexNumber);
	double lambda = 1.0;
	// make sure all 0
	Xt.setZero();
	P.setZero();
	// filling P and Xt
	for (int i = 0; i < vertexNumber; ++i) {
		Vertex* vertex = mVertexList[i];
		// fill Xt with position
		Eigen::Vector3f position = vertex->position();
		Xt.coeffRef(i, 0) = position[0];
		Xt.coeffRef(i, 1) = position[1];
		Xt.coeffRef(i, 2) = position[2];
		// fill P with corresponding weights
		P.coeffRef(i, i) = -1.0;

		double weight = 1.0 / ((double)vertex->valence());
		HEdge* edge = vertex->halfEdge();
		P.coeffRef(i, edge->end()->index()) = weight;
		HEdge* anedge = edge->twin()->next();
		while (edge != anedge) {
			P.coeffRef(i, anedge->end()->index()) = weight;
			anedge = anedge->twin()->next();
		}
	}

	// do matrix multiplication
	Eigen::SparseMatrix<double> Xt_p1 = Xt + lambda * P * Xt;
	// fill the result back to vertices
	for (int i = 0; i < vertexNumber; ++i) {
		Eigen::Vector3f position;
		position[0] = Xt_p1.coeff(i, 0);
		position[1] = Xt_p1.coeff(i, 1);
		position[2] = Xt_p1.coeff(i, 2);
		mVertexList[i]->setPosition(position);
	}
}
	/*====== Programming Assignment 1 ======*/

	computeVertexNormals();
	// Notify mesh shaders
	setVertexPosDirty(true);
}

void Mesh::implicitUmbrellaSmooth(bool cotangentWeights) {
	/*====== Programming Assignment 1 ======*/

	/* A sparse linear system Ax=b solver using the conjugate gradient method. */
	auto fnConjugateGradient = [](const Eigen::SparseMatrix< float >& A,
	                              const Eigen::VectorXf& b,
	                              int maxIterations,
	                              float errorTolerance,
	                              Eigen::VectorXf& x)
	{
		/**********************************************/
		/*          Insert your code here.            */
		/**********************************************/
		/*
		/* Params:
		/*  A: 
		/*  b: 
		/*  maxIterations:	Max number of iterations
		/*  errorTolerance: Error tolerance for the early stopping condition
		/*  x:				Stores the final solution, but should be initialized. 
		/**********************************************/
		/*
		/* Step 1: Implement the biconjugate gradient
		/* method.
		/* Hint: https://en.wikipedia.org/wiki/Biconjugate_gradient_method
		/**********************************************/

		Eigen::VectorXf r = b - A * x;
		Eigen::VectorXf r_star = r;
		double rou = 1.0;
		double alpha = 1.0;
		double w = 1.0;
		Eigen::VectorXf v = r;
		Eigen::VectorXf p = r;
		v.setZero(); p.setZero();
		for (int i = 0; i < maxIterations; ++i) {
			Eigen::VectorXf diff = b - A * x;
			double error = diff.dot(diff);
			if (error < errorTolerance) {
				// very close, further calculation not needed
				break;
			}

			// std::cout<<i<<" ";

			double rou_next = r_star.dot(r);
			double beta = (rou_next / rou) * (alpha / w);
			Eigen::VectorXf p_next = r + beta * (p - w * v);
			Eigen::VectorXf v_next = A * p_next;
			alpha = rou_next / (r_star.dot(v_next));
			Eigen::VectorXf h = x + alpha * p_next;

			diff = b - A * h;
			error = diff.dot(diff);
			if (error < errorTolerance) {
				x = h;
				// very close, further calculation not needed
				break;
			}

			Eigen::VectorXf s = r - alpha * v_next;
			Eigen::VectorXf t = A * s;
			double w_next = t.dot(s) / t.dot(t);
			Eigen::VectorXf x_next = h + w_next * s;

			Eigen::VectorXf r_next = s - w_next * t;
			// replace values
			r = r_next;
			rou = rou_next;
			w = w_next;
			v = v_next;
			p = p_next;
			x = x_next;
		}
		// std::cout<<"result x "<<x<<std::endl;
		return x;
	};
	

	/* IMPORTANT:
	/* Please refer to the following link about the sparse matrix construction in Eigen. */
	/* http://eigen.tuxfamily.org/dox/group__TutorialSparse.html#title3 */
	
	// fred 7 
	int MAX_ITERATIONS = 2000;
	float ERROR_TOLERANCE = 1e-7;

	if (cotangentWeights) {
		/**********************************************/
		/*          Insert your code here.            */
		/**********************************************/
		/*
		/* Step 2: Implement the cotangent weighting 
		/* scheme for implicit mesh smoothing. Use
		/* the above fnConjugateGradient for solving
		/* sparse linear systems.
		/*
		/* Hint:
		/* It is advised to double type to store the
		/* weights to avoid numerical issues.
		/**********************************************/
		int vertexNumber = mVertexList.size();
		Eigen::SparseMatrix<double> Xt(vertexNumber, 3);
		Eigen::SparseMatrix<float> P(vertexNumber, vertexNumber);
		double lambda = 1.0;
		// make sure all 0
		Xt.setZero();
		P.setZero();
		// filling P and Xt
		for (int i = 0; i < vertexNumber; ++i) {
			Vertex* vertex = mVertexList[i];
			// fill Xt with position
			Eigen::Vector3f position = vertex->position();
			Xt.coeffRef(i, 0) = position[0];
			Xt.coeffRef(i, 1) = position[1];
			Xt.coeffRef(i, 2) = position[2];
			// fill P with corresponding weights
			P.coeffRef(i, i) = 1.0 - lambda * (-1.0);
			// collect neighbors
			std::vector<Vertex*> neighbors;
			HEdge* edge = vertex->halfEdge();
			neighbors.push_back(edge->end());
			HEdge* anedge = edge->twin()->next();
			while (edge != anedge) {
				neighbors.push_back(anedge->end());
				anedge = anedge->twin()->next();
			}
			// collect weights and column-indices
			double totalWeights = 0.0;
			Eigen::Vector3f centerPosition = vertex->position();
			int length = neighbors.size();
			std::vector<double> weights;
			std::vector<int> indices;
			for (int j = 0; j < length; ++j) {
				int j_m1 = j - 1 < 0 ? j - 1 + length : j - 1;
				int j_p1 = j + 1 >= length ? j + 1 - length : j + 1;
				double weight = triangleCot(
					centerPosition, neighbors[j_m1]->position(), neighbors[j]->position()
				) + triangleCot(
					centerPosition, neighbors[j_p1]->position(), neighbors[j]->position()
				);
				totalWeights += weight;
				weights.push_back(weight);
				indices.push_back(neighbors[j]->index());
			}
			// fill P with weights
			for (int j = 0; j < length; ++j) {
				P.coeffRef(i, indices[j]) = -lambda * weights[j] / totalWeights;
			}
		}

		// solve linear system
		Eigen::MatrixXf Xt_p1(vertexNumber, 3);
		Xt_p1.setZero();
		for (int j = 0; j < 3; ++j) {
			Eigen::VectorXf x(vertexNumber); x.setZero();
			x = fnConjugateGradient(
				P, Xt.col(j), MAX_ITERATIONS, ERROR_TOLERANCE, x
			);
			Xt_p1.col(j) = x;
		}
		// fill the result back to vertices
		for (int i = 0; i < vertexNumber; ++i) {
			Eigen::Vector3f position;
			position[0] = Xt_p1.coeff(i, 0);
			position[1] = Xt_p1.coeff(i, 1);
			position[2] = Xt_p1.coeff(i, 2);
			mVertexList[i]->setPosition(position);
		}

	} else {
		/**********************************************/
		/*          Insert your code here.            */
		/**********************************************/
		/*
		/* Step 3: Implement the uniform weighting 
		/* scheme for implicit mesh smoothing. Use
		/* the above fnConjugateGradient for solving
		/* sparse linear systems.
		/**********************************************/
		int vertexNumber = mVertexList.size();
		Eigen::SparseMatrix<double> Xt(vertexNumber, 3);
		Eigen::SparseMatrix<float> P(vertexNumber, vertexNumber);
		double lambda = 1.0;
		// make sure all 0
		Xt.setZero();
		P.setZero();
		// filling P and Xt
		for (int i = 0; i < vertexNumber; ++i) {
			Vertex* vertex = mVertexList[i];
			// fill Xt with position
			Eigen::Vector3f position = vertex->position();
			Xt.coeffRef(i, 0) = position[0];
			Xt.coeffRef(i, 1) = position[1];
			Xt.coeffRef(i, 2) = position[2];
			// fill P with corresponding weights
			P.coeffRef(i, i) = 1.0 - lambda * (-1.0);

			double weight = -lambda * 1.0 / ((double)vertex->valence());
			HEdge* edge = vertex->halfEdge();
			P.coeffRef(i, edge->end()->index()) = weight;
			HEdge* anedge = edge->twin()->next();
			while (edge != anedge) {
				P.coeffRef(i, anedge->end()->index()) = weight;
				anedge = anedge->twin()->next();
			}
		}

		// solve linear system
		Eigen::MatrixXf Xt_p1(vertexNumber, 3);
		Xt_p1.setZero();
		for (int j = 0; j < 3; ++j) {
			Eigen::VectorXf x(vertexNumber); x.setZero();
			x = fnConjugateGradient(
				P, Xt.col(j), MAX_ITERATIONS, ERROR_TOLERANCE, x
			);
			Xt_p1.col(j) = x;
		}
		// fill the result back to vertices
		for (int i = 0; i < vertexNumber; ++i) {
			Eigen::Vector3f position;
			position[0] = Xt_p1.coeff(i, 0);
			position[1] = Xt_p1.coeff(i, 1);
			position[2] = Xt_p1.coeff(i, 2);
			mVertexList[i]->setPosition(position);
		}
	}

	/*====== Programming Assignment 1 ======*/

	computeVertexNormals();
	// Notify mesh shaders
	setVertexPosDirty(true);
}
