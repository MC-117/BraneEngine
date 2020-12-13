#define _AFXDLL
#include <afxdlgs.h>
#include "LoadingUI.h"
#include <filesystem>
#include <fstream>
#include "Engine.h"
#include "PythonManager.h"
#include "imgui_internal.h"

#define ENABLE_PYTHON 1

World world;

void SetTopWindow(HWND hWnd)
{
	HWND hForeWnd = GetForegroundWindow();
	DWORD dwForeID = GetWindowThreadProcessId(hForeWnd, NULL);
	DWORD dwCurID = GetCurrentThreadId();
	AttachThreadInput(dwCurID, dwForeID, TRUE);
	ShowWindow(hWnd, SW_SHOWNORMAL);
	SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	SetForegroundWindow(hWnd);
	AttachThreadInput(dwCurID, dwForeID, FALSE);
 }

void resizeWindow(int width, int height) {
#ifdef _glfw3_h_

#endif
	Engine::setViewportSize(width, height);
	/*----- Vendor resize window -----*/
	{
		if (!VendorManager::getInstance().getVendor().resizeWindow(Engine::engineConfig, Engine::windowContext, width, height))
			throw runtime_error("Vendor resie window failed");
	}
}

void engineMainLoop() {
	Timer timer;
	if (world.willQuit())
#ifdef _glfw3_h_
	{
		glfwSetWindowShouldClose((GLFWwindow*)Engine::windowContext.window, GLFW_TRUE);
	}
	glfwPollEvents();
#endif
	timer.record("PollEvent");
	world.tick(0);
	world.afterTick();
	timer.record("CPU");

	/*----- Vendor swap -----*/
	{
		if (!VendorManager::getInstance().getVendor().swap(Engine::engineConfig, Engine::windowContext))
			throw runtime_error("Vendor swap failed");
	}

	timer.record("GPU Wait");
	Console::getTimer("Engine") = timer;
}

