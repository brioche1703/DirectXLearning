#pragma once

#include "TechniqueProbe.h"
#include "Technique.h"
#include "DynamicConstant.h"
#include "Model.h"
#include "ModelProbe.h"
#include "Node.h"
#include "XMUtils.h"
#include "SceneEntity.h"

#include "external/imgui/imgui.h"

#include <string>
#include <unordered_map>

namespace dx = DirectX;

class TP : public TechniqueProbe {
public:
	void OnSetTechnique() override {
		using namespace std::string_literals;
		ImGui::TextColored({ 0.4f,1.0f,0.6f,1.0f }, pTech->GetName().c_str());
		bool active = pTech->IsActive();
		ImGui::Checkbox(("Tech Active##"s + std::to_string(techIdx)).c_str(), &active);
		pTech->SetActiveState(active);
	}

	bool OnVisitBuffer(Dcb::Buffer& buf) override {
		namespace dx = DirectX;
		float dirty = false;
		const auto dcheck = [&dirty](bool changed) {dirty = dirty || changed; };
		auto tag = [tagScratch = std::string{}, tagString = "##" + std::to_string(bufIdx)]
		(const char* label) mutable {
			tagScratch = label + tagString;
			return tagScratch.c_str();
		};

		if (auto v = buf["scale"]; v.Exists()) {
			dcheck(ImGui::SliderFloat(tag("Scale"), &v, 1.0f, 2.0f, "%.3f", ImGuiSliderFlags_Logarithmic));
		}
		if (auto v = buf["offset"]; v.Exists()) {
			dcheck(ImGui::SliderFloat(tag("Offset"), &v, 0.0f, 1.0f, "%.3f", ImGuiSliderFlags_Logarithmic));
		}
		if (auto v = buf["materialColor"]; v.Exists()) {
			dcheck(ImGui::ColorPicker3(tag("Color"), reinterpret_cast<float*>(&static_cast<dx::XMFLOAT3&>(v))));
		}
		if (auto v = buf["specularColor"]; v.Exists()) {
			dcheck(ImGui::ColorPicker3(tag("Spec. Color"), reinterpret_cast<float*>(&static_cast<dx::XMFLOAT3&>(v))));
		}
		if (auto v = buf["specularGloss"]; v.Exists()) {
			dcheck(ImGui::SliderFloat(tag("Glossiness"), &v, 1.0f, 100.0f, "%.1f", ImGuiSliderFlags_Logarithmic));
		}
		if (auto v = buf["specularWeight"]; v.Exists()) {
			dcheck(ImGui::SliderFloat(tag("Spec. Weight"), &v, 0.0f, 2.0f));
		}
		if (auto v = buf["useSpecularMap"]; v.Exists()) {
			dcheck(ImGui::Checkbox(tag("Spec. Map Enable"), &v));
		}
		if (auto v = buf["useNormalMap"]; v.Exists()) {
			dcheck(ImGui::Checkbox(tag("Normal Map Enable"), &v));
		}
		if (auto v = buf["normalMapWeight"]; v.Exists()) {
			dcheck(ImGui::SliderFloat(tag("Normal Map Weight"), &v, 0.0f, 2.0f));
		}
		return dirty;
	}
};

class MP : public ModelProbe {
public:
	MP(std::string name)
		:
		name(std::move(name)) {
	}

	void SpawnWindow(Model& model) {
		ImGui::Begin(name.c_str());
		ImGui::Columns(2, nullptr, true);
		model.Accept(*this);

		ImGui::NextColumn();
		if (pSelectedNode != nullptr) {
			bool dirty = false;
			const auto dcheck = [&dirty](bool changed) {dirty = dirty || changed; };
			auto& tf = ResolveTransform();
			ImGui::TextColored({ 0.4f,1.0f,0.6f,1.0f }, "Translation");
			dcheck(ImGui::SliderFloat("X", &tf.x, -60.f, 60.f));
			dcheck(ImGui::SliderFloat("Y", &tf.y, -60.f, 60.f));
			dcheck(ImGui::SliderFloat("Z", &tf.z, -60.f, 60.f));
			dcheck(ImGui::SliderFloat("Scale", &tf.scale, 0.1f, 5.0f));
			ImGui::TextColored({ 0.4f,1.0f,0.6f,1.0f }, "Orientation");
			dcheck(ImGui::SliderAngle("X-rotation", &tf.xRot, -180.0f, 180.0f));
			dcheck(ImGui::SliderAngle("Y-rotation", &tf.yRot, -180.0f, 180.0f));
			dcheck(ImGui::SliderAngle("Z-rotation", &tf.zRot, -180.0f, 180.0f));
			if (dirty) {
				pSelectedNode->SetAppliedTransform(
					dx::XMMatrixScaling(tf.scale, tf.scale, tf.scale) *
					dx::XMMatrixRotationX(tf.xRot) *
					dx::XMMatrixRotationY(tf.yRot) *
					dx::XMMatrixRotationZ(tf.zRot) *
					dx::XMMatrixTranslation(tf.x, tf.y, tf.z)
				);
			}

			TP probe;
			pSelectedNode->Accept(probe);
		}
		ImGui::End();
	}

