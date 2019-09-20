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
		// ConsoleWindow::LogItem nested constructor
		//================================================================================================================================
		ConsoleWindow::LogItem::LogItem( const Debug::LogItem& _logItem ) {
			severity = _logItem.severity;
			logType = _logItem.type;
			logMessage = fan::Time::SecondsToString(_logItem.time).c_str() + std::string(" ") + _logItem.message;
			color = GetSeverityColor(_logItem.severity);
		}

		//================================================================================================================================
		//================================================================================================================================
		ConsoleWindow::ConsoleWindow() :
			m_maxSizeLogBuffers(64),
			m_firstLogIndex(0){			
			m_inputBuffer[0] = '\0';
			m_logBuffer.reserve(m_maxSizeLogBuffers);
			fan::Debug::Get().onNewLog.Connect(&ConsoleWindow::OnNewLog, this);
		}

		//================================================================================================================================
		//================================================================================================================================
		void ConsoleWindow::OnNewLog( Debug::LogItem _item ) { 
			m_scrollDown = true; 

			if( m_logBuffer.size() < m_maxSizeLogBuffers ) {
				m_logBuffer.push_back(LogItem(_item));
			}
			else {
				m_logBuffer[m_firstLogIndex] = LogItem(_item);
			}
			m_firstLogIndex = (m_firstLogIndex + 1 ) % m_maxSizeLogBuffers;
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
						for (int logIndex = m_firstLogIndex; logIndex < m_firstLogIndex + m_logBuffer.size(); logIndex++) {
							const LogItem & item = m_logBuffer[logIndex % m_maxSizeLogBuffers];
							ImGui::TextColored(item.color, item.logMessage.c_str() );	// Time			
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
					if (ImGui::Button(">>") || Keyboard::IsKeyPressed(GLFW_KEY_ENTER, true ) || Keyboard::IsKeyPressed(GLFW_KEY_KP_ENTER, true)) {
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