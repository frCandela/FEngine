#include "fanGlobalIncludes.h"

#include "editor/windows/fanConsoleWindow.h"
#include "core/input/fanInput.h"
#include "core/input/fanKeyboard.h"
#include "core/input/fanMouse.h"
#include "core/fanTime.h"

namespace fan
{
	namespace editor {
		//================================================================================================================================
		//================================================================================================================================
		ConsoleWindow::ConsoleWindow() {
			m_inputBuffer[0] = '\0';
			fan::Debug::Get().onNewLog.Connect(&ConsoleWindow::OnNewLog, this);
		}

		//================================================================================================================================
		//================================================================================================================================
		void ConsoleWindow::Draw() {
			bool visible = IsVisible();
			if (visible == true) {

				ImGui::SetNextWindowSizeConstraints({ 120,60 }, { 10000,10000 });
				ImGui::Begin("Console", &visible); {
					// List the logs
					const float height = ImGui::GetWindowHeight();
					if (height > 60) {
						ImGui::BeginChild("scrolling", ImVec2(0, height - 60), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
						const std::vector<  fan::Debug::LogItem >& logBuffer = fan::Debug::Get().GetLogBuffer();
						for (int logIndex = 0; logIndex < logBuffer.size(); logIndex++) {
							const fan::Debug::LogItem & item = logBuffer[logIndex];
							const ImVec4 color = GetSeverityColor(item.severity);

							ImGui::TextColored(color, fan::Time::SecondsToString(item.time).c_str());	// Time
							ImGui::SameLine();
							ImGui::TextColored(color, item.message.c_str());			// Log				
						}
						if (m_scrollDown) {
							ImGui::SetScrollHere(1.0f);
							m_scrollDown = false;
						}
						ImGui::EndChild();
					}
					// Text input
					ImGui::PushItemWidth(ImGui::GetWindowWidth() - 60);
					if (ImGui::InputText("##input_console", m_inputBuffer, s_inputBufferSize)) {}
					ImGui::SameLine();
					if (ImGui::Button(">>") || Keyboard::IsKeyPressed(GLFW_KEY_ENTER) || Keyboard::IsKeyPressed(GLFW_KEY_KP_ENTER)) {
						const std::string message = m_inputBuffer;
						if (message.size() > 0) {
							if (message != "clear") {
								fan::Debug::Get().Log(std::string("Unknown command: ") + message, fan::Debug::Severity::log);
							}
							else {
								fan::Debug::Get().Clear();
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
		ImVec4 ConsoleWindow::GetSeverityColor(const fan::Debug::Severity & _severity) {
			switch (_severity) {
			case fan::Debug::Severity::log: {
				return { 1,1,1,1 };		// White
			} break;
			case fan::Debug::Severity::highlight: {
				return { 0,1,0,1 };		// Green
			} break;
			case fan::Debug::Severity::warning: {
				return { 1,1,0,1 };		// Yellow
			} break;
			case fan::Debug::Severity::error: {
				return { 1,0,0,1 };		// Red
			} break;
			default:
				assert(false);
				break;
			}
			return { 0,0,0,1 };		// Black
		}
	}
}