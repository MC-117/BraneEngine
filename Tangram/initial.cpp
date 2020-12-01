#define _AFXDLL
#include <afxdlgs.h>
#include "Engine.h"
#include "EditorWindow.h"
#include "WindowBar.h"
#include "GrassMeshActor.h"
#include "FaceTrackerObject.h"
#include "Character.h"
#include "ParticleSystem.h"
#include <fstream>
#include "Utility.h"

using namespace std;
using namespace Eigen;

struct LoadInfo {
	string mat;
	string mesh;
	int num;
	bool isComplexCollision;
	PhysicalMaterial physicalMat;
};

Material* newStdMat(const string& path, const string& name, Material& baseMat) {
	string pre = path + "/Texture/T_" + name;
	Material &mat = getAsset<Material>("Material", "PBR")->instantiate();
	Texture2D* cT = getAssetByPath<Texture2D>(pre + "_BC.TGA");
	if (cT != NULL) {
		//Texture2D& cT = *new Texture2D(pre + "_BC.TGA");
		mat.setTexture("colorMap", *cT);
	}
	Texture2D* mT = getAssetByPath<Texture2D>(pre + "_MT.TGA");
	if (mT != NULL) {
		//Texture2D& mT = *new Texture2D(pre + "_MT.TGA");
		mat.setTexture("metallicMap", *mT);
	}
	Texture2D* nT = getAssetByPath<Texture2D>(pre + "_N.TGA");
	if (nT != NULL) {
		nT->isStandard = false;
		//Texture2D& nT = *new Texture2D(pre + "_N.TGA", false);
		mat.setTexture("normalMap", *nT);
	}
	Texture2D* rT = getAssetByPath<Texture2D>(pre + "_R.TGA");
	if (rT != NULL) {
		//Texture2D& rT = *new Texture2D(pre + "_R.TGA");
		mat.setTexture("roughnessMap", *rT);
	}
	Texture2D* aT = getAssetByPath<Texture2D>(pre + "_AO.TGA");
	if (aT != NULL) {
		//Texture2D& aT = *new Texture2D(pre + "_AO.TGA");
		mat.setTexture("aoMap", *aT);
	}
	return &mat;
}

MeshActor* newStdMesh(const string& path, const string& name, Material& mat, bool complexCollsion = false) {
	Mesh *mesh = getAssetByPath<Mesh>(path + "/Mesh/SM_" + name + "_Internal.obj");
	/*if (!Mesh::ObjReader::load(path + "\\Mesh\\SM_" + path + "_Internal.obj", *mesh)) {
		cout << "Load " << path + "\\Mesh\\SM_" + path + "_Internal.obj" << " failed!!\n";
		delete mesh;
		return NULL;
	}*/
	if (mesh == NULL)
		return NULL;
	return new MeshActor(*mesh, mat, { 0, STATIC }, *mesh, name, complexCollsion ? COMPLEX : SIMPLE);
}

vector<MeshActor*> newStdMeshes(const string& path, const string& name, Material& mat, PhysicalMaterial pmat = PhysicalMaterial(100, DYNAMIC), int num = 1) {
	vector<MeshActor*> re;
	Mesh *mesh = getAssetByPath<Mesh>(path + "/Mesh/SM_" + name + "_Internal.obj");
	/*if (!Mesh::ObjReader::load(path + "\\Mesh\\SM_" + path + "_Internal.obj", *mesh)) {
		cout << "Load " << path + "\\Mesh\\SM_" + path + "_Internal.obj" << " failed!!\n";
		delete &mesh;
		return re;
	}*/
	if (mesh == NULL)
		return re;
	for (int i = 0; i < num; i++) {
		char s[5];
		itoa(i, s, 10);
		re.push_back(new MeshActor(*mesh, mat, pmat, *mesh, name + '_' + s));
	}
	return re;
}

MeshActor* newGun(const string& path, const string& name, Material& mat, PhysicalMaterial physicalMat = PhysicalMaterial(100, DYNAMIC)) {
	string pre = path + '/' + name + '/' + name;
	Mesh *mesh = getAssetByPath<Mesh>(pre + ".obj");
	/*if (!Mesh::ObjReader::load(pre + ".obj", *mesh)) {
		delete mesh;
		return NULL;
	}*/
	if (mesh == NULL)
		return NULL;
	MeshActor* gun = new MeshActor(*mesh, mat.instantiate(), physicalMat, *mesh, name);
	Texture2D& cT = *(getAssetByPath<Texture2D>(pre + "_BaseColor.TGA"));//*new Texture2D(pre + "_BaseColor.TGA");
	Texture2D& mT = *(getAssetByPath<Texture2D>(pre + "_Metallic.TGA"));//*new Texture2D(pre + "_Metallic.TGA");
	Texture2D& nT = *(getAssetByPath<Texture2D>(pre + "_Normal.TGA"));//*new Texture2D(pre + "_Normal.TGA", false);
	Texture2D& rT = *(getAssetByPath<Texture2D>(pre + "_Roughness.TGA"));//*new Texture2D(pre + "_Roughness.TGA");
	Texture2D& aT = *(getAssetByPath<Texture2D>(pre + "_AO.TGA"));//*new Texture2D(pre + "_AO.TGA");
	gun->meshRender.setBaseColor({ 255, 255, 255, 255 });
	gun->meshRender.getMaterial()->setTexture("colorMap", cT);
	gun->meshRender.getMaterial()->setTexture("metallicMap", mT);
	gun->meshRender.getMaterial()->setTexture("normalMap", nT);
	gun->meshRender.getMaterial()->setTexture("roughnessMap", rT);
	gun->meshRender.getMaterial()->setTexture("aoMap", aT);
	gun->meshRender.getMaterial()->setScalar("metallic", 1);
	gun->meshRender.getMaterial()->setScalar("roughness", 0.8);
	gun->meshRender.getMaterial()->setScalar("ao", 1);
	return gun;
}

//void newCar() {
//	Material& mat = *getAsset<Material>("Material", "PBR");
//	Mesh& LFWMesh = *getAsset<Mesh>("Mesh", "Car_LFW");
//	Mesh& RFWMesh = *getAsset<Mesh>("Mesh", "Car_RFW");
//	Mesh& LBWMesh = *getAsset<Mesh>("Mesh", "Car_LBW");
//	Mesh& RBWMesh = *getAsset<Mesh>("Mesh", "Car_RBW");
//	Mesh& CarMesh = *getAsset<Mesh>("Mesh", "Car_body");
//	Mesh& CarColMesh = *getAsset<Mesh>("Mesh", "Car_body_collision");
//
//	WheelInfo info;
//
//	info.wheelRadius = 2.477284f;
//	info.wheelWidth = 1.548302f;
//	info.wheelFriction = 1000;
//	info.suspensionStiffness = 20.f;
//	info.suspensionDamping = 2.3f;
//	info.suspensionCompression = 4.4f;
//	info.suspensionRestLength = 2.6;
//	info.maxSuspensionTravelCm = 20;
//	info.rollInfluence = .1f;
//
//	info.wheelDirection = { 0, 0, -1 };
//	info.wheelAxle = { 0, -1, 0 };
//
//	//info.isPowerWheel = true;
//	info.isSteerWheel = true;
//
//	info.connectionPoint = { 7.781f, 7.039f, 0.464f + 2.042f };
//	WheelActor &LFW = *new WheelActor(LFWMesh, mat.instantiate(), info, "Car_LFW");
//
//	info.connectionPoint = { 7.781f, -7.039f, 0.464f + 2.042f };
//	WheelActor &RFW = *new WheelActor(RFWMesh, mat.instantiate(), info, "Car_RFW");
//
//	info.isSteerWheel = false;
//	info.wheelRadius = 2.748525f;
//	info.suspensionRestLength = 2.8;
//	info.connectionPoint = { -6.426f, 7.039f, 0.714f + 2.042f };
//	WheelActor &LBW = *new WheelActor(LBWMesh, mat.instantiate(), info, "Car_LBW");
//
//	info.connectionPoint = { -6.426f, -7.039f, 0.714f + 2.042f };
//	WheelActor &RBW = *new WheelActor(RBWMesh, mat.instantiate(), info, "Car_RBW");
//
//	VehicleActor &car = *new VehicleActor(CarMesh, mat.instantiate(), { 800, DYNAMIC }, &CarColMesh, COMPLEX, "Car");
//	car.addWheel(LFW);
//	car.addWheel(RFW);
//	car.addWheel(LBW);
//	car.addWheel(RBW);
//
//	car.bindTick([](Object* obj, float dt) {
//		if (world.input.getKeyState(VK_CONTROL) == Up) {
//			VehicleActor& v = *((VehicleActor*)obj);
//			if (!(world.input.getKeyState(VK_UP) == Down) ^ (world.input.getKeyState(VK_DOWN) == Down))
//				v.ReleaseAccelerator();
//			else {
//				if (world.input.getKeyState(VK_UP) == Down)
//					v.PressAccelerator(false);
//				if (world.input.getKeyState(VK_DOWN) == Down)
//					v.PressAccelerator(true);
//			}
//			if (!(world.input.getKeyState(VK_LEFT) == Down) ^ (world.input.getKeyState(VK_RIGHT) == Down))
//				v.SetSteering(0);
//			else {
//				if (world.input.getKeyState(VK_LEFT) == Down)
//					v.SetSteering(1);
//				if (world.input.getKeyState(VK_RIGHT) == Down)
//					v.SetSteering(-1);
//			}
//		}
//	});
//	world += LFW;
//	world += RFW;
//	world += LBW;
//	world += RBW;
//	world += car;
//
//	//car.setPosition({ 0, 0, 40.f });
//}

void printAINode(aiNode* node, int& num, int depth = 0) {
	/*for (int i = 0; i < depth; i++) {
		cout << '\t';
	}*/
	num++;
	//cout << node->mName.C_Str() << " (Mesh: " << node->mNumMeshes << ")\n";
	for (int i = 0; i < node->mNumChildren; i++) {
		printAINode(node->mChildren[i], num, depth + 1);
	}
}

void printAIScene(const aiScene* scene) {
	for (int i = 0; i < scene->mNumMeshes; i++) {
		aiMesh* m = scene->mMeshes[i];
		cout << "Mesh " << i << ": " << m->mNumBones << "\n";
		/*for (int b = 0; b < m->mNumBones; b++) {
			cout << "Bone " << m->mBones[b]->mName.C_Str() << endl;
		}*/
	}
	//cout << "Scene (Mesh: " << scene->mNumMeshes << "):\n";
	int num = 0;
	printAINode(scene->mRootNode, num);
	cout << "Scene Node: " << num << endl;
}

