#include "AnimationClip.h"
#include <fstream>

string readString(istream& in)
{
	uint32_t len = 0;
	in.read((char*)&len, sizeof(uint32_t));
	string str;
	str.resize(len);
	for (int i = 0; i < len; i++) {
		uint8_t c;
		in.read((char*)&c, sizeof(uint8_t));
		str[i] = c;
	}
	return str;
}

void writeString(const string& str, ostream& out, bool haveLen = true)
{
	uint32_t len = str.length();
	if (haveLen) {
		out.write((char*)&len, sizeof(uint32_t));
	}
	for (int i = 0; i < len; i++) {
		uint8_t c = str[i];
		out.write((char*)&c, sizeof(uint8_t));
	}
}

TransformAnimationPlayer::TransformAnimationPlayer()
{
}

TransformAnimationPlayer::TransformAnimationPlayer(::Transform & target)
	: targetTransform(&target)
{
}

TransformAnimationPlayer::TransformAnimationPlayer(::Transform & target, TransformAnimationData & animationData)
	: targetTransform(&target), transformAnimationData(&animationData)
{
}

void TransformAnimationPlayer::setTargetTransform(::Transform & target)
{
	targetTransform = &target;
	reset();
}

void TransformAnimationPlayer::setAnimationData(TransformAnimationData & animationData)
{
	transformAnimationData = &animationData;
	reset();
}

TransformData & TransformAnimationPlayer::getTransformData()
{
	return transformData;
}

TransformData TransformAnimationPlayer::getTransformData(float timeRate)
{
	TransformData data;
	if (isLoop()) {
		timeRate = fmod(timeRate, 1);
	}
	else {
		timeRate = timeRate > 1 ? 1 : timeRate;
	}
	data.position = positionCurvePlayer.curve->get(timeRate * positionCurvePlayer.curve->duration);
	data.rotation = rotationCurvePlayer.curve->get(timeRate * rotationCurvePlayer.curve->duration);
	data.scale = scaleCurvePlayer.curve->get(timeRate * scaleCurvePlayer.curve->duration);
	return data;
}

bool TransformAnimationPlayer::update(float deltaTime)
{
	bool _isplay = false;
	if (transformAnimationData != NULL) {
		Vector3f pos = positionCurvePlayer.update(deltaTime);
		if (positionCurvePlayer.playing()) {
			_isplay = true;
			transformData.position = pos;
			if (targetTransform != NULL)
				targetTransform->setPosition(pos);
		}
		Quaternionf& rot = rotationCurvePlayer.update(deltaTime);
		if (rotationCurvePlayer.playing()) {
			_isplay = true;
			transformData.rotation = rot;
			if (targetTransform != NULL)
				targetTransform->setRotation(rot);
		}
		Vector3f sca = scaleCurvePlayer.update(deltaTime);
		if (scaleCurvePlayer.playing()) {
			_isplay = true;
			transformData.scale = sca;
			if (targetTransform != NULL)
				targetTransform->setScale(sca);
		}
	}
	return _isplay;
}

bool TransformAnimationPlayer::isLoop()
{
	return positionCurvePlayer.loop ||
		rotationCurvePlayer.loop ||
		scaleCurvePlayer.loop;
}

void TransformAnimationPlayer::setLoop(bool loop)
{
	if (transformAnimationData != NULL) {
		positionCurvePlayer.loop = loop;
		rotationCurvePlayer.loop = loop;
		scaleCurvePlayer.loop = loop;
	}
}

void TransformAnimationPlayer::play()
{
	if (transformAnimationData != NULL) {
		positionCurvePlayer.play();
		rotationCurvePlayer.play();
		scaleCurvePlayer.play();
	}
}

void TransformAnimationPlayer::pause()
{
	if (transformAnimationData != NULL) {
		positionCurvePlayer.pause();
		rotationCurvePlayer.pause();
		scaleCurvePlayer.pause();
	}
}

void TransformAnimationPlayer::stop()
{
	if (transformAnimationData != NULL) {
		positionCurvePlayer.stop();
		rotationCurvePlayer.stop();
		scaleCurvePlayer.stop();
	}
}

bool TransformAnimationPlayer::playing()
{
	return positionCurvePlayer.playing() ||
		rotationCurvePlayer.playing() ||
		scaleCurvePlayer.playing();
}

void TransformAnimationPlayer::setSpeed(float speed)
{
	if (transformAnimationData != NULL) {
		positionCurvePlayer.setSpeed(speed);
		rotationCurvePlayer.setSpeed(speed);
		scaleCurvePlayer.setSpeed(speed);
	}
}

void TransformAnimationPlayer::reset()
{
	if (transformAnimationData != NULL) {
		positionCurvePlayer.setCurve(transformAnimationData->positionCurve);
		rotationCurvePlayer.setCurve(transformAnimationData->rotationCurve);
		scaleCurvePlayer.setCurve(transformAnimationData->scaleCurve);
	}
}

AnimationClipData::AnimationClipData(const string & name) : name(name)
{
}

bool AnimationClipData::canApply(SkeletonData & skeletonData)
{
	/*if (skeletonData.boneName.size() != transformAnimationNames.size())
		return false;
	for (auto b = transformAnimationNames.begin(), b1 = skeletonData.boneName.begin(),
		e = transformAnimationNames.end(); b != e; b++, b1++)
		if (b->first != b1->first || b->second != b1->second)
			return false;*/
	return true;
}

bool AnimationClipData::canBlend(AnimationClipData & data)
{
	/*if (data.transformAnimationNames.size() != transformAnimationNames.size())
		return false;
	for (auto b = transformAnimationNames.begin(), b1 = data.transformAnimationNames.begin(),
		e = transformAnimationNames.end(); b != e; b++, b1++)
		if (b->first != b1->first || b->second != b1->second)
			return false;*/
	return true;
}