void loadAssets(const char* path, LoadingUI& log, vector<string>& delayLoadAsset)
{
	namespace FS = experimental::filesystem;
	for (auto& p : FS::recursive_directory_iterator(path)) {
		string path = p.path().generic_u8string();
#ifdef UNICODE
		USES_CONVERSION;
		wchar_t* pathStr = A2W(path.c_str());
#else
		const char* pathStr = path.c_str();
#endif // UNICODE
		if (p.status().type() == experimental::filesystem::file_type::regular) {
			string ext = p.path().extension().generic_string();
			string name = p.path().filename().generic_string();
			name = name.substr(0, name.size() - ext.size());
			Asset* asset = NULL;
			bool ignore = false;
			bool delay = false;
			SerializationInfo *ini = NULL;
			string iniPath = path + ".ini";
			if (PathFileExistsA(iniPath.c_str())) {
				ifstream f(iniPath);
				if (!f.fail()) {
					SerializationInfoParser iniParse(f);
					if (iniParse.parse())
						ini = new SerializationInfo(iniParse.infos[0]);
					f.close();
				}
			}
			if (!_stricmp(ext.c_str(), ".obj") || !_stricmp(ext.c_str(), ".fbx") ||
				!_stricmp(ext.c_str(), ".pmx")) {
				Importer imp = Importer(path);
				bool animationOnly = false;
				if (ini != NULL) {
					string b;
					ini->get("animationOnly", b);
					animationOnly = b == "true";
				}
				if (imp.isLoad()) {
					if (!animationOnly) {
						if (imp.isSkeletonMesh()) {
							SkeletonMesh* mesh = new SkeletonMesh();
							if (imp.getSkeletonMesh(*mesh)) {
								Asset* ass = new Asset(&SkeletonMeshAssetInfo::assetInfo, name, path);
								ass->asset[0] = mesh;
								if (AssetManager::registAsset(*ass)) {
									asset = ass;
								}
								else {
									delete ass;
									delete mesh;
								}
							}
							else {
								delete mesh;
							}
						}
						else {
							Mesh* mesh = new Mesh();
							if (imp.getMesh(*mesh)) {
								Asset* ass = new Asset(&MeshAssetInfo::assetInfo, name, path);
								ass->asset[0] = mesh;
								if (AssetManager::registAsset(*ass)) {
									asset = ass;
								}
								else {
									delete ass;
									delete mesh;
								}
							}
							else {
								delete mesh;
							}
						}
					}
					if (imp.hasAnimation()) {
						vector<AnimationClipData*> anims;
						if (imp.getAnimation(anims, true)) {
							for (int i = 0; i < anims.size(); i++) {
								Asset* ass = new Asset(&AnimationClipDataAssetInfo::assetInfo, name + ":" + anims[i]->name, path + ":Animation" + to_string(i));
								ass->asset[0] = anims[i];
								if (AssetManager::registAsset(*ass))
									asset = ass;
							}
						}
					}
				}
			}
			else if (!_stricmp(ext.c_str(), ".camanim")) {
				AnimationClipData* data = AnimationLoader::loadCameraAnimation(path);
				if (data != NULL) {
					Asset* ass = new Asset(&AnimationClipDataAssetInfo::assetInfo, data->name, path);
					ass->asset[0] = data;
					if (AssetManager::registAsset(*ass))
						asset = ass;
				}
			}
			else if (!_stricmp(ext.c_str(), ".charanim")) {
				AnimationClipData* data = AnimationLoader::readAnimation(path);
				if (data == NULL)
					data = AnimationLoader::loadMotionAnimation(path);
				if (data != NULL) {
					Asset* ass = new Asset(&AnimationClipDataAssetInfo::assetInfo, data->name, path);
					ass->asset[0] = data;
					if (AssetManager::registAsset(*ass))
						asset = ass;
				}
			}
			else if (!_stricmp(ext.c_str(), ".png") || !_stricmp(ext.c_str(), ".tga") ||
				!_stricmp(ext.c_str(), ".jpg") || !_stricmp(ext.c_str(), ".bmp") ||
				!_stricmp(ext.c_str(), ".mip")) {
				const char* stdStr = "false";
				const char* filterStr = "TF_Linear_Mip_Linear";
				if (name.find("_N") != string::npos)
					stdStr = "false";
				if (name.find("_lut") != string::npos)
					filterStr = "TF_Linear";
				asset = AssetManager::loadAsset("Texture2D", name, path, { stdStr, "TW_Repeat", "TW_Repeat", filterStr, filterStr }, {});
			}
			else if (!_stricmp(ext.c_str(), ".mat")) {
				asset = AssetManager::loadAsset("Material", name, path, { "" }, {});
			}
			else if (!_stricmp(ext.c_str(), ".wav")) {
				asset = AssetManager::loadAsset("AudioData", name, path, {}, {});
			}
			else if (!_stricmp(ext.c_str(), ".asset")) {
				asset = AssetManager::loadAsset("AssetFile", name, path, {}, {});
			}
#if ENABLE_PYTHON
			else if (!_stricmp(ext.c_str(), ".py")) {
				asset = AssetManager::loadAsset("PythonScript", name, path, {}, {});
			}
#endif
			else if (!_stricmp(ext.c_str(), ".imat")) {
				delayLoadAsset.push_back(path);
				delay = true;
			}
			else if (!_stricmp(ext.c_str(), ".ttf") || !_stricmp(ext.c_str(), ".ini")) {
				ignore = true;
			}
			else {
				CString str;
				str.Format("%s unknown file type", pathStr);
				log.SetText(str);
				Console::warn("%s unknown file type", path.c_str());
				
			}
			if (!delay && !ignore) {
				if (asset == NULL) {
#ifdef UNICODE
					wchar_t* namewc = A2W(name.c_str());
#else
					const char* nameStr = name.c_str();
#endif // UNICODE
					CString str;
					str.Format("%s %s load failed", pathStr, nameStr);
					log.SetText(str);
					Console::error("%s %s load failed", path.c_str(), name.c_str());
				}
				else {
					CString str;
					str.Format("%s load", pathStr);
					log.SetText(str);
					Console::log("%s load", path.c_str());
				}
			}
			if (ini != NULL)
				delete ini;
		}
		else {
			CString str;
			str.Format("%s folder", pathStr);
			log.SetText(str);
			Console::log("%s folder", path.c_str());
		}
	}
}

