#pragma once
#include <imgui.h>
#include <string>
#include "../Extension/uuid/UuidGenerator.h"
//extension cpp
#include "../Extension/misc/cpp/imgui_stdlib.h"
#include "../Extension/ImGuiColorTextEdit/TextEditor.h"
namespace editor
{
	class UIComponent
	{
	public:
		virtual void OnFrame(ImGuiIO& io,
			const float& time) = 0;
		virtual void endFrame(ImGuiIO& io,
			const float& time) = 0;
		template<class T>
		T* as()
		{
			return dynamic_cast<T*>(this);
		}
	protected:
		std::string m_str_key{ "" };
		std::string m_str_type{ "" }; 
	};


	class TextArea : public UIComponent
	{
	public:
		TextArea() {};
		TextArea(const std::string& key,
			const std::string& label, 
			const std::string& type) :
			m_str_label_button(label)
		{
			m_str_key = key;
			m_str_type = type; 
			m_str_id_button = UuidGenerator::CreateUUID();
			m_str_id_popup = UuidGenerator::CreateUUID();
			m_str_id_multiline_input_text = UuidGenerator::CreateUUID();
		};
		void setText(const std::string& str)
		{
			m_str_content = str; 
		}
		~TextArea()
		{

		};
		//override
	public:
		void OnFrame(ImGuiIO& io,
			const float& time) override
		{
			if (ImGui::Button(m_str_label_button.c_str()))
			{
				m_bIsClickPopup = true;
			}
			m_bIsDoToolTip = ImGui::IsItemHovered() ? true : false;
		}
		void endFrame(ImGuiIO& io,
			const float& time) override
		{
			if (m_bIsClickPopup)
			{
				ImGui::OpenPopup(m_str_id_popup.c_str());
				m_bIsClickPopup = false;
			}
			if (ImGui::BeginPopup(m_str_id_popup.c_str(), ImGuiWindowFlags_NoTitleBar
			))
			{
				ImGui::InputTextMultiline(m_str_id_multiline_input_text.c_str(),
					&m_str_content,
					m_multiline_input_size);
				ImGui::EndPopup();
			}
			if (m_bIsDoToolTip)
				if (m_str_content.length() != 0) ImGui::SetTooltip(m_str_content.data());
		}
		std::string getStr()
		{
			return m_str_content; 
		}
	protected:
		std::string m_str_content{ "" };
		std::string m_str_id_popup{ "" };
		std::string m_str_id_button{ "" };
		std::string m_str_id_multiline_input_text{ "" };
		std::string m_str_label_button{ "" };
		bool m_bIsClickPopup{ false };
		ImVec2 m_multiline_input_size{ 500.0f, 200.0f };
		bool m_bIsDoToolTip{ false };
	};


	class CodeEditor : public UIComponent
	{
	public: 
		CodeEditor() {};
		CodeEditor(const std::string& key,
			const std::string& label,
			const std::string& type,
			bool isReadOnly = false) :
			m_str_label_button(label)
		{
			m_str_key = key;
			m_str_type = type;
			m_bIsReadOnly = isReadOnly;
			m_str_id_button = UuidGenerator::CreateUUID();
			m_str_id_popup = UuidGenerator::CreateUUID();
			//m_str_id_multiline_input_text = UuidGenerator::CreateUUID();
			auto lang = TextEditor::LanguageDefinition::CPlusPlus();
			editor.SetLanguageDefinition(lang);
			editor.SetReadOnly(isReadOnly); 
		};

		void setText(const std::string& str)
		{
			editor.SetText(str); 
		}

		~CodeEditor()
		{

		};