TransformAnimationData & AnimationClipData::addAnimatinData(const string & objectName)
{
	auto iter = transformAnimationNames.find(objectName);
	if (iter == transformAnimationNames.end()) {
		transformAnimationNames.insert(pair<string, unsigned int>(objectName, transformAnimationDatas.size()));
		transformAnimationDatas.push_back(TransformAnimationData());
		transformAnimationDatas.back().objectName = objectName;
		return transformAnimationDatas.back();
	}
	return transformAnimationDatas[iter->second];
}

TransformAnimationData * AnimationClipData::getAnimatinData(const string & objectName)
{
	auto iter = transformAnimationNames.find(objectName);
	if (iter != transformAnimationNames.end()) {
		return &transformAnimationDatas[iter->second];
	}
	return NULL;
}

unsigned int AnimationClipData::getAnimatinDataIndex(const string & objectName)
{
	auto iter = transformAnimationNames.find(objectName);
	if (iter != transformAnimationNames.end()) {
		return iter->second;
	}
	return -1;
}

TransformAnimationData * AnimationClipData::getAnimatinData(unsigned int index)
{
	if (index < transformAnimationDatas.size())
		return &transformAnimationDatas[index];
	return NULL;
}

Curve<float, float> & AnimationClipData::addMorphAnimationData(unsigned int index)
{
	auto iter = curves.find(index);
	if (iter == curves.end())
		return curves.insert(pair<unsigned int, Curve<float, float>>(index, Curve<float, float>())).first->second;
	else
		return iter->second;
}

Curve<float, float>& AnimationClipData::addMorphAnimationData(const string & name)
{
	auto iter = curveNames.find(name);
	if (iter == curveNames.end()) {
		unsigned int index = curveNames.size();
		curveNames.insert(make_pair(name, index));
		return addMorphAnimationData(index);
	}
	else {
		auto _iter = curves.find(iter->second);
		if (_iter == curves.end())
			return curves.insert(pair<unsigned int, Curve<float, float>>(iter->second, Curve<float, float>())).first->second;
		else
			return _iter->second;
	}
}

Curve<float, float>* AnimationClipData::getMorphAnimationData(unsigned int index)
{
	auto iter = curves.find(index);
	if (iter == curves.end())
		return NULL;
	else
		return &iter->second;
}

Curve<float, float>* AnimationClipData::getMorphAnimationData(const string & name)
{
	auto iter = curveNames.find(name);
	if (iter == curveNames.end())
		return NULL;
	else
		return getMorphAnimationData(iter->second);
}

void AnimationClipData::pack()
{
	duration = 0;
	for (int i = 0; i < transformAnimationDatas.size(); i++) {
		duration = max(transformAnimationDatas[i].positionCurve.duration, duration);
		duration = max(transformAnimationDatas[i].rotationCurve.duration, duration);
		duration = max(transformAnimationDatas[i].scaleCurve.duration, duration);
	}
	for (auto b = curves.begin(), e = curves.end(); b != e; b++) {
		duration = max(b->second.duration, duration);
	}
	for (int i = 0; i < transformAnimationDatas.size(); i++) {
		transformAnimationDatas[i].positionCurve.duration = duration;
		transformAnimationDatas[i].rotationCurve.duration = duration;
		transformAnimationDatas[i].scaleCurve.duration = duration;
	}
	for (auto b = curves.begin(), e = curves.end(); b != e; b++) {
		b->second.duration = duration;
	}
}

bool AnimationClipData::read(istream & in)
{
	const string magic = "BEAnim";
	for (int i = 0; i < magic.length(); i++) {
		uint8_t c;
		in.read((char*)&c, sizeof(uint8_t));
		if (magic[i] != c) {
			in.seekg(0, ios::beg);
			return false;
		}
	}
	uint32_t boneLen = 0;
	in.read((char*)&boneLen, sizeof(uint32_t));
	for (int i = 0; i < boneLen; i++) {
		TransformAnimationData& data = addAnimatinData(readString(in));
		data.read(in);
	}
	uint32_t morphNameLen = 0;
	in.read((char*)&morphNameLen, sizeof(uint32_t));
	for (int i = 0; i < morphNameLen; i++) {
		string name = readString(in);
		uint32_t index;
		in.read((char*)&index, sizeof(uint32_t));
		curveNames.insert(make_pair(name, index));
	}
	uint32_t morphLen = 0;
	in.read((char*)&morphLen, sizeof(uint32_t));
	for (int i = 0; i < morphLen; i++) {
		uint32_t index;
		in.read((char*)&index, sizeof(uint32_t));
		Curve<float, float>& curve = curves.insert(make_pair(index, Curve<float, float>())).first->second;
		uint32_t curveLen;
		in.read((char*)&curveLen, sizeof(uint32_t));
		for (int ci = 0; ci < curveLen; ci++) {
			float_t frame;
			in.read((char*)&frame, sizeof(float_t));
			float_t weight;
			in.read((char*)&weight, sizeof(float_t));
			uint8_t mode;
			in.read((char*)&mode, sizeof(uint8_t));
			float_t inTangent;
			in.read((char*)&inTangent, sizeof(float_t));
			float_t inRate;
			in.read((char*)&inRate, sizeof(float_t));
			float_t outTangent;
			in.read((char*)&outTangent, sizeof(float_t));
			float_t outRate;
			in.read((char*)&outRate, sizeof(float_t));
			CurveValue<float> value = CurveValue<float>((CurveValue<float>::KeySmoothMode)mode, weight);
			value.inTangent = inTangent;
			value.inRate = inRate;
			value.outTangent = outTangent;
			value.outRate = outRate;
			curve.insert(frame, value);
		}
	}
	pack();
	return true;
}