void loadAssets(LoadingUI& log, bool loadDefaultAsset, bool loadEngineAsset, bool loadContentAsset)
{
	vector<string> delayLoadAsset;
	namespace FS = experimental::filesystem;
	if (!loadDefaultAsset)
		return;

	if (!FS::exists("Engine")) {
		MessageBox(NULL, _T("Engine Folder Missing"), _T("Error"), MB_OK);
		Engine::clean();
		exit(-1);
	}

	ShaderManager::loadDefaultAdapter("Engine/Shaders/Default");

	if (!Material::loadDefaultMaterial()) {
		MessageBox(NULL, _T("Default material load failed"), _T("Error"), MB_OK);
		Engine::clean();
		exit(-1);
	}

	if (!Texture2D::loadDefaultTexture()) {
		MessageBox(NULL, _T("Default texture load failed"), _T("Error"), MB_OK);
		Engine::clean();
		exit(-1);
	}

	if (loadEngineAsset)
		loadAssets("Engine", log, delayLoadAsset);
	if (loadContentAsset) {
		if (FS::exists("Content")) {
			loadAssets("Content", log, delayLoadAsset);
		}
	}
	for (int i = 0; i < delayLoadAsset.size(); i++) {
		FS::path p = FS::path(delayLoadAsset[i]);
		string path = p.generic_string();
		string ext = p.extension().generic_string();
		string name = p.filename().generic_string();
		name = name.substr(0, name.size() - ext.size());
		Asset* asset = NULL;
		if (!_stricmp(ext.c_str(), ".imat")) {
			asset = AssetManager::loadAsset("Material", name, path, { "" }, {});
		}
		else {
			CString str;
			str.Format("%s unknown file type", p.c_str());
			log.SetText(str);
			Console::warn("%s unknown file type", path.c_str());
			continue;
		}
		if (asset == NULL) {
			const wchar_t* namewc = p.filename().c_str();
			CString str;
			str.Format("%s %s load failed", p.c_str(), namewc);
			log.SetText(str);
			Console::error("%s %s load failed", path.c_str(), name.c_str());
		}
		else {
			CString str;
			str.Format("%s load", p.c_str());
			log.SetText(str);
			Console::log("%s load", path.c_str());
		}
	}
}

void main(int argc, char** argv) {
	Engine::config();
	Engine::setup();
	Engine::start();
	Engine::clean();
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	try {
		Engine::windowContext._hinstance = hInstance;
		Engine::config();
		Engine::setup();
		Engine::start();
		Engine::clean();
	}
	catch (exception e) {
		MessageBoxA(NULL, e.what(), "Error", MB_ICONERROR);
		return -1;
	}
	return 0;
}

string Engine::version = ENGINE_VERSION;
World* Engine::currentWorld = &world;
WindowContext Engine::windowContext =
{
	{ 100, 100 },
	{ 1280, 720 },
	{ 1280, 720 },
	"Test",
	0,
	NULL,
	NULL,
	false
};
EngineConfig Engine::engineConfig;

World * Engine::getCurrentWorld()
{
	return currentWorld;
}

void Engine::setViewportSize(int width, int height)
{
	if (!windowContext._fullscreen)
		windowContext.screenSize = { width, height };
	world.setViewportSize(width, height);
}

void Engine::toggleFullscreen()
{
	windowContext._fullscreen = !windowContext._fullscreen;
	if (windowContext._fullscreen)
		glfwSetWindowMonitor((GLFWwindow*)windowContext.window, glfwGetPrimaryMonitor(), 0, 0, windowContext.fullscreenSize.x, windowContext.fullscreenSize.y, GLFW_DONT_CARE);
	else
		glfwSetWindowMonitor((GLFWwindow*)windowContext.window, NULL, 0, 0, windowContext.screenSize.x, windowContext.screenSize.y, GLFW_DONT_CARE);
}

void Engine::config()
{
	ifstream f = ifstream("Config.ini");
	if (f.fail())
		return;
	SerializationInfoParser sip(f);
	if (!sip.parse()) {
		Console::error("Config.ini read failed");
		return;
	}
	f.close();
	SerializationInfo& conf = sip.infos[0];
	string vendorName;
	conf.get(".vendorName", vendorName);
	if (!vendorName.empty())
		engineConfig.vendorName = vendorName;
	string startMapPath = "Content/world.asset";
	if (conf.get(".startMapPath", startMapPath))
		engineConfig.startMapPath = startMapPath;
	string boolStr;
	conf.get(".fullscreen", boolStr);
	if (boolStr == "true")
		engineConfig.fullscreen = true;
	boolStr.clear();
	conf.get(".guiOnly", boolStr);
	if (boolStr == "true")
		engineConfig.guiOnly = true;
	boolStr.clear();
	conf.get(".loadDefaultAsset", boolStr);
	if (boolStr == "false")
		engineConfig.loadDefaultAsset = false;
	boolStr.clear();
	conf.get(".loadEngineAsset", boolStr);
	if (boolStr == "false")
		engineConfig.loadEngineAsset = false;
	boolStr.clear();
	conf.get(".loadContentAsset", boolStr);
	if (boolStr == "false")
		engineConfig.loadContentAsset = false;
	boolStr.clear();
	conf.get(".vsnyc", boolStr);
	if (boolStr == "true")
		engineConfig.vsnyc = true;
	float screenWidth = engineConfig.screenWidth, screenHeight = engineConfig.screenHeight;
	conf.get(".screenWidth", screenWidth);
	engineConfig.screenWidth = screenWidth;
	conf.get(".screenHeight", screenHeight);
	engineConfig.screenHeight = screenHeight;
}

