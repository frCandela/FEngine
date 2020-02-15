#pragma once

#include "scene/fanScenePrecompiled.hpp"

#include "core/resources/fanResourcePtr.hpp"
#include "render/core/fanTexture.hpp"
#include "render/fanMesh.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	class TexturePtr : public ResourcePtr<Texture>
	{
	public:
		TexturePtr( Texture* _texture = nullptr ) : ResourcePtr<Texture>( _texture ) {}
		
		void Init( const std::string _path ) { m_path = _path; }
		const std::string& GetPath() const { return m_path; }
		
		ResourcePtr& operator=( Texture* _resource ) { SetResource( _resource ); return *this; }
	private:
		std::string m_path;
	};

	//================================================================================================================================
	//================================================================================================================================
	class MeshPtr : public ResourcePtr<Mesh>
	{
	public:
		MeshPtr( Mesh* _mesh = nullptr ) : ResourcePtr<Mesh>( _mesh ) {}
		
		void Init( const std::string _path ) { m_path = _path; }
		const std::string&	GetPath() const { return m_path;  }

		ResourcePtr& operator=( Mesh* _resource ) { SetResource( _resource ); return *this; }
	private:
		std::string m_path;
	};
}

//================================================================================================================================
// ImGui bindings
//================================================================================================================================
namespace ImGui
{
	bool FanTexturePtr( const char* _label, fan::TexturePtr& _ptr );
	bool FanMeshPtr( const char* _label, fan::MeshPtr& _ptr );
}