		//override 
	public:
		void OnFrame(ImGuiIO& io,
			const float& time) override
		{
			if (ImGui::Button(m_str_label_button.c_str()))
			{
				m_bIsClickPopup = true;
			}
		}
		void endFrame(ImGuiIO& io,
			const float& time) override
		{
			if (m_bIsClickPopup)
			{
				ImGui::OpenPopup(m_str_id_popup.c_str());
				m_bIsClickPopup = false;
			}
			if (ImGui::BeginPopup(m_str_id_popup.c_str(),ImGuiWindowFlags_MenuBar | 
				ImGuiWindowFlags_AlwaysAutoResize))
			{
				auto cpos = editor.GetCursorPosition();
				if (ImGui::BeginMenuBar())
				{
					if (ImGui::BeginMenu("File"))
					{
						if (ImGui::MenuItem("Save"))
						{

						}
						ImGui::EndMenu();
					}
					ImGui::EndMenuBar();
				}
				ImGui::Text("%6d/%-6d %6d lines  | %s | %s | %s | %s", cpos.mLine + 1, cpos.mColumn + 1, editor.GetTotalLines(),
					editor.IsOverwrite() ? "Ovr" : "Ins",
					editor.CanUndo() ? "*" : " ",
					editor.GetLanguageDefinition().mName.c_str(), "file.cpp");
				editor.Render("TextEditor",ImVec2({1000,500}),true);
				ImGui::EndPopup();
			}
		}
		std::string getStr()
		{
			editor.GetText(); 
		}


	protected: 
		TextEditor editor;
		std::string m_str_id_button{ "" }; 
		std::string m_str_id_popup{ "" }; 
		std::string m_str_label_button{ "" }; 
		std::string m_str_content{ "" };
		bool m_bIsClickPopup{ false };
		bool m_bIsReadOnly{ true }; 
	};


	class EditText : public UIComponent
	{
	public:
		EditText() {}
		EditText(const std::string& key,
			const std::string& label, 
			const std::string& hint,
			const std::string& type)
			: m_str_label(label),
			m_str_hint(hint)
		{
			m_str_type = type; 
			m_str_key = key;
		};

		void setText(const std::string& str)
		{
			m_str_content = str; 
		}

		~EditText() {};
	public:
		void OnFrame(ImGuiIO& io,
			const float& time) override
		{
			ImGui::PushItemWidth(350);
			ImGui::InputTextWithHint(m_str_label.c_str(),
				m_str_hint.c_str(),
				&m_str_content); 
			ImGui::PopItemWidth(); 
		};
		void endFrame(ImGuiIO& io,
			const float& time) override
		{
		};
		std::string getStr()
		{
			return m_str_content;
		}
	protected:
		std::string m_str_label{ "" };
		std::string m_str_content{ "" }; 
		std::string m_str_hint{ "" }; 
	};

	class InputInt : public UIComponent
	{
	public:
		InputInt() {};
		InputInt(const std::string& key,
			const std::string& label,
			const std::string& type) : m_str_label(label)
		{
			m_str_type = type;
			m_str_key = key;
		};
	public:
		void OnFrame(ImGuiIO& io,
			const float& time) override
		{
			ImGui::PushItemWidth(350);
			ImGui::InputInt(m_str_label.c_str(), &m_num); 
		};
		void endFrame(ImGuiIO& io,
			const float& time) override
		{
		};
		int getNum()
		{
			return m_num;
		}
		void setNum(const int& num)
		{
			m_num = num; 
		}
	protected: 
		std::string m_str_label{ "" }; 
		int m_num{ 0 }; 
	}; 

	class ImageView : public UIComponent
	{
	public:
		ImageView(const std::string& key) : m_key(key)
		{
		}
		~ImageView() {}; 
		void setTextureId(ImTextureID textureId,
			const int& textureWidth,
			const int& textureHeight)
		{
			m_TextureID = textureId;
			m_texture_width = textureWidth;
			m_texture_height = textureHeight;
		}; 
		void OnFrame(ImGuiIO& io,
			const float& time) override
		{
			if (m_TextureID)
			{
				ImGui::Image(m_TextureID, ImVec2(m_texture_width, m_texture_height));
			}
		}; 
		void endFrame(ImGuiIO& io,
			const float& time) override
		{
		};
		ImTextureID& getTextureId()
		{
			return m_TextureID; 
		}

	public: 

	protected:
		ImTextureID m_TextureID{ nullptr };
		std::string m_key{ "" }; 
		int m_texture_width{ 0 }; 
		int m_texture_height{ 0 }; 
	};
}
