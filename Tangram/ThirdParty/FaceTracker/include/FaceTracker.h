#pragma once
#ifndef _FACETRACKER_H_
#define _FACETRACKER_H_

struct FaceParameter
{
	float* mouthWidth;
	float* mouthHeight;
	float* lEyebrowHeight;
	float* rEyebrowHeight;
	float* lEyeOpenness;
	float* rEyeOpenness;
	float* jawOpnness;
	float* nostrilFlare;
	float* headPosX;
	float* headPosY;
	float* headPosZ;
	float* headRotX;
	float* headRotY;
	float* headRotZ;
	float* lEyeDirX;
	float* lEyeDirY;
	float* lEyeDirZ;
	float* rEyeDirX;
	float* rEyeDirY;
	float* rEyeDirZ;
};

class FaceTracker
{
public:
	FaceParameter faceParams;

	FaceTracker();
	~FaceTracker();

	bool open(int id);
	bool open(const char* file);
	bool isOpen();
	void stop();
	bool update();
private:
	void* handle = 0;
};

#endif // !_FACETRACKER_H_