void AnimationClipData::write(ostream & out) const
{
	writeString("BEAnim", out, false);
	uint32_t boneLen = transformAnimationDatas.size();
	out.write((char*)&boneLen, sizeof(uint32_t));
	for (int i = 0; i < boneLen; i++) {
		const TransformAnimationData& data = transformAnimationDatas[i];
		writeString(data.objectName, out);
		data.write(out);
	}
	uint32_t morphNameLen = curveNames.size();
	out.write((char*)&morphNameLen, sizeof(uint32_t));
	for (auto b = curveNames.begin(), e = curveNames.end(); b != e; b++) {
		writeString(b->first, out);
		uint32_t index = b->second;
		out.write((char*)&index, sizeof(uint32_t));
	}
	uint32_t morphLen = curves.size();
	out.write((char*)&morphLen, sizeof(uint32_t));
	for (auto b = curves.begin(), e = curves.end(); b != e; b++) {
		uint32_t index = b->first;
		out.write((char*)&index, sizeof(uint32_t));
		const Curve<float, float>& curve = b->second;
		uint32_t curveLen = curve.keys.size();
		out.write((char*)&curveLen, sizeof(uint32_t));
		for (auto cb = curve.keys.begin(), ce = curve.keys.end(); cb != ce; cb++) {
			float_t frame = cb->first;
			out.write((char*)&frame, sizeof(float_t));
			float_t weight = cb->second.value;
			out.write((char*)&weight, sizeof(float_t));
			uint8_t mode = cb->second.mode;
			out.write((char*)&mode, sizeof(uint8_t));
			float_t inTangent = cb->second.inTangent;
			out.write((char*)&inTangent, sizeof(float_t));
			float_t inRate = cb->second.inRate;
			out.write((char*)&inRate, sizeof(float_t));
			float_t outTangent = cb->second.outTangent;
			out.write((char*)&outTangent, sizeof(float_t));
			float_t outRate = cb->second.outRate;
			out.write((char*)&outRate, sizeof(float_t));
		}
	}
}

AnimationClip::AnimationClip()
{
}

AnimationClip::AnimationClip(AnimationClipData & clipData) : animationClipData(&clipData)
{
	reset();
}

map<unsigned int, float>* AnimationClip::getCurveCurrentValue()
{
	if (useMapedCurve) {
		for (auto b = curveChannelMap.begin(), e = curveChannelMap.end(); b != e; b++) {
			curveCurrentValueMaped[b->second] = curveCurrentValue[b->first];
		}
		return &curveCurrentValueMaped;
	}
	else
		return &curveCurrentValue;
}

void AnimationClip::mapCurveChannel(const vector<string>& names)
{
	if (animationClipData == NULL)
		return;
	if (names.empty())
		return;
	if (animationClipData->curveNames.empty())
		return;
	useMapedCurve = true;
	for (auto b = animationClipData->curveNames.begin(), e = animationClipData->curveNames.end(); b != e; b++) {
		for (int i = 0;i < names.size(); i++) {
			if (b->first == names[i])
				curveChannelMap[b->second] = i;
		}
	}
}

void AnimationClip::setAnimationClipData(AnimationClipData & clipData)
{
	animationClipData = &clipData;
	reset();
}

bool AnimationClip::setTargetTransform(::Transform & target)
{
	if (animationClipData == NULL)
		return false;
	unsigned int id = animationClipData->getAnimatinDataIndex(target.name);
	if (id == -1)
		return false;
	animationPlayer[id].setTargetTransform(target);
	return true;
}

bool AnimationClip::setTargetTransform(const string & name, ::Transform & target)
{
	if (animationClipData == NULL)
		return false;
	unsigned int id = animationClipData->getAnimatinDataIndex(name);
	if (id == -1)
		return false;
	animationPlayer[id].setTargetTransform(target);
	return true;
}

bool AnimationClip::update(float deltaTime)
{
	if (animationClipData == NULL)
		return false;
	bool ok = false;
	for (int i = 0; i < animationPlayer.size(); i++) {
		if (animationPlayer[i].update(deltaTime))
			ok = true;
	}
	for (int i = 0; i < curvePlayer.size(); i++) {
		curveCurrentValue[i] = curvePlayer[i].update(deltaTime);
		if (curvePlayer[i].isPlaying)
			ok = true;
	}
	return ok;
}

void AnimationClip::getTransformData(vector<TransformData>& data)
{
	int s = animationPlayer.size();
	data.resize(s);
	for (int i = 0; i < s; i++)
		data[i] = animationPlayer[i].getTransformData();
}

TransformData * AnimationClip::getTransformData(const string & name)
{
	if (animationClipData == NULL)
		return NULL;
	unsigned int index = animationClipData->getAnimatinDataIndex(name);
	if (index >= animationPlayer.size())
		return NULL;
	return &animationPlayer[index].getTransformData();
}

bool AnimationClip::isLoop()
{
	if (animationClipData == NULL)
		return false;
	bool _loop = false;
	for (int i = 0; i < animationPlayer.size(); i++) {
		if (animationPlayer[i].isLoop()) {
			_loop = true;
			break;
		}
	}
	if (!_loop)
		for (auto b = curvePlayer.begin(), e = curvePlayer.end(); b != e; b++) {
			if (b->second.loop) {
				_loop = true;
				break;
			}
		}
	return _loop;
}

void AnimationClip::setLoop(bool loop)
{
	if (animationClipData == NULL)
		return;
	for (int i = 0; i < animationPlayer.size(); i++) {
		animationPlayer[i].setLoop(loop);
	}
	for (auto b = curvePlayer.begin(), e = curvePlayer.end(); b != e; b++) {
		b->second.loop = loop;
	}
}

