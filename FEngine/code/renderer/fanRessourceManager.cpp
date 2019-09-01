#include "fanGlobalIncludes.h"

#include "renderer/fanRessourceManager.h"
#include "fanGlobalValues.h"
#include "renderer/fanRenderer.h"
#include "renderer/core/fanTexture.h"
#include "renderer/core/fanBuffer.h"
#include "renderer/fanMesh.h"
#include "core/fanSignal.h"
#include "scene/fanGameobject.h"


namespace fan
{
	namespace vk {
		fan::Signal<> RessourceManager::onTextureLoaded;

		//================================================================================================================================
		//================================================================================================================================
		RessourceManager::RessourceManager(Device& _device) :
			m_device(_device) {
			m_textures.reserve(64);

			LoadTexture(GlobalValues::s_defaultTexturePath);
		}

		//================================================================================================================================
		//================================================================================================================================
		RessourceManager::~RessourceManager() {
			for (int textureIndex = 0; textureIndex < m_textures.size(); textureIndex++) {
				delete(m_textures[textureIndex]);
			}


			for (auto meshData : m_meshList) {
				delete meshData.second.indexBuffer;
				delete meshData.second.vertexBuffer;
			}
		}

		//================================================================================================================================
		//================================================================================================================================
		vk::Texture * RessourceManager::LoadTexture(const std::string _path) {
			// Dont add the texture if it already exists
			if (FindTexture(_path) != nullptr) {
				fan::Debug::Get() << fan::Debug::Severity::warning << "Texture already added: " << _path << std::endl;
				return nullptr;
			}

			// Add
			vk::Texture * texture = new Texture(m_device);
			if (texture->LoadTexture(_path) == true) {
				texture->SetRenderID(static_cast<int>(m_textures.size()));
				m_textures.push_back(texture);
				onTextureLoaded.Emmit();
			}

			return texture;
		}

		//================================================================================================================================
		//================================================================================================================================
		vk::Texture * RessourceManager::FindTexture(const std::string _path) {
			for (int textureIndex = 0; textureIndex < m_textures.size(); textureIndex++) {
				const vk::Texture * texture = m_textures[textureIndex];
				if (texture->GetPath() == _path) {
					return m_textures[textureIndex];
				}
			}
			return nullptr;
		}

		//================================================================================================================================
		//================================================================================================================================
		fan::Mesh * RessourceManager::LoadMesh(const std::string _path) {
			fan::Mesh * mesh = new fan::Mesh(_path);
			mesh->Load();
			AddMesh(mesh);
			return mesh;
		}

		//================================================================================================================================
		//================================================================================================================================
		fan::Mesh * RessourceManager::FindMesh(const std::string _path) {
			const std::map<uint32_t, MeshData>::iterator it = m_meshList.find(DSID(_path.c_str()));
			if (it == m_meshList.end()) {
				return nullptr;
			}
			else {
				return it->second.mesh;
			}
		}

		//================================================================================================================================
		// Return the mesh data associated with a mesh
		// Return a default cube if no mesh found
		//================================================================================================================================
		vk::MeshData * RessourceManager::FindMeshData(const fan::Mesh * _mesh) {
			const std::map<uint32_t, MeshData>::iterator it = m_meshList.find(_mesh->GetRessourceID());
			if (it != m_meshList.end()) {
				return &it->second;
			}
			else {
				fan::Debug::Get() << fan::Debug::Severity::error << "Mesh not found : " << _mesh->GetPath() << std::endl;
				return &m_meshList.find(m_defaultMesh->GetRessourceID())->second;
			}
		}


		//================================================================================================================================
		//================================================================================================================================
		void  RessourceManager::AddMesh(fan::Mesh * _mesh) {
			if (m_meshList.find(_mesh->GetRessourceID()) != m_meshList.end()) {
				fan::Debug::Get() << fan::Debug::Severity::warning << "Renderer::AddMesh : Mesh already registered: " << _mesh->GetPath() << std::endl;
				return;
			}

			const std::map<uint32_t, MeshData>::iterator it = m_meshList.insert(std::pair<uint32_t, MeshData>(_mesh->GetRessourceID(), {})).first;
			MeshData & meshData = it->second;

			meshData.mesh = _mesh;
			meshData.indexBuffer = new Buffer(m_device);
			meshData.vertexBuffer = new Buffer(m_device);

			{
				const std::vector<uint32_t> & indices = _mesh->GetIndices();
				const VkDeviceSize size = sizeof(indices[0]) * indices.size();
				meshData.indexBuffer->Create(
					size,
					VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
					VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				);
				Buffer stagingBuffer(m_device);
				stagingBuffer.Create(
					size,
					VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
				);
				stagingBuffer.SetData(indices.data(), size);
				VkCommandBuffer cmd = Renderer::GetRenderer().BeginSingleTimeCommands();
				stagingBuffer.CopyBufferTo(cmd, meshData.indexBuffer->GetBuffer(), size);
				Renderer::GetRenderer().EndSingleTimeCommands(cmd);
			}
			{
				const std::vector<vk::Vertex> & vertices = _mesh->GetVertices();
				const VkDeviceSize size = sizeof(vertices[0]) * vertices.size();
				meshData.vertexBuffer->Create(
					size,
					VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
					VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				);
				Buffer stagingBuffer2(m_device);
				stagingBuffer2.Create(
					size,
					VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
				);
				stagingBuffer2.SetData(vertices.data(), size);
				VkCommandBuffer cmd2 = Renderer::GetRenderer().BeginSingleTimeCommands();
				stagingBuffer2.CopyBufferTo(cmd2, meshData.vertexBuffer->GetBuffer(), size);
				Renderer::GetRenderer().EndSingleTimeCommands(cmd2);
			}
		}
	}
}