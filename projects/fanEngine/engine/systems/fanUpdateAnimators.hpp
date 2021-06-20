#include "core/ecs/fanEcsSystem.hpp"
#include "engine/components/fanAnimator.hpp"
#include "engine/components/fanSkinnedMeshRenderer.hpp"
#include <stack>

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct SUpdateAnimators : EcsSystem
    {
        static EcsSignature GetSignature( const EcsWorld& _world )
        {
            return _world.GetSignature<Animator>() |
                   _world.GetSignature<SkinnedMeshRenderer>();
        }

        static void Run( EcsWorld&, const EcsView& _view, const Fixed _delta )
        {
            auto animatorIt     = _view.begin<Animator>();
            auto meshRendererIt = _view.begin<SkinnedMeshRenderer>();

            for( ; animatorIt != _view.end<Animator>(); ++animatorIt, ++meshRendererIt )
            {
                Animator           & animator     = *animatorIt;
                SkinnedMeshRenderer& meshRenderer = *meshRendererIt;
                if( animator.mAnimation != nullptr && meshRenderer.mMesh != nullptr && meshRenderer.mMesh->mSkeleton.mNumBones == animator.mAnimation->mNumBones )
                {
                    Skeleton & skeleton  = meshRenderer.mMesh->mSkeleton;
                    Animation& animation = *animator.mAnimation;

                    {
                        struct BoneData
                        {
                            int     mIndex;
                            Matrix4 mParentAbsoluteTransform;
                        };

                        const Matrix4& ttt = Matrix4( animation.SampleRotation( skeleton.mNumBones - 1, animator.mTime ),
                                                      animation.SamplePosition( skeleton.mNumBones - 1, animator.mTime ),
                                                      animation.SampleScale( skeleton.mNumBones - 1, animator.mTime ) );

                        BoneData             root = { 0, skeleton.mRootTransform * ttt };
                        std::stack<BoneData> stack;
                        stack.push( root );
                        while( !stack.empty() )
                        {
                            BoneData boneData = stack.top();
                            stack.pop();

                            const Matrix4 animatedRelativeTransform = Matrix4( animation.SampleRotation( boneData.mIndex, animator.mTime ),
                                                                               animation.SamplePosition( boneData.mIndex, animator.mTime ),
                                                                               animation.SampleScale( boneData.mIndex, animator.mTime ) );
                            const Matrix4 ibm                       = skeleton.mInverseBindMatrix[boneData.mIndex];
                            const Matrix4 animatedAbsoluteTransform = boneData.mParentAbsoluteTransform * animatedRelativeTransform;

                            skeleton.mOffsetMatrix[boneData.mIndex] = animatedAbsoluteTransform * ibm;

                            Bone& bone = skeleton.mBones[boneData.mIndex];
                            for( int i = 0; i < bone.mNumChilds; i++ )
                            {
                                stack.push( { bone.mChilds[i], animatedAbsoluteTransform } );
                            }
                        }

                        if( animator.mLoop )
                        {
                            animator.mTime += _delta;
                            if( animator.mTime >= animation.mDuration )
                            {
                                animator.mTime = 0;
                            }
                        }
                    }
                }
            }
        }
    };
}