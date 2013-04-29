#include <vector>
#include "opengl/glew.h"
#include "opengl/wglew.h"
#include "graphics.h"

namespace
{
struct PointRenderContext
{
	PointRenderContext(const bbk::Point3 &position, const bbk::Colour &color) :
		pos(position), clr(color) {}
	
	bbk::Point3 pos;
	bbk::Colour clr;
}; // struct PointRenderContext

struct ModelRenderContext
{
	ModelRenderContext(bbk::Model *pModel, bool useTextures = true, const bbk::Colour& clr = bbk::Colour()) :
		model(pModel), bUseTextures(useTextures), surfaceClr(clr) {}

	bbk::Model* model;
	bool        bUseTextures;
	bbk::Colour surfaceClr;
}; // struct ModelRenderContext

struct TransformDrawRange
{
	TransformDrawRange(const bbk::Matrix4x4 &mtx) : transform(mtx), numToRender(0) {}

	bbk::Matrix4x4        transform;
	unsigned              numToRender;
	std::vector<unsigned> pointIndices;
	std::vector<unsigned> lineIndices;
	std::vector<unsigned> triIndices;
	std::vector<unsigned> meshIndices;
	std::vector<unsigned> shapeIndices;
}; // struct TransformDrawRange

struct TextRenderContext
{
	TextRenderContext(const std::string &str, int xCoord = 0, int yCoord = 0) : text(str), x(xCoord), y(yCoord) {}

	std::string text;
	int x, y;
}; // struct TextRenderContext

/** @name
 *  Geometry storage *///\{
bbk::Model* shapeMeshes[bbk::gfx::NUM_SHAPES] = {nullptr};
//std::vector<bbk::Mesh*> meshes;
//\}

/** @name
 *  Camera *///\{
//bbk::Camera* pCurrentCam = nullptr;
//bbk::Camera* pCullingCam = nullptr;
//\}

/** @name
 *  Geometry to render *///\{
std::vector<PointRenderContext>    points; ///< Pixels to draw in current frame
std::vector<PointRenderContext>    lines;  ///< Lines to draw in current frame
std::vector<PointRenderContext>    tris;
std::vector<ModelRenderContext>    models; ///< Models to draw in current frame
std::vector<ModelRenderContext>    shapes;
//\}
std::vector<TextRenderContext>     strings;

/** @name
 *  Matrix stacks *///\{
std::vector<bbk::Matrix4x4>     cl_MV_mtxStack;
std::vector<TransformDrawRange> transformRanges;
std::vector<unsigned>           lastPushIndices;
unsigned                        currTransformInd;
//\}

/** @name
 *  Projection and view vars *///\{
float nearDist, farDist;
bbk::Matrix4x4 currPerspProjMtx;
bbk::Matrix4x4 currWorldViewMtx;
//\}

/** @name
 *  Fonts *///\{
bbk::Texture font;
float charQuad[] = {
	/*    x,       y,    z,    u,    v */
	-0.024f, -0.032f, 0.0f, 0.0f, 1.0f,
	 0.024f, -0.032f, 0.0f, 1.0f, 1.0f,
	 0.024f,  0.032f, 0.0f, 1.0f, 0.0f,
	-0.024f,  0.032f, 0.0f, 0.0f, 0.0f};
//\}

bbk::Tuple<float, 4> clearcolor;

/** @name
 *  View Frustum Culling *///\{
bbk::Frustum cullingFrustum;
bbk::Vector4 r0, r1, r2, r3;

inline int BSvsLeftPlane(const bbk::BSphere& bsphere);
inline int BSvsRightPlane(const bbk::BSphere& bsphere);
inline int BSvsBottomPlane(const bbk::BSphere& bsphere);
inline int BSvsTopPlane(const bbk::BSphere& bsphere);
inline int BSvsNearPlane(const bbk::BSphere& bsphere);
inline int BSvsFarPlane(const bbk::BSphere& bsphere);

inline int AABBvsLeftPlane(const bbk::AABB& aabb);
inline int AABBvsRightPlane(const bbk::AABB& aabb);
inline int AABBvsBottomPlane(const bbk::AABB& aabb);
inline int AABBvsTopPlane(const bbk::AABB& aabb);
inline int AABBvsNearPlane(const bbk::AABB& aabb);
inline int AABBvsFarPlane(const bbk::AABB& aabb);

inline int OBBvsLeftPlane(const bbk::OBB& obb);
inline int OBBvsRightPlane(const bbk::OBB& obb);
inline int OBBvsBottomPlane(const bbk::OBB& obb);
inline int OBBvsTopPlane(const bbk::OBB& obb);
inline int OBBvsNearPlane(const bbk::OBB& obb);
inline int OBBvsFarPlane(const bbk::OBB& obb);

int (*bsTestFuncs[6])(const bbk::BSphere&) = {BSvsLeftPlane, BSvsRightPlane, BSvsNearPlane, BSvsFarPlane, BSvsBottomPlane, BSvsTopPlane};
int (*aabbTestFuncs[6])(const bbk::AABB&)  = {AABBvsLeftPlane, AABBvsRightPlane, AABBvsNearPlane, AABBvsFarPlane, AABBvsBottomPlane, AABBvsTopPlane};
int (*obbTestFuncs[6])(const bbk::OBB&)  = {OBBvsLeftPlane, OBBvsRightPlane, OBBvsNearPlane, OBBvsFarPlane, OBBvsBottomPlane, OBBvsTopPlane};

void BSphereVFC(bbk::RenderContext& rc);
void AABBVFC(bbk::RenderContext& rc);
void OBBVFC(bbk::RenderContext& rc);
void BSphereVFC_PC(bbk::RenderContext& rc);
void AABBVFC_PC(bbk::RenderContext& rc);
void OBBVFC_PC(bbk::RenderContext& rc);
void (*vfcScheme)(bbk::RenderContext&) = {OBBVFC_PC};

bbk::gfx::VFCScheme vfcBV_type= bbk::gfx::E_OBB;
bool bPlaneCoherency = true;
bool bDrawBV = false;
//\}

/** @name
 *  Debug info *///\{
bool     bPrintDebugInfo = false;
std::vector<std::string> debugStr;
unsigned                 numVertsToGPU = 0;
unsigned numObjsRequested = 0;
unsigned numObjsRendered = 0;
unsigned numObjsInside = 0;
unsigned numPlaneTests = 0;
//\}

bool vsyncOn = true;
} // anon namespace