void AnimationClip::play()
{
	if (animationClipData == NULL)
		return;
	for (int i = 0; i < animationPlayer.size(); i++) {
		animationPlayer[i].play();
	}
	for (auto b = curvePlayer.begin(), e = curvePlayer.end(); b != e; b++) {
		b->second.play();
	}
}

void AnimationClip::pause()
{
	if (animationClipData == NULL)
		return;
	for (int i = 0; i < animationPlayer.size(); i++) {
		animationPlayer[i].pause();
	}
	for (auto b = curvePlayer.begin(), e = curvePlayer.end(); b != e; b++) {
		b->second.pause();
	}
}

void AnimationClip::stop()
{
	if (animationClipData == NULL)
		return;
	for (int i = 0; i < animationPlayer.size(); i++) {
		animationPlayer[i].stop();
	}
	for (auto b = curvePlayer.begin(), e = curvePlayer.end(); b != e; b++) {
		b->second.stop();
	}
}

bool AnimationClip::playing()
{
	if (animationClipData == NULL)
		return false;
	for (int i = 0; i < animationPlayer.size(); i++) {
		if (animationPlayer[i].playing())
			return true;
	}
	for (auto b = curvePlayer.begin(), e = curvePlayer.end(); b != e; b++) {
		if (b->second.isPlaying)
			return true;
	}
	return false;
}

void AnimationClip::setSpeed(float speed)
{
	if (animationClipData == NULL)
		return;
	for (int i = 0; i < animationPlayer.size(); i++) {
		animationPlayer[i].setSpeed(speed);
	}
	for (auto b = curvePlayer.begin(), e = curvePlayer.end(); b != e; b++) {
		b->second.setSpeed(speed);
	}
}

void AnimationClip::reset()
{
	if (animationClipData != NULL) {
		if (animationClipData->transformAnimationDatas.size() != animationPlayer.size())
			animationPlayer.resize(animationClipData->transformAnimationDatas.size());
		for (int i = 0; i < animationPlayer.size(); i++) {
			animationPlayer[i].setAnimationData(animationClipData->transformAnimationDatas[i]);
		}
		for (auto b = animationClipData->curves.begin(), e = animationClipData->curves.end(); b != e; b++) {
			auto iterp = curvePlayer.find(b->first);
			if (iterp == curvePlayer.end())
				curvePlayer.insert(pair<unsigned int, CurvePlayer<float, float>>(b->first, CurvePlayer<float, float>(b->second)));
			else
				iterp->second.setCurve(b->second);
			auto iterv = curveCurrentValue.find(b->first);
			if (iterv == curveCurrentValue.end())
				curveCurrentValue.insert(pair<unsigned int, float>(b->first, 0));
			else
				iterv->second = 0;
		}
	}
}

BlendSpaceAnimation::BlendSpaceAnimation()
{
}

BlendSpaceAnimation::BlendSpaceAnimation(SkeletonData & skeletonData)
{
	setSkeletonData(skeletonData);
}

bool BlendSpaceAnimation::isValid()
{
	return skeletonData != NULL;
}

bool BlendSpaceAnimation::setSkeletonData(SkeletonData & skeletonData)
{
	if (!targetTransforms.empty())
		return false;
	this->skeletonData = &skeletonData;
	targetTransforms.resize(skeletonData.boneList.size());
	transformDatas.resize(skeletonData.boneList.size());
	return true;
}

bool BlendSpaceAnimation::addAnimationClipData(const Vector2f & pos, AnimationClipData & clipData)
{
	if (!isValid())
		return false;
	if (!animationClipWrap.empty()) {
		AnimationClipData& data = *animationClipWrap.back().second->animationClipData;
		if (!data.canBlend(clipData))
			return false;
	}
	stop();
	animationClipWrap.push_back(make_pair(pos, new AnimationClip()));
	animationClipWrap.back().second->setAnimationClipData(clipData);
	animationClipWrap.back().second->setLoop(true);
	setBlendWeight(blendWeight);
	return true;
}

bool BlendSpaceAnimation::removeAnimationClipData(unsigned int index)
{
	if (index < animationClipWrap.size()) {
		delete animationClipWrap[index].second;
		animationClipWrap.erase(animationClipWrap.begin() + index);
		setBlendWeight(blendWeight);
		return true;
	}
	return false;
}

void BlendSpaceAnimation::removeAllAnimationClipData()
{
	stop();
	for (int i = 0; i < animationClipWrap.size(); i++) {
		delete animationClipWrap[i].second;
	}
	animationClipWrap.clear();
	animationWeights.clear();
}

bool BlendSpaceAnimation::setTargetTransform(::Transform & target)
{
	if (!isValid())
		return false;
	BoneData* b = skeletonData->getBoneData(target.name);
	if (b == NULL)
		return false;
	targetTransforms[b->index] = &target;
	return true;
}

bool BlendSpaceAnimation::setTargetTransform(const string & name, ::Transform & target)
{
	if (!isValid())
		return false;
	BoneData* b = skeletonData->getBoneData(name);
	if (b == NULL)
		return false;
	targetTransforms[b->index] = &target;
	return true;
}

void BlendSpaceAnimation::setBlendWeight(const Vector2f & weight)
{
	blendWeight = weight;
	getWeights(animationWeights);
}

float clamp(float v, float a, float b) {
	float t = v > a ? v : a;
	return t < b ? t : b;
}

float dot(const Vector2f& a, const Vector2f& b) {
	return a.x() * b.x() + a.y() * b.y();
}

