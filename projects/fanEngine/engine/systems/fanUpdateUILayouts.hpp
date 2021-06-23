#include "ecs/fanEcsSystem.hpp"
#include "ecs/fanEcsWorld.hpp"
#include "engine/ui/fanUITransform.hpp"
#include "engine/ui/fanUILayout.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
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
            auto layoutIt      = _view.begin<UILayout>();
            auto transformUIIt = _view.begin<UITransform>();
            auto sceneNodeIt   = _view.begin<SceneNode>();

            std::vector<UITransform*> transforms;
            for( ; layoutIt != _view.end<UILayout>(); ++layoutIt, ++transformUIIt, ++sceneNodeIt )
            {

                UILayout   & layout          = *layoutIt;
                UITransform& parentTransform = *transformUIIt;
                SceneNode  & sceneNode       = *sceneNodeIt;

                // collects child transforms
                fanAssert( sceneNode.mParentHandle != 0 );
                glm::ivec2 totalSize( 0, 0 );
                glm::ivec2 maxSize( 0, 0 );
                transforms.clear();
                for( EcsHandle child : sceneNode.mChilds )
                {
                    EcsEntity childEntity = _world.GetEntity( child );
                    UITransform* childTransform = _world.SafeGetComponent<UITransform>( childEntity );
                    if( childTransform != nullptr )
                    {
                        totalSize += childTransform->mSize;
                        maxSize.x = std::max( childTransform->mSize.x, maxSize.x );
                        maxSize.y = std::max( childTransform->mSize.y, maxSize.y );
                        transforms.push_back( childTransform );
                    }
                }
                if( transforms.empty() ){ continue; }

                // calculates gap size
                const bool isAutoGap = ( layout.mGap < 0 );
                glm::ivec2 gap;
                if( isAutoGap ) // auto gap
                {
                    gap = ( parentTransform.mSize - totalSize );
                    gap /= ( transforms.size() + 1 );
                    gap = glm::ivec2( std::max( gap.x, 0 ), std::max( gap.y, 0 ) );
                }
                else
                {
                    gap = glm::ivec2( layout.mGap, layout.mGap );
                }

                const glm::ivec2 totalInternalGapSize = ( int( transforms.size() ) - 1 ) * gap;
                const glm::ivec2 externalGapSize      = ( ( parentTransform.mSize - totalSize ) -
                                                          totalInternalGapSize ) / 2;

                // calculate the position of the first child in the layout
                glm::ivec2 origin = parentTransform.mPosition;
                switch( layout.mType )
                {
                    case UILayout::Horizontal:
                        origin.x += externalGapSize.x;
                        break;
                    case UILayout::Vertical:
                        origin.y += externalGapSize.y;
                        break;
                    default:
                        fanAssert( false );
                        break;
                }

                // aligns all transforms on the other axis
                for( UITransform* transform : transforms )
                {
                    transform->mPosition = origin;
                    switch( layout.mType )
                    {
                        case UILayout::Horizontal:
                            origin.x += transform->mSize.x + gap.x;
                            transform->mPosition.y += ( parentTransform.mSize.y - transform->mSize.y ) / 2;
                            break;
                        case UILayout::Vertical:
                            origin.y += transform->mSize.y + gap.y;
                            transform->mPosition.x += ( parentTransform.mSize.x - transform->mSize.x ) / 2;
                            break;
                        default:
                            fanAssert( false );
                            break;
                    }
                }

                if( !isAutoGap )
                {
                    // calculates the origin & bounds of the layout
                    const glm::ivec2 maxExternalGap = ( parentTransform.mSize - maxSize ) / 2;
                    glm::ivec2       layoutPosition;
                    glm::ivec2       layoutSize;
                    switch( layout.mType )
                    {
                        case UILayout::Horizontal:
                            layoutPosition = parentTransform.mPosition + glm::ivec2( externalGapSize.x, maxExternalGap.y );
                            layoutSize     = glm::ivec2( totalSize.x + totalInternalGapSize.x, maxSize.y );
                            break;
                        case UILayout::Vertical:
                            layoutPosition = parentTransform.mPosition + glm::ivec2( maxExternalGap.x, externalGapSize.y );
                            layoutSize     = glm::ivec2( maxSize.x, totalSize.y + totalInternalGapSize.y );
                            break;
                        default:
                            layoutPosition = { 0, 0 };
                            layoutSize     = { 0, 0 };
                            fanAssert( false );
                            break;
                    }

                    // set the size & position of the parent to fit the layout bounds
                    parentTransform.mPosition = layoutPosition;
                    parentTransform.mSize     = layoutSize;
                }
            }
        }
    };
}