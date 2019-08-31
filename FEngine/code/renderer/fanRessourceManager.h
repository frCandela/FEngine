#pragma once

namespace ressource {
	class Mesh;
}

namespace vk {
	class Texture;
	class Device;
	class Buffer;

	struct MeshData {
		ressource::Mesh * mesh;
		vk::Buffer * indexBuffer;
		vk::Buffer * vertexBuffer;
	};

	//================================================================================================================================
	// Loads & references all the textures of the engine
	//================================================================================================================================
	class RessourceManager {
	public:
		static util::Signal<> onTextureLoaded;

		RessourceManager( Device& _device );
		~RessourceManager();

		// Mesh management
		ressource::Mesh *   LoadMesh(const std::string _path);
		ressource::Mesh *	FindMesh(const std::string _path);
		vk::MeshData *		FindMeshData( const ressource::Mesh * _mesh );
		void				SetDefaultMesh(const ressource::Mesh * _defaultMesh)	{ m_defaultMesh = _defaultMesh; }
		const std::map< uint32_t, MeshData > GetMeshData() const					{ return m_meshList;  }

		// Texture management
		vk::Texture *							LoadTexture( const std::string _path );
		vk::Texture *							FindTexture( const std::string _path );
		const std::vector< vk::Texture * > &	GetTextures() const	{ return m_textures; }
		std::vector< vk::Texture * > &			GetTextures()		{ return m_textures; }
		
		static const char * s_defaultTexture;

	private:
		vk::Device & m_device;

		std::map< uint32_t, MeshData > m_meshList;
		const ressource::Mesh * m_defaultMesh = nullptr;

		std::vector< vk::Texture * > m_textures;

		void AddMesh(ressource::Mesh * _mesh);
	};
}