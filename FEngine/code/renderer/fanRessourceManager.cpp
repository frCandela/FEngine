#include "fanGlobalIncludes.h"

#include "renderer/fanRessourceManager.h"
#include "renderer/fanRenderer.h"
#include "renderer/core/fanTexture.h"
#include "renderer/core/fanBuffer.h"
#include "renderer/fanMesh.h"
#include "core/fanSignal.h"
#include "scene/fanEntity.h"

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
		}
		for (std::pair<const uint32_t, Mesh *> meshPair : m_meshList) {
			delete meshPair.second;
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
		mesh->Load();
		AddMesh(mesh);
		return mesh;
	}

	//================================================================================================================================
	//================================================================================================================================
	Mesh * RessourceManager::FindMesh(const std::string _path) {
		const std::map<uint32_t, Mesh*>::iterator it = m_meshList.find(DSID(_path.c_str()));
		if (it == m_meshList.end()) {
			return nullptr;
		}
		else {
			return it->second;
		}
	}

	//================================================================================================================================
	// Return the mesh data associated with a mesh
	// Return a default cube if no mesh found
	//================================================================================================================================
	Mesh * RessourceManager::FindMesh(const Mesh * _mesh) {
		const std::map<uint32_t, Mesh*>::iterator it = m_meshList.find(_mesh->GetRessourceID());
		if (it != m_meshList.end()) {
			return it->second;
		}
		else {
			Debug::Get() << Debug::Severity::error << "Mesh not found : " << _mesh->GetPath() << Debug::Endl();
			return m_meshList.find(m_defaultMesh->GetRessourceID())->second;
		}
	}


	//================================================================================================================================
	//================================================================================================================================
	void  RessourceManager::AddMesh(Mesh * _mesh) {
		if (m_meshList.find(_mesh->GetRessourceID()) != m_meshList.end()) {
			Debug::Get() << Debug::Severity::warning << "Renderer::AddMesh : Mesh already registered: " << _mesh->GetPath() << Debug::Endl();
			return;
		}
		m_meshList.insert(std::pair<uint32_t, Mesh*>(_mesh->GetRessourceID(), _mesh ));
		_mesh->GenerateBuffers( m_device);
	}
}