#include "PointLight.h"
#include "Geometry.h"

unsigned int PointLight::pointLightCount = 0;

PointLight::PointLight(const string& name, Color color, float intensity, float attenuation, float radius) : Light::Light(name, color, intensity, attenuation, Sphere(radius))
{
	index = pointLightCount++;
}

void PointLight::setRadius(float radius)
{
	((Sphere*)&boundShape)->setRadius(radius);
}

void PointLight::preRender()
{
	unsigned int pid = Shader::getCurrentProgramId();
	char id[3];
	string name = "pointLights[";
	itoa(index, id, 10);
	name += id;
	positionIdx = Shader::getAttributeIndex(pid, name + "].pos");
	Vector4f worldLoc = ::Transform::transformMat * Vector4f(0, 0, 0, 1);
	worldLoc /= worldLoc.w();
	if(positionIdx != -1)
		glProgramUniform3f(pid, positionIdx, worldLoc.x(), worldLoc.y(), worldLoc.z());
	intensityIdx = Shader::getAttributeIndex(pid, name + "].intensity");
	if (intensityIdx != -1)
		glProgramUniform1f(pid, intensityIdx, intensity);
	colorIdx = Shader::getAttributeIndex(pid, name + "].color");
	if (colorIdx != -1)
		glProgramUniform3f(pid, colorIdx, color.r, color.g, color.b);
	/*ambientIdx = Shader::getAttributeIndex(pid, path + "].ambient");
	if (ambientIdx != -1)
		glProgramUniform4f(pid, ambientIdx, ambient.r, ambient.g, ambient.b, ambient.a);*/
}

void PointLight::render(RenderInfo & info)
{
}
