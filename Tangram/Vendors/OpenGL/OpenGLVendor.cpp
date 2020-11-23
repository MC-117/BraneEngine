#include "OpenGLVendor.h"
#include "../../imgui_impl_glfw.h"
#include "../../imgui_impl_opengl3.h"
#include "OpenGLTexture2D.h"

#ifdef VENDOR_USE_OPENGL

REG_VENDOR_DEC(OpenGLVendor);
REG_VENDOR_IMP(OpenGLVendor, "OpenGL");

OpenGLVendor::OpenGLVendor()
{
	name = "OpenGL";
}

OpenGLVendor::~OpenGLVendor()
{
}

bool OpenGLVendor::windowSetup(EngineConfig & config, WindowContext & context)
{
	return true;
}

bool OpenGLVendor::setup(const EngineConfig & config, const WindowContext & context)
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_PROGRAM_POINT_SIZE);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_FRAMEBUFFER_SRGB);
	Color backgroudColor = { 25, 25, 25 };
	glClearColor(backgroudColor.r, backgroudColor.g, backgroudColor.b, backgroudColor.a);
	return glewInit() == GLEW_OK;
}

bool OpenGLVendor::imGuiInit(const EngineConfig & config, const WindowContext & context)
{
	if (!ImGui_ImplGlfw_InitForOpenGL((GLFWwindow*)context.window, true))
		return false;
	if (!ImGui_ImplOpenGL3_Init("#version 410"))
		return false;
	return true;
}

bool OpenGLVendor::imGuiNewFrame(const EngineConfig & config, const WindowContext & context)
{
	glDisable(GL_FRAMEBUFFER_SRGB);
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	return true;
}

bool OpenGLVendor::imGuiDrawFrame(const EngineConfig & config, const WindowContext & context)
{
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	glEnable(GL_FRAMEBUFFER_SRGB);
	return true;
}

bool OpenGLVendor::swap(const EngineConfig & config, const WindowContext & context)
{
	glfwSwapBuffers((GLFWwindow*)context.window);
	return true;
}

bool OpenGLVendor::imGuiShutdown(const EngineConfig & config, const WindowContext & context)
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	return true;
}

bool OpenGLVendor::clean(const EngineConfig & config, const WindowContext & context)
{
	return true;
}

bool OpenGLVendor::guiOnlyRender(const Color & clearColor)
{
	glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
	glClear(GL_COLOR_BUFFER_BIT);
	return true;
}

bool OpenGLVendor::resizeWindow(const EngineConfig & config, const WindowContext & context, unsigned int width, unsigned int height)
{
	if (config.vsnyc)
		glfwSwapInterval(1);
	else
		glfwSwapInterval(0);
	return true;
}

ITexture2D * OpenGLVendor::newTexture2D(Texture2DDesc& desc) const
{
	return new OpenGLTexture2D(desc);
}

#endif // VENDOR_USE_OPENGL