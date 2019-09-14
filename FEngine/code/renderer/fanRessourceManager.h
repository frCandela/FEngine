#pragma once

namespace fan
{
	class Mesh;

	namespace vk {
		class Texture;
		class Device;
		class Buffer;

		struct MeshData {
			Mesh *		 mesh;
			vk::Buffer * indexBuffer;
			vk::Buffer * vertexBuffer;
		};

		//================================================================================================================================
		// Loads & references all the textures of the engine
		//================================================================================================================================
		class RessourceManager {
		public:

			RessourceManager(vk::Device& _device);
			~RessourceManager();

			// Mesh management
			fan::Mesh *		LoadMesh(const std::string _path);
			fan::Mesh *		FindMesh(const std::string _path);
			vk::MeshData *	FindMeshData(const fan::Mesh * _mesh);
			void			SetDefaultMesh(const fan::Mesh * _defaultMesh) { m_defaultMesh = _defaultMesh; }
			const std::map< uint32_t, MeshData > GetMeshData() const { return m_meshList; }

			// Texture management
			vk::Texture *							LoadTexture(const std::string _path);
			vk::Texture *							FindTexture(const std::string _path);
			size_t 									GetNumTextures() const	{ return m_textures.size(); }
			const std::vector< vk::Texture * > &	GetTextures() const		{ return m_textures;		}
			std::vector< vk::Texture * > &			GetTextures()			{ return m_textures;		}

			bool IsModified() const { return m_modified;	}
			void SetUnmodified()	{ m_modified = false;	}

		private:
			vk::Device & m_device;

			std::map< uint32_t, MeshData >	m_meshList;
			const fan::Mesh *				m_defaultMesh = nullptr;
			std::vector< vk::Texture * >	m_textures;
			bool							m_modified = false;

			void AddMesh(fan::Mesh * _mesh);
		};
	}
}