#include "Shortcut.h"
#include "GUI.h"

#include "../Common.h"
#include "../ConstData.h"
#include "Widgets.h"

int shortcutIndexKey = 0;

bool GUI::Shortcut::handleShortcut(std::string& name)
{
	if (GUI::isVisible)
	{
		bool openPopup = false;

		if (ImGui::BeginPopupContextItem(name.c_str(), ImGuiPopupFlags_MouseButtonRight))
		{
			if (ImGui::MenuItem(("Add shortcut##" + name).c_str()))
				openPopup = true;

			ImGui::EndPopup();
		}

		if (openPopup == true)
		{
			ImGui::OpenPopup(("Create shortcut for " + name).c_str());
			openPopup = false;
		}

		if (ImGui::BeginPopupModal(("Create shortcut for " + name).c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			GUI::hotkey("Shortcut Key", &shortcutIndexKey);

			if (ImGui::Button("Add"))
			{
				GUI::Shortcut s(shortcutIndexKey, name);
				GUI::shortcuts.push_back(s);
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
				ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
		}
	}

	if (GUI::currentShortcut == name)
	{
		GUI::currentShortcut = "";
		return true;
	}

	return false;
}

void GUI::Shortcut::renderWindow()
{
	if (GUI::shouldRender())
	{
		if (GUI::shortcuts.size() <= 0)
		{
			ImGui::Text("Right click an option\nto make a shortcut");
			ImGui::Spacing();
			return;
		}

		int uid = 0;
		for (GUI::Shortcut& s : GUI::shortcuts)
		{
			ImGui::AlignTextToFramePadding();
			ImGui::Text(KeyNames[s.key]);
			ImGui::SameLine();
			ImGui::Text(s.name.c_str());
			ImGui::SameLine(ImGui::GetWindowSize().x - 30);
			if (GUI::button(std::format("x##{}", std::to_string(uid))))
			{
				GUI::shortcuts.erase(GUI::shortcuts.begin() + uid);
				GUI::save();
			}

			ImGui::Separator();
			uid++;
		}
	}

	if (GUI::button("Toggle Practice") && Common::getBGL())
		reinterpret_cast<void(__thiscall*)(cocos2d::CCLayer*, bool)>(utils::gd_base + 0x2E4EB0)(
			Common::getBGL(), !MBO(bool, Common::getBGL(), 10868));

	if (GUI::button("Reset Level") && Common::getBGL())
		reinterpret_cast<void(__thiscall*)(cocos2d::CCLayer*)>(utils::gd_base + 0x2E42B0)(Common::getBGL());
}