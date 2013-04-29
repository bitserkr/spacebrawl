#include "Sandbox.h"
#include "bbk.h"
#include <cstring>

/*------------------------------------------------------------------------------
 * Structs */
struct LightContext
{
	int          type;
	bbk::Vector3 position;
	bbk::Vector3 direction;
	bbk::Vector4 I_ambient;
	bbk::Vector4 I_diffuse;
	bbk::Vector4 I_specular;
	bbk::Vector3 distAttCoeff;
	bbk::Vector3 spotAttCoeff;
};

struct MaterialContext
{
	bbk::Vector4 K_ambient;
	bbk::Vector4 K_diffuse;
	bbk::Vector4 K_specular;
	bbk::Vector4 K_emissive;
	float        shinePower;
};

struct FogContext
{
	bbk::Vector4 color;
    float        nearDist;
    float        farDist;
};

/******************************************************************************/
/* Constants ******************************************************************/

extern bool bUpdateLinear;
extern bool bUpdateAngular;

namespace
{
// Camera ----------------------------------------------------------------------
const float CameraNear = 0.1f;   ///< Near plane
const float CameraFar  = 256.0f; ///< Far plane
bbk::PerspCam *pCam = nullptr;

// Textures --------------------------------------------------------------------
enum TextureType
{
	TEX_AMBIENT = 0,
	TEX_DIFFUSE,
	TEX_SPECULAR,
	TEX_EMISSIVE,
	TEX_NUM_TYPES
};
const char* texNames[] = {"TexAmbient", "TexDiffuse", "TexSpecular", "TexEmissive"};
const char* texFilenames[] = {
	"Textures/duck.tga",
	"Textures/duck.tga",
	"Textures/TopBottom_Specular.tga",
	"Textures/TopBottom_Emissive.tga"};

// Lighting --------------------------------------------------------------------
enum LightingType
{
	LIGHT_POINT = 0,
	LIGHT_DIR,
	LIGHT_SPOT,
	LIGHT_NUM_TYPES
};

/******************************************************************************/
/* Static storage, file-scope vars ********************************************/

// Textures --------------------------------------------------------------------
bbk::Texture textures[TEX_NUM_TYPES];

// Lighting --------------------------------------------------------------------
LightContext lightSrc;            ///< Lighting parameters
bbk::Vector4 backgroundColor(0.16f, 0.16f, 0.16f, 1.0f);

// Shader Program --------------------------------------------------------------
unsigned int materialType   = TEX_NUM_TYPES;

// Scene variables -------------------------------------------------------------
bbk::Model* pSphereModel = nullptr;
bbk::Model* pCubeModel   = nullptr;
bbk::Model* pDuckModel   = nullptr;

bbk::BObject* obj;
bool vsync = true;
int  showOBBlvl = 0;
bbk::BVHNode* pickedBVHNode = nullptr;

float        pushPower = 0.0f;
bbk::Vector3 pushDisp;
bbk::Vector3 pushVec;

void DrawBVHLevel(bbk::BVHNode *root, size_t current, size_t target);
void DrawBVHNodeChildren(bbk::BVHNode* node);
bbk::BVHNode* LinevsBVHNode(const bbk::Line& line, bbk::BVHNode* node);
bbk::BVHNode* LinevsBVHChild(const bbk::Line& line, bbk::BVHNode* root);
bbk::BVHNode* LinevsBVHLeaf(const bbk::Line& line, bbk::BVHNode* root);
} // anon namespace

/******************************************************************************/
/* Function definitions *******************************************************/
bool Sandbox::Load()
{
	::pCam = new bbk::PerspCam;
	::obj = new bbk::BObject;

	/*--------------------------------------------------------------------------
	 * Load textures
	 */
	for (size_t i = 0; i < TEX_NUM_TYPES; ++i)
	{
		::textures[i].SetTextureName(::texNames[i]);
		::textures[i].LoadTexDataFromFile(::texFilenames[i]);
		::textures[i].LoadTexDataToGPU();
		::textures[i].FreeTexData();
		glUniform1i(bbk::gfx::locationMgrs[0].GetUniformLocHandle(::texNames[i]), i);
	}

	// Load models
	::pSphereModel = new bbk::Model;
	::pCubeModel = new bbk::Model;
	//::pDuckModel = new bbk::Model;
	::pSphereModel->LoadGeometryFromFile("Sphere.xml");
	::pCubeModel->LoadGeometryFromFile("Cube.xml");
	//::pDuckModel->LoadGeometryFromFile("Duck.xml");

	return true;
}