unsigned int BlendSpaceAnimation::getWeights(vector<pair<unsigned int, float>>& weights)
{
	weights.clear();
	float total_weight = 0.0;
	unsigned int POINT_COUNT = animationClipWrap.size();

	for (int i = 0; i < POINT_COUNT; ++i)
	{
		// Calc vec i -> sample
		Vector2f    point_i = animationClipWrap[i].first;
		Vector2f    vec_is = blendWeight - point_i;

		float   weight = 1.0;

		for (int j = 0; j < POINT_COUNT; ++j)
		{
			if (j == i)
				continue;

			// Calc vec i -> j
			Vector2f    point_j = animationClipWrap[j].first;
			Vector2f    vec_ij = point_j - point_i;

			// Calc Weight
			float lensq_ij = dot(vec_ij, vec_ij);
			float new_weight = dot(vec_is, vec_ij) / lensq_ij;
			new_weight = 1.0 - new_weight;
			new_weight = clamp(new_weight, 0, 1);

			weight = weight < new_weight ? weight : new_weight;
		}
		if (weight > 0)
			weights.emplace_back(make_pair(i, weight));
		total_weight += weight;
	}
	float maxW = 0;
	unsigned int maxI = -1;
	for (int i = 0; i < weights.size(); ++i)
	{
		weights[i].second /= total_weight;
		if (weights[i].second > maxW) {
			maxI = weights[i].second;
			maxI = i;
		}
	}
	return maxI;
}

bool BlendSpaceAnimation::update(float deltaTime)
{
	if (!isValid() || animationClipWrap.empty() || animationWeights.empty())
		return false;
	bool isplay = false;
	float duration = 0;
	for (int i = 0; i < animationWeights.size(); i++) {
		AnimationClip* clip = animationClipWrap[animationWeights[i].first].second;
		duration += clip->animationClipData->duration * animationWeights[i].second;
	}
	for (int i = 0; i < animationClipWrap.size(); i++) {
		AnimationClip* clip = animationClipWrap[i].second;
		if (clip->update((clip->animationClipData->duration / duration) * deltaTime))
			isplay = true;
	}
	if (!isplay)
		return false;
	int size = transformDatas.size();
	for (int t = 0; t < size; t++) {
		BoneData* bone = skeletonData->getBoneData(t);
		TransformData data;
		TransformData bondPos;
		bool getBindPos = false;
		data.scale = { 0, 0, 0 };
		if (animationWeights.empty()) {
			Matrix4f& tm = bone->transformMatrix;
			Eigen::Transform<float, 3, Eigen::Affine> t;
			t = tm;
			bondPos.position = t.translation();
			bondPos.rotation = Quaternionf(t.rotation());
			bondPos.scale = { tm.block(0, 0, 3, 1).norm(), tm.block(0, 1, 3, 1).norm(), tm.block(0, 2, 3, 1).norm() };
			data = bondPos;
			getBindPos = true;
		}
		else for (int i = 0; i < animationWeights.size(); i++) {
			TransformData* pd = animationClipWrap[animationWeights[i].first].second->getTransformData(bone->name);
			TransformData d;
			if (pd == NULL) {
				if (!getBindPos) {
					Matrix4f& tm = bone->transformMatrix;
					Eigen::Transform<float, 3, Eigen::Affine> t;
					t = tm;
					bondPos.position = t.translation();
					bondPos.rotation = Quaternionf(t.rotation());
					bondPos.scale = { tm.block(0, 0, 3, 1).norm(), tm.block(0, 1, 3, 1).norm(), tm.block(0, 2, 3, 1).norm() };
					getBindPos = true;
				}
				d = bondPos;
			}
			else
				d = *pd;
			data.add(d.mutiply(animationWeights[i].second));
		}
		transformDatas[t] = data;
		::Transform* trans = targetTransforms[t];
		if (trans != NULL) {
			trans->setPosition(data.position);
			trans->setRotation(data.rotation);
			trans->setScale(data.scale);
		}
	}
	return true;
}

vector<TransformData> & BlendSpaceAnimation::getTransformData()
{
	return transformDatas;
}

bool BlendSpaceAnimation::isLoop()
{
	if (!isValid() || animationClipWrap.empty())
		return false;
	for (int i = 0; i < animationClipWrap.size(); i++) {
		if (animationClipWrap[i].second->isLoop())
			return true;
	}
	return false;
}

void BlendSpaceAnimation::setLoop(bool loop)
{
	for (int i = 0; i < animationClipWrap.size(); i++)
		animationClipWrap[i].second->setLoop(loop);
}

void BlendSpaceAnimation::play()
{
	for (int i = 0; i < animationClipWrap.size(); i++)
		animationClipWrap[i].second->play();
}

void BlendSpaceAnimation::pause()
{
	for (int i = 0; i < animationClipWrap.size(); i++)
		animationClipWrap[i].second->pause();
}

void BlendSpaceAnimation::stop()
{
	for (int i = 0; i < animationClipWrap.size(); i++)
		animationClipWrap[i].second->stop();
}

bool BlendSpaceAnimation::playing()
{
	if (!isValid() || animationClipWrap.empty())
		return false;
	for (int i = 0; i < animationClipWrap.size(); i++) {
		if (animationClipWrap[i].second->playing())
			return true;
	}
	return false;
}

void BlendSpaceAnimation::setSpeed(float speed)
{
	for (int i = 0; i < animationClipWrap.size(); i++)
		animationClipWrap[i].second->setSpeed(speed);
}

void BlendSpaceAnimation::reset()
{
	for (int i = 0; i < animationClipWrap.size(); i++)
		animationClipWrap[i].second->reset();
}