bool loadIcon(GLFWwindow* win, const char* folder) {
	GLFWimage img[7];
	const char* file[7] = { "/logo_64.png", "/logo_48.png", "/logo_40.png", "/logo_32.png", "/logo_24.png", "/logo_20.png", "/logo_16.png" };
	int c;
	bool ok = true;
	for (int i = 0; i < 7; i++) {
		img[i].pixels = NULL;
		if (ok) {
			img[i].pixels = Texture2D::loadImageBytes((string(folder) + file[i]).c_str(), &img[i].width, &img[i].height, &c);
			if (img[i].pixels == NULL) {
				ok = false;
				continue;
			}
		}
	}
	if (!ok) {
		for (int i = 0; i < 7; i++) {
			if (img[i].pixels != NULL)
				free(img[i].pixels);
		}
		return false;
	}
	glfwSetWindowIcon(win, 7, img);
	return true;
}

void SetStyleColors(ImGuiStyle* dst = 0)
{
	ImGuiStyle* style = dst ? dst : &ImGui::GetStyle();
	ImVec4* colors = style->Colors;
	style->FramePadding = { 5, 5 };
	style->WindowTitleAlign = { 0.5, 0.5 };
	style->WindowRounding = 1;
	//style->ScaleAllSizes(1.3);

	colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 0.95f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.05f, 0.05f, 0.05f, 1.00f);
	colors[ImGuiCol_ChildBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.94f);
	colors[ImGuiCol_Border] = ImVec4(0.20f, 0.20f, 0.20f, 0.46f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.85f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.22f, 0.22f, 0.22f, 0.40f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.16f, 0.16f, 0.16f, 0.53f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.12f, 0.12f, 0.12f, 0.51f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.48f, 0.48f, 0.48f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.79f, 0.79f, 0.79f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.48f, 0.47f, 0.47f, 0.91f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.56f, 0.55f, 0.55f, 0.62f);
	colors[ImGuiCol_Button] = ImVec4(0.20f, 0.20f, 0.20f, 0.63f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.67f, 0.67f, 0.68f, 0.63f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.26f, 0.26f, 0.26f, 0.63f);
	colors[ImGuiCol_Header] = ImVec4(0.20f, 0.20f, 0.20f, 0.58f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.64f, 0.65f, 0.65f, 0.80f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.25f, 0.25f, 0.25f, 0.80f);
	colors[ImGuiCol_Separator] = ImVec4(0.00f, 0.68f, 0.84f, 0.50f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.81f, 0.81f, 0.81f, 0.64f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.81f, 0.81f, 0.81f, 0.64f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.87f, 0.87f, 0.87f, 0.53f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.87f, 0.87f, 0.87f, 0.74f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.87f, 0.87f, 0.87f, 0.74f);
	colors[ImGuiCol_Tab] = ImVec4(0.20f, 0.20f, 0.20f, 0.86f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.00f, 0.68f, 0.84f, 0.50f);
	colors[ImGuiCol_TabActive] = ImVec4(0.00f, 0.68f, 0.84f, 1.00f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.02f, 0.02f, 0.02f, 1.00f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.19f, 0.19f, 0.19f, 1.00f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.68f, 0.68f, 0.68f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.77f, 0.33f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.87f, 0.55f, 0.08f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.47f, 0.60f, 0.76f, 0.47f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(0.58f, 0.58f, 0.58f, 0.90f);
	colors[ImGuiCol_NavHighlight] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

	//for (int i = ImGuiCol_WindowBg; i <= ImGuiCol_ModalWindowDimBg; i++)
	//	if (i != ImGuiCol_PopupBg)
	//		colors[i].w *= 0.96;
}

