#include "ecs/fanEcsSystem.hpp"
#include "engine/ui/fanUITransform.hpp"
#include "engine/ui/fanUIRenderer.hpp"
#include "engine/ui/fanUIProgressBar.hpp"
#include "engine/fanEngineTags.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct SUpdateProgressBars : EcsSystem
    {
        static EcsSignature GetSignature( const EcsWorld& _world )
        {
            return _world.GetSignature<SceneNode, UIProgressBar, TagUIModified>();
        }

        static void Run( EcsWorld& _world, const EcsView& _view )
        {
            auto sceneNodeIt   = _view.begin<SceneNode>();
            auto progressBarIt = _view.begin<UIProgressBar>();
            for( ; sceneNodeIt != _view.end<SceneNode>(); ++sceneNodeIt, ++progressBarIt )
            {
                const SceneNode& sceneNode   = *sceneNodeIt;
                UIProgressBar  & progressBar = *progressBarIt;

                _world.RemoveTag<TagUIModified>( progressBarIt.GetEntity() );

                if( !sceneNode.mChilds.empty() )
                {
                    EcsEntity firstChild = _world.GetEntity( sceneNode.mChilds[0] );
                    UITransform* childTransform = _world.SafeGetComponent<UITransform>( firstChild );
                    if( childTransform != nullptr )
                    {
                        childTransform->mSize.x = int( progressBar.mProgress * progressBar.mMaxSize );
                    }
                }
            }
        }
    };
}