namespace bbk
{
namespace gfx
{
LocationMgr locationMgrs[8];
ShaderProg  shaderProgs[8];

bool Init()
{
	::clearcolor[0] = ::clearcolor[1] = ::clearcolor[2] = ::clearcolor[3] = 0.0f;
	::cl_MV_mtxStack.push_back(bbk::Matrix4x4::IDENTITY);
	::transformRanges.push_back(TransformDrawRange(bbk::Matrix4x4::IDENTITY));

	// Load meshes of shapes
	{
		::shapeMeshes[gfx::E_PLANE] = new bbk::Model;
		::shapeMeshes[gfx::E_PLANE]->LoadGeometryFromFile("Plane.xml");
		::shapeMeshes[gfx::E_CUBE] = new bbk::Model;
		::shapeMeshes[gfx::E_CUBE]->LoadGeometryFromFile("Cube.xml");
		::shapeMeshes[gfx::E_SPHERE] = new bbk::Model;
		::shapeMeshes[gfx::E_SPHERE]->LoadGeometryFromFile("Sphere.xml");
	}

	return true;
}

bool InitGL()
{
	if (glewInit() != GLEW_OK)
		return false;

	glEnable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	// Init font
	::font.SetTextureName("Default font");
	::font.LoadTexDataFromFile("Textures/font.png");
	::font.LoadTexDataToGPU();
	::font.FreeTexData();

	EnableVSync(::vsyncOn);

	LoadShaders();

	return true;
}

void LoadShaders()
{
	/*--------------------------------------------------------------------------
	 * Load shader program
	 */
	bbk::gfx::shaderProgs[0].AllocHandle();
	bbk::gfx::shaderProgs[0].AttachShader(bbk::ShaderObj::VTX_SHADER, "phong.vs");
	bbk::gfx::shaderProgs[0].AttachShader(bbk::ShaderObj::FRAG_SHADER, "phong.fs");
	bbk::gfx::shaderProgs[0].Link();
	bbk::gfx::shaderProgs[0].Use();

	// Link location manager to loaded shader prog
	bbk::gfx::locationMgrs[0].SetShaderProg(bbk::gfx::shaderProgs[0].GetGLHandle());

	/*--------------------------------------------------------------------------
	 * Register shader variables with location manager and set their values
	 */
	char buffer[64] = {0};
	std::sprintf(buffer, "lights[0].type");
	bbk::gfx::locationMgrs[0].AddUniformLocation(buffer);

	std::sprintf(buffer, "lights[0].position");
	bbk::gfx::locationMgrs[0].AddUniformLocation(buffer);

	std::sprintf(buffer, "lights[0].direction");
	bbk::gfx::locationMgrs[0].AddUniformLocation(buffer);

	std::sprintf(buffer, "lights[0].I_ambient");
	bbk::gfx::locationMgrs[0].AddUniformLocation(buffer);

	std::sprintf(buffer, "lights[0].I_diffuse");
	bbk::gfx::locationMgrs[0].AddUniformLocation(buffer);

	std::sprintf(buffer, "lights[0].I_specular");
	bbk::gfx::locationMgrs[0].AddUniformLocation(buffer);

	std::sprintf(buffer, "lights[0].spotAttCoeff");
	bbk::gfx::locationMgrs[0].AddUniformLocation(buffer);

	std::sprintf(buffer, "lights[0].distAttCoeff");
	bbk::gfx::locationMgrs[0].AddUniformLocation(buffer);

	bbk::gfx::locationMgrs[0].AddUniformLocation("I_globAmbient");

	// Fog parameters
	bbk::gfx::locationMgrs[0].AddUniformLocation("fog.color");
	bbk::gfx::locationMgrs[0].AddUniformLocation("fog.nearDist");
	bbk::gfx::locationMgrs[0].AddUniformLocation("fog.farDist");
	
	//glUniform1i(bbk::gfx::locationMgrs[0].AddUniformLocation("constLightType_Point"), 0);
	bbk::gfx::locationMgrs[0].AddUniformLocation("constLightType_Dir");
	bbk::gfx::locationMgrs[0].AddUniformLocation("constLightType_Spot");
	bbk::gfx::locationMgrs[0].AddUniformLocation("numLightSrc");
	
	bbk::gfx::locationMgrs[0].AddUniformLocation("isUseTextures");
	bbk::gfx::locationMgrs[0].AddUniformLocation("materialType");
	bbk::gfx::locationMgrs[0].AddUniformLocation("useVertexColor");

	bbk::gfx::locationMgrs[0].AddUniformLocation("surfaceClr.K_ambient");
	bbk::gfx::locationMgrs[0].AddUniformLocation("surfaceClr.K_diffuse");
	bbk::gfx::locationMgrs[0].AddUniformLocation("surfaceClr.K_specular");
	bbk::gfx::locationMgrs[0].AddUniformLocation("surfaceClr.K_emissive");

	bbk::gfx::locationMgrs[0].AddUniformLocation("TexAmbient");
	bbk::gfx::locationMgrs[0].AddUniformLocation("TexDiffuse");
	bbk::gfx::locationMgrs[0].AddUniformLocation("TexSpecular");
	bbk::gfx::locationMgrs[0].AddUniformLocation("TexEmissive");
}

void Halt()
{
	for (size_t i = 0; i < gfx::NUM_SHAPES; ++i)
	{
		if (::shapeMeshes[i])
			delete ::shapeMeshes[i];
	}
	::font.FreeTextureObj();
}

void EnableVSync(bool flag)
{
	wglSwapIntervalEXT(flag);
}

void PrintDebugInfo(bool flag)
{
	::bPrintDebugInfo = flag;
}

void Render()
{
	::numVertsToGPU = 0;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glLoadMatrixf(::currPerspProjMtx.elements);
	//glLoadMatrixf(::pCurrentCam->GetProjectionMtx().elements);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glLoadMatrixf(::currWorldViewMtx.elements);
	//glLoadMatrixf(::pCurrentCam->GetWorldToViewMtx().elements);

	for (size_t i = 0, size = ::transformRanges.size(); i < size; ++i)
	{
		if (::transformRanges[i].numToRender)
		{
			// Set transform for current batch
			glPushMatrix();
			glMultMatrixf(::transformRanges[i].transform.elements);

			glUniform1i(bbk::gfx::locationMgrs[0].GetUniformLocHandle("useVertexColor"), true);
		
			// Render points
			if (unsigned numPoints = ::transformRanges[i].pointIndices.size())
			{
				glVertexPointer(3, GL_FLOAT, sizeof(PointRenderContext), &(::points[0].pos));
				glColorPointer (4, GL_FLOAT, sizeof(PointRenderContext), &(::points[0].clr));

				glDrawElements(GL_POINTS, numPoints, GL_UNSIGNED_INT, &(::transformRanges[i].pointIndices[0]));

				//::numVertsToGPU += numPoints;
			}
			// Render lines
			if (unsigned numVtx = ::transformRanges[i].lineIndices.size())
			{
				glVertexPointer(3, GL_FLOAT, sizeof(PointRenderContext), &(::lines[0].pos));
				glColorPointer (4, GL_FLOAT, sizeof(PointRenderContext), &(::lines[0].clr));

				glDrawElements(GL_LINES, numVtx, GL_UNSIGNED_INT, &(::transformRanges[i].lineIndices[0]));

				//::numVertsToGPU += numVtx;
			}
			// Render triangles
			if (unsigned numVtx = ::transformRanges[i].triIndices.size())
			{
				glVertexPointer(3, GL_FLOAT, sizeof(PointRenderContext), &(::tris[0].pos));
				glColorPointer (4, GL_FLOAT, sizeof(PointRenderContext), &(::tris[0].clr));

				glDrawElements(GL_TRIANGLES, numVtx, GL_UNSIGNED_INT, &(::transformRanges[i].triIndices[0]));

				//::numVertsToGPU += numVtx;
			}

			glUniform1i(bbk::gfx::locationMgrs[0].GetUniformLocHandle("useVertexColor"), false);
			// Render models
			for (size_t j = 0, size = ::transformRanges[i].meshIndices.size(); j < size; ++j)
			{
				ModelRenderContext &currModel = ::models[::transformRanges[i].meshIndices[j]];

				glUniform1i(bbk::gfx::locationMgrs[0].GetUniformLocHandle("isUseTextures"), currModel.bUseTextures);
				if (!currModel.bUseTextures)
				{
					const Colour white(1.0f, 1.0f, 1.0f, 1.0f);
					const Colour black(0.0f, 0.0f, 0.0f, 0.0f);
					glUniform4fv(bbk::gfx::locationMgrs[0].GetUniformLocHandle("surfaceClr.K_ambient"), 1, &currModel.surfaceClr.r);
					glUniform4fv(bbk::gfx::locationMgrs[0].GetUniformLocHandle("surfaceClr.K_diffuse"), 1, &currModel.surfaceClr.r);
					glUniform4fv(bbk::gfx::locationMgrs[0].GetUniformLocHandle("surfaceClr.K_specular"), 1, &white.r);
					glUniform4fv(bbk::gfx::locationMgrs[0].GetUniformLocHandle("surfaceClr.K_emissive"), 1, &black.r);
				}

				if (currModel.model->hasTexCoords())
					glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				if (currModel.model->hasNormals())
					glEnableClientState(GL_NORMAL_ARRAY);

				glVertexPointer(3, GL_FLOAT, sizeof(bbk::Vertex), &(currModel.model->GetVertexArray()[0].pos));
				glColorPointer (4, GL_FLOAT, sizeof(bbk::Vertex), &(currModel.model->GetVertexArray()[0].clr));
				if (currModel.model->hasTexCoords())
					glTexCoordPointer(2, GL_FLOAT, sizeof(bbk::Vertex), &(currModel.model->GetVertexArray()[0].tc));
				if (currModel.model->hasNormals())
					glNormalPointer(GL_FLOAT, sizeof(bbk::Vertex), &(currModel.model->GetVertexArray()[0].nrm));

				glDrawElements(
					GL_TRIANGLES,
					currModel.model->GetNumIndices(),
					GL_UNSIGNED_INT,
					currModel.model->GetVertIndArray());

				if (currModel.model->hasTexCoords())
					glDisableClientState(GL_TEXTURE_COORD_ARRAY);
				if (currModel.model->hasNormals())
					glDisableClientState(GL_NORMAL_ARRAY);

				::numVertsToGPU += currModel.model->GetNumIndices();
			}
			// Render shapes
			for (size_t j = 0, size = ::transformRanges[i].shapeIndices.size(); j < size; ++j)
			{
				ModelRenderContext &currModel = ::shapes[::transformRanges[i].shapeIndices[j]];

				glUniform1i(bbk::gfx::locationMgrs[0].GetUniformLocHandle("isUseTextures"), false);
				{
					const Colour white(1.0f, 1.0f, 1.0f, 1.0f);
					const Colour black(0.0f, 0.0f, 0.0f, 0.0f);
					glUniform4fv(bbk::gfx::locationMgrs[0].GetUniformLocHandle("surfaceClr.K_ambient"), 1, &currModel.surfaceClr.r);
					glUniform4fv(bbk::gfx::locationMgrs[0].GetUniformLocHandle("surfaceClr.K_diffuse"), 1, &currModel.surfaceClr.r);
					glUniform4fv(bbk::gfx::locationMgrs[0].GetUniformLocHandle("surfaceClr.K_specular"), 1, &white.r);
					glUniform4fv(bbk::gfx::locationMgrs[0].GetUniformLocHandle("surfaceClr.K_emissive"), 1, &black.r);
				}

				if (currModel.model->hasTexCoords())
					glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				if (currModel.model->hasNormals())
					glEnableClientState(GL_NORMAL_ARRAY);

				glVertexPointer(3, GL_FLOAT, sizeof(bbk::Vertex), &(currModel.model->GetVertexArray()[0].pos));
				glColorPointer (4, GL_FLOAT, sizeof(bbk::Vertex), &(currModel.model->GetVertexArray()[0].clr));
				if (currModel.model->hasTexCoords())
					glTexCoordPointer(2, GL_FLOAT, sizeof(bbk::Vertex), &(currModel.model->GetVertexArray()[0].tc));
				if (currModel.model->hasNormals())
					glNormalPointer(GL_FLOAT, sizeof(bbk::Vertex), &(currModel.model->GetVertexArray()[0].nrm));

				glDrawElements(
					GL_TRIANGLES,
					currModel.model->GetNumIndices(),
					GL_UNSIGNED_INT,
					currModel.model->GetVertIndArray());

				if (currModel.model->hasTexCoords())
					glDisableClientState(GL_TEXTURE_COORD_ARRAY);
				if (currModel.model->hasNormals())
					glDisableClientState(GL_NORMAL_ARRAY);
			}

			glPopMatrix();
		}
	}
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	::points.clear();
	::lines.clear();
	::tris.clear();
	::models.clear();
	::shapes.clear();
	/*for (size_t i = 0, size = ::transformRanges.size(); i < size; ++i)
	{
		::transformRanges[i].pointIndices.clear();
		::transformRanges[i].lineIndices.clear();
		::transformRanges[i].meshIndices.clear();
		::transformRanges[i].shapeIndices.clear();
	}*/
	::transformRanges.clear();
	::transformRanges.push_back(TransformDrawRange(bbk::Matrix4x4::IDENTITY));

	// Print debug info
	if (::bPrintDebugInfo)
	{
		char buffer[64] = {0};
		std::sprintf(buffer, "#vertices sent: %u", ::numVertsToGPU);
		PrintDebugInfo(buffer);
		std::sprintf(buffer, "#objs outside frustum: %u", ::numObjsRequested - ::numObjsRendered);
		PrintDebugInfo(buffer);
		std::sprintf(buffer, "#objs straddling frustum: %u", ::numObjsRendered - ::numObjsInside);
		PrintDebugInfo(buffer);
		std::sprintf(buffer, "#objs inside frustum: %u", ::numObjsInside);
		PrintDebugInfo(buffer);
		std::sprintf(buffer, "Avg #plane eqn evals per obj: %.1f", static_cast<float>(::numPlaneTests) / ::numObjsRequested);
		PrintDebugInfo(buffer);
		std::sprintf(buffer, ::bPlaneCoherency ? "Plane coherency on" : "Plane coherency off");
		PrintDebugInfo(buffer);
		switch (::vfcBV_type)
		{
		case E_BSPHERE:
			std::sprintf(buffer, "Using Bounding Spheres");
			break;
		case E_AABB:
			std::sprintf(buffer, "Using AABBs");
			break;
		case E_OBB:
			std::sprintf(buffer, "Using OBBs");
			break;
		}
		PrintDebugInfo(buffer);
		
		::numObjsRequested = 0;
		::numObjsRendered = 0;
		::numObjsInside = 0;
		::numPlaneTests = 0;
	}

	/*========================================================================*/
	/* Fixed-function pipeline for rendering text                             */
	/*========================================================================*/
	shaderProgs[0].Deactivate();
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, ::font.GetGLHandle());
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	for (size_t i = 0, size = ::strings.size(); i < size; ++i)
	{
		bbk::Vector3 offset(
			-0.975f + ::strings[i].x * 0.048f,
			0.975f  - ::strings[i].y * 0.05f,
			0.0f);
		
		for (size_t j = 0, strlen = ::strings[i].text.size(); j < strlen; ++j)
		{
			bbk::Matrix4x4 mtx44_offset = bbk::Matrix4x4::MakeTranslate(offset);
			glPushMatrix();
			glMultMatrixf(mtx44_offset.elements);

			glMatrixMode(GL_TEXTURE);
			glLoadIdentity();

			bbk::Matrix4x4 mtx44_texcoord(bbk::Matrix4x4::MakeScale(0.0625f, 0.0625f, 1.0f));
			int col = strings[i].text[j] % 16;
			int row = strings[i].text[j] / 16;
			mtx44_texcoord.elements[12] = col * 0.0625f;
			mtx44_texcoord.elements[13] = row * 0.0625f;
			glLoadMatrixf(mtx44_texcoord.elements);
			
			glVertexPointer(3, GL_FLOAT, 5 * sizeof(float), charQuad);
			glTexCoordPointer(2, GL_FLOAT, 5 * sizeof(float), charQuad + 3);

			glDrawArrays(GL_QUADS, 0, 4);

			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();

			offset.x += 0.048f;
		}
	}

