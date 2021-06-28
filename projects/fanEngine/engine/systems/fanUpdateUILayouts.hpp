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
                transforms.clear();
                for( EcsHandle child : sceneNode.mChilds )
                {
                    UITransform* childTransform = _world.SafeGetComponent<UITransform>( _world.GetEntity( child ) );
                    if( childTransform != nullptr )
                    {
                        transforms.push_back( childTransform );
                    }
                }

                if( transforms.empty() ){ continue; }

                if( layout.mType == UILayout::Grid )
                {
                    glm::ivec2 gridSize = layout.mGap;

                    // resize elements to same size
                    if( layout.mFill && transforms.size() == 9 )
                    {
                        gridSize = { 3, 3 };
                        RescaleGridToFillParent( gridSize, parentTransform, transforms );
                    }

                    const int totalWidth  = SetHorizontalPositionsForGrid( gridSize, parentTransform, transforms );
                    const int totalHeight = SetVerticalPositionsForGrid( gridSize, parentTransform, transforms );

                    parentTransform.mSize = { totalWidth, totalHeight };
                }
                else
                {
                    const glm::ivec2 maxSize   = CalculateMaxSize( transforms );
                    const glm::ivec2 totalSize = CalculateTotalSize( transforms );
                    if( layout.mFill && transforms.size() == 3 )
                    {
                        RescaleLayoutToFillParent( layout.mType, parentTransform, transforms );
                        switch( layout.mType ) // rescale the other axis of the parent to fit the childs
                        {
                            case UILayout::Horizontal:
                                parentTransform.mSize.y = maxSize.y;
                                break;
                            case UILayout::Vertical:
                                parentTransform.mSize.x = maxSize.x;
                                break;
                            default:
                                fanAssert( false );
                                break;
                        }
                    }

                    // calculates gap size
                    const bool isAutoGap = ( layout.mGap.x < 0 );
                    glm::ivec2 gap;
                    if( isAutoGap || layout.mFill )
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
                    const glm::ivec2 externalGapSize      = ( ( parentTransform.mSize - totalSize ) - totalInternalGapSize ) / 2;

                    // Aligns all transforms on both axis
                    glm::ivec2 origin = parentTransform.mPosition;
                    switch( layout.mType )
                    {
                        case UILayout::Horizontal:
                            origin.x += externalGapSize.x; // calculate the position of the first child in the layout
                            for( UITransform* transform : transforms )
                            {
                                transform->mPosition = origin;
                                origin.x += transform->mSize.x + gap.x;
                                transform->mPosition.y += ( parentTransform.mSize.y - transform->mSize.y ) / 2;
                            }
                            break;
                        case UILayout::Vertical:
                            origin.y += externalGapSize.y; // calculate the position of the first child in the layout
                            for( UITransform* transform : transforms ) // aligns all transforms on both axis
                            {
                                transform->mPosition = origin;
                                origin.y += transform->mSize.y + gap.y;
                                transform->mPosition.x += ( parentTransform.mSize.x - transform->mSize.x ) / 2;
                            }
                            break;
                        default:
                            fanAssert( false );
                            break;
                    }

                    // set the size of the parent to fit the layout size
                    if( !isAutoGap && !layout.mFill )
                    {
                        const glm::ivec2 maxExternalGap = ( parentTransform.mSize - maxSize ) / 2;
                        switch( layout.mType )
                        {
                            case UILayout::Horizontal:
                                parentTransform.mSize = glm::ivec2( totalSize.x + totalInternalGapSize.x, maxSize.y );
                                break;
                            case UILayout::Vertical:
                                parentTransform.mSize = glm::ivec2( maxSize.x, totalSize.y + totalInternalGapSize.y );
                                break;
                            default:
                                fanAssert( false );
                                break;
                        }
                    }
                }
            }
        }

    private:
        //==============================================================================================================================================================================================
        //==============================================================================================================================================================================================
        static void RescaleGridToFillParent( const glm::ivec2& _gridSize, const UITransform& _parentTransform, std::vector<UITransform*>& _transforms )
        {
            const UITransform& transformTL = *_transforms[0 * _gridSize.x + 0];
            UITransform      & transformMM = *_transforms[1 * _gridSize.x + 1];
            transformMM.mSize = _parentTransform.mSize - 2 * transformTL.mSize;

            UITransform& transformTR = *_transforms[0 * _gridSize.x + 2];
            UITransform& transformBL = *_transforms[2 * _gridSize.x + 0];
            UITransform& transformBR = *_transforms[2 * _gridSize.x + 2];
            transformTR.mSize = transformTL.mSize;
            transformBL.mSize = transformTL.mSize;
            transformBR.mSize = transformTL.mSize;

            UITransform& transformTM = *_transforms[0 * _gridSize.x + 1];
            UITransform& transformBM = *_transforms[2 * _gridSize.x + 1];
            transformTM.mSize = { transformMM.mSize.x, transformTL.mSize.y };
            transformBM.mSize = { transformMM.mSize.x, transformTL.mSize.y };

            UITransform& transformML = *_transforms[1 * _gridSize.x + 0];
            UITransform& transformMR = *_transforms[1 * _gridSize.x + 2];
            transformML.mSize = { transformTL.mSize.x, transformMM.mSize.y };
            transformMR.mSize = { transformTL.mSize.x, transformMM.mSize.y };
        }

        //==============================================================================================================================================================================================
        //==============================================================================================================================================================================================
        static int SetHorizontalPositionsForGrid( const glm::ivec2& _gridSize, const UITransform& _parentTransform, std::vector<UITransform*>& _transforms )
        {
            // set horizontal positions
            int      xOffset = 0;
            for( int x       = 0; x < _gridSize.x; ++x )
            {
                // find max width
                int      maxWidth = 0;
                for( int y        = 0; y < _gridSize.y; ++y )
                {
                    const int index = y * _gridSize.x + x;
                    if( index >= (int)_transforms.size() ){ continue; }
                    const UITransform& transform = *_transforms[index];
                    maxWidth = std::max( maxWidth, transform.mSize.x );
                }

                // set horizontal positions
                for( int y = 0; y < _gridSize.y; ++y )
                {
                    const int index = y * _gridSize.x + x;
                    if( index >= (int)_transforms.size() ){ continue; }
                    UITransform& transform = *_transforms[index];
                    transform.mPosition.x = _parentTransform.mPosition.x + xOffset + ( maxWidth - transform.mSize.x ) / 2;
                }
                xOffset += maxWidth;
            }
            return xOffset;
        }

        //==============================================================================================================================================================================================
        //==============================================================================================================================================================================================
        static int SetVerticalPositionsForGrid( const glm::ivec2& _gridSize, const UITransform& _parentTransform, std::vector<UITransform*>& _transforms )
        {
            // set vertical positions
            int      yOffset = 0;
            for( int y       = 0; y < _gridSize.y; ++y )
            {
                // find max width
                int      maxHeight = 0;
                for( int x         = 0; x < _gridSize.x; ++x )
                {
                    const int index = y * _gridSize.x + x;
                    if( index >= (int)_transforms.size() ){ continue; }
                    const UITransform& transform = *_transforms[index];
                    maxHeight = std::max( maxHeight, transform.mSize.y );
                }

                // set vertical positions
                for( int x = 0; x < _gridSize.x; ++x )
                {
                    const int index = y * _gridSize.x + x;
                    if( index >= (int)_transforms.size() ){ continue; }
                    UITransform& transform = *_transforms[index];
                    transform.mPosition.y = _parentTransform.mPosition.y + yOffset + ( maxHeight - transform.mSize.y ) / 2;
                }
                yOffset += maxHeight;
            }
            return yOffset;
        }

        //==================================================================================================================================================================================================
        //==================================================================================================================================================================================================
        static void RescaleLayoutToFillParent( const UILayout::Type _type, const UITransform& _parentTransform, std::vector<UITransform*>& _transforms )
        {
            _transforms[_transforms.size() - 1]->mSize = _transforms[0]->mSize;
            const glm::ivec2 sizeToFill                = _parentTransform.mSize - 2 * _transforms[0]->mSize;
            switch( _type )
            {
                case UILayout::Horizontal:
                    _transforms[1]->mSize.x = sizeToFill.x;
                    break;
                case UILayout::Vertical:
                    _transforms[1]->mSize.y = sizeToFill.y;
                    break;
                default:
                    fanAssert( false );
                    break;
            }
        }

        //==================================================================================================================================================================================================
        //==================================================================================================================================================================================================
        static glm::ivec2 CalculateMaxSize( const std::vector<UITransform*>& _transforms )
        {
            glm::ivec2 maxSize = _transforms[0]->mSize;
            for( int   i       = 1; i < (int)_transforms.size(); ++i )
            {
                maxSize = glm::max( _transforms[i]->mSize, maxSize );
            }
            return maxSize;
        }

        //==================================================================================================================================================================================================
        //==================================================================================================================================================================================================
        static glm::ivec2 CalculateTotalSize( const std::vector<UITransform*>& _transforms )
        {
            glm::ivec2 totalSize = _transforms[0]->mSize;
            for( int   i         = 1; i < (int)_transforms.size(); ++i )
            {
                totalSize += _transforms[i]->mSize;
            }
            return totalSize;
        }
    };
}