void Engine::setup()
{
	{

#ifdef UNICODE
		wchar_t execPath[MAX_PATH];
#else
		char execPath[MAX_PATH];
#endif // UNICODE
		GetModuleFileName(NULL, execPath, MAX_PATH);
		windowContext.executionPath = execPath;
	}
	LoadingUI loadingUI("Engine/Banner/Banner.png");

	VendorManager::getInstance().instantiateVendor(engineConfig.vendorName);

	IVendor& vendor = VendorManager::getInstance().getVendor();
#if ENABLE_PYTHON
	PythonManager::start();
#endif
	if (!PhysicalWorld::init())
		throw runtime_error("Physics Engine init failed");

	setViewportSize(engineConfig.screenWidth, engineConfig.screenHeight);
#ifdef _glfw3_h_
	CRect rect;
	GetWindowRect(GetDesktopWindow(), &rect);
	windowContext.fullscreenSize = { rect.Width(), rect.Height() };
	glfwInit();
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	//glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
	//glfwWindowHint(GLFW_SAMPLES, 8);

	/*----- Vendor window setup -----*/
	{
		if (!vendor.windowSetup(engineConfig, windowContext))
			throw runtime_error("Vendor window setup failed");
	}

	windowContext.window = glfwCreateWindow(windowContext.screenSize.x, windowContext.screenSize.y, windowContext.srceenTile.c_str(), NULL, NULL);
	if (!loadIcon((GLFWwindow*)windowContext.window, "Icon"))
		Console::warn("Icon load failed");
	glfwMakeContextCurrent((GLFWwindow*)windowContext.window);

	if (engineConfig.vsnyc)
		glfwSwapInterval(1);
	else
		glfwSwapInterval(0);

	windowContext._hwnd = glfwGetWin32Window((GLFWwindow*)windowContext.window);
#endif
	world.input.setHWND(windowContext._hwnd);
	//ShowWindow(_hwnd, SW_HIDE);

	/*----- Vendor setup -----*/
	{
		if (!vendor.setup(engineConfig, windowContext))
			throw runtime_error("Vendor setup failed");
	}

	int argc = 1;
	char* argv = "BraneEnigne";
	if (!alutInit(NULL, NULL)) {
		//throw runtime_error("ALUT init failed");
	}

	// Setup Dear ImGui context
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls

	io.Fonts->AddFontFromFileTTF("Engine/Fonts/arialuni.ttf", 20, NULL, io.Fonts->GetGlyphRangesChineseFull());
	io.Fonts->AddFontFromFileTTF("Engine/Fonts/ChakraPetch-Light.ttf", 72, NULL, io.Fonts->GetGlyphRangesChineseFull());

	// Setup Dear ImGui style
	//ImGui::StyleColorsDark();
	SetStyleColors();
	//ImGui::StyleColorsClassic();

#ifdef _glfw3_h_
	glfwSetErrorCallback([](int code, const char* error) {
		Console::error("GLFW: %s(%d)", error, code);
	});
	//glfwSetDropCallback((GLFWwindow*)window, [](GLFWwindow* window, int n, const char** paths) {
	//	namespace FS = experimental::filesystem;
	//	FS::path path1(paths[0]);
	//	if (n == 1 && path1.extension().generic_string() == ".wav") {
	//		Actor& actor = *new Actor(path1.filename().generic_string() + "Actor");
	//		AudioData& ad = *new AudioData(path1.filename().generic_string(), paths[0]);
	//		if (actor.addAudioSource(ad) == NULL) {
	//			delete &ad;
	//			MessageBoxA(_hwnd, paths[0], "Load Failed", MB_ICONERROR);
	//		}
	//		else
	//			world += actor;
	//		return;
	//	}
	//	Importer imp = Importer(paths[0]);//"C:\\Users\\29556\Desktop\\MMD\\dotMiku_V3_ver.2.00\\dotMiku_V3.pmx");
	//	if (imp.isSkeletonMesh()) {
	//		SkeletonMesh* skm = new SkeletonMesh();
	//		if (!imp.getSkeletonMesh(*skm)) {
	//			MessageBoxA(_hwnd, paths[0], "Load Failed", MB_ICONERROR);
	//		}
	//		else {
	//			SkeletonMeshActor &sma = *new SkeletonMeshActor(*skm, Material::defaultMaterial, PathFindFileNameA(paths[0]));
	//			vector<AnimationClipData*> anims;
	//			imp.getAnimation(anims);
	//			if (n >= 2) {
	//				for (int i = 1; i < n; i++) {
	//					FS::path p(paths[i]);
	//					if (p.extension().generic_string() == ".wav") {
	//						AudioData& ad = *new AudioData(p.filename().generic_string(), paths[i]);
	//						if (sma.addAudioSource(ad) == NULL) {
	//							delete &ad;
	//							MessageBoxA(_hwnd, paths[i], "Load Failed", MB_ICONERROR);
	//						}
	//					}
	//					else {
	//						Importer impa = Importer(paths[i]);
	//						if (!impa.getAnimation(anims)) {
	//							MessageBoxA(_hwnd, paths[i], "Load Failed", MB_ICONERROR);
	//						}
	//					}
	//				}
	//			}
	//			for (int i = 0; i < anims.size(); i++) {
	//				sma.addAnimationClip(*anims[i]);
	//			}
	//			/*if (sma.animationClip != NULL)
	//				sma.animationClip->play();*/
	//			world += sma;
	//		}
	//	}
	//	else {
	//		Material &pbr_mat = *getAssetByPath<Material>("Engine/Shaders/PBR.mat");
	//		Mesh* m = new Mesh();
	//		if (!imp.getMesh(*m)) {
	//			MessageBoxA(_hwnd, paths[0], "Load Failed", MB_ICONERROR);
	//		}
	//		else {
	//			MeshActor &ma = *new MeshActor(*m, pbr_mat.instantiate(), PathFindFileNameA(paths[0]));
	//			world += ma;
	//		}
	//	}
	//});

	/*----- Vendor ImGui init -----*/
	{
		if (!vendor.imGuiInit(engineConfig, windowContext))
			throw runtime_error("Vendor ImGui init failed");
	}
	glfwSetWindowSizeCallback((GLFWwindow*)windowContext.window, [](GLFWwindow* win, int width, int height) {
		resizeWindow(width, height);
	});
#endif
	loadingUI.SetText(L"Start BraneEngine");
	thread loadingUIThread(LoadingUI::threadFunc, &loadingUI);
	loadingUIThread.detach();
	loadAssets(loadingUI, engineConfig.loadDefaultAsset, engineConfig.loadEngineAsset, engineConfig.loadContentAsset);
	if (std::experimental::filesystem::exists("Plugins")) {
		loadingUI.SetText(L"InitialPlugin...");
		PluginManager::loadPluginFolder("Plugins");
	}
	loadingUI.SetText(L"Initial...");

	if (engineConfig.guiOnly) {
		world.setGUIOnly(true);
		InitialTool();
	}
	else
		InitialWorld();

	loadingUI.Close();

	if (engineConfig.fullscreen)
		toggleFullscreen();
}