AnimationClipData * AnimationLoader::loadCameraAnimation(const string & file)
{
	ifstream f(file);
	if (f.fail()) {
		f.close();
		return NULL;
	}
	SerializationInfoParser sp(f);
	bool ok = sp.parse();
	if (!ok)
		return NULL;
	f.close();
	SerializationInfo& info = sp.infos[0];
	struct CameraFrame
	{
		float frame;
		SVector3f position;
		SVector3f positionInTan;
		SVector3f positionInRate;
		SVector3f positionOutTan;
		SVector3f positionOutRate;
		SVector3f rotation;
		float rotationInTan;
		float rotationInRate;
		float rotationOutTan;
		float rotationOutRate;
		float distance;
		float distanceInTan;
		float distanceInRate;
		float distanceOutTan;
		float distanceOutRate;
		float fov;
		float fovInTan;
		float fovInRate;
		float fovOutTan;
		float fovOutRate;
	};
	experimental::filesystem::path p = file;
	string ext = p.extension().generic_string();
	string name = p.filename().generic_string();
	name = name.substr(0, name.size() - ext.size());
	AnimationClipData* data = new AnimationClipData(name);
	TransformAnimationData& tranData = data->addAnimatinData("Camera");
	Curve<float, float>& fovCurve = data->addMorphAnimationData(0);
	Curve<float, float>& disCurve = data->addMorphAnimationData(1);
	float maxFrame = 0;
	for (int i = 0; i < info.sublists.size(); i++) {
		SerializationInfo& fi = info.sublists[i];
		CameraFrame f;
		fi.get("frame", f.frame);
		fi.get("position", f.position);
		fi.get("positionInTan", f.positionInTan);
		fi.get("positionInRate", f.positionInRate);
		fi.get("positionOutTan", f.positionOutTan);
		fi.get("positionOutRate", f.positionOutRate);
		fi.get("rotation", f.rotation);
		fi.get("rotationInTan", f.rotationInTan);
		fi.get("rotationInRate", f.rotationInRate);
		fi.get("rotationOutTan", f.rotationOutTan);
		fi.get("rotationOutRate", f.rotationOutRate);
		fi.get("distance", f.distance);
		fi.get("distanceInTan", f.distanceInTan);
		fi.get("distanceInRate", f.distanceInRate);
		fi.get("distanceOutTan", f.distanceOutTan);
		fi.get("distanceOutRate", f.distanceOutRate);
		fi.get("fov", f.fov);
		fi.get("fovInTan", f.fovInTan);
		fi.get("fovInRate", f.fovInRate);
		fi.get("fovOutTan", f.fovOutTan);
		fi.get("fovOutRate", f.fovOutRate);
		if (maxFrame < f.frame)
			maxFrame = f.frame;
		tranData.positionCurve.insert(f.frame, CurveValue<Vector3f>(f.position, f.positionInTan, f.positionOutTan, f.positionInRate, f.positionOutRate));
		Quaternionf R = AngleAxisf(f.rotation.x / 180.0 * PI, Vector3f::UnitX()) *
			AngleAxisf(f.rotation.y / 180.0 * PI, Vector3f::UnitY()) *
			AngleAxisf(f.rotation.z / 180.0 * PI, Vector3f::UnitZ());
		tranData.rotationCurve.insert(f.frame, CurveValue<Quaternionf>(R, f.rotationInTan, f.rotationOutTan, f.rotationInRate, f.rotationOutRate));
		disCurve.insert(f.frame, CurveValue<float>(f.distance, f.distanceInTan, f.distanceOutTan, f.distanceInRate, f.distanceOutRate));
		fovCurve.insert(f.frame, CurveValue<float>(f.fov, f.fovInTan, f.fovOutTan, f.fovInRate, f.fovOutRate));
	}
	tranData.positionCurve.duration = maxFrame;
	tranData.rotationCurve.duration = maxFrame;
	fovCurve.duration = maxFrame;
	disCurve.duration = maxFrame;
	data->duration = maxFrame;
	data->pack();
	return data;
}