	bbk::Vector3 offset(-0.975f, 0.975f, 0.0f);
	for (size_t i = 0, size = ::debugStr.size(); i < size; ++i)
	{
		offset.x = -0.975f;
		for (size_t j = 0, strlen = ::debugStr[i].size(); j < strlen; ++j)
		{
			bbk::Matrix4x4 mtx44_offset = bbk::Matrix4x4::MakeTranslate(offset);
			glPushMatrix();
			glMultMatrixf(mtx44_offset.elements);

			glMatrixMode(GL_TEXTURE);
			glLoadIdentity();

			bbk::Matrix4x4 mtx44_texcoord(bbk::Matrix4x4::MakeScale(0.0625f, 0.0625f, 1.0f));
			int col = debugStr[i][j] % 16;
			int row = debugStr[i][j] / 16;
			mtx44_texcoord.elements[12] = col * 0.0625f;
			mtx44_texcoord.elements[13] = row * 0.0625f;
			glLoadMatrixf(mtx44_texcoord.elements);
			
			glVertexPointer(3, GL_FLOAT, 5 * sizeof(float), charQuad);
			glTexCoordPointer(2, GL_FLOAT, 5 * sizeof(float), charQuad + 3);

			glDrawArrays(GL_QUADS, 0, 4);

			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();

			offset.x += 0.04f;
		}
		offset.y -= 0.05f;
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	shaderProgs[0].Use();

	::strings.clear();
	::debugStr.clear();
}

void DrawPoint(const Point3 &pos, const Colour &clr)
{
	++(::transformRanges[::currTransformInd].numToRender);
	::transformRanges[::currTransformInd].pointIndices.push_back(::points.size());
	::points.push_back(PointRenderContext(pos, clr));
}

void DrawPoint(const Vertex &vtx)
{
	DrawPoint(vtx.pos, vtx.clr);
}

void DrawLine(const Point3 &startPos, const Colour &startClr, const Point3 &endPos, const Colour &endClr)
{
	++(::transformRanges[::currTransformInd].numToRender);
	::transformRanges[::currTransformInd].lineIndices.push_back(::lines.size());
	::lines.push_back(PointRenderContext(startPos, startClr));
	::transformRanges[::currTransformInd].lineIndices.push_back(::lines.size());
	::lines.push_back(PointRenderContext(endPos, endClr));
}

void DrawLine(const Vertex &startVtx, const Vertex &endVtx)
{
	DrawLine(startVtx.pos, startVtx.clr, endVtx.pos, endVtx.clr);
}

void DrawTri(const Vertex &v0, const Vertex &v1, const Vertex &v2)
{
	++(::transformRanges[::currTransformInd].numToRender);
	::transformRanges[::currTransformInd].triIndices.push_back(::tris.size());
	tris.push_back(PointRenderContext(v0.pos, v0.clr));
	::transformRanges[::currTransformInd].triIndices.push_back(::tris.size());
	::tris.push_back(PointRenderContext(v1.pos, v1.clr));
	::transformRanges[::currTransformInd].triIndices.push_back(::tris.size());
	::tris.push_back(PointRenderContext(v2.pos, v2.clr));
}

void DrawQuad(const Vertex &v0, const Vertex &v1, const Vertex &v2, const Vertex &v3)
{
	DrawTri(v0, v1, v2);
	DrawTri(v0, v2, v3);
}

void DrawModel(Model *pModel, bool useTextures, const Colour& clr)
{
	++(::transformRanges[::currTransformInd].numToRender);
	::transformRanges[::currTransformInd].meshIndices.push_back(::models.size());
	::models.push_back(ModelRenderContext(pModel, useTextures, clr));
}

void DrawObject(RenderContext& rc)
{
	++::numObjsRequested;
	::vfcScheme(rc);
}

void DrawShape(Shape shape, const Colour& clr)
{
	++(::transformRanges[::currTransformInd].numToRender);
	::transformRanges[::currTransformInd].shapeIndices.push_back(::shapes.size());
	::shapes.push_back(ModelRenderContext(::shapeMeshes[shape], false, clr));
}

void SetClearColor(float r, float g, float b, float a)
{
	glClearColor(r, g, b, a);
	::clearcolor[0] = r;
	::clearcolor[1] = g;
	::clearcolor[2] = b;
	::clearcolor[3] = a;
}

void PrintStr(const char *string, int x, int y)
{
	::strings.push_back(TextRenderContext(string, x, y));
}

void PrintDebugInfo(const char *string)
{
	::debugStr.push_back(string);
}

void PushMVMatrixStack()
{
	::lastPushIndices.push_back(::currTransformInd);
	::cl_MV_mtxStack.push_back(::cl_MV_mtxStack.back());
}

void PopMVMatrixStack()
{
	if (::cl_MV_mtxStack.size() > 1)
		::cl_MV_mtxStack.pop_back();
	if (::lastPushIndices.size() > 0)
	{
		::currTransformInd = ::lastPushIndices.back();
		::lastPushIndices.pop_back();
	}
}

void ClearMVMatrixStack()
{
	::cl_MV_mtxStack.clear();
	::cl_MV_mtxStack.push_back(bbk::Matrix4x4::IDENTITY);
	::lastPushIndices.clear();
	::currTransformInd = 0;
}

void MV_Push(const Matrix4x4 &mtx)
{
	::cl_MV_mtxStack.back() = ::cl_MV_mtxStack.back() * mtx;
	::currTransformInd = ::transformRanges.size();
	::transformRanges.push_back(::cl_MV_mtxStack.back());
}

void MV_Scale(float x_scalar, float y_scalar, float z_scalar)
{
	MV_Scale(bbk::Vector3(x_scalar, y_scalar, z_scalar));
}

void MV_Scale(const Vector3 &scalar)
{
	::cl_MV_mtxStack.back() = ::cl_MV_mtxStack.back() * bbk::Matrix4x4::MakeScale(scalar);
	::currTransformInd = ::transformRanges.size();
	::transformRanges.push_back(::cl_MV_mtxStack.back());
}

void MV_Rotate(const Vector3 &axis, float angle_rad)
{
	::cl_MV_mtxStack.back() = ::cl_MV_mtxStack.back() * bbk::Matrix4x4::MakeRotate(axis, angle_rad);
	::currTransformInd = ::transformRanges.size();
	::transformRanges.push_back(::cl_MV_mtxStack.back());
}

void MV_Translate(float x_disp, float y_disp, float z_disp)
{
	MV_Translate(bbk::Vector3(x_disp, y_disp, z_disp));
}

void MV_Translate(const Vector3 &displacement)
{
	::cl_MV_mtxStack.back() = ::cl_MV_mtxStack.back() * bbk::Matrix4x4::MakeTranslate(displacement);
	::currTransformInd = ::transformRanges.size();
	::transformRanges.push_back(::cl_MV_mtxStack.back());
}

Model* MakeModel()
{
	return new Model;
}

//void SetCurrentCam(Camera *pCam)
//{
//	::pCurrentCam = pCam;
//}
//
//void SetCullingCam(Camera *pCam)
//{
//	::pCullingCam = pCam;
//}
//
//void DrawCameraFrustum(Camera* pCam, const Colour& colour)
//{
//	float planeDists[6] = {0.0f};
//	pCam->GetPlaneDists(planeDists);
//	float &l = planeDists[0];//pCam->GetLeftVal();
//	float &r = planeDists[1];//pCam->GetRightVal();
//	float &b = planeDists[2];//pCam->GetBottomVal();
//	float &t = planeDists[3];//pCam->GetTopVal();
//	float &n = planeDists[4];//pCam->GetNearPlaneDist();
//	float &f = planeDists[5];//pCam->GetFarPlaneDist();
//	Vector3 campos = pCam->GetPosition();
//	Vector3 negz(pCam->GetViewVector().Normalise());
//	Vector3 x(pCam->GetRightVector());
//	Vector3 y(x.Cross(negz).Normalise());
//
//	Vector3 nearplane(n * negz);
//	Point3 nbl(b * y + l * x + nearplane);
//	Point3 nbr(b * y + r * x + nearplane);
//	Point3 ntl(t * y + l * x + nearplane);
//	Point3 ntr(t * y + r * x + nearplane);
//
//	float mult = f / n;
//	Point3 fbl(nbl * mult);
//	Point3 fbr(nbr * mult);
//	Point3 ftl(ntl * mult);
//	Point3 ftr(ntr * mult);
//
//	nbl += campos; nbr += campos; ntl += campos; ntr += campos;
//	fbl += campos; fbr += campos; ftl += campos; ftr += campos;
//
//	// Draw nearplane
//	DrawLine(nbl, colour, nbr, colour);
//	DrawLine(nbr, colour, ntr, colour);
//	DrawLine(ntr, colour, ntl, colour);
//	DrawLine(ntl, colour, nbl, colour);
//	
//	// Draw farplane
//	DrawLine(fbl, colour, fbr, colour);
//	DrawLine(fbr, colour, ftr, colour);
//	DrawLine(ftr, colour, ftl, colour);
//	DrawLine(ftl, colour, fbl, colour);
//
//	// Draw frustum edges
//	DrawLine(campos, colour, fbl, colour);
//	DrawLine(campos, colour, fbr, colour);
//	DrawLine(campos, colour, ftl, colour);
//	DrawLine(campos, colour, ftr, colour);
//
//	// Highlight frustum sides
//	/*Colour fade(colour);
//	fade.a *= 0.5f;
//	DrawQuad(Vertex(nbr, fade, 0.0f, 1.0f), Vertex(nbl, fade, 1.0f, 1.0f), Vertex(fbl, fade, 1.0f, 0.0f), Vertex(fbr, fade, 0.0f, 0.0f));
//	DrawQuad(Vertex(ntl, fade, 0.0f, 1.0f), Vertex(ntr, fade, 1.0f, 1.0f), Vertex(ftr, fade, 1.0f, 0.0f), Vertex(ftl, fade, 0.0f, 0.0f));
//	DrawQuad(Vertex(nbl, fade, 0.0f, 1.0f), Vertex(ntl, fade, 1.0f, 1.0f), Vertex(ftl, fade, 1.0f, 0.0f), Vertex(fbl, fade, 0.0f, 0.0f));
//	DrawQuad(Vertex(ntr, fade, 0.0f, 1.0f), Vertex(nbr, fade, 1.0f, 1.0f), Vertex(fbr, fade, 1.0f, 0.0f), Vertex(ftr, fade, 0.0f, 0.0f));
//	DrawQuad(Vertex(nbl, fade, 0.0f, 1.0f), Vertex(nbr, fade, 1.0f, 1.0f), Vertex(ntr, fade, 1.0f, 0.0f), Vertex(ntl, fade, 0.0f, 0.0f));
//	DrawQuad(Vertex(fbr, fade, 0.0f, 1.0f), Vertex(fbl, fade, 1.0f, 1.0f), Vertex(ftl, fade, 1.0f, 0.0f), Vertex(ftr, fade, 0.0f, 0.0f));*/
//
//	// Draw camera axes
//	{
//		const Matrix4x4 &mtx44_camview = pCam->GetWorldToViewMtx();
//
//		gfx::DrawLine(
//			campos,
//			bbk::Colour(1.0f, 0.0f, 0.0f, 1.0f),
//			campos + bbk::Vector3(mtx44_camview.elements[0], mtx44_camview.elements[4], mtx44_camview.elements[8]),
//			bbk::Colour(1.0f, 0.0f, 0.0f, 1.0f));
//		gfx::DrawLine(
//			campos,
//			bbk::Colour(0.0f, 1.0f, 0.0f, 1.0f),
//			campos + bbk::Vector3(mtx44_camview.elements[1], mtx44_camview.elements[5], mtx44_camview.elements[9]),
//			bbk::Colour(0.0f, 1.0f, 0.0f, 1.0f));
//		gfx::DrawLine(
//			campos,
//			bbk::Colour(0.0f, 0.0f, 1.0f, 1.0f),
//			campos + bbk::Vector3(mtx44_camview.elements[2], mtx44_camview.elements[6], mtx44_camview.elements[10]),
//			bbk::Colour(0.0f, 0.0f, 1.0f, 1.0f));
//	}
//}

const Matrix4x4& GetPerspProjMtx()                          {return ::currPerspProjMtx;}
void             SetPerspProjMtx(const Matrix4x4 &perspMtx) {::currPerspProjMtx = perspMtx;}
const Matrix4x4& GetViewMtx()                               {return ::currWorldViewMtx;}
void             SetViewMtx(const Matrix4x4 &viewMtx)       {::currWorldViewMtx = viewMtx;}

Matrix4x4 MakeOrthoProjMtx(float l, float r, float b, float t, float n, float f)
{
	return Matrix4x4(
		2.0f / (r - l), 0.0f, 0.0f, 0.0f,
		0.0f, 2.0f / (t - b), 0.0f, 0.0f,
		0.0f, 0.0f, -2.0f / (f - n), 0.0f,
		-(r + l) / (r - l), -(t + b) / (t - b), -(f + n) / (f - n), 1.0f);
}

Matrix4x4 MakePerspProjMtx(float l, float r, float b, float t, float nearDist, float farDist)
{
	return Matrix4x4(
		2.0f * nearDist / (r - l), 0.0f, 0.0f, 0.0f,
		0.0f, 2.0f * nearDist / (t - b), 0.0f, 0.0f,
		(r + l) / (r - l), (t + b) / (t - b), (-farDist - nearDist) / (farDist - nearDist), -1.0f,
		0.0f, 0.0f, (-2.0f * nearDist * farDist) / (farDist - nearDist), 0.0f);
}

Matrix4x4 MakePerspProjMtx(float vertFoV, float aspectRatio, float nearDist, float farDist)
{
	float cotan = 1.0f / std::tan(DEG_TO_RAD(vertFoV) * 0.5f);
	
	return Matrix4x4(
		cotan / aspectRatio, 0.0f, 0.0f, 0.0f,
		0.0f, cotan, 0.0f, 0.0f,
		0.0f, 0.0f, -(farDist + nearDist) / (farDist - nearDist), -1.0f,
		0.0f, 0.0f, (-2.0f * nearDist * farDist) / (farDist - nearDist), 0.0f);
}

Matrix4x4 MakeWorldToViewMtx(const Vector3 &view, const Point3 &pos, const Vector3 &globalUp)
{
	Vector3 w = view.Normalise();
	Vector3 u = (w.Cross(globalUp)).Normalise();
	Vector3 v = (u.Cross(w)).Normalise();

	return Matrix4x4(
		u.x, v.x, -w.x, 0.0f,
		u.y, v.y, -w.y, 0.0f,
		u.z, v.z, -w.z, 0.0f,
		-(u.Dot(pos)), -(v.Dot(pos)), w.Dot(pos), 1.0f);
}

Matrix4x4 MakeViewToWorldMtx(const Vector3 &view, const Point3 &pos, const Vector3 &globalUp)
{
	Vector3 w = view.Normalise();
	Vector3 u = (view.Cross(globalUp)).Normalise();
	Vector3 v = (u.Cross(w)).Normalise();

	return Matrix4x4(
		u.x, u.y, u.z, 0.0f,
		v.x, v.y, v.z, 0.0f,
		-w.x, -w.y, -w.z, 0.0f,
		pos.x, pos.y, pos.z, 1.0f);
}

Vector3 Unproject(int x, int y, int viewportWidth, int viewportHeight)
{
	Matrix4x4 mtx44_unproj = ::currPerspProjMtx.Inverse();

	Vector4 ndc_coords(
		2.0f * (x + 0.5f) / viewportWidth - 1.0f,
		2.0f * (viewportHeight - (y + 0.5f)) / viewportHeight - 1.0f,
		-1.0f,
		1.0f);

	Vector4 viewframe_coords(mtx44_unproj * ndc_coords);
	viewframe_coords /= viewframe_coords.w;

	return Vector3(viewframe_coords);
}

Vector3 UnprojtoViewframe(const Matrix4x4& persp, int x, int y, int vpWidth, int vpHeight)
{
	Matrix4x4 mtx44_unproj = persp.Inverse();

	Vector4 ndc_coords(
		2.0f * (x + 0.5f) / vpWidth - 1.0f,
		2.0f * (vpHeight - (y + 0.5f)) / vpHeight - 1.0f,
		-1.0f,
		1.0f);

	Vector4 viewframe_coords(mtx44_unproj * ndc_coords);
	viewframe_coords /= viewframe_coords.w;
	return Vector3(viewframe_coords);
}

Vector3 UnprojToWorldframe(const Matrix4x4& persp, const Matrix4x4& viewToWorld, int x, int y, int vpWidth, int vpHeight)
{
	Matrix4x4 mtx44_unproj = persp.Inverse();

	Vector4 ndc_coords(
		2.0f * (x + 0.5f) / vpWidth - 1.0f,
		2.0f * (vpHeight - (y + 0.5f)) / vpHeight - 1.0f,
		-1.0f,
		1.0f);

	Vector4 viewframe_coords(mtx44_unproj * ndc_coords);
	viewframe_coords /= viewframe_coords.w;

	return Vector3(viewToWorld * viewframe_coords);
}
void SetCullingFrustum(const Frustum& frustum)
{
	::cullingFrustum = frustum;
}

void SetVFCScheme(VFCScheme scheme)
{
	::vfcBV_type = scheme;
	if (::bPlaneCoherency)
	{
		switch (scheme)
		{
		case E_BSPHERE:
			::vfcScheme = ::BSphereVFC_PC;
			break;
		case E_AABB:
			::vfcScheme = ::AABBVFC_PC;
			break;
		case E_OBB:
			::vfcScheme = ::OBBVFC_PC;
			break;
		}
	}
	else
	{
		switch (scheme)
		{
		case E_BSPHERE:
			::vfcScheme = ::BSphereVFC;
			break;
		case E_AABB:
			::vfcScheme = ::AABBVFC;
			break;
		case E_OBB:
			::vfcScheme = ::OBBVFC;
			break;
		}
	}
}

bool GetPlaneCoherency()
{
	return ::bPlaneCoherency;
}

void SetPlaneCoherency(bool bEnabled)
{
	::bPlaneCoherency = bEnabled;
	switch (::vfcBV_type)
	{
	case E_BSPHERE:
		::vfcScheme = ::bPlaneCoherency ? ::BSphereVFC_PC : ::BSphereVFC;
		break;
	case E_AABB:
		::vfcScheme = ::bPlaneCoherency ? ::AABBVFC_PC : ::AABBVFC;
		break;
	case E_OBB:
		::vfcScheme = ::bPlaneCoherency ? ::OBBVFC_PC : ::OBBVFC;
		break;
	}
}

void DrawBoundingVolumes(bool flag)
{
	::bDrawBV = flag;
}
} // namespace gfx
} // namespace bbk

