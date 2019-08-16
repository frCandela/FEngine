#include "fanIncludes.h"

#include "editor/windows/fanConsoleWindow.h"
#include "core/fanInput.h"

namespace editor {
	//================================================================================================================================
	//================================================================================================================================
	void ConsoleWindow::Draw() {
		bool visible = IsVisible();
		if (visible == true) {
			
			ImGui::SetNextWindowSizeConstraints({120,60}, {10000,10000});
			ImGui::Begin("Console", &visible); {
				const float height = ImGui::GetWindowHeight();
				if (height > 60) {
					ImGui::BeginChild("scrolling", ImVec2(0, height -60), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
					for (int logIndex = 0; logIndex < m_logBuffer.size(); logIndex++) {
						const LogItem & item = m_logBuffer[logIndex];

						if (item.severity == Severity::log) {
							ImGui::Text(item.message.c_str());
						} else {
							ImGui::TextColored(GetSeverityColor(item.severity),  item.message.c_str());
						}						
					}
					if (m_scrollDown) {
						ImGui::SetScrollHere(1.0f);
					}					
					ImGui::EndChild();
				}

				ImGui::PushItemWidth(ImGui::GetWindowWidth() - 60);
				if (ImGui::InputText("##input_console", m_inputBuffer, s_inputBufferSize)) {}
				ImGui::SameLine();
				if (ImGui::Button(">>") || Keyboard::IsKeyPressed(GLFW_KEY_ENTER) || Keyboard::IsKeyPressed(GLFW_KEY_KP_ENTER)) {
					const std::string message = m_inputBuffer;
					if (message.size() > 0) {
						if (message != "clear") {
							Log( std::string("Unknown command: ") + message, Severity::log);
						}
						else {
							Clear();
						}
						
						m_inputBuffer[0] = '\0';			
						m_scrollDown = true;
					}
				}
				
			} ImGui::End();
			SetVisible(visible);
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void ConsoleWindow::Log(const std::string _message, const Severity & _severity) {
		std::cout << _message << std::endl;
		m_logBuffer.push_back({});

		LogItem & item = m_logBuffer[m_logBuffer.size() - 1];
		item.message = _message;
		item.severity = _severity;		
	}

	//================================================================================================================================
	//================================================================================================================================
	void ConsoleWindow::Clear() {
		m_logBuffer.clear();
		Log("clear", Severity::highlight);
	}

	//================================================================================================================================
	//================================================================================================================================
	ImVec4 ConsoleWindow::GetSeverityColor(const Severity & _severity) {
		switch (_severity)
		{
		case Severity::log: {
			return { 1,1,1,1 };		// White
		} break;
		case Severity::highlight: {
			return { 0,1,0,1 };		// Green
		} break;
		case Severity::warning: {
			return { 1,1,0,1 };		// Yellow
		} break;
		case Severity::error: {
			return { 1,0,0,1 };		// Red
		} break;
		default:
			return { 0,0,0,1 };		// Black
			break;
		}
	}
}