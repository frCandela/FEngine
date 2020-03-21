#include "scene/fanComponentPtr.hpp"

#include "scene/fanSceneResourcePtr.hpp"
#include "scene/singletonComponents/fanSceneInstantiate.hpp"
#include "scene/singletonComponents/fanScene.hpp"
#include "game/fanGameSerializable.hpp"
#include "editor/fanDragnDrop.hpp"

namespace fan {
// 	//================================================================================================================================
// 	//================================================================================================================================
// 	ComponentPtrBase::~ComponentPtrBase()
// 	{
// 		(*this) = nullptr;
// 	}
// 
// 	//================================================================================================================================
// 	//================================================================================================================================
// 	void ComponentPtrBase::Init( Scene& _scene, uint64_t _gameobjectId, uint32_t _componentId )
// 	{
// 		m_gameobjectId = _gameobjectId;
// 		m_componentId = _componentId;
// 		m_scene = &_scene;
// 		if (m_gameobjectId != 0)
// 		{
// 			_scene.GetInstanciator().RegisterUnresolvedComponentPtr( *this );
// 		}
// 	}
// 
// 	//================================================================================================================================
// 	//================================================================================================================================
// 	ComponentPtrBase& ComponentPtrBase::operator=( Component* _resource )
// 	{
// 		if (_resource == GetResource()) { return  *this; }
// 
// 		if (GetResource() != nullptr)
// 		{
// 			assert( m_scene != nullptr );
// 			m_scene->GetInstanciator().UnRegisterComponentPtr( *this );
// 		}
// 
// 		if (_resource != nullptr)
// 		{
// 			m_scene = &_resource->GetScene();
// 			m_scene->GetInstanciator().RegisterComponentPtr( *this );
// 		}
// 		else
// 		{
// 			m_scene = nullptr;
// 		}
// 
// 		SetResource( _resource );
// 
// 		return *this;
// 	}
}

namespace ImGui
{
// 	//================================================================================================================================
// 	//================================================================================================================================
// 	bool FanComponent( const char* _label, const uint32_t _typeID, fan::ComponentPtrBase& _ptr )
// 	{
// 		bool returnValue = false;
// 
// 		fan::Component* component = *_ptr;
// 		const std::string name = component != nullptr ? ((std::string( component->GetName() ) + ": ") + component->GetGameobject().GetName()) : "null";
// 		const fan::Component* componentSample = fan::TypeInfo::Get().GetInstance<fan::Component>( _typeID );
// 
// 		// icon & set from selection
// 		if (ImGui::ButtonIcon( componentSample->GetIcon(), { 16,16 } ))
// 		{
// 			returnValue = true;
// 		}
// 
// 		ImGui::SameLine();
// 
// 		// name button 
// 		float width = 0.6f * ImGui::GetWindowWidth() - ImGui::GetCursorPosX() + 8;
// 		ImGui::Button( name.c_str(), ImVec2( width, 0.f ) ); ImGui::SameLine();
// 		ImGui::SameLine();
// 
// 		// dragndrop
// 		ImGui::FanBeginDragDropSourceComponent( component );
// 		fan::Component* componentDrop = ImGui::FanBeginDragDropTargetComponent( _typeID );
// 		if (componentDrop)
// 		{
// 			_ptr = componentDrop;
// 			returnValue = true;
// 		}
// 
// 		// Right click = clear
// 		if (ImGui::IsItemClicked( 1 ))
// 		{
// 			_ptr = nullptr;
// 			returnValue = true;
// 		}
// 
// 		// label	
// 		ImGui::Text( _label );
// 
// 		return returnValue;
// 	}
}