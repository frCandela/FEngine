#include "fanGlobalIncludes.h"

#include "renderer/fanRessourceManager.h"
#include "renderer/fanRenderer.h"
#include "renderer/core/fanTexture.h"
#include "renderer/core/fanBuffer.h"
#include "renderer/fanMesh.h"
#include "core/fanSignal.h"
#include "scene/fanGameobject.h"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	RessourceManager::RessourceManager(Device& _device) :
		m_device(_device) {
		m_textures.reserve(64);

		LoadTexture(GlobalValues::s_defaultTexturePath);
		SetUnmodified();
	}

	//================================================================================================================================
	//================================================================================================================================
	RessourceManager::~RessourceManager() {
		for (int textureIndex = 0; textureIndex < m_textures.size(); textureIndex++) {
			delete(m_textures[textureIndex]);
		} m_textures.clear();

		for (int meshIndex = 0; meshIndex < m_meshList.size(); meshIndex++)	{
			delete m_meshList[meshIndex];
		} m_meshList.clear();
	}

	//================================================================================================================================
	//================================================================================================================================
	Texture * RessourceManager::LoadTexture(const std::string _path) {
		// Dont add the texture if it already exists
		if (FindTexture(_path) != nullptr) {
			Debug::Get() << Debug::Severity::warning << "Texture already added: " << _path << Debug::Endl();
			return nullptr;
		}

		// Add
		Texture * texture = new Texture(m_device);
		if (texture->LoadTexture(_path) == true) {
			texture->SetRenderID(static_cast<int>(m_textures.size()));
			m_textures.push_back(texture);
			m_modified = true;
		}

		return texture;
	}

	//================================================================================================================================
	//================================================================================================================================
	Texture * RessourceManager::FindTexture(const std::string _path) {
		for (int textureIndex = 0; textureIndex < m_textures.size(); textureIndex++) {
			const Texture * texture = m_textures[textureIndex];
			if (texture->GetPath() == _path) {
				return m_textures[textureIndex];
			}
		}
		return nullptr;
	}

	//================================================================================================================================
	//================================================================================================================================
	Mesh * RessourceManager::LoadMesh(const std::string _path) {
		Mesh * mesh = new Mesh(_path);
		if ( mesh->Load() ) {
			AddMesh( mesh );
			return mesh;
		}		
		delete mesh;
		return nullptr;
		
	}

	//================================================================================================================================
	//================================================================================================================================
	Mesh * RessourceManager::FindMesh(const std::string _path) {
		for (int meshIndex = 0; meshIndex < m_meshList.size() ; meshIndex++) {
			if ( m_meshList[meshIndex]->GetPath() == _path ) {
				return m_meshList[meshIndex];
			}
		}
		return nullptr;
	}

	//================================================================================================================================
	//================================================================================================================================
	void  RessourceManager::AddMesh(Mesh * _mesh) {
		m_meshList.push_back( _mesh );
		_mesh->GenerateBuffers( m_device);
	}
}