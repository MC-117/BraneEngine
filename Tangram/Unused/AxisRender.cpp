#include "AxisRender.h"
//#include "MaterialLoader.h"

Shader AxisRender::shader = Shader();
Material AxisRender::material = Material(AxisRender::shader);
bool AxisRender::loaded = false;

AxisRender::AxisRender() : Object::Object("AxisXYZ")
{
	if (!loaded) {
		if (!Material::MaterialLoader::loadMaterial(material, "Engine/Shaders/Axis.mat"))
			throw runtime_error("Axis material load failed");
	}
}

IRendering::RenderType AxisRender::getRenderType() const
{
	return IRendering::RenderType::Normal;
}

void AxisRender::render(RenderInfo& info)
{
	if (vao == NULL || vbo == NULL) {
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2, data, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	material.processShader(info.projectionViewMat, transformMat, info.lightSpaceMat, info.cameraLoc, info.cameraDir);
	glBindVertexArray(vao);
	glDrawArrays(GL_POINTS, 0, 1);
	glBindVertexArray(0);
}