//void InitialWorld() {
//	/*::Transform& playerCollision = *new ::Transform("playerCollision");
//	playerCollision.updataRigidBody(new Capsule(7, 20), ShapeComplexType::SIMPLE, PhysicalMaterial(10000, DYNAMIC));
//	world += playerCollision;
//	playerCollision.setPosition(0, 0, 21);
//	playerCollision.rigidBody->setAngularFactor(0);
//	playerCollision.rigidBody->setFriction(10);
//	playerCollision.bindTick([](Object* obj, float dt) {
//		if (world.input.getCursorHidden()) {
//			Camera& cam = world.getCurrentCamera();
//			::Transform& p = *(::Transform*)obj;
//			Vector3f from = p.getPosition(WORLD) - Vector3f(0, 0, p.rigidBody->shape->getHeight() / 2 + 0.5);
//			Vector3f to = from - Vector3f(0, 0, 1);
//			ContactInfo re;
//			bool isFly = false;
//			if (world.physicalWorld.rayTest(from, to, re))
//				isFly = true;
//			Vector3f v, v1;
//			v = cam.forward * ((int)world.input.getKeyState('W') - (int)world.input.getKeyState('S')) +
//				cam.rightward * ((int)world.input.getKeyState('A') - (int)world.input.getKeyState('D'));
//			v.z() = 0;
//			v.normalize();
//			v1 = v;
//			v *= dt * 30 * (isFly ? 0.2 : 1);
//			p.rigidBody->setGravity({ 0, 0, -15 });
//			p.rigidBody->translate(toPVec3(v));
//			static bool down = false;
//			if (world.input.getKeyState(' ') == Down) {
//				if (!down && !isFly) {
//					v1.z() = 200000;
//					p.rigidBody->applyCentralImpulse(toPVec3(v1));
//				}
//				down = true;
//			}
//			if (world.input.getKeyState(' ') == Up) {
//				down = false;
//			}
//		}
//	});*/
//
//	PostProcessingCamera& camera = *new PostProcessingCamera();
//	camera.updataRigidBody(new Sphere(5));
//	world += camera;
//	//playerCollision.addChild(camera);
//	camera.setPosition(0, 0, 17);
//	world.switchCamera(camera);
//
//	Material &grassM1 = *getAssetByPath<Material>("Content/Scene/Material/GrassM1.imat");
//	Material &grassM2 = *getAssetByPath<Material>("Content/Scene/Material/GrassM2.imat");
//	Material &grassM3 = *getAssetByPath<Material>("Content/Scene/Material/GrassM3.imat");
//	Mesh &grassMesh = *getAssetByPath<Mesh>("Content/Scene/Mesh/Grass.fbx");
//
//	GrassMeshActor &grass = *new GrassMeshActor(grassMesh, grassM1, "Grass");
//	grassM1.setTwoSide(true);
//	grassM2.setTwoSide(true);
//	grassM3.setTwoSide(true);
//	grass.meshRender.setMaterial(0, grassM1);
//	grass.meshRender.setMaterial(1, grassM2);
//	grass.meshRender.setMaterial(2, grassM3);
//	grass.set(50, Vector2i(500, 500));
//	grass.setPosition(0, 0, 0.6);
//	world += grass;
//
//	//newCar();
//
//	Material &pbr_mat = *getAssetByPath<Material>("Engine/Shaders/PBR.mat");
//	string path = "Content/Gun";
//	MeshActor* UMP = newGun(path, "UMP", pbr_mat);
//	if (UMP != NULL) {
//		world += *UMP;
//		UMP->setPosition(0, 3, 50);
//	}
//	MeshActor* Flaregun = newGun(path, "Flaregun", pbr_mat);
//	if (Flaregun != NULL) {
//		world += *Flaregun;
//		Flaregun->setPosition(0, 0, 70);
//	}
//
//	vector<LoadInfo> stdMeshList = {
//		{ "FloorConcrete", "Floor", 35 },
//		//{ "Hangarwall", "Wall", 32 },
//		{ "Gate01", "Gate", 1 },
//		/*{ "Barrel02", "Barrel", 1, true },
//		{ "Container", "Container" },
//		{ "Crate", "Crate" },
//		{ "Door01", "Door_Combined" },
//		{ "plane", "Plane" },
//		{ "Wallfabric01", "Wallfabric_Corner" },
//		{ "Wallfabric01", "Wallfabric" }*/
//	};
//
//	for (auto b = stdMeshList.begin(), e = stdMeshList.end(); b != e; b++) {
//		Material& _mat = *newStdMat("Content/Scene", b->mat, pbr_mat);
//		vector<MeshActor*> _meshAs = newStdMeshes("Content/Scene", b->mesh, _mat, b->physicalMat, b->num);
//		for (auto b = _meshAs.begin(), e = _meshAs.end(); b != e; b++) {
//			if (*b != NULL) {
//				world += *b;
//				//(*b)->setPosition(0, 0, 5);
//			}
//		}
//	}
//
//	Material& woodBoxMat = *newStdMat("Content/Scene", "WoodBox", pbr_mat);
//	vector<MeshActor*> boxes = newStdMeshes("Content/Scene", "WoodBox", woodBoxMat, PhysicalMaterial(1000, DYNAMIC), 60);
//
//	for (int i = 0; i < boxes.size(); i++) {
//		if (boxes[i] != NULL) {
//			world += boxes[i];
//			boxes[i]->setPosition(0, 0, 200);
//		}
//	}
//
//	Material& bbMat = *newStdMat("Content/Scene", "Baseball", pbr_mat);
//	bbMat.setScalar("roughness", 1);
//	bbMat.setScalar("metallic", 0);
//
//	Mesh *bbmesh = NULL;
//	if ((bbmesh = getAssetByPath<Mesh>("Content/Scene/Mesh/SM_Baseball_Internal.obj")) == NULL) {
//		cout << "Load " << "Content/Scene/Mesh/SM_Baseball_Internal.obj" << " failed!!\n";
//	}
//	else
//		new MeshActor(*bbmesh, bbMat, PhysicalMaterial(100, DYNAMIC), *new Sphere(bbmesh->getRadius()), "Baseball");
//
//	DirectLight &dirLight = *new DirectLight("DirLight", { 255, 255, 200 }, 1.5);
//	camera.setVolumnicLight(dirLight);
//	dirLight.intensity = 1.5;
//	//dirLight.setRotation(0, -45, 45);
//	/*dirLight.bindTick([](Object* s, float dt) {
//		MeshActor* a = (MeshActor*)s;
//		a->rotate(0, 0, dt * 20, WORLD);
//	});*/
//	world += dirLight;
//	SkySphere &sky = *new SkySphere(dirLight);
//	sky.loadDefaultTexture();
//	sky.scaling(400, 400, 400);
//	world += sky;
//	PointLight &pointLight1 = *new PointLight("PointLight1", { 20, 255, 255, 255 }, 10);
//	PointLight &pointLight2 = *new PointLight("PointLight2", { 255, 20, 255, 255 }, 10);
//	PointLight &pointLight3 = *new PointLight("PointLight3", { 255, 255, 20, 255 }, 10);
//	PointLight &pointLight4 = *new PointLight("PointLight4", { 20, 20, 255, 255 }, 10);
//	pointLight1.setPosition(-5, 5, 5);
//	pointLight2.setPosition(5, 5, 5);
//	pointLight3.setPosition(5, -5, 5);
//	pointLight4.setPosition(-5, -5, 5);
//	/*world += pointLight1;
//	world += pointLight2;
//	world += pointLight3;
//	world += pointLight4;*/
//	dirLight.addChild(pointLight1);
//	dirLight.addChild(pointLight2);
//	dirLight.addChild(pointLight3);
//	dirLight.addChild(pointLight4);
//
//	world.loadTransform("Save.world");
//
//	world.input.keyboardInputHandler += [](Input* input, char key, InputStateEnum state) {
//		if (GUI::isAnyItemFocus()/* | world.input.getKeyState(VK_CONTROL) == Up*/)
//			return;
//		if (key == 'F' && state == Up)
//#ifdef __FREEGLUT_H__
//			glutFullScreenToggle();
//#endif
//#ifdef _glfw3_h_
//		{
//			Engine::toggleFullscreen();
//		}
//#endif
//		if (key == 'G' && state == Down && world.physicalWorld.getGravity().z() < 0)
//			world.physicalWorld.setGravity({ 0, 0, 10.f });
//		if (key == 'G' && state == Up && world.physicalWorld.getGravity().z() > 0)
//			world.physicalWorld.setGravity({ 0, 0, -10.f });
//	};
//
//	world.getCurrentCamera().bindTick([](Object* s, float dt) {
//		Input& input = world.input;
//		Camera& cam = *(Camera*)s;
//		if (/*world.input.getKeyState(VK_CONTROL) == Down &&*/ (input.getCursorHidden() || input.getMouseButtonState(MouseButtonEnum::Right) == InputStateEnum::Down)) {
//			Vector3f v(0, 0, 0);
//			v.x() = (int)input.getKeyState('W') - (int)input.getKeyState('S');
//			v.x() += world.input.getMouseWheelValue();
//			v.y() = (int)input.getKeyState('A') - (int)input.getKeyState('D');
//			v.z() = (int)input.getKeyState('E') - (int)input.getKeyState('Q');
//			v *= dt * 30;
//			cam.translate(v.x(), v.y(), 0);
//			cam.translate(0, 0, v.z(), WORLD);
//
//			Unit2Di m = input.getMouseMove();
//			cam.rotate(0, 0, -m.x * dt * 5, WORLD);
//			cam.rotate(0, m.y * dt * 5, 0);
//
//			//static float eTime = 0;
//			//static int bcount = 0;
//			static RigidBody* target = NULL;
//			static btGeneric6DofSpring2Constraint* constraint = NULL;
//			if (input.getMouseButtonState(MouseButtonEnum::Left) == InputStateEnum::Down) {
//				Vector3f pos = cam.getPosition(WORLD);
//				Vector3f forward = cam.getForward(WORLD);
//				if (target == NULL) {
//					ContactInfo res;
//					if (world.physicalWorld.rayTest(pos, pos + forward * 1500, res)) {
//						RigidBody* obj = dynamic_cast<RigidBody*>((PhysicalBody*)res.physicalObject);
//						if (obj != NULL) {
//							Console::log("RayTest: %s", res.objectName);
//							if (cam.rigidBody != NULL) {
//								target = obj;
//								float distance = (obj->targetTransform.getPosition(WORLD) - pos).norm();
//								Vector3f rel = res.location;
//								rel = toVector3f(obj->getWorldTransform().inverse() * toPVec3(rel));
//								PTransform at = PTransform::getIdentity(), bt = PTransform::getIdentity();
//								at.setOrigin({ distance, 0, 0 });
//								bt.setOrigin(toPVec3(rel));
//								constraint = new btGeneric6DofSpring2Constraint(*cam.rigidBody, *obj, at, bt);
//								constraint->enableSpring(3, true);
//								constraint->setStiffness(3, 35.0f);
//								constraint->setDamping(3, 0.5f);
//								constraint->enableSpring(4, true);
//								constraint->setStiffness(4, 35.0f);
//								constraint->setDamping(4, 0.5f);
//								constraint->enableSpring(5, true);
//								constraint->setStiffness(5, 35.0f);
//								constraint->setDamping(5, 0.5f);
//
//								cam.rigidBody->addConstraintRef(constraint);
//								obj->addConstraintRef(constraint);
//
//								world.physicalWorld.physicsScene->addConstraint(constraint);
//							}
//						}
//					}
//				}
//				//if (eTime == 0) {
//					//Vector3f pos = cam.getPosition(WORLD);
//					/*string str = "LaunchBaseBall_";
//					char num[5];
//					itoa(bcount++, num, 10);
//					str += num;
//					MeshActor* box = (MeshActor*)Brane::find(typeid(Object).hash_code(), "Baseball");
//					MeshActor* t = new MeshActor(box->meshRender.mesh, *box->meshRender.getMaterial(), PhysicalMaterial(100, DYNAMIC), Sphere(box->meshRender.mesh.getRadius()), str);
//					t->rigidBody.setHitFraction(1000);
//					world += t;
//					t->setPosition(pos + cam.forward * 2);
//					t->rigidBody.applyCentralImpulse(toPVec3(cam.forward.normalized() * 10000));*/
//					/*ContactInfo res;
//					if (world.physicalWorld.rayTest(pos, pos + cam.forward.normalized() * 1500, res)) {
//						RigidBody* obj = dynamic_cast<RigidBody*>((PhysicalBody*)res.physicalObject);
//						if (obj != NULL) {
//							Console::log("RayTest: %s", res.objectName);
//						}
//					}*/
//					//}
//					//eTime += dt;
//			}
//			if (input.getMouseButtonState(MouseButtonEnum::Left) == InputStateEnum::Up) {
//				if (cam.rigidBody != NULL && target != NULL) {
//					cam.rigidBody->removeConstraintRef(constraint);
//					target->removeConstraintRef(constraint);
//					world.physicalWorld.physicsScene->removeConstraint(constraint);
//					delete constraint;
//					target = NULL;
//					constraint = NULL;
//				}
//				//eTime = 0;
//			}
//			/*if (eTime > 1) {
//				eTime = 0;
//			}*/
//		}
//	});
//	world.input.setCursorHidden(true);
//
//	EditorWindow& editorWindow = *new EditorWindow(world, pbr_mat);
//	editorWindow.blurBackground = true;
//	editorWindow.showCloseButton = false;
//	world += editorWindow;
//
//	world.bindUI([](GUI& gui) {
//		//ImGui::SetNextWindowBgAlpha(0);
//		ImGui::SetNextWindowSize({ 40, 40 }, ImGuiCond_Always);
//		ImGui::SetNextWindowPosCenter(ImGuiCond_Always);
//		ImGui::Begin("HUD", 0,
//			ImGuiWindowFlags_NoResize |
//			ImGuiWindowFlags_NoCollapse |
//			ImGuiWindowFlags_NoScrollbar |
//			ImGuiWindowFlags_NoBackground |
//			ImGuiWindowFlags_NoInputs |
//			ImGuiWindowFlags_NoMouseInputs |
//			ImGuiWindowFlags_NoMove |
//			ImGuiWindowFlags_NoTitleBar);
//		ImVec2 pos = ImGui::GetWindowPos();
//		ImGui::GetForegroundDrawList()->AddRectFilled({ pos.x + 18, pos.y + 18 }, { pos.x + 22, pos.y + 22 }, IM_COL32(0, 255, 0, 255), 1);
//		ImGui::End();
//		if (world.input.getCursorHidden()) {
//			if (world.input.getKeyState(VK_ESCAPE) == Down) {
//				world.input.setCursorHidden(false);
//			}
//			gui.hideUIControl("Editor");
//		}
//		else {
//			gui.showUIControl("Editor");
//		}
//	});
//
//	//world.camera.setRotation(180, -90, 0);
//	//world.camera.bindTick([](Object* s, float dt) {
//	//	Input& input = world.input;
//	//	Camera& camera = *((Camera*)s);
//	//	if (input.getKeyState(VK_ESCAPE))
//	//		world.quit();
//	//	Vector3f v(0, 0, 0);
//	//	v.z() = (int)input.getKeyState('W') - (int)input.getKeyState('S');
//	//	v.y() = (int)input.getKeyState('A') - (int)input.getKeyState('D');
//	//	v.x() = (int)input.getKeyState('E') - (int)input.getKeyState('Q');
//	//	v *= dt * 10;
//	//	if ((!GUI::isMouseOnUI())) {
//	//		if (input.getMouseButtonState(Left)) {
//	//			Unit2Di m = { 0, 0 };
//	//			m = input.getMouseMove();
//	//			v.z() = m.y * camera.position.z() / camera.size.x;
//	//			v.y() = m.x * camera.position.z() / camera.size.x;
//	//		}
//	//		v.x() += world.input.getMouseWheelValue();
//	//	}
//	//	camera.translate(v);
//	//});
//	FaceRigObject* faceRigObject = new FaceRigObject();
//	world += faceRigObject;
//}

