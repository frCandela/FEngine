#include "core/ecs/fanEcsSystem.hpp"
#include "game/fanDR3Tags.hpp"
#include "engine/singletons/fanRenderWorld.hpp"
#include "engine/ui/fanUITransform.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    // Deselects all units
    //==================================================================================================================================================================================================
    struct SClearSelection : EcsSystem
    {
        static EcsSignature GetSignature( const EcsWorld& _world )
        {
            return _world.GetSignature<Unit>() | _world.GetSignature<TagSelected>();
        }

        static void Run( EcsWorld& _world, const EcsView& _view )
        {
            auto unitIt = _view.begin<Unit>();
            for( ; unitIt != _view.end<Unit>(); ++unitIt )
            {
                _world.RemoveTag<TagSelected>( unitIt.GetEntity() );
            }
        }
    };

    //==================================================================================================================================================================================================
    // Deselects all units
    //==================================================================================================================================================================================================
    struct SPlaceSelectionFrames : EcsSystem
    {
        static EcsSignature GetSignature( const EcsWorld& _world )
        {
            return _world.GetSignature<Transform>() | _world.GetSignature<MeshRenderer>() | _world.GetSignature<TagSelected>();
        }

        static void Run( EcsWorld& _world, const EcsView& _view, const Fixed _delta )
        {
            if( _delta == 0 ){ return; }

            //singletons
            Scene      & scene       = _world.GetSingleton<Scene>();
            Selection  & selection   = _world.GetSingleton<Selection>();
            RenderWorld& renderWorld = _world.GetSingleton<RenderWorld>();

            // instantiate missing selection frames
            const int numSelectedUnits = _view.Size();
            if( selection.mSelectionFrames.size() < _view.Size() )
            {
                int      numFramesToInstantiate = numSelectedUnits - (int)selection.mSelectionFrames.size();
                for( int i                      = 0; i < numFramesToInstantiate; i++ )
                {
                    SceneNode* frame = selection.mSelectionFramePrefab->Instantiate( scene.GetRootNode() );
                    selection.mSelectionFrames.push_back( frame->mHandle );
                }
            }

            // hide unused frames
            for( int i = numSelectedUnits; i < selection.mSelectionFrames.size(); i++ )
            {
                UITransform& uiTransform = _world.GetComponent<UITransform>( _world.GetEntity( selection.mSelectionFrames[i] ) );
                uiTransform.mPosition = { 100000, 100000 };
            }

            EcsEntity cameraEntity = _world.GetEntity( scene.mMainCameraHandle );
            Camera   & camera          = _world.GetComponent<Camera>( cameraEntity );
            Transform& cameraTransform = _world.GetComponent<Transform>( cameraEntity );

            int  frameIndex     = 0;
            auto transformIt    = _view.begin<Transform>();
            auto meshRendererIt = _view.begin<MeshRenderer>();
            for( ; transformIt != _view.end<Transform>(); ++transformIt, ++meshRendererIt )
            {
                Transform   & transform    = *transformIt;
                MeshRenderer& meshRenderer = *meshRendererIt;
                const Fixed   radius = meshRenderer.mMesh != nullptr ? meshRenderer.mMesh->mBoundingSphere.mRadius : 1;
                const Vector3 offset = meshRenderer.mMesh != nullptr ? meshRenderer.mMesh->mBoundingSphere.mCenter : Vector3::sZero;

                const glm::vec2 screenPos    = renderWorld.mTargetSize * camera.WorldPosToScreen( cameraTransform, transform.mPosition + offset );
                const glm::vec2 screenPosTop = renderWorld.mTargetSize * camera.WorldPosToScreen( cameraTransform, transform.mPosition + offset + radius * cameraTransform.Right() );
                const float     size         = screenPosTop.x - screenPos.x;

                UITransform& uiTransform = _world.GetComponent<UITransform>( _world.GetEntity( selection.mSelectionFrames[frameIndex] ) );
                uiTransform.mSize     = size * glm::vec2( 2, 2 ) + glm::vec2( 8, 8 );
                uiTransform.mPosition = screenPos - 0.5f * glm::vec2( uiTransform.mSize );

                frameIndex++;
            }
        }
    };
}