void Sandbox::Init()
{
	bbk::appwindow::SetTitle("Sandbox");
	
	/*--------------------------------------------------------------------------
	 * Set graphics states
	 */
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	bbk::gfx::SetClearColor(::backgroundColor.x, ::backgroundColor.y, ::backgroundColor.z, ::backgroundColor.w);
	glClearDepth(1.0f);
	glViewport(0, 0, 1024, 768);
	bbk::gfx::EnableVSync(::vsync);
	bbk::gfx::DrawBoundingVolumes(false);
	bbk::gfx::PrintDebugInfo(true);

	::pCam->Init(60.0f, 1024.0f / 768.0f, ::CameraNear, ::CameraFar);
	::pCam->SetPosition(bbk::Vector3(4.0f, 8.0f, 8.0f));
	::pCam->SetTargetPos(bbk::Vector3());
	
	/*--------------------------------------------------------------------------
	 * Set texture units to use appropriate textures
	 */
	for (size_t i = 0; i < TEX_NUM_TYPES; ++i)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, ::textures[i].GetGLHandle());
	}

	/*--------------------------------------------------------------------------
	 * Register shader variables with location manager and set their values
	 */
	::lightSrc.type         = ::LIGHT_DIR;
	::lightSrc.position     = bbk::Vector3(5.0f, 5.0f, 0.0f);
	::lightSrc.direction    = -::lightSrc.position;
	::lightSrc.I_ambient    = bbk::Vector4(0.16f, 0.16f, 0.16f, 1.0f);
	::lightSrc.I_diffuse    = bbk::Vector4(0.32f, 0.32f, 0.32f, 1.0f);
	::lightSrc.I_specular   = bbk::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	::lightSrc.distAttCoeff = bbk::Vector3(1.0f, 0.1f, 0.0f);
	::lightSrc.spotAttCoeff = bbk::Vector3(
		std::cos(15.0f / 180.0f * bbk::PIf),
		std::cos(30.0f / 180.0f * bbk::PIf),
		1.0f);
		
	/* Register light variables and set their values */
	char buffer[64] = {0};
	std::sprintf(buffer, "lights[0].type");
	glUniform1i(bbk::gfx::locationMgrs[0].GetUniformLocHandle(buffer), ::lightSrc.type);

	std::sprintf(buffer, "lights[0].position");
	glUniform3fv(bbk::gfx::locationMgrs[0].GetUniformLocHandle(buffer), 1, &::lightSrc.position.x);

	std::sprintf(buffer, "lights[0].direction");
	bbk::gfx::locationMgrs[0].GetUniformLocHandle(buffer);

	std::sprintf(buffer, "lights[0].I_ambient");
	glUniform4fv(bbk::gfx::locationMgrs[0].GetUniformLocHandle(buffer), 1, &::lightSrc.I_ambient.x);

	std::sprintf(buffer, "lights[0].I_diffuse");
	glUniform4fv(bbk::gfx::locationMgrs[0].GetUniformLocHandle(buffer), 1, &::lightSrc.I_diffuse.x);

	std::sprintf(buffer, "lights[0].I_specular");
	glUniform4fv(bbk::gfx::locationMgrs[0].GetUniformLocHandle(buffer), 1, &::lightSrc.I_specular.x);

	std::sprintf(buffer, "lights[0].spotAttCoeff");
	glUniform3fv(bbk::gfx::locationMgrs[0].GetUniformLocHandle(buffer), 1, &::lightSrc.spotAttCoeff.x);

	std::sprintf(buffer, "lights[0].distAttCoeff");
	glUniform3fv(bbk::gfx::locationMgrs[0].GetUniformLocHandle(buffer), 1, &::lightSrc.distAttCoeff.x);

	glUniform4f(bbk::gfx::locationMgrs[0].GetUniformLocHandle("I_globAmbient"), 0.2f, 0.2f, 0.2f, 1.0f);

	// Fog parameters
	glUniform4fv(bbk::gfx::locationMgrs[0].GetUniformLocHandle("fog.color"), 1, &backgroundColor.x);
	glUniform1f(bbk::gfx::locationMgrs[0].GetUniformLocHandle("fog.nearDist"), 0.5f * ::pCam->GetFarPlaneDist());
	glUniform1f(bbk::gfx::locationMgrs[0].GetUniformLocHandle("fog.farDist"), 10.0f * ::pCam->GetFarPlaneDist());
	
	//glUniform1i(bbk::gfx::locationMgrs[0].GetUniformLocHandle("constLightType_Point"), 0);
	glUniform1i(bbk::gfx::locationMgrs[0].GetUniformLocHandle("constLightType_Dir"),   LIGHT_DIR);
	glUniform1i(bbk::gfx::locationMgrs[0].GetUniformLocHandle("constLightType_Spot"),  LIGHT_SPOT);
	glUniform1i(bbk::gfx::locationMgrs[0].GetUniformLocHandle("numLightSrc"), 1);
	
	glUniform1i(bbk::gfx::locationMgrs[0].GetUniformLocHandle("isUseTextures"), true);
	glUniform1i(bbk::gfx::locationMgrs[0].GetUniformLocHandle("materialType"), ::materialType);
	glUniform1i(bbk::gfx::locationMgrs[0].GetUniformLocHandle("useVertexColor"), false);

	bbk::gfx::locationMgrs[0].GetUniformLocHandle("surfaceClr.K_ambient");
	bbk::gfx::locationMgrs[0].GetUniformLocHandle("surfaceClr.K_diffuse");
	bbk::gfx::locationMgrs[0].GetUniformLocHandle("surfaceClr.K_specular");
	bbk::gfx::locationMgrs[0].GetUniformLocHandle("surfaceClr.K_emissive");

	/*--------------------------------------------------------------------------
	 * Print instructions to debug console
	 */
	std::fprintf(stdout, "CONTROLS:\n---------\n");
	std::fprintf(stdout, "W: Thrust forward.\n");
	std::fprintf(stdout, "A/D: Yaw left/right.\n");
	std::fprintf(stdout, "\n");
	std::fprintf(stdout, "Hold down LMB and move mouse to pitch/roll.\n");
	std::fprintf(stdout, "\n");
	std::fprintf(stdout, "Esc: Quit.\n");

	// Init objects
	::obj->SetGeometry(::pSphereModel);
	//obj.SetScale(2.0f);
	::obj->SetPosition(bbk::Vector3());
	//::obj->ApplyNettForce(bbk::Vector3(50.0f, 20.0f, -25.0f), bbk::Vector3(0.0f, 1.0f, 0.0f));
}

