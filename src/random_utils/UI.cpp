#include "UI.hpp"
#include <imgui/imgui.h>

void ColorfulDragFloat3(const char* label, float* vals, float resetValue)
{
	ImGui::PushID(label);
	ImGui::Text(label);
	ImGui::SameLine();

	ImGui::PushItemWidth(ImGui::CalcItemWidth() / 3.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_Button,		  ImVec4(0.8f, 0.1f, 0.15f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.2f, 0.25f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.8f, 0.1f, 0.15f, 1.0f));

	ImVec2 buttonSize(22.0f, 22.0f);

	if (ImGui::Button("X", buttonSize))
	{
		vals[0] = resetValue;
	}

	ImGui::PopStyleColor(3);
	ImGui::SameLine();
	ImGui::DragFloat("##X", &vals[0], 0.1f, 0.0f, 0.0f, "%.2f");
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button,		  ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.2f, 0.7f, 0.2f, 1.0f));

	if (ImGui::Button("Y", buttonSize))
	{
		vals[1] = resetValue;
	}

	ImGui::PopStyleColor(3);
	ImGui::SameLine();
	ImGui::DragFloat("##Y", &vals[1], 0.1f, 0.0f, 0.0f, "%.2f");
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button,		  ImVec4(0.1f, 0.25f, 0.8f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.35f, 0.9f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.1f, 0.25f, 0.8f, 1.0f));

	if (ImGui::Button("Z", buttonSize))
	{
		vals[2] = resetValue;
	}

	ImGui::PopStyleColor(3);
	ImGui::SameLine();
	ImGui::DragFloat("##Z", &vals[2], 0.1f, 0.0f, 0.0f, "%.2f");

	ImGui::PopItemWidth();
	ImGui::PopStyleVar();
	ImGui::PopID();
}