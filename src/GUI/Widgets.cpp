#include "Widgets.h"
#include "../ConstData.h"
#include "GUI.h"
#include "Shortcut.h"

#include <imgui_internal.h>

inline ImVec2 operator+(const ImVec2& a, const ImVec2& b)
{
	return {a.x + b.x, a.y + b.y};
}
inline ImVec2 operator-(const ImVec2& a, const ImVec2& b)
{
	return {a.x - b.x, a.y - b.y};
}

bool GUI::button(std::string name)
{
	bool result = false;
	if (GUI::shouldRender())
		result = ImGui::Button(name.c_str());

	if (!result)
		result = GUI::Shortcut::handleShortcut(name);

	return result;
}

bool GUI::checkbox(std::string name, bool* value)
{
	bool result = false;
	if (GUI::shouldRender())
		result = ImGui::Checkbox(name.c_str(), value);

	if (!result)
	{
		result = GUI::Shortcut::handleShortcut(name);

		if (result)
			*value = !*value;
	}

	return result;
}

bool GUI::hotkey(std::string name, int* keyPointer, const ImVec2& size_arg)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	ImGuiIO& io = g.IO;
	const ImGuiStyle& style = g.Style;

	const ImGuiID id = window->GetID(name.c_str());
	const ImVec2 label_size = ImGui::CalcTextSize(name.c_str(), NULL, true);
	ImVec2 size = ImGui::CalcItemSize(size_arg, ImGui::CalcItemWidth(), label_size.y + style.FramePadding.y * 2.0f);
	const ImRect frame_bb(window->DC.CursorPos + ImVec2(label_size.x + style.ItemInnerSpacing.x, 0.0f),
						  window->DC.CursorPos + size);
	const ImRect total_bb(window->DC.CursorPos, frame_bb.Max);

	ImGui::ItemSize(total_bb, style.FramePadding.y);
	if (!ImGui::ItemAdd(total_bb, id))
		return false;

	const bool focus_requested = ImGui::FocusableItemRegister(window, g.ActiveId == id);
	const bool focus_requested_by_code = focus_requested;
	const bool focus_requested_by_tab = focus_requested && !focus_requested_by_code;

	const bool hovered = ImGui::ItemHoverable(frame_bb, id);

	if (hovered)
	{
		ImGui::SetHoveredID(id);
		g.MouseCursor = ImGuiMouseCursor_TextInput;
	}

	const bool user_clicked = hovered && io.MouseClicked[0];

	if (focus_requested || user_clicked)
	{
		if (g.ActiveId != id)
		{
			*keyPointer = 0;
		}
		ImGui::SetActiveID(id, window);
		ImGui::FocusWindow(window);
	}
	else if (io.MouseClicked[0])
	{
		if (g.ActiveId == id)
			ImGui::ClearActiveID();
	}

	bool value_changed = false;
	int key = *keyPointer;

	if (g.ActiveId == id)
	{
		if (!value_changed)
		{
			for (auto i = 0x08; i <= 0xA5; i++)
			{
				if (ImGui::IsKeyDown(static_cast<ImGuiKey>(i)))
				{
					key = i;
					value_changed = true;
					ImGui::ClearActiveID();
				}
			}
		}

		if (ImGui::IsKeyPressedMap(static_cast<ImGuiKey>(ImGuiKey_Escape)))
		{
			*keyPointer = 0;
			ImGui::ClearActiveID();
		}
		else
		{
			*keyPointer = key;
		}
	}

	char buf_display[64] = "None";

	ImGui::RenderFrame(frame_bb.Min, frame_bb.Max, ImGui::GetColorU32(style.Colors[ImGuiCol_FrameBg]), true,
					   style.FrameRounding);

	if (*keyPointer != 0 && g.ActiveId != id)
	{
		strcpy_s(buf_display, KeyNames[*keyPointer]);
	}
	else if (g.ActiveId == id)
	{
		strcpy_s(buf_display, "<Press a key>");
	}

	const ImRect clip_rect(frame_bb.Min.x, frame_bb.Min.y, frame_bb.Min.x + size.x, frame_bb.Min.y + size.y);
	ImVec2 render_pos = frame_bb.Min + style.FramePadding;
	ImGui::RenderTextClipped(frame_bb.Min + style.FramePadding, frame_bb.Max - style.FramePadding, buf_display, NULL,
							 NULL, style.ButtonTextAlign, &clip_rect);

	if (label_size.x > 0)
		ImGui::RenderText(ImVec2(total_bb.Min.x, frame_bb.Min.y + style.FramePadding.y), name.c_str());

	return value_changed;
}