AnimationClipData * AnimationLoader::loadMotionAnimation(const string & file)
{
	ifstream f(file);
	if (f.fail()) {
		f.close();
		return NULL;
	}
	SerializationInfoParser sp(f);
	bool ok = sp.parse();
	if (!ok)
		return NULL;
	f.close();
	SerializationInfo& info = sp.infos[0];
	struct BoneFrame
	{
		float frame;
		SVector3f position;
		SVector3f positionInTan;
		SVector3f positionInRate;
		SVector3f positionOutTan;
		SVector3f positionOutRate;
		SQuaternionf rotation;
		float rotationInTan;
		float rotationInRate;
		float rotationOutTan;
		float rotationOutRate;
	};
	struct MorphFrame
	{
		float frame;
		float weight;
	};
	experimental::filesystem::path p = file;
	string ext = p.extension().generic_string();
	string name = p.filename().generic_string();
	name = name.substr(0, name.size() - ext.size());
	AnimationClipData* data = new AnimationClipData(name);
	float maxFrame = 0;
	const SerializationInfo* boneFrames = info.get("boneFrames");
	if (boneFrames != NULL) {
		for (int i = 0; i < boneFrames->sublists.size(); i++) {
			const SerializationInfo& ffi = boneFrames->sublists[i];
			TransformAnimationData& tranData = data->addAnimatinData(ffi.name);
			for (int t = 0; t < ffi.sublists.size(); t++) {
				const SerializationInfo& fi = ffi.sublists[t];
				BoneFrame f;
				fi.get("frame", f.frame);
				if (fi.get("position", f.position)) {
					bool hasInterp = true;
					hasInterp &= fi.get("positionInTan", f.positionInTan);
					hasInterp &= fi.get("positionInRate", f.positionInRate);
					hasInterp &= fi.get("positionOutTan", f.positionOutTan);
					hasInterp &= fi.get("positionOutRate", f.positionOutRate);
					if (hasInterp)
						tranData.positionCurve.insert(f.frame, CurveValue<Vector3f>(f.position, f.positionInTan, f.positionOutTan, f.positionInRate, f.positionOutRate));
					else
						tranData.positionCurve.insert(f.frame, CurveValue<Vector3f>(CurveValue<Vector3f>::Linear, f.position));
				}
				if (fi.get("rotation", f.rotation)) {
					bool hasInterp = true;
					hasInterp &= fi.get("rotationInTan", f.rotationInTan);
					hasInterp &= fi.get("rotationInRate", f.rotationInRate);
					hasInterp &= fi.get("rotationOutTan", f.rotationOutTan);
					hasInterp &= fi.get("rotationOutRate", f.rotationOutRate);
					if (hasInterp)
						tranData.rotationCurve.insert(f.frame, CurveValue<Quaternionf>(f.rotation, f.rotationInTan, f.rotationOutTan, f.rotationInRate, f.rotationOutRate));
					else
						tranData.rotationCurve.insert(f.frame, CurveValue<Quaternionf>(CurveValue<Quaternionf>::Linear, f.rotation));
				}
				if (maxFrame < f.frame)
					maxFrame = f.frame;
			}
		}
		for (int i = 0; i < data->transformAnimationDatas.size(); i++) {
			data->transformAnimationDatas[i].positionCurve.duration = maxFrame;
			data->transformAnimationDatas[i].rotationCurve.duration = maxFrame;
		}
	}
	const SerializationInfo* morphFrames = info.get("morphFrames");
	if (morphFrames != NULL) {
		for (int i = 0; i < morphFrames->sublists.size(); i++) {
			const SerializationInfo& ffi = morphFrames->sublists[i];
			Curve<float, float>& morphData = data->addMorphAnimationData(ffi.name);
			for (int t = 0; t < ffi.sublists.size(); t++) {
				const SerializationInfo& fi = ffi.sublists[t];
				MorphFrame f;
				fi.get("frame", f.frame);
				fi.get("weight", f.weight);
				morphData.insert(f.frame, CurveValue<float>(CurveValue<float>::Linear, f.weight));
			}
		}
		for (auto b = data->curves.begin(), e = data->curves.end(); b != e; b++)
			b->second.duration = maxFrame;
		data->duration = maxFrame;
	}
	data->pack();
	return data;
}

AnimationClipData * AnimationLoader::readAnimation(const string & file)
{
	ifstream f(file, ios::binary | ios::in);
	if (f.fail()) {
		f.close();
		return NULL;
	}
	experimental::filesystem::path p = file;
	string ext = p.extension().generic_string();
	string name = p.filename().generic_string();
	name = name.substr(0, name.size() - ext.size());
	AnimationClipData* data = new AnimationClipData(name);
	if (!data->read(f)) {
		delete data;
		f.close();
		return NULL;
	}
	f.close();
	return data;
}

bool AnimationLoader::writeAnimation(const AnimationClipData data, const string & file)
{
	ofstream f(file, ios::binary | ios::out);
	if (f.fail()) {
		f.close();
		return false;
	}
	data.write(f);
	f.close();
	return true;
}

void TransformAnimationData::read(istream & in)
{
	float_t maxFrame = 0;
	uint32_t posLen = 0;
	in.read((char*)&posLen, sizeof(uint32_t));
	for (int i = 0; i < posLen; i++) {
		float_t frame;
		in.read((char*)&frame, sizeof(float_t));
		float_t pos[3];
		in.read((char*)&pos, sizeof(float_t) * 3);
		uint8_t mode;
		in.read((char*)&mode, sizeof(uint8_t));
		float_t inTangent[3];
		in.read((char*)&inTangent, sizeof(float_t) * 3);
		float_t inRate[3];
		in.read((char*)&inRate, sizeof(float_t) * 3);
		float_t outTangent[3];
		in.read((char*)&outTangent, sizeof(float_t) * 3);
		float_t outRate[3];
		in.read((char*)&outRate, sizeof(float_t) * 3);
		CurveValue<Vector3f> value = CurveValue<Vector3f>((CurveValue<Vector3f>::KeySmoothMode)mode, Vector3f(pos[0], pos[1], pos[2]));
		value.inTangent.x() = inTangent[0];
		value.inTangent.y() = inTangent[1];
		value.inTangent.z() = inTangent[2];
		value.inRate.x() = inRate[0];
		value.inRate.y() = inRate[1];
		value.inRate.z() = inRate[2];
		value.outTangent.x() = outTangent[0];
		value.outTangent.y() = outTangent[1];
		value.outTangent.z() = outTangent[2];
		value.outRate.x() = outRate[0];
		value.outRate.y() = outRate[1];
		value.outRate.z() = outRate[2];
		positionCurve.insert(frame, value);
		if (frame > maxFrame)
			maxFrame = frame;
	}
	uint32_t rotLen = 0;
	in.read((char*)&rotLen, sizeof(uint32_t));
	for (int i = 0; i < rotLen; i++) {
		float_t frame;
		in.read((char*)&frame, sizeof(float_t));
		float_t rot[4];
		in.read((char*)&rot, sizeof(float_t) * 4);
		uint8_t mode;
		in.read((char*)&mode, sizeof(uint8_t));
		float_t inTangent;
		in.read((char*)&inTangent, sizeof(float_t));
		float_t inRate;
		in.read((char*)&inRate, sizeof(float_t));
		float_t outTangent;
		in.read((char*)&outTangent, sizeof(float_t));
		float_t outRate;
		in.read((char*)&outRate, sizeof(float_t));
		CurveValue<Quaternionf> value = CurveValue<Quaternionf>((CurveValue<Quaternionf>::KeySmoothMode)mode, Quaternionf(rot[3], rot[0], rot[1], rot[2]));
		value.inTangent = inTangent;
		value.inRate = inRate;
		value.outTangent = outTangent;
		value.outRate = outRate;
		rotationCurve.insert(frame, value);
		if (frame > maxFrame)
			maxFrame = frame;
	}
	uint32_t scaLen = 0;
	in.read((char*)&scaLen, sizeof(uint32_t));
	for (int i = 0; i < scaLen; i++) {
		float_t frame;
		in.read((char*)&frame, sizeof(float_t));
		float_t pos[3];
		in.read((char*)&pos, sizeof(float_t) * 3);
		uint8_t mode;
		in.read((char*)&mode, sizeof(uint8_t));
		float_t inTangent[3];
		in.read((char*)&inTangent, sizeof(float_t) * 3);
		float_t inRate[3];
		in.read((char*)&inRate, sizeof(float_t) * 3);
		float_t outTangent[3];
		in.read((char*)&outTangent, sizeof(float_t) * 3);
		float_t outRate[3];
		in.read((char*)&outRate, sizeof(float_t) * 3);
		CurveValue<Vector3f> value = CurveValue<Vector3f>((CurveValue<Vector3f>::KeySmoothMode)mode, Vector3f(pos[0], pos[1], pos[2]));
		value.inTangent.x() = inTangent[0];
		value.inTangent.y() = inTangent[1];
		value.inTangent.z() = inTangent[2];
		value.inRate.x() = inRate[0];
		value.inRate.y() = inRate[1];
		value.inRate.z() = inRate[2];
		value.outTangent.x() = outTangent[0];
		value.outTangent.y() = outTangent[1];
		value.outTangent.z() = outTangent[2];
		value.outRate.x() = outRate[0];
		value.outRate.y() = outRate[1];
		value.outRate.z() = outRate[2];
		scaleCurve.insert(frame, value);
		if (frame > maxFrame)
			maxFrame = frame;
	}
	positionCurve.duration = maxFrame;
	rotationCurve.duration = maxFrame;
	scaleCurve.duration = maxFrame;
}