//void spawnFPS(Camera& camera) {
//	Importer imp = Importer("Content/Garden/Character/FPSArm/FPSArm.fbx");
//	SkeletonMesh* armm = new SkeletonMesh();
//	Character *arm = NULL;
//	if (imp.getSkeletonMesh(*armm)) {
//		vector<AnimationClipData*> anims;
//		imp.getAnimation(anims);
//		Material &mat = *getAssetByPath<Material>("Content/Garden/Character/FPSArm/FPSArmMat.imat");
//		arm = new Character(*armm, mat, "FPSArm");
//		for (int i = 0; i < anims.size(); i++) {
//			arm->addAnimationClip(*anims[i]);
//		}
//		Importer impC = Importer("Content/Garden/Character/FPSArm/FPS_Arm_Carbine_Anim.fbx");
//		anims.clear();
//		if (impC.getAnimation(anims)) {
//			for (int i = 0; i < anims.size(); i++) {
//				arm->addAnimationClip(*anims[i]);
//			}
//		}
//
//		arm->bindBegin([](Object* obj) {
//			SkeletonMeshActor* a = (SkeletonMeshActor*)obj;
//			a->activeAnimationClip(0);
//			if (a->animationClip != NULL) {
//				a->animationClip->play();
//			}
//			SkeletonMeshActor* revolverGun = (SkeletonMeshActor*)a->findChild("FPSRevolver");
//			if (revolverGun != NULL) {
//				revolverGun->activeAnimationClip(0);
//				if (revolverGun->animationClip != NULL) {
//					revolverGun->animationClip->play();
//				}
//			}
//			SkeletonMeshActor* carbineGun = (SkeletonMeshActor*)a->findChild("FPSCarbine");
//			if (carbineGun != NULL) {
//				carbineGun->activeAnimationClip(0);
//				if (carbineGun->animationClip != NULL) {
//					carbineGun->animationClip->play();
//				}
//				carbineGun->setHidden(true);
//			}
//		});
//		arm->bindTick([](Object* obj, float dt) {
//			SkeletonMeshActor* a = (SkeletonMeshActor*)obj;
//			static SkeletonMeshActor* revolverGun = (SkeletonMeshActor*)a->findChild("FPSRevolver");
//			static SkeletonMeshActor* carbineGun = (SkeletonMeshActor*)a->findChild("FPSCarbine");
//			static SkeletonMeshActor* gun = revolverGun;
//			static bool fire = false;
//			static bool reload = false;
//			static bool melee = false;
//			static bool takeAway = false;
//			static bool takeOut = false;
//			static auto checkFunc = []()->bool { return !fire && !reload && !melee && !takeAway && !takeOut; };
//			if (world.input.getCursorHidden()) {
//				if (world.input.getMouseButtonDown(MouseButtonEnum::Left)) {
//					if (gun == revolverGun) {
//						if (checkFunc()) {
//							if (a->animationClip != NULL && !a->animationClip->playing()) {
//								a->activeAnimationClip("Revolver_Shoot");
//								a->animationClip->play();
//								gun->activeAnimationClip("Shoot");
//								gun->animationClip->play();
//								gun->events("Fire");
//								if (gun->audioSources.size() > 0) {
//									gun->audioSources[0]->setVolume(0.8);
//									gun->audioSources[0]->play();
//								}
//							}
//							fire = true;
//						}
//					}
//					else {
//						if (!reload && !melee && !takeAway && !takeOut) {
//							if (a->animationClip != NULL && !a->animationClip->playing()) {
//								a->activeAnimationClip("Carbine_Shoot");
//								a->animationClip->setSpeed(2);
//								a->animationClip->play();
//								gun->activeAnimationClip("Shoot");
//								gun->animationClip->setSpeed(2);
//								gun->animationClip->play();
//								gun->events("Fire");
//								if (gun->audioSources.size() >= 6) {
//									int ra = rand() % 4;
//									ra = ra > 0 ? (ra + 2) : ra;
//									gun->audioSources[ra]->play();
//								}
//							}
//							fire = true;
//						}
//					}
//				}
//				else {
//					if (gun == revolverGun)
//						fire = false;
//					else {
//						if (fire && a->animationClip != NULL && !a->animationClip->playing()) {
//							fire = false;
//						}
//					}
//				}
//
//				if (world.input.getKeyPress('R')) {
//					if (checkFunc()) {
//						if (a->animationClip != NULL && !a->animationClip->playing()) {
//							if (gun == revolverGun)
//								a->activeAnimationClip("Revolver_Reload");
//							else
//								a->activeAnimationClip("Carbine_Reload");
//							a->animationClip->play();
//							gun->activeAnimationClip("Reload");
//							gun->animationClip->play();
//							if (gun->audioSources.size() > 1) {
//								gun->audioSources[1]->setVolume(0.8);
//								gun->audioSources[1]->play();
//							}
//						}
//						reload = true;
//					}
//				}
//				else {
//					reload = false;
//				}
//				if (world.input.getKeyPress('V')) {
//					if (checkFunc()) {
//						if (a->animationClip != NULL && !a->animationClip->playing()) {
//							if (gun == revolverGun)
//								a->activeAnimationClip("Revolver_Melee");
//							else
//								a->activeAnimationClip("Carbine_Melee");
//							a->animationClip->play();
//						}
//						melee = true;
//					}
//				}
//				else {
//					melee = false;
//				}
//				if (world.input.getKeyPress('T')) {
//					if (checkFunc()) {
//						if (a->animationClip != NULL && !a->animationClip->playing()) {
//							takeAway = true;
//							if (gun == revolverGun)
//								a->activeAnimationClip("Revolver_TakeAway");
//							else
//								a->activeAnimationClip("Carbine_TakeAway");
//							a->animationClip->play();
//						}
//					}
//				}
//			}
//			if (takeAway && a->animationClip != NULL && !a->animationClip->playing()) {
//				takeAway = false;
//				takeOut = true;
//				gun->setHidden(true);
//				if (gun == revolverGun) {
//					gun = carbineGun;
//					a->activeAnimationClip("Carbine_TakeOut");
//				}
//				else {
//					gun = revolverGun;
//					a->activeAnimationClip("Revolver_TakeOut");
//				}
//				a->animationClip->play();
//				gun->setHidden(false);
//				gun->activeAnimationClip("TakeOut");
//				gun->animationClip->play();
//				if (gun->audioSources.size() >= 3)
//					gun->audioSources[2]->play();
//			}
//			if (takeOut && a->animationClip != NULL && !a->animationClip->playing()) {
//				takeOut = false;
//			}
//		});
//
//		arm->setPosition(4.07, -2.62, -3.66);
//		arm->setRotation(90, 90, 0);
//		arm->setScale(0.2, 0.2, 0.2);
//		camera.addChild(*arm);
//	}
//	else {
//		delete armm;
//	}
//
//	if (arm != NULL) {
//		Importer impg = Importer("Content/Garden/Character/FPSArm/FPSRevolver.fbx");
//		SkeletonMesh* revolverm = new SkeletonMesh();
//		if (impg.getSkeletonMesh(*revolverm)) {
//			vector<AnimationClipData*> anims;
//			impg.getAnimation(anims);
//
//			Material* rmat = getAssetByPath<Material>("Content/Garden/Character/FPSArm/FPSRevolverMat.imat");
//
//			SkeletonMeshActor &revolver = *new SkeletonMeshActor(*revolverm, *rmat, "FPSRevolver");
//			revolver.skeletonMeshRender.setMaterial("revolver", *rmat, true);
//			for (int i = 0; i < anims.size(); i++) {
//				revolver.addAnimationClip(*anims[i]);
//			}
//			revolver.setPosition(-0.091, 1.985, -8.589);
//			revolver.setRotation(173.524, 4.323, -123.535);
//
//			revolver.events.registerFunc("Fire", [](void* ptr) {
//				SkeletonMeshActor* obj = (SkeletonMeshActor*)ptr;
//				Camera& cam = world.getCurrentCamera();
//				Vector3f pos = cam.getPosition(WORLD);
//				Vector3f forward = cam.getForward(WORLD);
//				vector<ContactInfo> ress;
//				if (world.physicalWorld.rayTestAll(pos, pos + forward * 1500, ress)) {
//					for (int i = 0; i < ress.size(); i++) {
//						if (ress[i].otherObject->rigidBody->material.physicalType != NOCOLLISIOIN) {
//							ContactInfo &res = ress[i];
//							Console::log("Hit %s", res.objectName.c_str());
//							if (res.otherObject->rigidBody->material.physicalType == DYNAMIC) {
//								Vector3f rel = res.location;
//								//rel = toVector3f(res.otherObject->getWorldTransform().inverse() * toPVec3(rel));
//								//res.otherObject->rigidBody->applyImpulse(toPVec3(res.impact * 100000), toPVec3(rel));
//							}
//							break;
//						}
//					}
//				}
//			});
//
//			AudioData &fireAudio = *new AudioData("revolverfire", "Content/Garden/Character/FPSArm/revolverfire.wav");
//			if (revolver.addAudioSource(fireAudio) == NULL) {
//				delete &fireAudio;
//			}
//
//			AudioData &reloadAudio = *new AudioData("revolverreload", "Content/Garden/Character/FPSArm/revolverreload.wav");
//			if (revolver.addAudioSource(reloadAudio) == NULL) {
//				delete &reloadAudio;
//			}
//
//			AudioData &readyAudio = *new AudioData("revolverready", "Content/Garden/Character/FPSArm/revolverReady.wav");
//			if (revolver.addAudioSource(readyAudio) == NULL) {
//				delete &readyAudio;
//			}
//
//			Bone* bone = arm->getBone("R_wrist");
//			if (bone != NULL) {
//				bone->addChild(revolver);
//			}
//
//			//ParticleSystem& testPS = *new ParticleSystem("TestPS");
//			//world.addChild(testPS);
//			//testPS.setPosition(0, 8.14, 34.15);
//			//ParticleEmitter& emitter = testPS.particleRender.addEmitter();
//			//emitter.initPosition = Vector3f(0, 0, 0);
//			//emitter.initColor = Color(0.1f, 0.1f, 0.1f);
//			////emitter.initScale.minVal = Vector3f(1, 1, 1);
//			////emitter.initScale.maxVal = Vector3f(2, 2, 2);
//			//emitter.initScale = Vector3f(5, 5, 5);
//			//emitter.initVelocity.minVal = Vector3f(-200, -200, -200);
//			//emitter.initVelocity.maxVal = Vector3f(200, 200, 200);
//			//emitter.acceleration = Vector3f(0, 0, 0);
//			//emitter.lifetime = 2;
//			//emitter.screenAlignmentType = emitter.Velocity;
//			//emitter.colorCurve.insert(0, CurveValue<Color>(CurveValue<Color>::Linear, Color(0.1f, 0.1f, 0.1f, 0.3f)));
//			//emitter.colorCurve.insert(0.3, CurveValue<Color>(CurveValue<Color>::Linear, Color(1.0f, 1.0f, 1.0f, 0.6f)));
//			//emitter.colorCurve.insert(1, CurveValue<Color>(CurveValue<Color>::Linear, Color(1.0f, 1.0f, 0.f, 0.f)));
//			///*emitter.scaleCurve.insert(0, CurveValue<Vector3f>(CurveValue<Vector3f>::Linear, Vector3f(0.5, 0.5, 0.5)));
//			//emitter.scaleCurve.insert(1, CurveValue<Vector3f>(CurveValue<Vector3f>::Linear, Vector3f(0, 0, 0)));*/
//			//emitter.emitCountSec = 1000;
//			//emitter.maxCount = 10000;
//			//Material& pMat = Material::defaultParticleMaterial.instantiate();
//			//Texture2D* starTex = getAssetByPath<Texture2D>("Engine/Textures/StarTexP.tga");
//			//if (starTex != NULL) {
//			//	pMat.setTexture("colorMap", *starTex);
//			//}
//			//Texture2D* noiseTex = getAssetByPath<Texture2D>("Engine/Textures/SmokeNoise.png");
//			//if (noiseTex != NULL) {
//			//	pMat.setTexture("noiseMap", *noiseTex);
//			//}
//			//emitter.setMaterial(pMat);
//			//testPS.activate();
//		}
//		else {
//			delete revolverm;
//		}
//
//		Importer impc = Importer("Content/Garden/Character/FPSArm/FPSCarbine.fbx");
//		SkeletonMesh* carbinem = new SkeletonMesh();
//		if (impc.getSkeletonMesh(*carbinem)) {
//			vector<AnimationClipData*> anims;
//			impc.getAnimation(anims);
//
//			Material* rmat = getAssetByPath<Material>("Content/Garden/Character/FPSArm/FPSCarbineMat.imat");
//
//			SkeletonMeshActor &carbine = *new SkeletonMeshActor(*carbinem, *rmat, "FPSCarbine");
//			carbine.skeletonMeshRender.setMaterial("carbine", *rmat, true);
//			for (int i = 0; i < anims.size(); i++) {
//				carbine.addAnimationClip(*anims[i]);
//			}
//			carbine.setPosition(1.571, 2.218, -9.899);
//			carbine.setRotation(168.556, 0.795, -124.251);
//
//			carbine.events.registerFunc("Fire", [](void* ptr) {
//				SkeletonMeshActor* obj = (SkeletonMeshActor*)ptr;
//				Camera& cam = world.getCurrentCamera();
//				Vector3f pos = cam.getPosition(WORLD);
//				Vector3f forward = cam.getForward(WORLD);
//				vector<ContactInfo> ress;
//				if (world.physicalWorld.rayTestAll(pos, pos + forward * 1500, ress)) {
//					for (int i = 0; i < ress.size(); i++) {
//						if (ress[i].otherObject->rigidBody->material.physicalType != NOCOLLISIOIN) {
//							ContactInfo &res = ress[i];
//							Console::log("Hit %s", res.objectName.c_str());
//							if (res.otherObject->rigidBody->material.physicalType == DYNAMIC) {
//								Vector3f rel = res.location;
//								//rel = toVector3f(res.otherObject->getWorldTransform().inverse() * toPVec3(rel));
//								//res.otherObject->rigidBody->applyImpulse(toPVec3(res.impact * 80000), toPVec3(rel));
//							}
//							break;
//						}
//					}
//				}
//			});
//
//			AudioData &fireAudio1 = *new AudioData("carbinefire1", "Content/Garden/Character/FPSArm/M16Shot1.wav");
//			if (carbine.addAudioSource(fireAudio1) == NULL) {
//				delete &fireAudio1;
//			}
//
//			AudioData &reloadAudio = *new AudioData("carbinereload", "Content/Garden/Character/FPSArm/carbineReloadB.wav");
//			if (carbine.addAudioSource(reloadAudio) == NULL) {
//				delete &reloadAudio;
//			}
//
//			AudioData &readyAudio = *new AudioData("carbineready", "Content/Garden/Character/FPSArm/carbineReady.wav");
//			if (carbine.addAudioSource(readyAudio) == NULL) {
//				delete &readyAudio;
//			}
//
//			AudioData &fireAudio2 = *new AudioData("carbinefire2", "Content/Garden/Character/FPSArm/M16Shot2.wav");
//			if (carbine.addAudioSource(fireAudio2) == NULL) {
//				delete &fireAudio2;
//			}
//
//			AudioData &fireAudio3 = *new AudioData("carbinefire3", "Content/Garden/Character/FPSArm/M16Shot3.wav");
//			if (carbine.addAudioSource(fireAudio3) == NULL) {
//				delete &fireAudio3;
//			}
//
//			AudioData &fireAudio4 = *new AudioData("carbinefire4", "Content/Garden/Character/FPSArm/M16Shot4.wav");
//			if (carbine.addAudioSource(fireAudio4) == NULL) {
//				delete &fireAudio4;
//			}
//
//			Bone* bone = arm->getBone("R_wrist");
//			if (bone != NULL) {
//				bone->addChild(carbine);
//			}
//		}
//		else {
//			delete carbinem;
//		}
//	}
//}

