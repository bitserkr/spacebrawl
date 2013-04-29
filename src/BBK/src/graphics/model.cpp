#include <vector>
#include "model.h"
#include "fileio/fileio.h"
#include "utils.h"

namespace bbk
{
Model::Model() :
	numVertices_(0),
	vertices_(nullptr),
	numIndices_(0),
	indices_(nullptr),
	hasTexCoords_(false),
	hasNormals_(false),
	bvhroot_(nullptr)
{}

Model::~Model()
{
	if (vertices_)
		delete[] vertices_;
	if (indices_)
		delete[] indices_;
	if (bvhroot_)
		delete bvhroot_;
}

bool Model::LoadGeometryFromFile(const char *filename)
{
	xmlElement *pRoot = bbk::fileio::ReadFile(filename);
	if (!pRoot) return false;

	modelName_ = pRoot->GetAttrib("name")->GetValue_str();

	// Mesh
	{
		xmlElement *mesh = pRoot->GetChildElem("Geometry");
		numVertices_ = mesh->GetAttrib("numVertices")->GetValue_int();
		numIndices_  = mesh->GetAttrib("numIndices")->GetValue_int();

		if (vertices_) delete[] vertices_;
		vertices_ = new bbk::Vertex[numVertices_];
		if (indices_) delete[] indices_;
		indices_ = new unsigned[numIndices_];

		// Positions
		xmlElement *pos = mesh->GetChildElem("Positions");
		{
			std::vector<std::string> posvec(3 * numVertices_);
			posvec = bbk::utils::SplitStrToWords(pos->GetContent_str(), ' ');

			for (size_t i = 0; i < numVertices_; ++i)
			{
				vertices_[i].pos.x = static_cast<float>(std::atof(posvec[3 * i].c_str()));
				vertices_[i].pos.y = static_cast<float>(std::atof(posvec[3 * i + 1].c_str()));
				vertices_[i].pos.z = static_cast<float>(std::atof(posvec[3 * i + 2].c_str()));
				indices_[i] = i;
			}
		}

		// Texture coords
		xmlElement *tc = mesh->GetChildElem("Texture_coords");
		if (tc)
		{
			hasTexCoords_ = true;

			std::vector<std::string> tcvec(2 * numVertices_);
			tcvec = bbk::utils::SplitStrToWords(tc->GetContent_str(), ' ');

			for (size_t i = 0; i < numVertices_; ++i)
			{
				vertices_[i].tc[0] = static_cast<float>(std::atof(tcvec[2 * i].c_str()));
				vertices_[i].tc[1] = static_cast<float>(std::atof(tcvec[2 * i + 1].c_str()));
			}
		}

		// Normals
		xmlElement *nrm = mesh->GetChildElem("Normals");
		if (nrm)
		{
			hasNormals_ = true;

			std::vector<std::string> nrmvec(3 * numVertices_);
			nrmvec = bbk::utils::SplitStrToWords(nrm->GetContent_str(), ' ');

			for (size_t i = 0; i < numVertices_; ++i)
			{
				vertices_[i].nrm.x = static_cast<float>(std::atof(nrmvec[3 * i].c_str()));
				vertices_[i].nrm.y = static_cast<float>(std::atof(nrmvec[3 * i + 1].c_str()));
				vertices_[i].nrm.z = static_cast<float>(std::atof(nrmvec[3 * i + 2].c_str()));
			}
		}

		// Indices
		xmlElement *ind = mesh->GetChildElem("Indices");
		if (ind)
		{
			std::vector<std::string> indvec(numIndices_);
			indvec = bbk::utils::SplitStrToWords(ind->GetContent_str(), ' ');
			for (size_t i = 0; i < numIndices_; ++i)
			{
				indices_[i] = static_cast<unsigned>(std::atoi(indvec[i].c_str()));
			}
		}
	}

	// Bounding volumes
	{
		xmlElement *bv = pRoot->GetChildElem("BoundingVolumes");
		// Bounding Sphere
		{
			xmlElement *sphere = bv->GetChildElem("BSphere");
			xmlElement *center = sphere->GetChildElem("center");
			bsphere_.center.x = center->GetAttrib("x")->GetValue_float();
			bsphere_.center.y = center->GetAttrib("y")->GetValue_float();
			bsphere_.center.z = center->GetAttrib("z")->GetValue_float();
			bsphere_.radius   = sphere->GetChildElem("radius")->GetAttrib("radius")->GetValue_float();
			bsphereOffset_ = bsphere_.center;
			bsphere_.center = Vector3();
		}
		// AABB
		{
			xmlElement *aabb = bv->GetChildElem("AABB");
			xmlElement *center = aabb->GetChildElem("center");
			aabb_.center.x = center->GetAttrib("x")->GetValue_float();
			aabb_.center.y = center->GetAttrib("y")->GetValue_float();
			aabb_.center.z = center->GetAttrib("z")->GetValue_float();
			xmlElement *extents = aabb->GetChildElem("halfextents");
			aabb_.diag.x = extents->GetAttrib("x")->GetValue_float();
			aabb_.diag.y = extents->GetAttrib("y")->GetValue_float();
			aabb_.diag.z = extents->GetAttrib("z")->GetValue_float();
			aabbOffset_ = aabb_.center;
			aabb_.center = Vector3();
		}
		// OBB
		{
			xmlElement *obb = bv->GetChildElem("OBB");
			xmlElement *u = obb->GetChildElem("u");
			obb_.u.x = u->GetAttrib("x")->GetValue_float();
			obb_.u.y = u->GetAttrib("y")->GetValue_float();
			obb_.u.z = u->GetAttrib("z")->GetValue_float();

			xmlElement *v = obb->GetChildElem("v");
			obb_.v.x = v->GetAttrib("x")->GetValue_float();
			obb_.v.y = v->GetAttrib("y")->GetValue_float();
			obb_.v.z = v->GetAttrib("z")->GetValue_float();

			xmlElement *w = obb->GetChildElem("w");
			obb_.w.x = w->GetAttrib("x")->GetValue_float();
			obb_.w.y = w->GetAttrib("y")->GetValue_float();
			obb_.w.z = w->GetAttrib("z")->GetValue_float();

			xmlElement *offset = obb->GetChildElem("center");
			obbOffset_.x = offset->GetAttrib("x")->GetValue_float();
			obbOffset_.y = offset->GetAttrib("y")->GetValue_float();
			obbOffset_.z = offset->GetAttrib("z")->GetValue_float();
			
			xmlElement *extents = obb->GetChildElem("halfextents");
			obb_.halfExtents.x = extents->GetAttrib("x")->GetValue_float();
			obb_.halfExtents.y = extents->GetAttrib("y")->GetValue_float();
			obb_.halfExtents.z = extents->GetAttrib("z")->GetValue_float();
			
			obb_.center = Vector3();
		}
	}

	// Rigid Body Dynamics
	{
		xmlElement *massInfo = pRoot->GetChildElem("MassInfo");
		if (massInfo)
		{
			mass_ = massInfo->GetAttrib("mass")->GetValue_float();
		
			xmlElement *com = massInfo->GetChildElem("CenterOfMass");
			centerMass_.x = com->GetAttrib("x")->GetValue_float();
			centerMass_.y = com->GetAttrib("y")->GetValue_float();
			centerMass_.z = com->GetAttrib("z")->GetValue_float();

			xmlElement *it = massInfo->GetChildElem("InertiaTensor");
			inertiaTensor_.elements[0] = it->GetAttrib("a")->GetValue_float();
			inertiaTensor_.elements[1] = it->GetAttrib("b")->GetValue_float();
			inertiaTensor_.elements[2] = it->GetAttrib("c")->GetValue_float();
			inertiaTensor_.elements[3] = it->GetAttrib("d")->GetValue_float();
			inertiaTensor_.elements[4] = it->GetAttrib("e")->GetValue_float();
			inertiaTensor_.elements[5] = it->GetAttrib("f")->GetValue_float();
			inertiaTensor_.elements[6] = it->GetAttrib("g")->GetValue_float();
			inertiaTensor_.elements[7] = it->GetAttrib("h")->GetValue_float();
			inertiaTensor_.elements[8] = it->GetAttrib("i")->GetValue_float();
		}
	}

	// Create array of points for BVH construction
	{
		Vector3* verts = new Vector3[numIndices_];
		for (size_t i = 0; i < numIndices_; ++i)
			verts[i] = vertices_[indices_[i]].pos;
		bvhroot_ = BuildBVH(verts, numIndices_);
		delete[] verts;
	}

	delete pRoot;
	return true;
}
} // namespace bbk