namespace
{
int BSvsLeftPlane(const bbk::BSphere& bsphere)
{
	++::numPlaneTests;
	return bbk::BSpherevsPlane(bsphere, ::cullingFrustum.leftPl);
}

int BSvsRightPlane(const bbk::BSphere& bsphere)
{
	++::numPlaneTests;
	return bbk::BSpherevsPlane(bsphere, ::cullingFrustum.rightPl);
}

int BSvsBottomPlane(const bbk::BSphere& bsphere)
{
	++::numPlaneTests;
	return bbk::BSpherevsPlane(bsphere, ::cullingFrustum.bottomPl);
}

int BSvsTopPlane(const bbk::BSphere& bsphere)
{
	++::numPlaneTests;
	return bbk::BSpherevsPlane(bsphere, ::cullingFrustum.topPl);
}

int BSvsNearPlane(const bbk::BSphere& bsphere)
{
	++::numPlaneTests;
	return bbk::BSpherevsPlane(bsphere, ::cullingFrustum.nearPl);
}

int BSvsFarPlane(const bbk::BSphere& bsphere)
{
	++::numPlaneTests;
	return bbk::BSpherevsPlane(bsphere, ::cullingFrustum.farPl);
}

int AABBvsLeftPlane(const bbk::AABB& aabb)
{
	++::numPlaneTests;
	return bbk::AABBvsPlane(aabb, ::cullingFrustum.leftPl);
}

int AABBvsRightPlane(const bbk::AABB& aabb)
{
	++::numPlaneTests;
	return bbk::AABBvsPlane(aabb, ::cullingFrustum.rightPl);
}

int AABBvsBottomPlane(const bbk::AABB& aabb)
{
	++::numPlaneTests;
	return bbk::AABBvsPlane(aabb, ::cullingFrustum.bottomPl);
}

int AABBvsTopPlane(const bbk::AABB& aabb)
{
	++::numPlaneTests;
	return bbk::AABBvsPlane(aabb, ::cullingFrustum.topPl);
}

int AABBvsNearPlane(const bbk::AABB& aabb)
{
	++::numPlaneTests;
	return bbk::AABBvsPlane(aabb, ::cullingFrustum.nearPl);
}

int AABBvsFarPlane(const bbk::AABB& aabb)
{
	++::numPlaneTests;
	return bbk::AABBvsPlane(aabb, ::cullingFrustum.farPl);
}

int OBBvsLeftPlane(const bbk::OBB& obb)
{
	++::numPlaneTests;
	return bbk::OBBvsPlane(obb, ::cullingFrustum.leftPl);
}

int OBBvsRightPlane(const bbk::OBB& obb)
{
	++::numPlaneTests;
	return bbk::OBBvsPlane(obb, ::cullingFrustum.rightPl);
}

int OBBvsBottomPlane(const bbk::OBB& obb)
{
	++::numPlaneTests;
	return bbk::OBBvsPlane(obb, ::cullingFrustum.bottomPl);
}

int OBBvsTopPlane(const bbk::OBB& obb)
{
	++::numPlaneTests;
	return bbk::OBBvsPlane(obb, ::cullingFrustum.topPl);
}

int OBBvsNearPlane(const bbk::OBB& obb)
{
	++::numPlaneTests;
	return bbk::OBBvsPlane(obb, ::cullingFrustum.nearPl);
}

int OBBvsFarPlane(const bbk::OBB& obb)
{
	++::numPlaneTests;
	return bbk::OBBvsPlane(obb, ::cullingFrustum.farPl);
}

void BSphereVFC(bbk::RenderContext& rc)
{
	unsigned char inCode = 0;
	const bbk::BSphere& bsphere(rc.bsphere);
	
	for (char i = 0; i < 6; ++i)
	{
		int result = ::bsTestFuncs[i](bsphere);
		if (result == 1) // Outside
		{
			rc.planeInd = i; // Save plane coherency info
			if (::bDrawBV)
				bbk::DrawBSphereR(bsphere);
			return;
		}
		else if (result)
			inCode |= 1 << i;
	}

	// Bounding volume intersects view frustum
	bbk::gfx::PushMVMatrixStack();
	bbk::gfx::MV_Push(rc.transform);
	bbk::gfx::DrawModel(rc.model);
	bbk::gfx::PopMVMatrixStack();
	if (::bDrawBV)
		bbk::DrawBSphereG(bsphere);
	++::numObjsRendered;
	if (inCode == 0x3F)
		++::numObjsInside;
}

void AABBVFC(bbk::RenderContext& rc)
{
	unsigned char inCode = 0;
	const bbk::AABB& aabb(rc.aabb);

	for (char i = 0; i < 6; ++i)
	{
		int result = ::aabbTestFuncs[i](aabb);
		if (result == 1) // Outside
		{
			rc.planeInd = i; // Save plane coherency info
			if (::bDrawBV)
				bbk::DrawAABBR(aabb);
			return;
		}
		else if (result)
			inCode |= 1 << i;
	}
		
	// Bounding volume intersects view frustum
	bbk::gfx::PushMVMatrixStack();
	bbk::gfx::MV_Push(rc.transform);
	bbk::gfx::DrawModel(rc.model);
	bbk::gfx::PopMVMatrixStack();
	if (::bDrawBV)
		bbk::DrawAABBG(aabb);
	++::numObjsRendered;
	if (inCode == 0x3F)
		++::numObjsInside;
}

void OBBVFC(bbk::RenderContext& rc)
{
	unsigned char inCode = 0;
	const bbk::OBB& obb(rc.obb);

	for (char i = 0; i < 6; ++i)
	{
		int result = ::obbTestFuncs[i](obb);
		if (result == 1) // Outside
		{
			rc.planeInd = i; // Save plane coherency info
			if (::bDrawBV)
				bbk::DrawOBBR(obb);
			return;
		}
		else if (result)
			inCode |= 1 << i;
	}
		
	// Bounding volume intersects view frustum
	bbk::gfx::PushMVMatrixStack();
	bbk::gfx::MV_Push(rc.transform);
	bbk::gfx::DrawModel(rc.model);
	bbk::gfx::PopMVMatrixStack();
	if (::bDrawBV)
		bbk::DrawOBBG(obb);
	++::numObjsRendered;
	if (inCode == 0x3F)
		++::numObjsInside;
}

void BSphereVFC_PC(bbk::RenderContext& rc)
{
	unsigned char inCode = 0;
	int result = 0;
	const bbk::BSphere& bsphere(rc.bsphere);

	// Plane-coherency test
	result = ::bsTestFuncs[rc.planeInd](bsphere);
	if (result < 1)
	{
		if (result)
			inCode |= 1 << rc.planeInd;
		for (char i = 0; i < 6; ++i)
		{
			if (i != rc.planeInd)
			{
				int result = ::bsTestFuncs[i](bsphere);
				if (result == 1) // Outside
				{
					rc.planeInd = i; // Save plane coherency info
					if (::bDrawBV)
						bbk::DrawBSphereR(bsphere);
					return;
				}
				else if (result)
					inCode |= 1 << i;
			}
		}

		// Bounding volume intersects view frustum
		bbk::gfx::PushMVMatrixStack();
		bbk::gfx::MV_Push(rc.transform);
		bbk::gfx::DrawModel(rc.model);
		bbk::gfx::PopMVMatrixStack();
		if (::bDrawBV)
			bbk::DrawBSphereG(bsphere);
		++::numObjsRendered;
		if (inCode == 0x3F)
			++::numObjsInside;
	}
	else if (::bDrawBV)
		bbk::DrawBSphereR(bsphere);
}

void AABBVFC_PC(bbk::RenderContext& rc)
{
	unsigned char inCode = 0;
	int result = 0;
	const bbk::AABB& aabb(rc.aabb);

	// Plane-coherency test
	result = ::aabbTestFuncs[rc.planeInd](aabb);
	if (result < 1)
	{
		if (result)
			inCode |= 1 << rc.planeInd;
		for (char i = 0; i < 6; ++i)
		{
			if (i != rc.planeInd)
			{
				int result = ::aabbTestFuncs[i](aabb);
				if (result == 1) // Outside
				{
					rc.planeInd = i; // Save plane coherency info
					if (::bDrawBV)
						bbk::DrawAABBR(aabb);
					return;
				}
				else if (result)
					inCode |= 1 << i;
			}
		}
		
		// Bounding volume intersects view frustum
		bbk::gfx::PushMVMatrixStack();
		bbk::gfx::MV_Push(rc.transform);
		bbk::gfx::DrawModel(rc.model);
		bbk::gfx::PopMVMatrixStack();
		if (::bDrawBV)
			bbk::DrawAABBG(aabb);
		++::numObjsRendered;
		if (inCode == 0x3F)
			++::numObjsInside;
	}
	else if (::bDrawBV)
		bbk::DrawAABBR(aabb);
}

void OBBVFC_PC(bbk::RenderContext& rc)
{
	unsigned char inCode = 0;
	int result = 0;
	const bbk::OBB& obb(rc.obb);

	// Plane-coherency test
	result = ::obbTestFuncs[rc.planeInd](obb);
	if (result < 1)
	{
		if (result)
			inCode |= 1 << rc.planeInd;
		for (char i = 0; i < 6; ++i)
		{
			if (i != rc.planeInd)
			{
				int result = ::obbTestFuncs[i](obb);
				if (result == 1) // Outside
				{
					rc.planeInd = i; // Save plane coherency info
					if (::bDrawBV)
						bbk::DrawOBBR(obb);
					return;
				}
				else if (result)
					inCode |= 1 << i;
			}
		}
		
		// Bounding volume intersects view frustum
		bbk::gfx::PushMVMatrixStack();
		bbk::gfx::MV_Push(rc.transform);
		bbk::gfx::DrawModel(rc.model);
		bbk::gfx::PopMVMatrixStack();
		if (::bDrawBV)
			bbk::DrawOBBG(obb);
		++::numObjsRendered;
		if (inCode == 0x3F)
			++::numObjsInside;
	}
	else if (::bDrawBV)
		bbk::DrawOBBR(obb);
}
} // anon namespace
