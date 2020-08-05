#include "scene/singletons/fanInputMouse.hpp"
#include "glfw/glfw3.h"
#include "core/input/fanInput.hpp"
#include "editor/fanModals.hpp"

namespace fan
{
    //========================================================================================================
    //========================================================================================================
    void InputMouse::SetInfo( EcsSingletonInfo& _info )
    {
        _info.icon  = ImGui::IconType::INPUT16;
        _info.group = EngineGroups::Scene;
        _info.name  = "input mouse";
        _info.onGui = &InputMouse::OnGui;
    }

    //========================================================================================================
    //========================================================================================================
    void InputMouse::Init( EcsWorld& /*_world*/, EcsSingleton& _singleton )
    {
        InputMouse& inputMouse = static_cast<InputMouse&>( _singleton );
        (void) inputMouse;
    }

    //========================================================================================================
    //========================================================================================================
    void InputMouse::Update(  InputMouse& _mouse, const glm::ivec2 _sceenPos )
    {
        double x, y;
        glfwGetCursorPos( Input::Get().Window(), &x, &y );
        _mouse.mPosition = glm::ivec2( (int)x, (int)y ) - _sceenPos;
    }

    //========================================================================================================
    //========================================================================================================
    void InputMouse::OnGui( EcsWorld& /*_world*/, EcsSingleton& _singleton )
    {
        ImGui::Indent();
        ImGui::Indent();
        {
            InputMouse& inputMouse = static_cast<InputMouse&>( _singleton );
            ImGui::PushReadOnly();
            ImGui::InputInt2( "position", &inputMouse.mPosition.x );
            ImGui::PopReadOnly();
        }
        ImGui::Unindent();
        ImGui::Unindent();
    }
}
