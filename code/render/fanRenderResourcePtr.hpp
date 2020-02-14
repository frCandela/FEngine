#pragma once

#include "scene/fanScenePrecompiled.hpp"
#include "core/resources/fanResourcePtr.hpp"

namespace fan
{
	class Texture;
	class Mesh;

	using TexturePtr = ResourcePtr<Texture>;

	//================================================================================================================================
	//================================================================================================================================
	class MeshPtr : public ResourcePtr<Mesh>
	{
	public:
		MeshPtr( Mesh* _mesh = nullptr );
		void Init( const std::string _path ){ m_path = _path; }
		const std::string&	GetPath() const { return m_path;  }

	private:
		std::string m_path;
	};
}

//================================================================================================================================
// ImGui bindings
//================================================================================================================================
namespace ImGui
{
	bool FanTexture( const char* _label, fan::TexturePtr* _ptr );
	bool FanMesh( const char* _label, fan::MeshPtr* _ptr );
}