//void spawnRin(Camera& camera) {
//	SerializationInfo* info = getAssetByPath<SerializationInfo>("Content/Scene/YYBRin/YYBRin.asset");
//	if (info == NULL)
//		return;
//	string path = info->path;
//	info->path = "YYBRinMesh";
//	Serializable* ser = info->serialization->instantiate(*info);
//	info->path = path;
//	if (ser == NULL)
//		return;
//	SkeletonMeshActor* rin = dynamic_cast<SkeletonMeshActor*>(ser);
//	rin->deserialize(*info);
//	if (rin == NULL) {
//		delete ser;
//		return;
//	}
//
//	AnimationClipData* idleAnim = getAssetByPath<AnimationClipData>("Content/Scene/YYBRin/YYBRin_Idle.fbx:Animation0");
//	if (idleAnim == NULL) {
//		delete ser;
//		return;
//	}
//	AnimationClipData* walkAnim = getAssetByPath<AnimationClipData>("Content/Scene/YYBRin/YYBRin_Walk.fbx:Animation0");
//	if (walkAnim == NULL) {
//		delete ser;
//		return;
//	}
//	AnimationClipData* runAnim = getAssetByPath<AnimationClipData>("Content/Scene/YYBRin/YYBRin_Run.fbx:Animation0");
//	if (runAnim == NULL) {
//		delete ser;
//		return;
//	}
//
//	BlendSpaceAnimation& blendSpace = *rin->addBlendSpaceAnimation("Move");
//	blendSpace.addAnimationClipData({ 0, 0 }, *idleAnim);
//	blendSpace.addAnimationClipData({ 0.2, 0 }, *walkAnim);
//	blendSpace.addAnimationClipData({ 1, 0 }, *runAnim);
//	rin->activeAnimationClip("Move");
//	blendSpace.play();
//
//	::Transform& character = *new ::Transform("YYBRin");
//	character.addChild(*rin);
//	rin->setPosition(0, 0, 0);
//	rin->setRotation(90, 0, 90);
//	character.updataRigidBody(new Capsule(4.5, 22, { 0, 0, 22 }), SIMPLE, PhysicalMaterial(10000, DYNAMIC));
//	character.setPosition(0, -150, 25);
//	character.rigidBody->setAngularFactor(0);
//	character.rigidBody->setFriction(10);
//	::Transform& camHandle = *new ::Transform("CamHandle");
//	camHandle.setPosition(0, 0, 30);
//	character.addChild(camHandle);
//	camHandle.addChild(camera);
//	camera.setRotation(0, 0, 0);
//	camera.setPosition(-60, 0, 0);
//	character.bindTick([](Object* obj, float dt) {
//		if (world.input.getCursorHidden()) {
//			::Transform& p = *(::Transform*)obj;
//			SkeletonMeshActor& rin = *(::SkeletonMeshActor*)p.findChild("YYBRinMesh");
//			::Transform& camHandle = *(::Transform*)p.findChild("CamHandle");
//			Camera& cam = *(Camera*)camHandle.findChild("PostProcessingCamera");
//
//			Vector3f from = p.getPosition(WORLD) - Vector3f(0, 0, p.rigidBody->shape->getHeight() / 2 + 0.5);
//			Vector3f to = from - Vector3f(0, 0, 1);
//			ContactInfo re;
//			bool isFly = false;
//			if (world.physicalWorld.rayTest(from, to, re))
//				isFly = true;
//			Vector3f v, v1;
//			v = cam.getForward(WORLD) * ((int)world.input.getKeyState('W') - (int)world.input.getKeyState('S')) +
//				cam.getRightward(WORLD) * ((int)world.input.getKeyState('A') - (int)world.input.getKeyState('D'));
//			v.z() = 0;
//			v.normalize();
//			v1 = v;
//			v *= dt * 60 * (isFly ? 0.2 : 1);
//			p.rigidBody->setGravity({ 0, 0, -15 });
//			Vector3f cfw = rin.getUpward(WORLD);
//			cfw.z() = 0;
//			cfw.normalize();
//			Quaternionf rotation = Quaternionf::FromTwoVectors(cfw, v);
//			rin.rotate(Quaternionf::Identity().slerp(min(5 * dt, 1), rotation));
//			p.rigidBody->translate(toPVec3(v));
//
//			static float speed = 0;
//			if (v.x() != 0 || v.y() != 0)
//			{
//				speed += dt * 3;
//				speed = min(1, speed);
//			}
//			else {
//				speed -= dt * 5;
//				speed = max(0, speed);
//			}
//			BlendSpaceAnimation& blendSpace = *(BlendSpaceAnimation*)rin.animationClip;
//			blendSpace.setBlendWeight({ speed, 0 });
//
//			Unit2Di m = world.input.getMouseMove();
//			camHandle.rotate(0, 0, -m.x * dt * 5, WORLD);
//			camHandle.rotate(0, m.y * dt * 5, 0);
//
//			static bool down = false;
//			if (world.input.getKeyState(' ') == Down) {
//				if (!down && !isFly) {
//					v1.z() = 200000;
//					p.rigidBody->applyCentralImpulse(toPVec3(v1));
//				}
//				down = true;
//			}
//			if (world.input.getKeyState(' ') == Up) {
//				down = false;
//			}
//		}
//	});
//	world += character;
//}

