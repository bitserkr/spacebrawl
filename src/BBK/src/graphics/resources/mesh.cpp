#include "resources/mesh.h"
#include "math/mathlib.h"
#include "fileio/fileio.h"
#include "utils.h"

namespace
{
bbk::Tuple<float, 3> ComputeRoots(const bbk::Matrix3x3 &mtx);

inline bbk::Vector3 GetComplement1(const bbk::Vector3 &u, const bbk::Vector3 &v) {return u.Cross(v).Normalise();}
void GetComplement2(const bbk::Vector3 &u, bbk::Vector3 &v, bbk::Vector3 &w);
int ComputeRank(bbk::Matrix3x3 &mtx);
void EigenSolver(const bbk::Matrix3x3 &mtx, float evalues[3], bbk::Vector3 evectors[3]);

bbk::Tuple<bbk::Vector3, 3> GenerateEigenValues(const bbk::Matrix3x3 &mtx);
double eigen[3];
} // anon namespace

namespace bbk
{
Mesh::Mesh() :
	numVertices_(0),
	vertices_(nullptr),
	vertInd_(nullptr),
	hasNormals_(false),
	hasTexCoords_(false)
{}

Mesh::~Mesh()
{
	if (vertices_)
		delete[] vertices_;
	if (vertInd_)
		delete[] vertInd_;
}

bool Mesh::LoadMeshFromFile(const char *filename)
{
	xmlElement *pRoot = bbk::fileio::ReadFile(filename);
	if (!pRoot) return false;

	bool y_up = true;
	if (pRoot->GetChildElem("asset")->GetChildElem("up_axis")->GetContent_str().compare(0, 4, "Z_UP") == 0)
		y_up = false;

	// Locate mesh node of collada doc
	bbk::xmlElement *pMeshNode = pRoot->GetChildElem("library_geometries")->GetChildElem("geometry")->GetChildElem("mesh");

	/*==========================================================================
	 * Build array of positions
	 *------------------------------------------------------------------------*/
	//--------------------------------------------------------------------------
	// Find XML node that contains positions
	bbk::xmlElement *pSrcPosNode = nullptr;
	{
		/* To determine source id of positions source node */
		
		// Get list of vertices input nodes
		std::list<bbk::xmlElement*> childlist = pMeshNode->GetChildElem("vertices")->GetChildElemListByTag("input");
		// Locate input node for POSITION
		std::string srcPosID;
		std::list<bbk::xmlElement*>::const_iterator it     = childlist.begin();
		std::list<bbk::xmlElement*>::const_iterator it_end = childlist.end();
		for (size_t semlen = sizeof("POSITION"); it != it_end; ++it)
		{
			if ((*it)->GetAttrib("semantic")->GetValue_str().compare(0, semlen, "POSITION") == 0) // Found POSITION input node
			{
				// Save source id of positions node
				srcPosID = (*it)->GetAttrib("source")->GetValue_str();
				break;
			}
		}

		// Locate positions source node using source id
		childlist = pMeshNode->GetChildElemListByTag("source");
		it     = childlist.begin();
		it_end = childlist.end();
		for (size_t semlen = srcPosID.size(); it != it_end; ++it)
		{
			if (srcPosID.compare(1, semlen, (*it)->GetAttrib("id")->GetValue_str()) == 0) // Found matching source id
			{
				pSrcPosNode = *it; // Found positions source node
				break;
			}
		}
	}
	if (!pSrcPosNode) return false;

	//--------------------------------------------------------------------------
	// Convert positions source node float array into array of Point3
	unsigned numPositions = pSrcPosNode->GetChildElem("technique_common")->GetChildElem("accessor")->GetAttrib("count")->GetValue_int();
	Point3  *positions    = new Point3[numPositions];
	{
		// Tokenise long string of floats into individual floats in string format
		std::vector<std::string> pos_floatarray(bbk::utils::SplitStrToWords(pSrcPosNode->GetChildElem("float_array")->GetContent_str(), ' '));
		// Save floats into points array
		unsigned stride = pSrcPosNode->GetChildElem("technique_common")->GetChildElem("accessor")->GetAttrib("stride")->GetValue_int();
		for (size_t i = 0; i < numPositions; ++i)
		{
			positions[i].x = static_cast<float>(std::atof(pos_floatarray[stride * i    ].c_str()));
			positions[i].y = static_cast<float>(std::atof(pos_floatarray[stride * i + 1].c_str()));
			positions[i].z = static_cast<float>(std::atof(pos_floatarray[stride * i + 2].c_str()));

			if (!y_up)
			{
				float temp = positions[i].y;
				positions[i].y = positions[i].z;
				positions[i].z = -temp;
			}
		}
	}

	/*==========================================================================
	 * Build array of texture coordinates
	 *------------------------------------------------------------------------*/
	unsigned         numTexCoords = 0;
	Tuple<float, 2> *texcoords    = nullptr;
	{
		//----------------------------------------------------------------------
		// Find XML node that contains tex coords
		
		/* To determine source id of texcoords source node */
		// Get list of triangles input nodes
		std::list<bbk::xmlElement*> childlist = pMeshNode->GetChildElem("triangles")->GetChildElemListByTag("input");
		// Locate input node for TEXCOORD
		std::string srcTCID;
		std::list<bbk::xmlElement*>::const_iterator it     = childlist.begin();
		std::list<bbk::xmlElement*>::const_iterator it_end = childlist.end();
		for (size_t semlen = sizeof("TEXCOORD"); it != it_end; ++it)
		{
			if ((*it)->GetAttrib("semantic")->GetValue_str().compare(0, semlen, "TEXCOORD") == 0) // Found TEXCOORD input node
			{
				// Save source id of texcoords node
				srcTCID = (*it)->GetAttrib("source")->GetValue_str();
				break;
			}
		}

		if (srcTCID.size()) // Found a name for texcoords source id
		{
			bbk::xmlElement *pSrcTCNode = nullptr;

			// Locate texcoords source node using source id
			childlist = pMeshNode->GetChildElemListByTag("source");
			it     = childlist.begin();
			it_end = childlist.end();
			for (size_t semlen = srcTCID.size(); it != it_end; ++it)
			{
				if (srcTCID.compare(1, semlen, (*it)->GetAttrib("id")->GetValue_str()) == 0) // Found matching source id
				{
					pSrcTCNode = *it; // Found texcoords source node
					break;
				}
			}

			if (pSrcTCNode)
			{
				hasTexCoords_ = true;
				//--------------------------------------------------------------------------
				// Convert texcoords source node float array into array of texcoords
				numTexCoords = pSrcTCNode->GetChildElem("technique_common")->GetChildElem("accessor")->GetAttrib("count")->GetValue_int();
				texcoords    = new Tuple<float, 2>[numTexCoords];
				{
					// Tokenise long string of floats into individual floats in string format
					std::vector<std::string> tc_floatarray(bbk::utils::SplitStrToWords(pSrcTCNode->GetChildElem("float_array")->GetContent_str(), ' '));
					// Save floats into texcoords array
					unsigned stride = pSrcTCNode->GetChildElem("technique_common")->GetChildElem("accessor")->GetAttrib("stride")->GetValue_int();
					for (size_t i = 0; i < numTexCoords; ++i)
					{
						texcoords[i][0] =  static_cast<float>(std::atof(tc_floatarray[stride * i    ].c_str()));
						texcoords[i][1] = -static_cast<float>(std::atof(tc_floatarray[stride * i + 1].c_str()));
					}
				}
			}
			else
			{
				hasTexCoords_ = false;
			}
		}
		else
		{
			hasTexCoords_ = false;
		}
	}

	/*==========================================================================
	 * Build array of vertex normals
	 *------------------------------------------------------------------------*/
	unsigned numNormals = 0;
	Vector3 *normals    = nullptr;
	{
		//----------------------------------------------------------------------
		// Find XML node that contains normals
		
		/* To determine source id of texcoords source node */
		// Get list of triangles input nodes
		std::list<bbk::xmlElement*> childlist = pMeshNode->GetChildElem("triangles")->GetChildElemListByTag("input");
		// Locate input node for NORMAL
		std::string srcNrmID;
		std::list<bbk::xmlElement*>::const_iterator it     = childlist.begin();
		std::list<bbk::xmlElement*>::const_iterator it_end = childlist.end();
		for (size_t semlen = sizeof("NORMAL"); it != it_end; ++it)
		{
			if ((*it)->GetAttrib("semantic")->GetValue_str().compare(0, semlen, "NORMAL") == 0) // Found NORMAL input node
			{
				// Save source id of normals node
				srcNrmID = (*it)->GetAttrib("source")->GetValue_str();
				break;
			}
		}

		if (srcNrmID.size()) // Found a name for normals source id
		{
			bbk::xmlElement *pSrcNrmNode = nullptr;

			// Locate normals source node using source id
			childlist = pMeshNode->GetChildElemListByTag("source");
			it     = childlist.begin();
			it_end = childlist.end();
			for (size_t semlen = srcNrmID.size(); it != it_end; ++it)
			{
				if (srcNrmID.compare(1, semlen, (*it)->GetAttrib("id")->GetValue_str()) == 0) // Found matching source id
				{
					pSrcNrmNode = *it; // Found normals source node
					break;
				}
			}

			if (pSrcNrmNode)
			{
				hasNormals_ = true;
				//--------------------------------------------------------------------------
				// Convert normals source node float array into array of normals
				numNormals = pSrcNrmNode->GetChildElem("technique_common")->GetChildElem("accessor")->GetAttrib("count")->GetValue_int();
				normals    = new Vector3[numNormals];
				{
					// Tokenise long string of floats into individual floats in string format
					std::vector<std::string> nrm_floatarray(bbk::utils::SplitStrToWords(pSrcNrmNode->GetChildElem("float_array")->GetContent_str(), ' '));
					// Save floats into normals array
					unsigned stride = pSrcNrmNode->GetChildElem("technique_common")->GetChildElem("accessor")->GetAttrib("stride")->GetValue_int();
					for (size_t i = 0; i < numNormals; ++i)
					{
						normals[i].x = static_cast<float>(std::atof(nrm_floatarray[stride * i    ].c_str()));
						normals[i].y = static_cast<float>(std::atof(nrm_floatarray[stride * i + 1].c_str()));
						normals[i].z = static_cast<float>(std::atof(nrm_floatarray[stride * i + 2].c_str()));

						if (!y_up)
						{
							float temp = normals[i].y;
							normals[i].y = normals[i].z;
							normals[i].z = -temp;
						}
					}
				}
			}
			else
			{
				hasNormals_ = false;
			}
		}
		else
		{
			hasNormals_ = false;
		}
	}

	/*==========================================================================
	 * Build array of vertex indices
	 *------------------------------------------------------------------------*/
	//--------------------------------------------------------------------------
	// Find XML node that contains triangle indices
	bbk::xmlElement *pTriNode = pMeshNode->GetChildElem("triangles");
	
	//--------------------------------------------------------------------------
	// Convert indices node int array into array of indexed-triangle vertex indices
	unsigned numTri = pTriNode->GetAttrib("count")->GetValue_int();
	
	if (vertices_)
		delete[] vertices_;
	if (vertInd_)
		delete[] vertInd_;
	numVertices_ = 3 * numTri;
	vertices_    = new Vertex[numVertices_];
	vertInd_     = new unsigned[numVertices_];

	std::list<bbk::xmlElement*> triInputList = pTriNode->GetChildElemListByTag("input");
	std::list<bbk::xmlElement*>::const_iterator it     = triInputList.begin();
	std::list<bbk::xmlElement*>::const_iterator it_end = triInputList.end();
	unsigned stride = 1, pos_offset = 0, tc_offset = 0, nrm_offset = 0;

	for (size_t posSemLen = sizeof ("VERTEX"), tcSemLen = sizeof("TEXCOORD"), nrmSemLen = sizeof("NORMAL"); it != it_end; ++it)
	{
		if ((*it)->GetAttrib("semantic")->GetValue_str().compare(0, posSemLen, "VERTEX") == 0)
			pos_offset = (*it)->GetAttrib("offset")->GetValue_int();
		else if ((*it)->GetAttrib("semantic")->GetValue_str().compare(0, tcSemLen, "TEXCOORD") == 0)
		{
			tc_offset = (*it)->GetAttrib("offset")->GetValue_int();
			++stride;
		}
		else if ((*it)->GetAttrib("semantic")->GetValue_str().compare(0, nrmSemLen, "NORMAL") == 0)
		{
			nrm_offset = (*it)->GetAttrib("offset")->GetValue_int();
			++stride;
		}
	}

	bbk::Vector3 barycenter(0.0f, 0.0f, 0.0f);
	// Tokenise long string of ints into individual ints in string format
	std::vector<std::string> index_array(bbk::utils::SplitStrToWords(pTriNode->GetChildElem("p")->GetContent_str(), ' '));
	for (size_t i = 0; i < numVertices_; ++i)
	{
		vertInd_[i]      = i;
		vertices_[i].pos = positions[std::atoi(index_array[stride * i + pos_offset].c_str())];
		vertices_[i].clr = Colour(0.0f, 0.0f, 1.0f, 1.0f);
		if (hasTexCoords_)
			vertices_[i].tc = texcoords[std::atoi(index_array[stride * i + tc_offset].c_str())];
		if (hasNormals_)
			vertices_[i].nrm = normals[std::atoi(index_array[stride * i + nrm_offset].c_str())];

		barycenter += vertices_[i].pos;
	}

	// Compute barycenter and set pivot at barycenter
	barycenter /= static_cast<float>(numVertices_);
	if (barycenter.x < 0.000001f)
		barycenter.x = 0.0f;
	if (barycenter.y < 0.000001f)
		barycenter.y = 0.0f;
	if (barycenter.z < 0.000001f)
		barycenter.z = 0.0f;

	// Bounding volumes vars
	bbk::Vector3 min(0.0f, 0.0f, 0.0f);
	bbk::Vector3 max(0.0f, 0.0f, 0.0f);
	bbk::Vector3 mean(0.0f, 0.0f, 0.0f);
	for (size_t i = 0; i < numVertices_; ++i)
	{
		// Set vertex relative to barycenter as origin
		vertices_[i].pos -= barycenter;
		
		if (vertices_[i].pos.x < min.x)
			min.x = vertices_[i].pos.x;
		else if (vertices_[i].pos.x > max.x)
			max.x = vertices_[i].pos.x;
		if (vertices_[i].pos.y < min.y)
			min.y = vertices_[i].pos.y;
		else if (vertices_[i].pos.y > max.y)
			max.y = vertices_[i].pos.y;
		if (vertices_[i].pos.z < min.z)
			min.z = vertices_[i].pos.z;
		else if (vertices_[i].pos.z > max.z)
			max.z = vertices_[i].pos.z;

		mean += vertices_[i].pos;
	}

	aabb_.center = (min + max) * 0.5f;
	aabb_.diag   = (max - min) * 0.5f;
	mean /= static_cast<float>(numVertices_);

	// Compute covariance matrix
	bbk::Matrix3x3 cov;
	{
		float xx = 0.0f, yy = 0.0f, zz = 0.0f, xy = 0.0f, xz = 0.0f, yz = 0.0f;

		for (size_t i = 0; i < numVertices_; ++i)
		{
			xx += (vertices_[i].pos.x - mean.x) * (vertices_[i].pos.x - mean.x);
			yy += (vertices_[i].pos.y - mean.y) * (vertices_[i].pos.y - mean.y);
			zz += (vertices_[i].pos.z - mean.z) * (vertices_[i].pos.z - mean.z);
			xy += (vertices_[i].pos.x - mean.x) * (vertices_[i].pos.y - mean.y);
			xz += (vertices_[i].pos.x - mean.x) * (vertices_[i].pos.z - mean.z);
			yz += (vertices_[i].pos.y - mean.y) * (vertices_[i].pos.z - mean.z);
		}

		xx /= static_cast<float>(numVertices_);
		yy /= static_cast<float>(numVertices_);
		zz /= static_cast<float>(numVertices_);
		xy /= static_cast<float>(numVertices_);
		xz /= static_cast<float>(numVertices_);
		yz /= static_cast<float>(numVertices_);

		cov.elements[0] = xx; cov.elements[3] = xy; cov.elements[6] = xz;
		cov.elements[1] = xy; cov.elements[4] = yy; cov.elements[7] = yz;
		cov.elements[2] = xz; cov.elements[5] = yz; cov.elements[8] = zz;
	}

	bbk::Tuple<bbk::Vector3, 3> evecs = ::GenerateEigenValues(cov);

	obb_.u = evecs[0];
	obb_.v = evecs[1];
	obb_.w = evecs[2];

	// Compute extents of OBB
	{
		bbk::Tuple<float, 2> u_extents;
		bbk::Tuple<float, 2> v_extents;
		bbk::Tuple<float, 2> w_extents;
		
		u_extents[0] = u_extents[1] = vertices_[0].pos.Dot(obb_.u);
		v_extents[0] = v_extents[1] = vertices_[0].pos.Dot(obb_.v);
		w_extents[0] = w_extents[1] = vertices_[0].pos.Dot(obb_.w);
		
		for (size_t i = 1; i < numVertices_; ++i)
		{
			float proj_u = vertices_[i].pos.Dot(obb_.u);
			if (proj_u > u_extents[1])
				u_extents[1] = proj_u;
			else if (proj_u < u_extents[0])
				u_extents[0] = proj_u;

			float proj_v = vertices_[i].pos.Dot(obb_.v);
			if (proj_v > v_extents[1])
				v_extents[1] = proj_v;
			else if (proj_v < v_extents[0])
				v_extents[0] = proj_v;

			float proj_w = vertices_[i].pos.Dot(obb_.w);
			if (proj_w > w_extents[1])
				w_extents[1] = proj_w;
			else if (proj_w < w_extents[0])
				w_extents[0] = proj_w;
		}

		obb_.halfExtents.x = (u_extents[1] - u_extents[0]) * 0.5f;
		obb_.halfExtents.y = (v_extents[1] - v_extents[0]) * 0.5f;
		obb_.halfExtents.z = (w_extents[1] - w_extents[0]) * 0.5f;

		obb_.center.x = (u_extents[1] + u_extents[0]) * 0.5f;
		obb_.center.y = (v_extents[1] + v_extents[0]) * 0.5f;
		obb_.center.z = (w_extents[1] + w_extents[0]) * 0.5f;
	}
	
	delete[] positions;
	if (texcoords)
		delete[] texcoords;
	if (normals)
		delete[] normals;
	delete pRoot;
	
	return true;
}
} // namespace bbk