void Sandbox::Update(float deltatime)
{
	/*--------------------------------------------------------------------------
	 * User input
	 */
	// Gamestate changes
	if (bbk::keyboard::IsKeyTriggered(bbk::KB_ESCAPE))
		bbk::GameStateMgr::EndGame();
	else if(bbk::keyboard::IsKeyTriggered(bbk::KB_F5))
		bbk::GameStateMgr::SwitchGameState(1);
	else if(bbk::keyboard::IsKeyTriggered(bbk::KB_F6))
		bbk::GameStateMgr::SwitchGameState(2);
	else if(bbk::keyboard::IsKeyTriggered(bbk::KB_F7))
		bbk::GameStateMgr::SwitchGameState(3);

	if (bbk::keyboard::IsKeyTriggered(bbk::KB_F1))
	{
		::obj->Serialise("object.xml");
	}

	// Camera controls
	/*{
		if (bbk::mouse::IsPressed(bbk::MOUSE_LMB))
		{
			const int dx = bbk::mouse::GetDeltaX();
			const int dy = bbk::mouse::GetDeltaY();

			::pCam->Yaw(0.01f * -dx);
			::pCam->Pitch(0.01f * -dy);
		}
		if (bbk::mouse::IsPressed(bbk::MOUSE_RMB))
		{
			const int dx = bbk::mouse::GetDeltaX();
			const int dy = bbk::mouse::GetDeltaY();

			::pCam->OrbitalDispAndLook(bbk::Vector3(), 0.01f * -dx, 0.01f * dy);
		}

		if (bbk::keyboard::IsKeyPressed(bbk::KB_w))
			::pCam->Displace(::pCam->GetViewVector().Normalise() * 10.0f * deltatime);
		else if (bbk::keyboard::IsKeyPressed(bbk::KB_s))
			::pCam->Displace(::pCam->GetViewVector().Normalise() * -10.0f * deltatime);

		const bbk::Vector3 &right = ::pCam->GetRightVector();

		if (bbk::keyboard::IsKeyPressed(bbk::KB_a))
			::pCam->Displace(right * -10.0f * deltatime);
		else if (bbk::keyboard::IsKeyPressed(bbk::KB_d))
			::pCam->Displace(right * 10.0f * deltatime);

		bbk::Vector3 camup = right.Cross(::pCam->GetViewVector()).Normalise();

		if (bbk::keyboard::IsKeyPressed(bbk::KB_q))
			::pCam->Displace(camup * -10.0f * deltatime);
		else if (bbk::keyboard::IsKeyPressed(bbk::KB_e))
			::pCam->Displace(camup * 10.0f * deltatime);
	}*/

	// Ship controls
	{
		const bbk::Matrix3x3 objorient(bbk::QuatToMatrix(::obj->GetQuat()));
		const bbk::Vector3 fore(objorient.GetCol(0));
		if (bbk::keyboard::IsKeyPressed(bbk::KB_w))
			::obj->AddForce(400.0f * fore, ::obj->GetPosition());
		else if (bbk::keyboard::IsKeyPressed(bbk::KB_s))
			::obj->AddForce(-15.0f * fore, ::obj->GetPosition());

		if (bbk::keyboard::IsKeyPressed(bbk::KB_a))
		{
			::obj->AddForce(objorient * (16.0f * bbk::Vector3(0.0f, 0.0f, -1.0f)), objorient.GetCol(0));
			::obj->AddForce(objorient * (16.0f * bbk::Vector3(0.0f, 0.0f, 1.0f)), -objorient.GetCol(0));
		}
		else if (bbk::keyboard::IsKeyPressed(bbk::KB_d))
		{
			::obj->AddForce(objorient * (16.0f * bbk::Vector3(0.0f, 0.0f, 1.0f)), objorient.GetCol(0));
			::obj->AddForce(objorient * (16.0f * bbk::Vector3(0.0f, 0.0f, -1.0f)), -objorient.GetCol(0));
		}

		if (bbk::mouse::IsPressed(bbk::MOUSE_LMB))
		{
			const float dy = static_cast<float>(bbk::mouse::GetDeltaY());
			::obj->AddForce(objorient * (dy * bbk::Vector3(16.0f, 0.0f, 0.0f)), objorient * bbk::Vector3(0.0f, -1.0f, 0.0f));
			::obj->AddForce(objorient * (dy * bbk::Vector3(-16.0f, 0.0f, 0.0f)), objorient * bbk::Vector3(0.0f, 1.0f, 0.0f));
			const float dx = static_cast<float>(bbk::mouse::GetDeltaX());
			::obj->AddForce(objorient * (dx * bbk::Vector3(0.0f, 0.0f, 16.0f)), objorient * bbk::Vector3(0.0f, 1.0f, 0.0f));
			::obj->AddForce(objorient * (dx * bbk::Vector3(0.0f, 0.0f, -16.0f)), objorient * bbk::Vector3(0.0f, -1.0f, 0.0f));
		}

		if (::obj->GetAngularMom().MagnitudeSq() > 0.0f)
		{
			::obj->SetAngularMom(0.7f * ::obj->GetAngularMom());
		}
	}

	// Show OBB level
	if (bbk::mouse::IsWheelDown())
		--::showOBBlvl;
	else if (bbk::mouse::IsWheelUp())
		++::showOBBlvl;

	// Pushing
	if (bbk::keyboard::IsKeyTriggered(bbk::KB_SPACE))
	{
		bbk::Point3 pickpos = bbk::gfx::UnprojToWorldframe(
			::pCam->GetProjectionMtx(),
			::pCam->GetViewToWorldMtx(),
			bbk::mouse::GetX(),
			bbk::mouse::GetY(),
			bbk::appwindow::GetWindowWidth(),
			bbk::appwindow::GetWindowHeight());

		bbk::Line pickline(::pCam->GetPosition(), (pickpos - ::pCam->GetPosition()).Normalise());

		float intertimes[2] = {0.0f};
		if (bbk::LinevsOBB(pickline, ::obj->GetOBB(), intertimes))
		{
			::pushPower = 0.0f;
			::pushDisp = pickline.pt + intertimes[0]*pickline.vec - ::obj->GetPosition();
			::pushVec = pickline.vec;
		}
	}
	else if (bbk::keyboard::IsKeyReleased(bbk::KB_SPACE))
	{
		::obj->ApplyNettForce(::pushPower * ::pushVec, ::pushDisp);
	}
	if (bbk::keyboard::IsKeyPressed(bbk::KB_SPACE))
		::pushPower += 1.0f;

	if (bbk::keyboard::IsKeyTriggered(bbk::KB_t))
		::bUpdateLinear = !::bUpdateLinear;
	if (bbk::keyboard::IsKeyTriggered(bbk::KB_r))
		::bUpdateAngular = !::bUpdateAngular;
	
	/*--------------------------------------------------------------------------
	 * Update positions and directions of light sources
	 */

	// Update objects' rigid body dynamics
	::obj->Integrate(deltatime);
	::obj->Update(deltatime);

	// Set camera position and direction
	{
		const bbk::Matrix3x3 objorient(bbk::QuatToMatrix(::obj->GetQuat()));
		const bbk::Vector3 camdisp(objorient * bbk::Vector3(-20.4f, 0.f, 0.0f));
		const bbk::Vector3 nose(objorient * bbk::Vector3(3.2f, 0.0f, 0.0f));
		::pCam->SetGlobalUpVec(objorient * bbk::Vector3(0.0f, 1.0f, 0.0f));
		::pCam->SetPosition(::obj->GetPosition() + camdisp);
		::pCam->SetTargetPos(::obj->GetPosition() + nose);
	}
}

