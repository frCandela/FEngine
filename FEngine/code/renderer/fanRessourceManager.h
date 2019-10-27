#pragma once

namespace fan
{
	class Mesh;
	class Texture;
	class Device;
	class Buffer;

	//================================================================================================================================
	// Loads & references all the textures of the engine
	//================================================================================================================================
	class RessourceManager {
	public:

		RessourceManager(Device& _device);
		~RessourceManager();

		// Mesh management
		Mesh *		LoadMesh( const std::string _path);
		Mesh *		FindMesh( const std::string _path);
		void		SetDefaultMesh(const Mesh * _defaultMesh) { m_defaultMesh = _defaultMesh; }
		const std::vector< Mesh * >& GetMeshList() const { return m_meshList; }

		// Texture management
		Texture *							LoadTexture(const std::string _path);
		Texture *							FindTexture(const std::string _path);
		size_t 								GetNumTextures() const { return m_textures.size(); }
		const std::vector< Texture * > &	GetTextures() const { return m_textures; }
		std::vector< Texture * > &			GetTextures() { return m_textures; }

		bool IsModified() const { return m_modified; }
		void SetUnmodified() { m_modified = false; }

	private:
		Device & m_device;

		std::vector< Mesh * >	 m_meshList;
		const Mesh *			 m_defaultMesh = nullptr;
		std::vector< Texture * > m_textures;
		bool					 m_modified = false;

		void AddMesh(Mesh * _mesh);
	};
}
