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

                if( layout.mType == UILayout::Grid || layout.mType == UILayout::FilledGrid )
                {
                    glm::ivec2 gridSize = layout.mGap;

                    // resize elements to same size
                    if( layout.mType == UILayout::FilledGrid && transforms.size() == 9 )
                    {
                        gridSize = { 3, 3 };
                        const UITransform& transformTL = *transforms[0 * gridSize.x + 0];
                        UITransform& transformMM = *transforms[1 * gridSize.x + 1];
                        transformMM.mSize = parentTransform.mSize - 2 * transformTL.mSize;

                        UITransform& transformTR = *transforms[0 * gridSize.x + 2];
                        UITransform& transformBL = *transforms[2 * gridSize.x + 0];
                        UITransform& transformBR = *transforms[2 * gridSize.x + 2];
                        transformTR.mSize = transformTL.mSize;
                        transformBL.mSize = transformTL.mSize;
                        transformBR.mSize = transformTL.mSize;

                        UITransform& transformTM = *transforms[0 * gridSize.x + 1];
                        UITransform& transformBM = *transforms[2 * gridSize.x + 1];
                        transformTM.mSize = { transformMM.mSize.x, transformTL.mSize.y };
                        transformBM.mSize = { transformMM.mSize.x, transformTL.mSize.y };

                        UITransform& transformML = *transforms[1 * gridSize.x + 0];
                        UITransform& transformMR = *transforms[1 * gridSize.x + 2];
                        transformML.mSize = { transformTL.mSize.x, transformMM.mSize.y };
                        transformMR.mSize = { transformTL.mSize.x, transformMM.mSize.y };
                    }

                    // set horizontal positions
                    int      xOffset = 0;
                    for( int x       = 0; x < gridSize.x; ++x )
                    {
                        // find max width
                        int      maxWidth = 0;
                        for( int y        = 0; y < gridSize.y; ++y )
                        {
                            const int index = y * gridSize.x + x;
                            if( index >= (int)transforms.size() ){ continue; }
                            const UITransform& transform = *transforms[index];
                            maxWidth = std::max( maxWidth, transform.mSize.x );
                        }

                        // set horizontal positions
                        for( int y = 0; y < gridSize.y; ++y )
                        {
                            const int index = y * gridSize.x + x;
                            if( index >= (int)transforms.size() ){ continue; }
                            UITransform& transform = *transforms[index];
                            transform.mPosition.x = parentTransform.mPosition.x + xOffset + ( maxWidth - transform.mSize.x ) / 2;
                        }
                        xOffset += maxWidth;
                    }

                    // set vertical positions
                    int      yOffset = 0;
                    for( int y       = 0; y < gridSize.y; ++y )
                    {
                        // find max width
                        int      maxHeight = 0;
                        for( int x         = 0; x < gridSize.x; ++x )
                        {
                            const int index = y * gridSize.x + x;
                            if( index >= (int)transforms.size() ){ continue; }
                            const UITransform& transform = *transforms[index];
                            maxHeight = std::max( maxHeight, transform.mSize.y );
                        }

                        // set horizontal positions
                        for( int x = 0; x < gridSize.x; ++x )
                        {
                            const int index = y * gridSize.x + x;
                            if( index >= (int)transforms.size() ){ continue; }
                            UITransform& transform = *transforms[index];
                            transform.mPosition.y = parentTransform.mPosition.y + yOffset + ( maxHeight - transform.mSize.y ) / 2;
                        }
                        yOffset += maxHeight;
                    }
                    parentTransform.mSize = { xOffset, yOffset };
                }
                else
                {
                    // calculates gap size
                    const bool isAutoGap = ( layout.mGap.x < 0 );
                    glm::ivec2 gap;
                    if( isAutoGap ) // auto gap
                    {
                        gap = ( parentTransform.mSize - totalSize );
                        gap /= ( transforms.size() + 1 );
                        gap = glm::ivec2( std::max( gap.x, 0 ), std::max( gap.y, 0 ) );
                    }
                    else
                    {
                        gap = glm::ivec2( layout.mGap.x, layout.mGap.x );
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
        }
    };
}