void spawnRin(Camera& camera) {
	SerializationInfo* info = getAssetByPath<SerializationInfo>("Content/Scene/YYBRin/YYBRin.asset");
	if (info == NULL)
		return;
	string name = info->name;
	info->name = "YYBRinMesh";
	Serializable* ser = info->serialization->instantiate(*info);
	info->name = name;
	if (ser == NULL)
		return;
	SkeletonMeshActor* rin = dynamic_cast<SkeletonMeshActor*>(ser);
	rin->deserialize(*info);
	if (rin == NULL) {
		delete ser;
		return;
	}

	AnimationClipData* idleAnim = getAssetByPath<AnimationClipData>("Content/Scene/YYBRin/YYBRin_Idle.fbx:Animation0");
	if (idleAnim == NULL) {
		delete ser;
		return;
	}
	AnimationClipData* walkAnim = getAssetByPath<AnimationClipData>("Content/Scene/YYBRin/YYBRin_Walk.fbx:Animation0");
	if (walkAnim == NULL) {
		delete ser;
		return;
	}
	AnimationClipData* runAnim = getAssetByPath<AnimationClipData>("Content/Scene/YYBRin/YYBRin_Run.fbx:Animation0");
	if (runAnim == NULL) {
		delete ser;
		return;
	}

	BlendSpaceAnimation& blendSpace = *rin->addBlendSpaceAnimation("Move");
	blendSpace.addAnimationClipData({ 0, 0 }, *idleAnim);
	blendSpace.addAnimationClipData({ 0.2, 0 }, *walkAnim);
	blendSpace.addAnimationClipData({ 1, 0 }, *runAnim);
	rin->activeAnimationClip("Move");
	blendSpace.play();

	Character& character = *new Character(Capsule(4.5, 22), "YYBRin");
	character.physicalController.gravityScale = { 10, 10, 10 };
	character.addChild(*rin);
	rin->setPosition(0, 0, -22);
	rin->setRotation(90, 0, 90);
	character.setPosition(100, 0, 100);
	::Transform& camHandle = *new ::Transform("CamHandle");
	camHandle.setPosition(0, 0, 30);
	character.addChild(camHandle);
	camHandle.addChild(camera);
	camera.setRotation(0, 0, 0);
	camera.setPosition(-60, 0, 0);
	character.bindTick([](Object* obj, float dt) {
		if (world.input.getCursorHidden()) {
			Character& p = *(Character*)obj;
			SkeletonMeshActor& rin = *(::SkeletonMeshActor*)p.findChild("YYBRinMesh");
			::Transform& camHandle = *(::Transform*)p.findChild("CamHandle");
			Camera& cam = *(Camera*)camHandle.findChild("PostProcessingCamera");
			if (&world.getCurrentCamera() != &cam)
				return;
			bool isFly = p.isFly();
			Vector3f v;
			v = cam.getForward(WORLD) * ((int)world.input.getKeyState('W') - (int)world.input.getKeyState('S')) +
				cam.getRightward(WORLD) * ((int)world.input.getKeyState('A') - (int)world.input.getKeyState('D'));
			v.z() = 0;
			v.normalize();
			bool canRun = world.input.getKeyDown(VK_SHIFT);
			v *= dt * (canRun ? 65 : 25) * (isFly ? 0.2 : 1);
			Vector3f cfw = rin.getUpward(WORLD);
			cfw.z() = 0;
			cfw.normalize();
			Quaternionf rotation = Quaternionf::FromTwoVectors(cfw, v);
			rin.rotate(Quaternionf::Identity().slerp(min(5 * dt, 1), rotation));
			p.move(v);

			static float springLength = 60;

			springLength += world.input.getMouseWheelValue();
			springLength = max(springLength, 0);
			PxSweepBuffer hit;
			if (world.physicalWorld.physicsScene->sweep(PxSphereGeometry(2), camHandle.getWorldTransform(),
				toPVec3(-camHandle.getForward(WORLD)), springLength, hit, PxHitFlag::eDEFAULT, PxQueryFilterData())) {
				if (hit.block.distance > 0)
					cam.setPosition(-hit.block.distance, 0, 0);
				else
					cam.setPosition(-springLength, 0, 0);
			}
			else {
				cam.setPosition(-springLength, 0, 0);
			}

			static float speed = 0;
			static float targetSpeed = 0;
			if (v.x() != 0 || v.y() != 0)
			{
				if (canRun)
					targetSpeed = 1;
				else
					targetSpeed = 0.2;
			}
			else {
				targetSpeed = 0;
			}

			float sign = targetSpeed >= speed ? 1 : -1;
			if (fabs(targetSpeed - speed) < dt * 3)
				speed = targetSpeed;
			else
				speed += sign * dt * 3;

			BlendSpaceAnimation& blendSpace = *(BlendSpaceAnimation*)rin.animationClip;
			blendSpace.setBlendWeight({ speed, 0 });

			Unit2Di m = world.input.getMouseMove();
			camHandle.rotate(0, 0, -m.x * dt * 5, WORLD);
			camHandle.rotate(0, m.y * dt * 5, 0);

			if (world.input.getKeyPress(VK_SPACE)) {
				if (!isFly) {
					p.jump(50);
				}
			}
		}
	});
	world += character;
}