void Engine::start()
{
#ifdef _glfw3_h_
	world.begin();
	bool init = false, show = false;
	Timer timer;
	while (!glfwWindowShouldClose((GLFWwindow*)windowContext.window))
	{
		engineMainLoop();
		/*try {
			GL_ERR;
		}
		catch (exception e) {
			Console::error("GL: %s", e.what());
		}*/
		if (!init) {
			if (!show) {
				glfwShowWindow((GLFWwindow*)windowContext.window);
				SetTopWindow(windowContext._hwnd);
				show = true;
			}
		}
		else
			init = true;

		/*static int tcount = 0;
		if (tcount == 10000)
		MessageBoxA(NULL, "Wait", "Test", MB_OK);
		else
		tcount++;*/
		timer.record("DeltaTime");
		Console::getTimer("DeltaTime") = timer;
		timer.reset();
	}
#endif
}

void Engine::clean()
{
#if ENABLE_PYTHON
	PythonManager::end();
#endif
	world.physicalWorld.physicsScene->release();
	PhysicalWorld::release();

	IVendor& vendor = VendorManager::getInstance().getVendor();

	/*----- Vendor ImGui shutdown -----*/
	{
		if (!vendor.imGuiShutdown(engineConfig, windowContext))
			throw runtime_error("Vendor ImGui shutdown failed");
	}

	ImGui::DestroyContext();

	/*----- Vendor clean -----*/
	{
		if (!vendor.clean(engineConfig, windowContext))
			throw runtime_error("Vendor clean failed");
	}

	alutExit();
	glfwDestroyWindow((GLFWwindow*)windowContext.window);
	glfwTerminate();
}