	void PrintProbe(Model& model) {
		ImGui::Columns(2, nullptr, true);

		model.Accept(*this);
		ImGui::NextColumn();
		if (pSelectedNode != nullptr) {
			bool dirty = false;
			const auto dcheck = [&dirty](bool changed) {dirty = dirty || changed; };
			auto& tf = ResolveTransform();
			ImGui::TextColored({ 0.4f,1.0f,0.6f,1.0f }, "Translation");
			dcheck(ImGui::SliderFloat("X", &tf.x, -60.f, 60.f));
			dcheck(ImGui::SliderFloat("Y", &tf.y, -60.f, 60.f));
			dcheck(ImGui::SliderFloat("Z", &tf.z, -60.f, 60.f));
			dcheck(ImGui::SliderFloat("Scale", &tf.scale, 0.1f, 5.0f));
			ImGui::TextColored({ 0.4f,1.0f,0.6f,1.0f }, "Orientation");
			dcheck(ImGui::SliderAngle("X-rotation", &tf.xRot, -180.0f, 180.0f));
			dcheck(ImGui::SliderAngle("Y-rotation", &tf.yRot, -180.0f, 180.0f));
			dcheck(ImGui::SliderAngle("Z-rotation", &tf.zRot, -180.0f, 180.0f));
			if (dirty) {
				pSelectedNode->SetAppliedTransform(
					dx::XMMatrixScaling(tf.scale, tf.scale, tf.scale) *
					dx::XMMatrixRotationX(tf.xRot) *
					dx::XMMatrixRotationY(tf.yRot) *
					dx::XMMatrixRotationZ(tf.zRot) *
					dx::XMMatrixTranslation(tf.x, tf.y, tf.z)
				);
			}

			TP probe;
			pSelectedNode->Accept(probe);
		}
	}

protected:
	bool PushNode(Node& node) override {
		const int selectedId = (pSelectedNode == nullptr) ? -1 : pSelectedNode->GetId();
		const auto node_flags =
			ImGuiTreeNodeFlags_OpenOnArrow
			| ((node.GetId() == selectedId) ? ImGuiTreeNodeFlags_Selected : 0)
			| (node.HasChildren() ? 0 : ImGuiTreeNodeFlags_Leaf);

		const auto expanded = ImGui::TreeNodeEx(
			(void*)(intptr_t)node.GetId(),
			node_flags, node.GetName().c_str()
		);

		struct Probe : public TechniqueProbe {
			virtual void OnSetTechnique() {
				if (pTech->GetName() == "Outline") {
					pTech->SetActiveState(highlighted);
				}
			}
			bool highlighted = false;
		} outlineProbe;


		if (ImGui::IsItemClicked()) {
			if (pSelectedNode != nullptr) {
				if (pSelectedNode != &node) {
					outlineProbe.highlighted = false;
				}
				pSelectedNode->Accept(outlineProbe);
			}
			outlineProbe.highlighted = true;
			node.Accept(outlineProbe);

			pSelectedNode = &node;
		}

		return expanded;
	}

	void PopNode(Node& node) override {
		ImGui::TreePop();
	}

private:
	Node* pSelectedNode = nullptr;

	struct TransformParameters {
		float xRot = 0.0f;
		float yRot = 0.0f;
		float zRot = 0.0f;
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
		float scale = 1.0f;
	};

	std::string name;
	std::unordered_map<int, TransformParameters> transformParams;

private:
	TransformParameters& ResolveTransform() noexcept {
		const auto id = pSelectedNode->GetId();
		auto i = transformParams.find(id);
		if (i == transformParams.end()) {
			return LoadTransform(id);
		}
		return i->second;
	}

	TransformParameters& LoadTransform(int id) noexcept {
		const auto& applied = pSelectedNode->GetAppliedTransform();
		const auto angles = ExtractEulerAngles(applied);
		const auto translation = ExtractTranslation(applied);
		TransformParameters tp;
		tp.zRot = angles.z;
		tp.xRot = angles.x;
		tp.yRot = angles.y;
		tp.x = translation.x;
		tp.y = translation.y;
		tp.z = translation.z;
		tp.scale = 1.0f;
		return transformParams.insert({ id,{ tp } }).first->second;
	}
};