VehicleActor* spawnVehicle(Camera& camera)
{
	Mesh* bodyMesh = getAssetByPath<Mesh>("Content/Car/CarChassis.fbx");
	if (bodyMesh == NULL)
		return NULL;
	Mesh* bodyCollisionMesh = getAssetByPath<Mesh>("Content/Car/CarChassis_Collision.fbx");
	if (bodyCollisionMesh == NULL)
		return NULL;
	Mesh* wheelLMesh = getAssetByPath<Mesh>("Content/Car/CarWheelL.fbx");
	if (wheelLMesh == NULL)
		return NULL;
	Mesh* wheelRMesh = getAssetByPath<Mesh>("Content/Car/CarWheelR.fbx");
	if (wheelRMesh == NULL)
		return NULL;
	Material* bodypartsM = getAssetByPath<Material>("Content/Car/Car_BodyPartsM.imat");
	if (bodypartsM == NULL)
		return NULL;
	Material* chassisM = getAssetByPath<Material>("Content/Car/Car_ChassisM.imat");
	if (chassisM == NULL)
		return NULL;
	Material* mainBodyM = getAssetByPath<Material>("Content/Car/Car_MainBodyM.imat");
	if (mainBodyM == NULL)
		return NULL;
	Material* wheelM = getAssetByPath<Material>("Content/Car/Car_WheelAndBottomM.imat");
	if (wheelM == NULL)
		return NULL;
	AudioData* engineIdleWav = getAssetByPath<AudioData>("Content/Car/Engine_Idle_loop.wav");
	if (engineIdleWav == NULL)
		return NULL;
	AudioData* engineRunWav = getAssetByPath<AudioData>("Content/Car/Engine_Run_loop.wav");
	if (engineRunWav == NULL)
		return NULL;

	VehicleInfo info;
	info.peakTorque = 12000.0f;
	info.maxOmega = 8000.0f;
	info.chassisMass = 1500;
	info.wheelFriction = 1;
	info.chassisCMOffset = { 2.674f, 0.0f, 38.21f };

	info.wheelInfos[FRONT_LEFT].type = FRONT_LEFT;
	info.wheelInfos[FRONT_LEFT].mesh = wheelLMesh;
	info.wheelInfos[FRONT_LEFT].material = wheelM;
	info.wheelInfos[FRONT_LEFT].wheelRadius = wheelLMesh->getRadius() + 0.1f;
	info.wheelInfos[FRONT_LEFT].wheelWidth = wheelLMesh->getWidth();
	info.wheelInfos[FRONT_LEFT].maxHandBrakeTorque = 0;
	info.wheelInfos[FRONT_LEFT].maxBrakeTorque = 16000;
	info.wheelInfos[FRONT_LEFT].wheelCenterActorOffset = { 36.9f, 29.357f, 9.832f };
	info.wheelInfos[FRONT_LEFT].wheelCenterActorOffset -= info.chassisCMOffset;

	info.wheelInfos[FRONT_RIGHT].type = FRONT_RIGHT;
	info.wheelInfos[FRONT_RIGHT].mesh = wheelRMesh;
	info.wheelInfos[FRONT_RIGHT].material = wheelM;
	info.wheelInfos[FRONT_RIGHT].wheelRadius = wheelRMesh->getRadius() + 0.1f;
	info.wheelInfos[FRONT_RIGHT].wheelWidth = wheelRMesh->getWidth();
	info.wheelInfos[FRONT_RIGHT].maxHandBrakeTorque = 0;
	info.wheelInfos[FRONT_RIGHT].maxBrakeTorque = 16000;
	info.wheelInfos[FRONT_RIGHT].wheelCenterActorOffset = { 36.9f, -29.357f, 9.832f };
	info.wheelInfos[FRONT_RIGHT].wheelCenterActorOffset -= info.chassisCMOffset;

	info.wheelInfos[REAR_LEFT].type = REAR_LEFT;
	info.wheelInfos[REAR_LEFT].mesh = wheelLMesh;
	info.wheelInfos[REAR_LEFT].material = wheelM;
	info.wheelInfos[REAR_LEFT].wheelRadius = wheelLMesh->getRadius() + 0.1f;
	info.wheelInfos[REAR_LEFT].wheelWidth = wheelLMesh->getWidth();
	info.wheelInfos[REAR_LEFT].maxSteer = 0;
	info.wheelInfos[REAR_LEFT].maxHandBrakeTorque = 16000;
	info.wheelInfos[REAR_LEFT].maxBrakeTorque = 16000;
	info.wheelInfos[REAR_LEFT].wheelCenterActorOffset = { -39.5f, 29.357f, 9.832f };
	info.wheelInfos[REAR_LEFT].wheelCenterActorOffset -= info.chassisCMOffset;

	info.wheelInfos[REAR_RIGHT].type = REAR_RIGHT;
	info.wheelInfos[REAR_RIGHT].mesh = wheelRMesh;
	info.wheelInfos[REAR_RIGHT].material = wheelM;
	info.wheelInfos[REAR_RIGHT].wheelRadius = wheelRMesh->getRadius() + 0.1f;
	info.wheelInfos[REAR_RIGHT].wheelWidth = wheelRMesh->getWidth();
	info.wheelInfos[REAR_RIGHT].maxSteer = 0;
	info.wheelInfos[REAR_RIGHT].maxHandBrakeTorque = 16000;
	info.wheelInfos[REAR_RIGHT].maxBrakeTorque = 16000;
	info.wheelInfos[REAR_RIGHT].wheelCenterActorOffset = { -39.5f, -29.357f, 9.832f };
	info.wheelInfos[REAR_RIGHT].wheelCenterActorOffset -= info.chassisCMOffset;

	info.chassisCMOffset = { 0, 0, 0 };

	VehicleActor& vehicle = *new VehicleActor(info, *bodyMesh, *bodypartsM, bodyCollisionMesh, COMPLEX, "Vehicle");
	vehicle.meshRender.setMaterial("BodyPartsM", *bodypartsM);
	vehicle.meshRender.setMaterial("HodovajaChiastM", *chassisM);
	vehicle.meshRender.setMaterial("MainBodyM", *mainBodyM);
	vehicle.meshRender.setMaterial("WheelsAndBottomM", *wheelM);

	AudioSource &idleAu = *vehicle.addAudioSource(*engineIdleWav);
	AudioSource &runAu = *vehicle.addAudioSource(*engineRunWav);
	idleAu.setVolume(0);
	idleAu.setLoop(true);
	idleAu.play();
	runAu.setVolume(0);
	runAu.setLoop(true);
	runAu.play();

	vehicle.setPosition(200, 0, 200);

	::Transform& camHandle = *new ::Transform("VehicleCamHandle");
	camHandle.setPosition(0, 0, 20);
	vehicle.addChild(camHandle);
	camHandle.addChild(camera);
	camera.setRotation(0, 0, 0);
	camera.setPosition(-60, 0, 0);
	vehicle.bindTick([](Object* obj, float dt) {
		VehicleActor& vehicle = *(VehicleActor*)obj;
		::Transform& camHandle = *(::Transform*)obj->findChild("VehicleCamHandle");
		Camera& cam = *(Camera*)camHandle.findChild("VehicleCamera");
		if (&world.getCurrentCamera() != &cam)
			return;
		bool fb = world.input.getKeyDown('W');
		bool bb = world.input.getKeyDown('S');
		bool sb = world.input.getKeyDown(VK_SPACE);
		if (fb && !bb)
			vehicle.PressAccelerator(false);
		if (bb && !fb)
			vehicle.PressAccelerator(true);
		if (fb && bb)
			vehicle.PressBrake();
		if (!fb && !bb) {
			vehicle.ReleaseAccelerator();
			if (!sb)
				vehicle.ReleaseBrake();
		}
		if (sb)
			vehicle.PressBrake();
		if (world.input.getKeyDown(VK_SHIFT))
			vehicle.PressHandBrake();
		else
			vehicle.ReleaseHandBrake();
		
		if (world.input.getKeyPress(VK_UP))
			vehicle.PressGearUp();
		if (world.input.getKeyRelease(VK_UP))
			vehicle.ReleaseGearUp();
		if (world.input.getKeyPress(VK_DOWN))
			vehicle.PressGearDown();
		if (world.input.getKeyRelease(VK_DOWN))
			vehicle.ReleaseGearDown();

		float steering = (int)world.input.getKeyDown('A') - (int)world.input.getKeyDown('D');
		vehicle.SetSteering(steering);

		float runV = 0, runP;
		runV = (vehicle.vehicleState.engineRotationSpeed - 100.0f) / 200.0f;
		runP = runV / 10.0f;
		runV = min(max(runV, 0), 1);
		runP = max(runP, 0);

		vehicle.audioSources[0]->setVolume(1 - runV);
		vehicle.audioSources[1]->setVolume(runV);
		vehicle.audioSources[1]->setPitch(runP + 1);
	});
	camHandle.bindTick([](Object* obj, float dt) {
		if (world.input.getCursorHidden()) {
			::Transform& camHandle = *(::Transform*)obj;
			Camera& cam = *(Camera*)camHandle.findChild("VehicleCamera");

			static float springLength = 60;

			springLength += world.input.getMouseWheelValue();
			springLength = max(springLength, 0);
			PxSweepBuffer hit;
			if (world.physicalWorld.physicsScene->sweep(PxSphereGeometry(2), camHandle.getWorldTransform(),
				toPVec3(-camHandle.getForward(WORLD)), springLength, hit, PxHitFlag::eDEFAULT, PxQueryFilterData())) {
				if (hit.block.distance > 0)
					cam.setPosition(-hit.block.distance, 0, 0);
				else
					cam.setPosition(-springLength, 0, 0);
			}
			else {
				cam.setPosition(-springLength, 0, 0);
			}

			Unit2Di m = world.input.getMouseMove();
			camHandle.rotate(0, 0, -m.x * dt * 5, WORLD);
			camHandle.rotate(0, m.y * dt * 5, 0);
		}
	});
	world += vehicle;
	return &vehicle;
}