void Sandbox::Draw()
{
	bbk::gfx::SetPerspProjMtx(::pCam->GetProjectionMtx());
	bbk::gfx::SetViewMtx(::pCam->GetWorldToViewMtx());
	bbk::gfx::SetCullingFrustum(::pCam->GetFrustum());

	const bbk::Matrix4x4 &mtx44_view = ::pCam->GetWorldToViewMtx();

	/*==========================================================================
	 * Update shader variables
	 *------------------------------------------------------------------------*/
	bbk::Vector4 viewFrame_lightPos = mtx44_view * bbk::Vector4(::lightSrc.position, 1.0f);
	bbk::Vector3 viewFrame_lightDir = mtx44_view * ::lightSrc.direction;

	char buffer[64] = {0};
	std::sprintf(buffer, "lights[0].position");
	glUniform3fv(bbk::gfx::locationMgrs[0].GetUniformLocHandle(buffer), 1, &viewFrame_lightPos.x);
		
	std::sprintf(buffer, "lights[0].direction");
	glUniform3fv(bbk::gfx::locationMgrs[0].GetUniformLocHandle(buffer), 1, &viewFrame_lightDir.x);

	/*==========================================================================
	 * Draw scene
	 *------------------------------------------------------------------------*/
	
	/*--------------------------------------------------------------------------
	 * Set texture units to use appropriate textures
	 */
	for (size_t i = 0; i < TEX_NUM_TYPES; ++i)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, ::textures[i].GetGLHandle());
	}

	/*--------------------------------------------------------------------------
	 * Render objects
	 */
	::obj->Draw();
	
	glActiveTexture(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	/*--------------------------------------------------------------------------
	 * Display debug info
	 */
	// Draw BVH
	if (::showOBBlvl > -1)
	{
		DrawBVHLevel(::obj->GetModel()->GetBVH(), 0, ::showOBBlvl);
	}
	{
		char buffer[64] = {0};
		std::sprintf(buffer, "bvh lvl %d", ::showOBBlvl);
		bbk::gfx::PrintDebugInfo(buffer);
	}

	// Draw spatial grid
	{
		const bbk::Vector3 &center(::pCam->GetPosition());
		int ix = (static_cast<int>(center.x) / 5) * 5;
		int iz = (static_cast<int>(center.z) / 5) * 5;
		
		bbk::Vector3 minv(bbk::Vector3((float)ix, 0.0f, (float)iz) - bbk::Vector3(50.0f, 0.0f, 50.0f));
		bbk::Vector3 maxv(bbk::Vector3((float)ix, 0.0f, (float)iz) + bbk::Vector3(50.0f, 0.0f, 50.0f));
		//int iy = ::pCam->GetViewVector().y < 0.0f ? ((int)::obj->GetPosition().y - 5) / 10 : ((int)::obj->GetPosition().y + 5) / 10;
		float y = 0.0f;//(float)iy * 10.0f;
		{
			float x = minv.x;
			for (size_t j = 0; j < 21; ++j)
			{
				bbk::gfx::DrawLine(bbk::Vector3(x, y, minv.z), bbk::Colour(0.0f, 0.0f, 1.0f, 0.64f), bbk::Vector3(x, y, maxv.z), bbk::Colour(0.0f, 0.0f, 1.0f, 0.64f));
				x += 5.0f;
			}

			float z = minv.z;
			for (size_t j = 0; j < 21; ++j)
			{
				bbk::gfx::DrawLine(bbk::Vector3(minv.x, y, z), bbk::Colour(0.0f, 0.0f, 1.0f, 0.64f), bbk::Vector3(maxv.x, y, z), bbk::Colour(0.0f, 0.0f, 1.0f, 0.64f));
				z += 5.0f;
			}
		}
		bbk::gfx::DrawLine(bbk::Vector3(0.0f, y-50.0f, 0.0f), bbk::Colour(1.0f, 1.0f, 1.0f, 0.32f), bbk::Vector3(0.0f, y + 50.0f, 0.0f), bbk::Colour(1.0f, 1.0f, 1.0f, 0.32f));
		{
			const bbk::Vector3 &pos(::obj->GetPosition());
			bbk::Vector3 height(bbk::Vector3(pos.x, 0.0f, pos.z).Normalise());
			height *= 0.5f;
			bbk::gfx::DrawLine(bbk::Vector3(-height.x, pos.y, -height.z), bbk::Colour(0.0f, 1.0f, 0.0f, 0.32f), bbk::Vector3(height.x, pos.y, height.z), bbk::Colour(0.0f, 1.0f, 0.0f, 0.32f));
			bbk::gfx::DrawLine(bbk::Vector3(0.0f, y, 0.0f), bbk::Colour(1.0f, 0.0f, 1.0f, 0.32f), bbk::Vector3(pos.x, y, pos.z), bbk::Colour(1.0f, 0.0f, 1.0f, 0.32f));
		}
		
		bbk::gfx::PushMVMatrixStack();
		bbk::gfx::MV_Translate(0.0f, y, 0.0f);
		bbk::gfx::MV_Scale(0.1f, 0.1f, 0.1f);
		bbk::gfx::DrawShape(bbk::gfx::E_SPHERE);
		bbk::gfx::PopMVMatrixStack();
	}

	// Debug info
	{
		char buffer[64] = {0};
		
		const bbk::Vector3& pos(::obj->GetPosition());
		std::sprintf(buffer, "Pos (%.2f %.2f %.2f)", pos.x, pos.y, pos.z);
		bbk::gfx::PrintDebugInfo(buffer);
		
		const bbk::Vector3& vel(::obj->GetVelocity());
		std::sprintf(buffer, "Lin. vel (%.2f %.2f %.2f", vel.x, vel.y, vel.z);
		bbk::gfx::PrintDebugInfo(buffer);

		const bbk::Quat& quat(::obj->GetQuat());
		std::sprintf(buffer, "Orient (%.2f %.2f %.2f %.2f", quat.s, quat.v.x, quat.v.y, quat.v.z);
		bbk::gfx::PrintDebugInfo(buffer);

		const bbk::Vector3& angmom(::obj->GetAngularMom());
		std::sprintf(buffer, "Ang. mom (%.2f %.2f %.2f ", angmom.x, angmom.y, angmom.z);
		bbk::gfx::PrintDebugInfo(buffer);

		const bbk::Quat& angvel(::obj->GetAngularVel());
		std::sprintf(buffer, "Ang. vel (%.2f %.2f %.2f %.2f", angvel.s, angvel.v.x, angvel.v.y, angvel.v.z);
		bbk::gfx::PrintDebugInfo(buffer);

		/*const bbk::Matrix3x3 rot(::obj->GetRotation());
		bbk::Vector3 x(rot.elements[0], rot.elements[1], rot.elements[2]);
		bbk::Vector3 y(rot.elements[3], rot.elements[4], rot.elements[5]);
		bbk::Vector3 z(rot.elements[6], rot.elements[7], rot.elements[8]);
		
		std::sprintf(buffer, "x %f", x.Magnitude());
		bbk::gfx::PrintDebugInfo(buffer);
		std::sprintf(buffer, "y %f", y.Magnitude());
		bbk::gfx::PrintDebugInfo(buffer);
		std::sprintf(buffer, "z %f", z.Magnitude());
		bbk::gfx::PrintDebugInfo(buffer);*/
		
		/*float ang = std::acosf(x.Dot(y)) / bbk::PIf * 180.0f;
		std::sprintf(buffer, "xy %f", ang);
		bbk::gfx::PrintDebugInfo(buffer);
		ang = std::acosf(y.Dot(z)) / bbk::PIf * 180.0f;
		std::sprintf(buffer, "yz %f", ang);
		bbk::gfx::PrintDebugInfo(buffer);
		ang = std::acosf(z.Dot(x)) / bbk::PIf * 180.0f;
		std::sprintf(buffer, "zx %f", ang);
		bbk::gfx::PrintDebugInfo(buffer);*/
	}
	if (::pickedBVHNode)
		::DrawBVHNodeChildren(::pickedBVHNode);
}

