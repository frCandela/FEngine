#include "ecs/fanEcsSystem.hpp"
#include "ecs/fanEcsWorld.hpp"
#include "scene/components/ui/fanUITransform.hpp"
#include "scene/components/ui/fanUILayout.hpp"

namespace fan
{
    //========================================================================================================
    //========================================================================================================
    struct SUpdateUILayouts : EcsSystem
    {
        static EcsSignature GetSignature( const EcsWorld& _world )
        {
            return _world.GetSignature<UITransform>() |
                   _world.GetSignature<SceneNode>() |
                   _world.GetSignature<UILayout>();
        }

        static void Run( EcsWorld& _world, const EcsView& _view )
        {
            auto layoutIt = _view.begin<UILayout>();
            auto transformUIIt = _view.begin<UITransform>();
            auto sceneNodeIt = _view.begin<SceneNode>();

            std::vector<UITransform*> transforms;
            for( ; layoutIt != _view.end<UILayout>(); ++layoutIt, ++transformUIIt, ++sceneNodeIt )
            {

                UILayout   & layout        = *layoutIt;
                UITransform& parentTransform = *transformUIIt;
                SceneNode& sceneNode = *sceneNodeIt;

                // collects child transforms
                fanAssert( sceneNode.mParentHandle != 0 );
                glm::ivec2 totalSize(0,0);
                transforms.clear();
                for( EcsHandle child : sceneNode.mChilds )
                {
                    EcsEntity childEntity = _world.GetEntity( child );
                    UITransform* childTransform = _world.SafeGetComponent<UITransform>( childEntity );
                    if( childTransform != nullptr )
                    {
                        totalSize += childTransform->mSize;
                        transforms.push_back( childTransform );
                    }
                }
                if( transforms.empty() ){ continue; }

                // calculates gap size
                glm::ivec2 gap;
                if( layout.mGap < 0 ) // auto gap
                {
                    gap = ( parentTransform.mSize - totalSize );
                    gap /= ( transforms.size() + 1 );
                    gap = glm::ivec2( std::max( gap.x, 0 ), std::max( gap.y, 0 ) );
                }
                else
                {
                    gap = glm::ivec2( layout.mGap, layout.mGap );
                }

                glm::ivec2 externalGap = parentTransform.mSize - totalSize;
                externalGap -= ( int( transforms.size()) - 1 ) * gap;
                externalGap /= 2;

                // aligns all transforms in the layout
                glm::ivec2 position = parentTransform.mPosition;
                switch(  layout.mType )
                {
                    case UILayout::Horizontal:  position.x += externalGap.x;   break;
                    case UILayout::Vertical:    position.y += externalGap.y;   break;
                    default:                    fanAssert( false );  break;
                }

                for( UITransform * transform : transforms)
                {
                    transform->mPosition = position;
                    switch(  layout.mType )
                    {
                        case UILayout::Horizontal:
                            position.x += transform->mSize.x + gap.x;
                            transform->mPosition.y += (parentTransform.mSize.y - transform->mSize.y) / 2;
                            break;
                        case UILayout::Vertical:
                            position.y += transform->mSize.y + gap.y;
                            transform->mPosition.x += (parentTransform.mSize.x - transform->mSize.x) / 2;
                            break;
                        default: fanAssert( false ); break;
                    }
                }
            }
        }
    };
}