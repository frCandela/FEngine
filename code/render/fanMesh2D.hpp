#pragma once

#include "core/resources/fanResource.hpp"
#include "core/shapes/fanConvexHull.hpp"
#include "render/fanVertex.hpp"
#include "render/fanMesh2DManager.hpp"
#include "render/core/fanBuffer.hpp"
#include "render/core/fanSwapChain.hpp"

namespace fan
{
	struct Device;

	//================================================================================================================================
	//================================================================================================================================
	class Mesh2D : public Resource
	{
	public:
		bool LoadFromVertices( const std::vector<UIVertex>&	_vertices );
		void CreateBuffers( Device & _device );
		void DestroyBuffers( Device & _device );

		std::vector<UIVertex> mVertices;
		std::string           mPath;
		Buffer                mVertexBuffer[ SwapChain::s_maxFramesInFlight ];
		uint32_t              mCurrentBuffer = 0;
        int                   mIndex = -1;
		bool                  mHostVisible = false;
        bool                  mBuffersOutdated  = false;
        bool                  mExternallyOwned = false; //if false, the manager is responsible for destruction
	};
}