#define _AFXDLL
#include <afxdlgs.h>
#include "Engine.h"
#include "ToolShelf.h"
#include "AnimationConverter.h"
#include "ConsoleWindow.h"

void InitialTool() {
	Material &pbr_mat = *getAssetByPath<Material>("Engine/Shaders/PBR.mat");
	ToolShelf& toolShelf = *new ToolShelf("ToolShelf", true);
	toolShelf.showCloseButton = false;
	world += toolShelf;
	
	toolShelf.registTool(*new ConsoleWindow(*Engine::getCurrentWorld()));
	toolShelf.registTool(*new AnimationConverter());
}