#define DEBUG_CAM 1

void InitialWorld() {
	static PostProcessingCamera& camera = *new PostProcessingCamera();
	camera.postProcessCameraRender.addExtraRenderTex();
	world.switchCamera(camera);
#if DEBUG_CAM
	static float cameraSpeed = 1;
	camera.updataRigidBody(new Sphere(5));
	world += camera;
	camera.setPosition(0, 0, 100);
	camera.events.registerFunc("SetSpeed", [](void* p, float v) {
		cameraSpeed = v;
	});
#else
	//static PostProcessingCamera& vcamera = *new PostProcessingCamera("VehicleCamera");
	//world.switchCamera(vcamera);
	//spawnFPS(camera);
	spawnRin(camera);
	//static VehicleActor* vehicle = spawnVehicle(vcamera);
#endif // DEBUG_CAM

	//Material& woodBoxMat = *getAsset<Material>("Material", "Plain_Metal");
	//vector<MeshActor*> boxes = newStdMeshes("Content/Scene", "WoodBox", woodBoxMat, PhysicalMaterial(1000, DYNAMIC), 500);
	//
	//for (int i = 0; i < boxes.size(); i++) {
	//	if (boxes[i] != NULL) {
	//		//boxes[i]->updataRigidBody(new Sphere(boxes[i]->meshRender.mesh.getRadius()), SIMPLE, PhysicalMaterial(1000, DYNAMIC));
	//		world += boxes[i];
	//		boxes[i]->setPosition(150, 0, 200);
	//	}
	//}

	Material* grassM1 = getAssetByPath<Material>("Content/Scene/Material/GrassM1.imat");
	Material* grassM2 = getAssetByPath<Material>("Content/Scene/Material/GrassM2.imat");
	Material* grassM3 = getAssetByPath<Material>("Content/Scene/Material/GrassM3.imat");
	Mesh* grassMesh = getAssetByPath<Mesh>("Content/Scene/Mesh/Grass.fbx");
	
	if (grassM1 != NULL && grassM2 != NULL && grassM3 != NULL && grassMesh != NULL) {
		grassM1->setTwoSide(true);
		grassM2->setTwoSide(true);
		grassM3->setTwoSide(true);
		for (int i = 0; i < 2; i++) {
			GrassMeshActor &grass = *new GrassMeshActor(*grassMesh, *grassM1, "Grass" + (i == 0 ? "" : to_string(i)));
			grass.meshRender.setMaterial(0, *grassM1);
			grass.meshRender.setMaterial(1, *grassM2);
			grass.meshRender.setMaterial(2, *grassM3);
			grass.set(120, Vector2i(1000, 1000));
			grass.setPosition(0, 0, 0);
			grass.bindTick([](Object* obj, float dt) {
				float time = Engine::getCurrentWorld()->getEngineTime() / 1000.f;
				obj->getRender()->getMaterial(0)->setScalar("time", time);
				obj->getRender()->getMaterial(1)->setScalar("time", time);
				obj->getRender()->getMaterial(2)->setScalar("time", time);
			});
			world += grass;
		}
	}

	Material &pbr_mat = *getAssetByPath<Material>("Engine/Shaders/PBR.mat");

	/*Material& woodBoxMat = *newStdMat("Content/Scene", "WoodBox", pbr_mat);
	vector<MeshActor*> boxes = newStdMeshes("Content/Scene", "WoodBox", woodBoxMat, PhysicalMaterial(10, DYNAMIC), 100);
	
	for (int i = 0; i < boxes.size(); i++) {
		if (boxes[i] != NULL) {
			world += boxes[i];
			boxes[i]->setPosition(-150, 0, 100 + i * 10);
		}
	}*/

	DirectLight &dirLight = *new DirectLight("DirLight", { 255, 255, 200 }, 1.5);
	camera.setVolumnicLight(dirLight);
#if DEBUG_CAM

#else
	/*vcamera.setVolumnicLight(dirLight);
	camera.bindTick([](Object* s, float dt) {
		Input& input = world.input;
		Camera& cam = *(Camera*)s;
		if (&world.getCurrentCamera() != &cam)
			return;
		if (input.getKeyPress('O'))
			world.switchCamera(vcamera);
	});

	vcamera.bindTick([](Object* s, float dt) {
		Input& input = world.input;
		Camera& cam = *(Camera*)s;
		if (&world.getCurrentCamera() != &cam)
			return;
		if (input.getKeyPress('P'))
			world.switchCamera(camera);
	});*/

#endif

	dirLight.intensity = 1.5;
	world += dirLight;
	SkySphere &sky = *new SkySphere(dirLight);
	sky.loadDefaultTexture();
	sky.scaling(400, 400, 400);
	world += sky;
	world.input.keyboardInputHandler += [](Input* input, char key, InputStateEnum state) {
		if (!input->getCursorHidden()/* | world.input.getKeyState(VK_CONTROL) == Up*/)
			return;
		if (key == 'F' && state == OnRelease)
#ifdef __FREEGLUT_H__
			glutFullScreenToggle();
#endif
#ifdef _glfw3_h_
		{
			Engine::toggleFullscreen();
		}
#endif
		if (key == 'G' && state == Down && world.physicalWorld.getGravity().z() < 0)
			world.physicalWorld.setGravity({ 0, 0, 10.f });
		if (key == 'G' && state == Up && world.physicalWorld.getGravity().z() > 0)
			world.physicalWorld.setGravity({ 0, 0, -10.f });
	};

#if DEBUG_CAM
	world.getDefaultCamera().bindTick([](Object* s, float dt) {
		Input& input = world.input;
		Camera& cam = *(Camera*)s;
		if (&world.getCurrentCamera() != &cam)
			return;
		if (input.getKeyPress('O'))
			world.switchCamera(camera);
		if ((input.getCursorHidden() || input.getMouseButtonDown(MouseButtonEnum::Right))) {
			Vector3f v(0, 0, 0);
			v.x() = (int)input.getKeyDown('W') - (int)input.getKeyDown('S');
			v.x() += world.input.getMouseWheelValue();
			v.y() = (int)input.getKeyDown('A') - (int)input.getKeyDown('D');
			v.z() = (int)input.getKeyDown('E') - (int)input.getKeyDown('Q');
			v *= dt * 30 * cameraSpeed;
			cam.translate(v.x(), v.y(), 0);
			cam.translate(0, 0, v.z(), WORLD);

			Unit2Di m = input.getMouseMove();
			cam.rotate(0, 0, -m.x * dt * 5, WORLD);
			cam.rotate(0, m.y * dt * 5, 0);
		}
	});
	world.getCurrentCamera().bindTick([](Object* s, float dt) {
		Input& input = world.input;
		Camera& cam = *(Camera*)s;
		if (&world.getCurrentCamera() != &cam)
			return;
		if (input.getKeyPress('P'))
			world.switchToDefaultCamera();
		if ((input.getCursorHidden() || input.getMouseButtonDown(MouseButtonEnum::Right))) {
			Vector3f v(0, 0, 0);
			v.x() = (int)input.getKeyDown('W') - (int)input.getKeyDown('S');
			v.x() += world.input.getMouseWheelValue();
			v.y() = (int)input.getKeyDown('A') - (int)input.getKeyDown('D');
			v.z() = (int)input.getKeyDown('E') - (int)input.getKeyDown('Q');
			v *= dt * 30 * cameraSpeed;
			cam.translate(v.x(), v.y(), 0);
			cam.translate(0, 0, v.z(), WORLD);

			Unit2Di m = input.getMouseMove();
			cam.rotate(0, 0, -m.x * dt * 5, WORLD);
			cam.rotate(0, m.y * dt * 5, 0);

			/*static RigidBody* target = NULL;
			static btGeneric6DofSpring2Constraint* constraint = NULL;
			if (input.getMouseButtonState(MouseButtonEnum::Left) == InputStateEnum::Down) {
				Vector3f pos = cam.getPosition(WORLD);
				Vector3f forward = cam.getForward(WORLD);
				if (target == NULL) {
					ContactInfo res;
					if (world.physicalWorld.rayTest(pos, pos + forward * 1500, res)) {
						RigidBody* obj = dynamic_cast<RigidBody*>((PhysicalBody*)res.physicalObject);
						if (obj != NULL) {
							Console::log("RayTest: %s", res.objectName);
							if (cam.rigidBody != NULL) {
								target = obj;
								float distance = (obj->targetTransform.getPosition(WORLD) - pos).norm();
								Vector3f rel = res.location;
								rel = toVector3f(obj->getWorldTransform().inverse() * toPVec3(rel));
								PTransform at = PTransform::getIdentity(), bt = PTransform::getIdentity();
								at.setOrigin({ distance, 0, 0 });
								bt.setOrigin(toPVec3(rel));
								constraint = new btGeneric6DofSpring2Constraint(*cam.rigidBody, *obj, at, bt);
								constraint->enableSpring(3, true);
								constraint->setStiffness(3, 35.0f);
								constraint->setDamping(3, 0.5f);
								constraint->enableSpring(4, true);
								constraint->setStiffness(4, 35.0f);
								constraint->setDamping(4, 0.5f);
								constraint->enableSpring(5, true);
								constraint->setStiffness(5, 35.0f);
								constraint->setDamping(5, 0.5f);

								cam.rigidBody->addConstraintRef(constraint);
								obj->addConstraintRef(constraint);

								world.physicalWorld.physicsScene->addConstraint(constraint);
							}
						}
					}
				}
			}
			if (input.getMouseButtonState(MouseButtonEnum::Left) == InputStateEnum::Up) {
				if (cam.rigidBody != NULL && target != NULL) {
					cam.rigidBody->removeConstraintRef(constraint);
					target->removeConstraintRef(constraint);
					world.physicalWorld.physicsScene->removeConstraint(constraint);
					delete constraint;
					target = NULL;
					constraint = NULL;
				}
			}*/
		}
	});
#endif
	world.input.setCursorHidden(true);

	EditorWindow& editorWindow = *new EditorWindow(world, pbr_mat);
	editorWindow.blurBackground = true;
	editorWindow.showCloseButton = false;
	world += editorWindow;

	/*WindowBar& windowBar = *new WindowBar(world);
	world += windowBar;*/

	world.bindUI([](GUI& gui) {
#if DEBUG_CAM
#else
		//ImGui::SetNextWindowBgAlpha(0);
		//ImGui::SetNextWindowSize({ (float)gui.viewSize.x, (float)gui.viewSize.y }, ImGuiCond_Always);
		//ImGui::SetNextWindowPos({ 0.0f, 0.0f }, ImGuiCond_Always);
		//ImGui::Begin("HUD", 0,
		//	ImGuiWindowFlags_NoResize |
		//	ImGuiWindowFlags_NoCollapse |
		//	ImGuiWindowFlags_NoScrollbar |
		//	ImGuiWindowFlags_NoBackground |
		//	ImGuiWindowFlags_NoInputs |
		//	ImGuiWindowFlags_NoMouseInputs |
		//	ImGuiWindowFlags_NoMove |
		//	ImGuiWindowFlags_NoTitleBar);
		//ImVec2 pos = ImGui::GetWindowPos();
		//ImDrawList* list = ImGui::GetForegroundDrawList();//->AddRectFilled({ pos.x + 18, pos.y + 18 }, { pos.x + 22, pos.y + 22 }, IM_COL32(0, 255, 0, 255), 1);
		//char str[20];
		//sprintf_s(str, "%d Km/h", int(abs(vehicle->vehicleState.forwardSpeed)));
		//ImFont* font = ImGui::GetIO().Fonts->Fonts[1];
		//ImGui::PushFont(font);
		//ImVec2 size = ImGui::CalcTextSize(str);
		//list->AddText({ gui.viewSize.x - size.x - 20, gui.viewSize.y - size.y - 20 }, ImColor(1.0f, 1.0f, 1.0f), str);
		//ImGui::PopFont();
		//ImGui::End();
#endif
		if (world.input.getCursorHidden()) {
			if (world.input.getKeyPress(VK_ESCAPE)) {
				world.input.setCursorHidden(false);
			}
			gui.hideUIControl("Editor");
			gui.hideUIControl("WindowBar");
		}
		else {
			if (world.input.getKeyPress(VK_ESCAPE)) {
				world.input.setCursorHidden(true);
				gui.hideAllUIControl();
			}
			gui.showUIControl("Editor");
			gui.showUIControl("WindowBar");
		}
	});
	/*FaceRigObject* faceRigObject = new FaceRigObject();
	world += faceRigObject;*/

	SerializationInfo* info = getAssetByPath<SerializationInfo>("Content/world.asset");
	if (info == NULL)
		return;//throw runtime_error("");

	world.loadWorld(*info);
	world.deserialize(*info);
}

