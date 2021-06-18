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

        static void Run( EcsWorld&, const EcsView& _view, const Fixed )
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

                        BoneData             root = { 0, skeleton.mRootTransform };
                        std::stack<BoneData> stack;
                        stack.push( root );
                        while( !stack.empty() )
                        {
                            BoneData boneData = stack.top();
                            stack.pop();

                            const Matrix4& animatedRelativeTransform = Matrix4( animation.SampleRotation(boneData.mIndex, animator.mKeyframe), animation.SamplePosition(boneData.mIndex, animator.mKeyframe) );
                            const Matrix4 animatedAbsoluteTransform = boneData.mParentAbsoluteTransform * animatedRelativeTransform;
                            const Matrix4 ibm                       = skeleton.mInverseBindMatrix[boneData.mIndex];

                            skeleton.mOffsetMatrix[boneData.mIndex] = animatedAbsoluteTransform * ibm;

                            Bone& bone = skeleton.mBones[boneData.mIndex];
                            for( int i = 0; i < bone.mNumChilds; i++ )
                            {
                                stack.push( { bone.mChilds[i], animatedAbsoluteTransform } );
                            }
                        }
                    }
                }
            }
        }
    };
}