#include "ConsoleWindow.h"
#include "Utility.h"
#include <fstream>
#include "imgui_stdlib.h"
#include "PythonManager.h"
#include "Camera.h"

ConsoleWindow::ConsoleWindow(Object & object, string name, bool defaultShow) : UIWindow(object, name, defaultShow)
{
}

void ConsoleWindow::onRenderWindow(GUIRenderInfo & info)
{
	if (ImGui::BeginTabBar("ConsoleTab")) {
		if (ImGui::BeginTabItem("Engine")) {
			ImGui::Text("Filter");
			ImGui::Selectable("Log", &showLog, 0, { 60, 0 });
			ImGui::SameLine(0, 15);
			ImGui::Selectable("Warning", &showWarning, 0, { 60, 0 });
			ImGui::SameLine(0, 15);
			ImGui::Selectable("Error", &showError, 0, { 60, 0 });
			ImGui::BeginChild("Output_Area", { 0, 0 }, false, ImGuiWindowFlags_HorizontalScrollbar);
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
			for (auto b = Console::logs.begin(), e = Console::logs.end(); b != e; b++) {
				switch (b->state)
				{
				case Console::Log_Normal:
					if (showLog)
						ImGui::Text("[Log][%d:%d:%d:%d]%s", b->timeStamp.getHour() + b->timeStamp.getDay() * 24,
							b->timeStamp.getMinute(), b->timeStamp.getSecond(), b->timeStamp.getMillisecond(),
							b->text.c_str());
					break;
				case Console::Log_Warning:
					if (showWarning)
						ImGui::TextColored({ 1, 1, 0, 1 }, "[Warning][%d:%d:%d:%d]%s", b->timeStamp.getHour() + b->timeStamp.getDay() * 24,
							b->timeStamp.getMinute(), b->timeStamp.getSecond(), b->timeStamp.getMillisecond(),
							b->text.c_str());
					break;
				case Console::Log_Error:
					if (showError)
						ImGui::TextColored({ 1, 0, 0, 1 }, "[Error][%d:%d:%d:%d]%s", b->timeStamp.getHour() + b->timeStamp.getDay() * 24,
							b->timeStamp.getMinute(), b->timeStamp.getSecond(), b->timeStamp.getMillisecond(),
							b->text.c_str());
					break;
				default:
					break;
				}
			}
			if (logSize != Console::logs.size()) {
				if ((Console::logs.back().state == Console::Log_Normal && showLog) ||
					(Console::logs.back().state == Console::Log_Warning && showWarning) ||
					(Console::logs.back().state == Console::Log_Error && showError))
					ImGui::SetScrollHereY(1.0f);
				logSize = Console::logs.size();
			}
			ImGui::PopStyleVar();
			ImGui::EndChild();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Stdout")) {
			ImGui::BeginChild("Output_Area", { 0, 0 }, false, ImGuiWindowFlags_HorizontalScrollbar);
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
			ImGui::Text(Console::STDOUT.str().c_str());
			ImGui::PopStyleVar();
			ImGui::EndChild();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Stderr")) {
			ImGui::BeginChild("Output_Area", { 0, 0 }, false, ImGuiWindowFlags_HorizontalScrollbar);
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
			ImGui::Text(Console::STDERR.str().c_str());
			ImGui::PopStyleVar();
			ImGui::EndChild();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("TimerRecord")) {
			ImGui::BeginChild("Timer_Area", { 0, 0 }, false, ImGuiWindowFlags_HorizontalScrollbar);
			for (auto b = Console::timers.begin(), e = Console::timers.end(); b != e; b++) {
				if (ImGui::CollapsingHeader(b->first.c_str())) {
					float* avgs = new float[b->second.times.size()];
					for (int i = 0; i < b->second.times.size(); i++) {
						Tag tag = { b->first, i };
						auto iter = timerSamples.find(tag);
						Sampler<float>* sampler = NULL;
						if (iter == timerSamples.end()) {
							sampler = &timerSamples.insert(make_pair(tag, Sampler<float>(60))).first->second;
						}
						else {
							sampler = &iter->second;
						}
						float dt = b->second.getInterval(i).toMillisecond();
						sampler->add(dt);
						float avg = sampler->getAverage();
						avgs[i] = avg;
						ImGui::PlotLines(b->second.times[i].first.c_str(), [](void* d, int i)->float { return ((Sampler<float>*)d)->get(i); },
							sampler, sampler->getSize(), 0, ("value: " + to_string(dt) + " avg: " + to_string(avg)).c_str());
					}
					struct PData
					{
						Timer* timer;
						float* avg;
					} pdata = { &b->second, avgs };
					ImGui::PlotTimeLine(b->first.c_str(), [](void* data, int i)->ImGui::PlotTimeLineData {
						Timer* timer = ((PData*)data)->timer;
						float* avg = ((PData*)data)->avg;
						return{ timer->times[i].first.c_str(), avg[i],
							(ImU32)ImColor::HSV((i + 1) / (float)timer->times.size(), 0.5, 0.5),
							(ImU32)ImColor(255, 255, 255) };
					}, (void*)&pdata, b->second.times.size(), 0, { 0, 40 });
					delete[] avgs;
				}
			}
			ImGui::EndChild();
			ImGui::EndTabItem();
		}
		if (info.renderCommandList != NULL && ImGui::BeginTabItem("RenderCommandList")) {
			ImGui::BeginChild("RenderCommandList_Area", { 0, 0 }, false, ImGuiWindowFlags_HorizontalScrollbar);
			ImGui::Text("Total MeshData: %d", MeshData::MeshDataCollection.size());
			for (auto camB = info.renderCommandList->commandList.begin(), camE = info.renderCommandList->commandList.end(); camB != camE; camB++) {
				if (ImGui::TreeNode(camB->first, "Camera (%s)", camB->first->name.c_str())) {
					ImGui::Text("ViewWidth: %d\tViewHeighr: %d", camB->first->size.x, camB->first->size.y);
					ImGui::Text("ZNear: %f\tZFar: %f", camB->first->zNear, camB->first->zFar);
					ImGui::Text("FOV: %f", camB->first->fov);
					ImGui::Text("Position: (%f, %f, %f)", camB->first->position.x(), camB->first->position.y(), camB->first->position.z());
					Vector3f r = camB->first->getEulerAngle(WORLD);
					ImGui::Text("Rotation: (%f, %f, %f)", r.x(), r.y(), r.z());
					for (auto shaderB = camB->second.begin(), shaderE = camB->second.end(); shaderB != shaderE; shaderB++) {
						if (ImGui::TreeNode(shaderB->first, "Shader (%s)", shaderB->first->name.c_str())) {
							ImGui::Text("RenderOrder: %d", shaderB->first->renderOrder);
							ImGui::Text("Shader Program ID: %d", shaderB->first->getProgramID());
							for (auto b = shaderB->first->shaderStages.begin(), e = shaderB->first->shaderStages.end(); b != e; b++) {
								ImGui::Text("%s Shader ID: %d", ShaderStage::enumShaderStageType(b->first), b->second->getShaderID());
							}
							for (auto matB = shaderB->second.begin(), matE = shaderB->second.end(); matB != matE; matB++) {
								if (ImGui::TreeNode(matB->first, "Material Instance (addr: %o)", matB->first)) {
									Material* mat = matB->first;
									if (mat != NULL) {
										ImGui::Text("Shader: %s", mat->getShaderName().c_str());
										for (auto b = mat->getColorField().begin(), e = mat->getColorField().end(); b != e; b++) {
											Color color = b->second.val;
											ImGui::ColorEdit4(b->first.c_str(), (float*)&color);
										}
										for (auto b = mat->getScalarField().begin(), e = mat->getScalarField().end(); b != e; b++) {
											float val = b->second.val;
											ImGui::DragFloat(b->first.c_str(), &val, 0.01);
										}
										static string choice;
										for (auto b = mat->getTextureField().begin(), e = mat->getTextureField().end(); b != e; b++) {
											/*if (b->first == "depthMap")
												continue;*/
											unsigned int id = b->second.val->getTextureID();
											ImGui::Image((void*)id, { 64, 64 }, { 0, 1 }, { 1, 0 });
											ImGui::SameLine();
											ImGui::Text(b->first.c_str());
										}
									}
									for (auto meshDataB = matB->second.begin(), meshDataE = matB->second.end(); meshDataB != meshDataE; meshDataB++) {
										if (meshDataB->first != NULL && ImGui::TreeNode(meshDataB->first, "MeshData (addr: %d)", meshDataB->first)) {
											ImGui::Text("Vertex Count: %d", meshDataB->first->vertices.size());
											ImGui::Text("VAO: %d", meshDataB->first->getVAO());
											ImGui::TreePop();
										}
									}
									ImGui::TreePop();
								}
							}
							ImGui::TreePop();
						}
					}
					ImGui::TreePop();
				}
			}
			ImGui::EndChild();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Python")) {
			if (ImGui::ArrowButton("Run", ImGuiDir_Right))
				PythonManager::run(code);
			float h = ImGui::GetWindowHeight();
			ImGui::BeginChild("TextView", { -1, -h * 0.3f - 3});
			ImGui::InputTextMultiline("##Code", &code, { -1, -1 }, ImGuiInputTextFlags_AllowTabInput);
			ImGui::EndChild();
			ImGui::BeginChild("LogView", { -1, -1 });
			ImGui::Selectable("PyLog", &showPyLog, 0, { 60, 0 });
			ImGui::SameLine(0, 15);
			ImGui::Selectable("PyError", &showPyError, 0, { 60, 0 });
			ImGui::BeginChild("PyOutput_Area", { 0, 0 }, false, ImGuiWindowFlags_HorizontalScrollbar);
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
			for (auto b = Console::pyLogs.begin(), e = Console::pyLogs.end(); b != e; b++) {
				switch (b->state)
				{
				case Console::Log_Normal:
					if (showPyLog)
						ImGui::Text(b->text.c_str());
					break;
				case Console::Log_Error:
					if (showPyError)
						ImGui::TextColored({ 1, 0, 0, 1 }, b->text.c_str());
					break;
				default:
					break;
				}
			}
			if (pyLogSize != Console::pyLogs.size()) {
				if ((Console::pyLogs.back().state == Console::Log_Normal && showPyLog) ||
					(Console::pyLogs.back().state == Console::Log_Error && showPyError))
					ImGui::SetScrollHereY(1.0f);
				pyLogSize = Console::pyLogs.size();
			}
			ImGui::PopStyleVar();
			ImGui::EndChild();
			ImGui::EndChild();
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
}