namespace
{
const float INV3  = 1.0f / 3.0f;
const float ROOT3 = std::sqrt(3.0f);

bbk::Tuple<float, 3> ComputeRoots(const bbk::Matrix3x3 &mtx)
{
	const float &e00 = mtx.elements[0];
	const float &e11 = mtx.elements[4];
	const float &e22 = mtx.elements[8];
	const float &e01 = mtx.elements[3];
	const float &e02 = mtx.elements[6];
	const float &e12 = mtx.elements[7];

	float c0 = e00*e11*e22 + 2.0f*e01*e02*e12 - e00*e12*e12 - e11*e02*e02 - e22*e01*e01;
	float c1 = e00*e11 - e01*e01 + e00*e22 - e02*e02 + e11*e22 - e12*e12;
	float c2 = e00 + e11 + e22;

	float c2div3 = c2 * INV3;
	float adiv3  = (c1 - c2*c2div3) * INV3;

	if (adiv3 > 0.0f)
		adiv3 = 0.0f;

	float mbdiv2 = 0.5f * (c0 + c2div3 * (2.0f * c2div3 * c2div3 - c1));

	float q = mbdiv2 * mbdiv2 + adiv3 * adiv3 * adiv3;

	if (q > 0.0f)
		q = 0.0f;

	float mag    = std::sqrt(-adiv3);
	float angle  = std::atan2(std::sqrt(-q), mbdiv2) * INV3;
	float cosine = std::cos(angle);
	float sine   = std::sin(angle);

	float min = c2div3 + 2.0f * mag * cosine;
	float mid = c2div3 - mag * (cosine + ROOT3 * sine);
	float max = c2div3 - mag * (cosine - ROOT3 * sine);

	if (min > mid)
		bbk::utils::swap(min, mid);
	if (min > max)
		bbk::utils::swap(min, max);
	if (min > max)
		bbk::utils::swap(mid, max);

	bbk::Tuple<float, 3> result;
	result[0] = min;
	result[1] = mid;
	result[2] = max;

	return result;
}

void GetComplement2(const bbk::Vector3 &u, bbk::Vector3 &v, bbk::Vector3 &w)
{
	bbk::Vector3 src = u.Normalise();

	if (std::fabs(src.x) >= std::fabs(src.y))
	{
		float invlen = 1.0f / std::sqrt(src.x * src.x + src.z * src.z);
		v.x = -src.z * invlen;
		v.y = 0.0f;
		v.z = src.x * invlen;
		w.x = u.y * v.z;
		w.y = u.z * v.x - u.x * v.z;
		w.z = -u.y * v.x;
	}
	else
	{
		float invlen = 1.0f / std::sqrt(src.y * src.y + src.z * src.z);
		v.x = 0.0f;
		v.y = src.z * invlen;
		v.z = -src.y * invlen;
		w.x = src.y * v.z - src.z * v.y;
		w.y = -src.x * v.z;
		w.z = src.x * v.y;
	}
}

int ComputeRank(bbk::Matrix3x3 &mtx)
{
	// Compute max mag matrix entry
	float abs, save, max = -1;
	int row, col, maxRow = -1, maxCol = -1;

	for (row = 0; row < 3; ++row)
	{
		for (col = 0; col < 3; ++col)
		{
			abs = std::fabs(mtx.m[row][col]);
			if (abs > max)
			{
				max = abs;
				maxRow = row;
				maxCol = col;
			}
		}
	}
	if (max < bbk::EPSILONf)
		return 0;

	// Swap 
	if (maxRow != 0)
	{
		for (col = 0; col < 3; ++col)
		{
			save = mtx.m[0][col];
			mtx.m[0][col] = mtx.m[maxRow][col];
			mtx.m[maxRow][col] = save;
		}
	}

	// Row reduce
	float invmax = 1.0f / mtx.m[0][maxCol];
	mtx.m[0][0] *= invmax;
	mtx.m[0][1] *= invmax;
	mtx.m[0][2] *= invmax;

	if (maxCol == 0)
	{
		mtx.m[1][1] -= mtx.m[1][0] * mtx.m[0][1];
		mtx.m[1][2] -= mtx.m[1][0] * mtx.m[0][2];
		mtx.m[2][1] -= mtx.m[2][0] * mtx.m[0][1];
		mtx.m[2][2] -= mtx.m[2][0] * mtx.m[0][2];
		mtx.m[1][0] = 0.0f;
		mtx.m[2][0] = 0.0f;
	}
	else if (maxCol == 1)
	{
		mtx.m[1][0] -= mtx.m[1][1] * mtx.m[0][0];
		mtx.m[1][2] -= mtx.m[1][1] * mtx.m[0][2];
		mtx.m[2][0] -= mtx.m[2][1] * mtx.m[0][0];
		mtx.m[2][2] -= mtx.m[2][1] * mtx.m[0][2];
		mtx.m[1][1] = 0.0f;
		mtx.m[2][1] = 0.0f;
	}
	else
	{
		mtx.m[1][0] -= mtx.m[1][2] * mtx.m[0][0];
		mtx.m[1][1] -= mtx.m[1][2] * mtx.m[0][1];
		mtx.m[2][0] -= mtx.m[2][2] * mtx.m[0][0];
		mtx.m[2][1] -= mtx.m[2][2] * mtx.m[0][1];
		mtx.m[1][2] = 0.0f;
		mtx.m[2][2] = 0.0f;
	}

	max = -1;
	maxRow = -1;
	maxCol = -1;
	for (row = 1; row < 3; ++row)
	{
		for (col = 0; col < 3; ++col)
		{
			abs = std::fabs(mtx.m[row][col]);
			if (abs > max)
			{
				max = abs;
				maxRow = row;
				maxCol = col;
			}
		}
	}

	if (max < bbk::EPSILONf)
		return 1;

	if (maxRow == 2)
	{
		for (col = 0; col < 3; ++col)
		{
			save = mtx.m[1][col];
			mtx.m[1][col] = mtx.m[2][col];
			mtx.m[2][col] = save;
		}
	}

	invmax = 1.0f / mtx.m[1][maxCol];
	mtx.m[1][0] *= invmax;
	mtx.m[1][1] *= invmax;
	mtx.m[1][2] *= invmax;

	return 2;
}

void EigenSolver(const bbk::Matrix3x3 &mtx, float evalues[3], bbk::Vector3 evectors[3])
{
	bbk::Tuple<float, 3> roots = ::ComputeRoots(mtx);
	evalues[0] = roots[0];
	evalues[1] = roots[1];
	evalues[2] = roots[2];

	bbk::Matrix3x3 m0, m1, m2;
	int rank0, rank1, rank2;
	bbk::Vector3 row0, row1;

	m0 = mtx - (bbk::Matrix3x3::IDENTITY * evalues[0]);
	rank0 = ComputeRank(m0);
	if (rank0 == 0)
	{
		m1 = mtx - (bbk::Matrix3x3::IDENTITY * evalues[1]);
		m2 = mtx - (bbk::Matrix3x3::IDENTITY * evalues[2]);
		rank1 = ComputeRank(m1);
		rank2 = ComputeRank(m2);
		
		evectors[0] = bbk::Vector3(1.0f, 0.0f, 0.0f);
		evectors[1] = bbk::Vector3(0.0f, 1.0f, 0.0f);
		evectors[2] = bbk::Vector3(0.0f, 0.0f, 1.0f);
		return;
	}

	if (rank0 == 1)
	{
		m1 = mtx - (bbk::Matrix3x3::IDENTITY * evalues[1]);
		m2 = mtx - (bbk::Matrix3x3::IDENTITY * evalues[2]);
		rank1 = ComputeRank(m1);
		rank2 = ComputeRank(m2);

		row0 = m0.GetRow(0);
		row0.NormaliseThis();

		GetComplement2(row0, evectors[0], evectors[1]);
		evectors[2] = evectors[0].Cross(evectors[1]);
		return;
	}

	// rank0 == 2
	row0 = m0.GetRow(0);
	row1 = m0.GetRow(1);
	row0.NormaliseThis();
	row1.NormaliseThis();
	evectors[0] = GetComplement1(row0, row1);

	m1 = mtx - (bbk::Matrix3x3::IDENTITY * evalues[1]);
	rank1 = ComputeRank(m1);
	if (rank1 == 1)
	{
		m2 = mtx - (bbk::Matrix3x3::IDENTITY * evalues[2]);
		rank2 = ComputeRank(m2);
		GetComplement2(evectors[0], evectors[1], evectors[2]);
		return;
	}

	// rank1 == 2
	row0 = m1.GetRow(0);
	row1 = m1.GetRow(1);
	row0.NormaliseThis();
	row1.NormaliseThis();
	evectors[1] = GetComplement1(row0, row1);

	// rank 2 == 2
	m2 = mtx - (bbk::Matrix3x3::IDENTITY * evalues[2]);
	rank2 = ComputeRank(m2);
	evectors[2] = evectors[0].Cross(evectors[1]);
}

bbk::Tuple<bbk::Vector3, 3> GenerateEigenValues(const bbk::Matrix3x3 &mtx)
{
	///cubic solving
	const float *matrix = mtx.elements;

	double eps1 = 1e-10, eps2 = 1e-10, eps3 = 1e-5;
	double aik[3];
	double q = 0.0, p = 0.0, spq = 0.0, csa = 0.0, sna = 0.0, temp = 0.0, s1 = 0.0, s2 = 0.0;
	bool flag = true;

	double a[3][3] = {0.0};
	double t[3][3] = {0.0};

	a[0][0] = matrix[0];
	a[0][1] = matrix[1];
	a[0][2] = matrix[2];

	a[1][0] = matrix[4];
	a[1][1] = matrix[5];
	a[1][2] = matrix[6];

	a[2][0] = matrix[8];
	a[2][1] = matrix[9];
	a[2][2] = matrix[10];

	double sum = 0.0;

	for(int i = 0; i < 3; ++i)
	{
		t[i][i] = 1.0;
		s1 += a[i][i] * a[i][i];
		sum += a[i][i];
	}

	int iteration = 0;
	do
	{
		++iteration;

		for(int i = 0; i < 2; ++i)
			for(int j = (i + 1); j < 3; ++j)
			{
				if(fabs(a[i][j]) < eps1)
				{
					a[i][j] = 0.0;
				}
				else
				{
					q = fabs(a[i][i] - a[j][j]);

					if( q > eps2)
					{
						p = 2 * a[i][j] * q / (a[i][i] - a[j][j]);
						spq = sqrt( p*p + q*q );
						csa = sqrt((1.0 + q / spq) /2.0);
						sna = p / (2.0 * csa * spq);
					}
					else
					{
						csa = 0.707107;
						sna = csa;
					}

					for(int k = 0; k < 3; ++k)
					{
						temp = t[k][i];
						t[k][i] = temp * csa + t[k][j] * sna;
						t[k][j] = temp * sna - t[k][j] * csa;
					}
					for(int k = i; k < 3; ++k)
					{
						if(k > j)
						{
							temp = a[i][k];
							a[i][k] = csa * temp + sna * a[j][k];
							a[j][k] = sna * temp - csa * a[j][k];
						}
						else
						{
							aik[k] = a[i][k];
							a[i][k] = csa * aik[k] + sna * a[k][j];

							if( k == j)
							{
								a[j][k] = sna * aik[k] - csa * a[j][k];
							}
						}
					}

					aik[j] = sna*aik[i] - csa*aik[j];

					for(int k = 0; k <= j; ++k)
					{
						if(k<=i)
						{
							temp = a[k][i];

							a[k][i] = csa * temp + sna* a[k][j];
							a[k][j] = sna * temp - csa * a[k][j];
						}
						else
						{
							a[k][j] = sna * aik[k] - csa*a[k][j];
						}
					}
				}
			}

			s2 = 0.0;

			for(int i = 0; i < 3; ++i)
			{
				eigen[i] = a[i][i];
				s2 +=  eigen[i] * eigen[i];
			}

			if(fabs( 1.0 - s1/s2) < eps3)
			{
				flag = 0;
			}
			else
			{
				s1 = s2;
			}


	}
	while(flag == 1);
	
	///eigen values received
	bbk::Vector3 vec1(
		(float)t[0][0],
		(float)t[1][0],
		(float)t[2][0]);
	
	bbk::Vector3 vec2(
		(float)t[0][1],
		(float)t[1][1],
		(float)t[2][1]);
	
	vec1.NormaliseThis();
	vec2.NormaliseThis();

	bbk::Tuple<bbk::Vector3, 3> result;
	
	result[0] = vec1.Normalise();
	result[1] = vec2;
	result[2] = vec1.Cross(vec2).Normalise();

	result[1] -= result[0].Dot(result[1]) * result[0];
	result[1].NormaliseThis();

	result[2] -= result[0].Dot(result[2]) * result[0];
	result[2].NormaliseThis();

	return result;
}
} // anon namespace