void Sandbox::Cleanup()
{
}

void Sandbox::Unload()
{
	delete ::pCam;
	delete ::pSphereModel;
	delete ::pCubeModel;
	delete ::pDuckModel;
	delete ::obj;
}

namespace
{
void DrawBVHLevel(bbk::BVHNode *root, size_t current, size_t target)
{
	if (current == target)
	{
		// Draw obb
		bbk::OBB obb = bbk::TransformOBB(::obj->GetTransform(), root->obb);
		const bbk::Vector3 scaledOffset(::obj->GetModel()->GetOBBOffset() * ::obj->GetScale());
		//obb.center += ::obj->GetRotation() * scaledOffset;
		obb.halfExtents *= ::obj->GetScale();
		bbk::DrawOBBG(obb);

		/*{
			for (size_t i = 0; i < root->numVerts; i+=3)
			{
				bbk::Vertex v0(root->triVerts[i], bbk::Colour(0.0f, 0.0f, 1.0f, 0.64f));
				bbk::Vertex v1(root->triVerts[i+1], bbk::Colour(0.0f, 0.0f, 1.0f, 0.64f));
				bbk::Vertex v2(root->triVerts[i+2], bbk::Colour(0.0f, 0.0f, 1.0f, 0.64f));
				bbk::gfx::DrawTri(v0, v1, v2);
			}
		}*/
		return;
	}
	
	if (root->left)
		DrawBVHLevel(root->left, current + 1, target);
	if (root->right)
		DrawBVHLevel(root->right, current + 1, target);
}

void DrawBVHNodeChildren(bbk::BVHNode* node)
{
	// Draw obb
	if (node->left)
	{
		bbk::OBB obb = bbk::TransformOBB(::obj->GetTransform(), node->left->obb);
		const bbk::Vector3 scaledOffset(::obj->GetModel()->GetOBBOffset() * ::obj->GetScale());
		obb.center += ::obj->GetRotation() * scaledOffset;
		obb.halfExtents *= ::obj->GetScale();
		bbk::DrawOBBR(obb);

		{
			/*bbk::gfx::PushMVMatrixStack();
			bbk::gfx::MV_Translate(obb.center);
			bbk::gfx::MV_Push(::obj->GetTransform());*/
			{
				for (size_t i = 0; i < node->left->numVerts; i+=3)
				{
					bbk::Vertex v0(node->left->triVerts[i], bbk::Colour(1.0f, 0.0f, 0.0f, 0.64f));
					bbk::Vertex v1(node->left->triVerts[i+1], bbk::Colour(1.0f, 0.0f, 0.0f, 0.64f));
					bbk::Vertex v2(node->left->triVerts[i+2], bbk::Colour(1.0f, 0.0f, 0.0f, 0.64f));
					bbk::gfx::DrawTri(v0, v1, v2);
				}
			}
			//bbk::gfx::PopMVMatrixStack();
		}
	}
	if (node->right)
	{
		bbk::OBB obb = bbk::TransformOBB(::obj->GetTransform(), node->right->obb);
		const bbk::Vector3 scaledOffset(::obj->GetModel()->GetOBBOffset() * ::obj->GetScale());
		obb.center += ::obj->GetRotation() * scaledOffset;
		obb.halfExtents *= ::obj->GetScale();
		bbk::DrawOBBB(obb);

		{
			/*bbk::gfx::PushMVMatrixStack();
			bbk::gfx::MV_Translate(obb.center);
			bbk::gfx::MV_Push(::obj->GetTransform());*/
			{
				for (size_t i = 0; i < node->right->numVerts; i+=3)
				{
					bbk::Vertex v0(node->right->triVerts[i], bbk::Colour(0.0f, 0.0f, 1.0f, 0.64f));
					bbk::Vertex v1(node->right->triVerts[i+1], bbk::Colour(0.0f, 0.0f, 1.0f, 0.64f));
					bbk::Vertex v2(node->right->triVerts[i+2], bbk::Colour(0.0f, 0.0f, 1.0f, 0.64f));
					bbk::gfx::DrawTri(v0, v1, v2);
				}
			}
			//bbk::gfx::PopMVMatrixStack();
		}
	}
}

bbk::BVHNode* LinevsBVHNode(const bbk::Line& line, bbk::BVHNode* node)
{
	// Transform bvhnode obb to world space
	bbk::OBB obb = bbk::TransformOBB(::obj->GetTransform(), node->obb);
	const bbk::Vector3 scaledOffset(::obj->GetModel()->GetOBBOffset() * ::obj->GetScale());
	//obb.center += ::obj->GetRotation() * scaledOffset;
	obb.halfExtents *= ::obj->GetScale();

	float intertimes[2] = {0.0f};
	return bbk::LinevsOBB(line, obb, intertimes) ? node : nullptr;
}

bbk::BVHNode* LinevsBVHChild(const bbk::Line& line, bbk::BVHNode* root)
{
	bbk::BVHNode* hitChild = nullptr;
	if (root->left)
		hitChild = ::LinevsBVHNode(line, root->left);
	if (!hitChild && root->right)
		hitChild = ::LinevsBVHNode(line, root->right);
	return hitChild;
}

bbk::BVHNode* LinevsBVHLeaf(const bbk::Line& line, bbk::BVHNode* root)
{
	// Transform bvhnode obb to world space
	bbk::OBB obb = bbk::TransformOBB(::obj->GetTransform(), root->obb);
	const bbk::Vector3 scaledOffset(::obj->GetModel()->GetOBBOffset() * ::obj->GetScale());
	//obb.center += ::obj->GetRotation() * scaledOffset;
	obb.halfExtents *= ::obj->GetScale();

	float intertimes[2] = {0.0f};
	if (bbk::LinevsOBB(line, obb, intertimes)) // Line intersects current node
	{
		if (root->left == nullptr && root->right == nullptr) // leaf node
			return root;

		bbk::BVHNode* hitChild = nullptr;
		if (root->left)
			hitChild = ::LinevsBVHLeaf(line, root->left);

		if (!hitChild && root->right)
			hitChild = ::LinevsBVHLeaf(line, root->right);

		return hitChild;
	}
	return nullptr;
}
} // anon namespace