void TransformAnimationData::write(ostream & out) const
{
	uint32_t posLen = positionCurve.keys.size();
	out.write((char*)&posLen, sizeof(uint32_t));
	for (auto b = positionCurve.keys.begin(), e = positionCurve.keys.end(); b != e; b++) {
		float_t frame = b->first;
		out.write((char*)&frame, sizeof(float_t));
		float_t pos[3] = { b->second.value.x(), b->second.value.y(), b->second.value.z() };
		out.write((char*)&pos, sizeof(float_t) * 3);
		uint8_t mode = b->second.mode;
		out.write((char*)&mode, sizeof(uint8_t));
		float_t inTangent[3] = { b->second.inTangent.x(), b->second.inTangent.y(), b->second.inTangent.z() };
		out.write((char*)&inTangent, sizeof(float_t) * 3);
		float_t inRate[3] = { b->second.inRate.x(), b->second.inRate.y(), b->second.inRate.z() };
		out.write((char*)&inRate, sizeof(float_t) * 3);
		float_t outTangent[3] = { b->second.outTangent.x(), b->second.outTangent.y(), b->second.outTangent.z() };
		out.write((char*)&outTangent, sizeof(float_t) * 3);
		float_t outRate[3] = { b->second.outRate.x(), b->second.outRate.y(), b->second.outRate.z() };
		out.write((char*)&outRate, sizeof(float_t) * 3);
	}
	uint32_t rotLen = rotationCurve.keys.size();
	out.write((char*)&rotLen, sizeof(uint32_t));
	for (auto b = rotationCurve.keys.begin(), e = rotationCurve.keys.end(); b != e; b++) {
		float_t frame = b->first;
		out.write((char*)&frame, sizeof(float_t));
		float_t rot[4] = { b->second.value.x(), b->second.value.y(), b->second.value.z(), b->second.value.w() };
		out.write((char*)&rot, sizeof(float_t) * 4);
		uint8_t mode = b->second.mode;
		out.write((char*)&mode, sizeof(uint8_t));
		float_t inTangent = b->second.inTangent;
		out.write((char*)&inTangent, sizeof(float_t));
		float_t inRate = b->second.inRate;
		out.write((char*)&inRate, sizeof(float_t));
		float_t outTangent = b->second.outTangent;
		out.write((char*)&outTangent, sizeof(float_t));
		float_t outRate = b->second.outRate;
		out.write((char*)&outRate, sizeof(float_t));
	}
	uint32_t scaleLen = scaleCurve.keys.size();
	out.write((char*)&scaleLen, sizeof(uint32_t));
	for (auto b = scaleCurve.keys.begin(), e = scaleCurve.keys.end(); b != e; b++) {
		float_t frame = b->first;
		out.write((char*)&frame, sizeof(float_t));
		float_t sca[3] = { b->second.value.x(), b->second.value.y(), b->second.value.z() };
		out.write((char*)&sca, sizeof(float_t) * 3);
		uint8_t mode = b->second.mode;
		out.write((char*)&mode, sizeof(uint8_t));
		float_t inTangent[3] = { b->second.inTangent.x(), b->second.inTangent.y(), b->second.inTangent.z() };
		out.write((char*)&inTangent, sizeof(float_t) * 3);
		float_t inRate[3] = { b->second.inRate.x(), b->second.inRate.y(), b->second.inRate.z() };
		out.write((char*)&inRate, sizeof(float_t) * 3);
		float_t outTangent[3] = { b->second.outTangent.x(), b->second.outTangent.y(), b->second.outTangent.z() };
		out.write((char*)&outTangent, sizeof(float_t) * 3);
		float_t outRate[3] = { b->second.outRate.x(), b->second.outRate.y(), b->second.outRate.z() };
		out.write((char*)&outRate, sizeof(float_t) * 3);
	}
}