bool GUI::modalPopup(std::string name, const std::function<void()>& popupFunction, int flags)
{
	if (!GUI::isVisible || ImGui::BeginPopupModal(name.c_str(), NULL, flags) || GUI::shortcutLoop)
	{
		popupFunction();

		if (GUI::shouldRender())
		{
			if (ImGui::Button("Cancel"))
			{
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}

	return true;
}

void GUI::arrowButton(std::string popupName)
{
	if (!GUI::shouldRender())
		return;

	float ww = ImGui::GetWindowSize().x;

	ImGui::SameLine(ww - 39);
	if (ImGui::ArrowButton((popupName + "arrow").c_str(), 1))
		ImGui::OpenPopup(popupName.c_str());
}

bool GUI::combo(std::string name, int* value, const char* const items[], int itemsCount)
{
	if (!GUI::shouldRender())
		return false;

	ImGui::PushItemWidth(80);
	bool result = ImGui::Combo(name.c_str(), value, items, itemsCount);
	ImGui::PopItemWidth();
	return result;
}

bool GUI::inputInt(std::string name, int* value, int min, int max)
{
	bool result = false;
	if (GUI::shouldRender())
	{
		ImGui::PushItemWidth(50);
		result = ImGui::InputInt(name.c_str(), value);
		ImGui::PopItemWidth();
	}

	if (*value < min)
		*value = min;
	if (*value > max)
		*value = max;

	return result;
}

bool GUI::inputFloat(std::string name, float* value, float min, float max)
{
	bool result = false;
	if (GUI::shouldRender())
	{
		ImGui::PushItemWidth(50);
		result = ImGui::InputFloat(name.c_str(), value);
		ImGui::PopItemWidth();
	}

	if (*value < min)
		*value = min;
	if (*value > max)
		*value = max;

	return result;
}

bool GUI::dragInt(std::string name, int* value, int min, int max)
{
	bool result = false;
	if (GUI::shouldRender())
	{
		ImGui::PushItemWidth(50);
		result = ImGui::DragInt(name.c_str(), value);
		ImGui::PopItemWidth();
	}

	if (*value < min)
		*value = min;
	if (*value > max)
		*value = max;

	return result;
}

bool GUI::dragFloat(std::string name, float* value, float min, float max)
{
	bool result = false;
	if (GUI::shouldRender())
	{
		ImGui::PushItemWidth(50);
		result = ImGui::DragFloat(name.c_str(), value);
		ImGui::PopItemWidth();
	}

	if (*value < min)
		*value = min;
	if (*value > max)
		*value = max;

	return result;
}

bool GUI::colorEdit(std::string name, float* color, bool inputs, bool alpha)
{
	if (!GUI::shouldRender())
		return false;

	bool result = false;

	if (alpha)
		result = ImGui::ColorEdit4(name.c_str(), color, inputs ? 0 : ImGuiColorEditFlags_NoInputs);
	else
		result = ImGui::ColorEdit3(name.c_str(), color, inputs ? 0 : ImGuiColorEditFlags_NoInputs);

	return result;
}

void GUI::marker(std::string title, std::string description)
{
	if (!GUI::shouldRender())
		return;
	
	ImGui::TextDisabled(title.c_str());
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(description.c_str());
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}