//void InitialWorld() {
//	PostProcessingCamera& camera = *new PostProcessingCamera();
//	camera.updataRigidBody(new Sphere(5));
//	world += camera;
//	camera.setPosition(0, 0, 17);
//	world.switchCamera(camera);
//
//	DirectLight &dirLight = *new DirectLight("DirLight", { 255, 255, 200 }, 1.5);
//	camera.setVolumnicLight(dirLight);
//	dirLight.intensity = 1.5;
//	world += dirLight;
//	SkySphere &sky = *new SkySphere(dirLight);
//	sky.loadDefaultTexture();
//	Material *spacemat = getAsset<Material>("Material", "Space");
//	if (spacemat) {
//		sky.meshRender.setMaterial(0, *spacemat);
//		spacemat->setTexture("skyMap", *getAssetByPath<Texture2D>("Engine/Textures/SpaceBox.tga"));
//	}
//	sky.scaling(400, 400, 400);
//	world += sky;
//	world.input.keyboardInputHandler += [](Input* input, char key, InputStateEnum state) {
//		if (GUI::isAnyItemFocus()/* | world.input.getKeyState(VK_CONTROL) == Up*/)
//			return;
//		if (key == 'F' && state == OnRelease)
//#ifdef __FREEGLUT_H__
//			glutFullScreenToggle();
//#endif
//#ifdef _glfw3_h_
//		{
//			Engine::toggleFullscreen();
//		}
//#endif
//		if (key == 'G' && state == Down && world.physicalWorld.getGravity().z() < 0)
//			world.physicalWorld.setGravity({ 0, 0, 10.f });
//		if (key == 'G' && state == Up && world.physicalWorld.getGravity().z() > 0)
//			world.physicalWorld.setGravity({ 0, 0, -10.f });
//	};
//
//	world.getCurrentCamera().bindTick([](Object* s, float dt) {
//		Input& input = world.input;
//		Camera& cam = *(Camera*)s;
//		if ((input.getCursorHidden() || input.getMouseButtonDown(MouseButtonEnum::Right))) {
//			Vector3f v(0, 0, 0);
//			v.x() = (int)input.getKeyDown('W') - (int)input.getKeyDown('S');
//			v.x() += world.input.getMouseWheelValue();
//			v.y() = (int)input.getKeyDown('A') - (int)input.getKeyDown('D');
//			v.z() = (int)input.getKeyDown('E') - (int)input.getKeyDown('Q');
//			v *= dt * 30;
//			cam.translate(v.x(), v.y(), 0);
//			cam.translate(0, 0, v.z(), WORLD);
//
//			Unit2Di m = input.getMouseMove();
//			cam.rotate(0, 0, -m.x * dt * 5, WORLD);
//			cam.rotate(0, m.y * dt * 5, 0);
//
//			static RigidBody* target = NULL;
//			static btGeneric6DofSpring2Constraint* constraint = NULL;
//			if (input.getMouseButtonState(MouseButtonEnum::Left) == InputStateEnum::Down) {
//				Vector3f pos = cam.getPosition(WORLD);
//				Vector3f forward = cam.getForward(WORLD);
//				if (target == NULL) {
//					ContactInfo res;
//					if (world.physicalWorld.rayTest(pos, pos + forward * 1500, res)) {
//						RigidBody* obj = dynamic_cast<RigidBody*>((PhysicalBody*)res.physicalObject);
//						if (obj != NULL) {
//							Console::log("RayTest: %s", res.objectName);
//							if (cam.rigidBody != NULL) {
//								target = obj;
//								float distance = (obj->targetTransform.getPosition(WORLD) - pos).norm();
//								Vector3f rel = res.location;
//								rel = toVector3f(obj->getWorldTransform().inverse() * toPVec3(rel));
//								PTransform at = PTransform::getIdentity(), bt = PTransform::getIdentity();
//								at.setOrigin({ distance, 0, 0 });
//								bt.setOrigin(toPVec3(rel));
//								constraint = new btGeneric6DofSpring2Constraint(*cam.rigidBody, *obj, at, bt);
//								constraint->enableSpring(3, true);
//								constraint->setStiffness(3, 35.0f);
//								constraint->setDamping(3, 0.5f);
//								constraint->enableSpring(4, true);
//								constraint->setStiffness(4, 35.0f);
//								constraint->setDamping(4, 0.5f);
//								constraint->enableSpring(5, true);
//								constraint->setStiffness(5, 35.0f);
//								constraint->setDamping(5, 0.5f);
//
//								cam.rigidBody->addConstraintRef(constraint);
//								obj->addConstraintRef(constraint);
//
//								world.physicalWorld.physicsScene->addConstraint(constraint);
//							}
//						}
//					}
//				}
//			}
//			if (input.getMouseButtonState(MouseButtonEnum::Left) == InputStateEnum::Up) {
//				if (cam.rigidBody != NULL && target != NULL) {
//					cam.rigidBody->removeConstraintRef(constraint);
//					target->removeConstraintRef(constraint);
//					world.physicalWorld.physicsScene->removeConstraint(constraint);
//					delete constraint;
//					target = NULL;
//					constraint = NULL;
//				}
//			}
//		}
//	});
//	world.input.setCursorHidden(true);
//
//	Material &pbr_mat = *getAssetByPath<Material>("Engine/Shaders/PBR.mat");
//
//	EditorWindow& editorWindow = *new EditorWindow(world, pbr_mat);
//	editorWindow.blurBackground = true;
//	editorWindow.showCloseButton = false;
//	world += editorWindow;
//
//	world.bindUI([](GUI& gui) {
//		//ImGui::SetNextWindowBgAlpha(0);
//		ImGui::SetNextWindowSize({ 40, 40 }, ImGuiCond_Always);
//		ImGui::SetNextWindowPosCenter(ImGuiCond_Always);
//		ImGui::Begin("HUD", 0,
//			ImGuiWindowFlags_NoResize |
//			ImGuiWindowFlags_NoCollapse |
//			ImGuiWindowFlags_NoScrollbar |
//			ImGuiWindowFlags_NoBackground |
//			ImGuiWindowFlags_NoInputs |
//			ImGuiWindowFlags_NoMouseInputs |
//			ImGuiWindowFlags_NoMove |
//			ImGuiWindowFlags_NoTitleBar);
//		ImVec2 pos = ImGui::GetWindowPos();
//		ImGui::GetForegroundDrawList()->AddRectFilled({ pos.x + 18, pos.y + 18 }, { pos.x + 22, pos.y + 22 }, IM_COL32(0, 255, 0, 255), 1);
//		ImGui::End();
//		if (world.input.getCursorHidden()) {
//			if (world.input.getKeyPress(VK_ESCAPE)) {
//				world.input.setCursorHidden(false);
//			}
//			gui.hideUIControl("Editor");
//		}
//		else {
//			gui.showUIControl("Editor");
//		}
//	});
//
//	ifstream wf = ifstream("Content/world.asset");
//	if (!wf.fail()) {
//		SerializationInfoParser sip = SerializationInfoParser(wf);
//		if (sip.parse()) {
//			world.loadWorld(sip.infos[0]);
//			world.deserialize(sip.infos[0]);
//			return;
//		}
//	}
//	Console::error("Cannot find Content/world.asset file");
//}