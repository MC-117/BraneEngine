#pragma once
#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "RigidBody.h"
#include "CameraRender.h"
#include "AnimationClip.h"

class RigidBody;

struct CameraData
{
	Matrix4f projectionViewMat;
	Matrix4f projectionMat;
	Matrix4f viewMat;
	Vector3f cameraLoc;
	float zNear;
	Vector3f cameraDir;
	float zFar;
	Vector3f cameraUp;
	float fovy;
	Vector3f cameraLeft;
	float user1;
	Vector2f viewSize;
	Vector2f user2;
};

class Camera : public ::Transform
{
private:
	CameraRender* _cameraRender = NULL;
public:
	enum CameraMode {
		Perspective, Orthotropic
	} mode = Perspective;

	Unit2Di size = { 1280, 720 };
	Color clearColor = { 0, 0, 0 };
	float fov = 90;
	float aspect = 1280.0f / 720.0f;
	float left = 0, right = 0, top = 0, bottom = 0;
	float zNear = 0.1, zFar = 10000000;
	float distance = 0;
	bool active = false;

	Matrix4f projectionViewMat;

	CameraRender& cameraRender;

	AnimationClip animationClip;

	Camera(string name = "Camera");
	Camera(CameraRender& cameraRender, string name = "Camera");
	Camera(RenderTarget& renderTarget, Material& material = Material::nullMaterial, string name = "Camera");
	virtual ~Camera();

	void setAnimationClip(AnimationClipData& data);

	Matrix4f getProjectionMatrix() const;
	Matrix4f getViewMatrix() const;

	virtual void tick(float deltaTime);
	virtual void afterTick();

	void setSize(Unit2Di size);
	void setMode(CameraMode mode);
	void setActive(bool active);

	bool isActive();

	void uploadCameraData();
	void bindCameraData();

	static Matrix4f perspective(float fovy, float aspect, float zNear, float zFar);
	static Matrix4f orthotropic(float left, float right, float bottom, float top, float zNear, float zFar);
	static Matrix4f lookAt(Vector3f const& eye, Vector3f const& center, Vector3f const& up);
	static Matrix4f viewport(int x, int y, int width, int height, float zNear, float zFar);
protected:
	GLBuffer cameraDataBuffer = GLBuffer(GL_UNIFORM_BUFFER, sizeof(CameraData));
};

#endif // !_CAMERA_H_
