#ifndef _GRAPHICS_H
#define _GRAPHICS_H

#include "opengl/glew.h"
#include "math/mathlib.h"
#include "model.h"
#include "resources/texture.h"
#include "shaders/shaderprog.h"
#include "shaders/locationmgr.h"
#include "intersect/intersect.h"
#include "rendercontext.h"

namespace bbk
{
class BObject;

namespace gfx
{
extern LocationMgr locationMgrs[8];
extern ShaderProg  shaderProgs[8];

enum Shape
{
	E_PLANE,
	E_CUBE,
	E_SPHERE,
	NUM_SHAPES
}; // enum Shape

enum VFCScheme
{
	E_BSPHERE,
	E_AABB,
	E_OBB
};

bool Init();
bool InitGL();
void LoadShaders();
void Halt();

void EnableVSync(bool flag);
void PrintDebugInfo(bool flag);

/** @name
 *  Rendering *///\{
/// Calls graphics engine to render scene
void Render();

void DrawPoint(const Point3 &pos, const Colour &clr = Colour());
void DrawPoint(const Vertex &vtx);
void DrawLine(const Point3 &startPos, const Colour &startClr = Colour(), const Point3 &endPos = Point3(), const Colour &endClr = Colour());
void DrawLine(const Vertex &startVtx, const Vertex &endVtx);
void DrawTri(const Vertex &v0, const Vertex &v1, const Vertex &v2);
void DrawQuad(const Vertex &v0, const Vertex &v1, const Vertex &v2, const Vertex &v3);
void DrawModel(Model *pModel, bool useTextures = true, const Colour& clr = Colour(1.0f, 1.0f, 1.0f, 1.0f));
void DrawObject(RenderContext& renderContext);
void DrawShape(Shape shape, const Colour& clr = Colour(1.0f, 1.0f, 1.0f, 1.0f));
//\}

void SetClearColor(float r, float g, float b, float a);
void PrintStr(const char *string, int x = 0, int y = 0);
void PrintDebugInfo(const char *string);

/** @name
 *  Global transforms that affect the entire scene *///\{
void PushMVMatrixStack();
void PopMVMatrixStack();
void ClearMVMatrixStack();
void MV_Push(const Matrix4x4 &mtx);
void MV_Scale(float x_scalar, float y_scalar, float z_scalar);
void MV_Scale(const Vector3 &scalar);
void MV_Rotate(const Vector3 &axis, float angle_rad);
void MV_Translate(float x_disp, float y_disp, float z_disp);
void MV_Translate(const Vector3 &displacement);
//\}

/** @name
 *  Geometry *///\{
Model* MakeModel();
//\}

/** @name
 *  Camera *///\{
//void SetCurrentCam(Camera *pCam);
//void SetCullingCam(Camera *pCullingCam);
//void DrawCameraFrustum(Camera *pCam, const Colour& colour);
//\}

/** @name
 *  Projection and view matrices *///\{
const Matrix4x4& GetPerspProjMtx();
void SetPerspProjMtx(const Matrix4x4 &perspMtx);
const Matrix4x4& GetViewMtx();
void SetViewMtx(const Matrix4x4 &viewMtx);

Matrix4x4 MakeOrthoProjMtx(float l, float r, float b, float t, float n, float f);
Matrix4x4 MakePerspProjMtx(float l, float r, float b, float t, float nearDist, float farDist);
Matrix4x4 MakePerspProjMtx(float vertFoV_deg, float aspectRatio, float nearDist, float farDist);

Matrix4x4 MakeWorldToViewMtx(const Vector3 &view, const Point3 &position, const Vector3 &nrmGlobalUp = Vector3(0.f,1.f,0.f));
Matrix4x4 MakeViewToWorldMtx(const Vector3 &view, const Point3 &position, const Vector3 &nrmGlobalUp = Vector3(0.f,1.f,0.f));

Vector3 Unproject(int x, int y, int viewportWidth, int viewportHeight);
Vector3 UnprojToViewframe(const Matrix4x4& persp, int x, int y, int vpWidth, int vpHeight);
Vector3 UnprojToWorldframe(const Matrix4x4& persp, const Matrix4x4& viewToWorld, int x, int y, int vpWidth, int vpHeight);
//\}

/** @name
 *  View-Frustum Culling *///\{
void SetCullingFrustum(const Frustum& frustum);
void SetVFCScheme(VFCScheme scheme);
bool GetPlaneCoherency();
void SetPlaneCoherency(bool bEnabled);
void DrawBoundingVolumes(bool bDrawBV);
//\}
} // namespace gfx
} // namespace bbk

#endif /* _GRAPHICS_H */
