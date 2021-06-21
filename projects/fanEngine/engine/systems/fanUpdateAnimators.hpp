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
                if( meshRenderer.mMesh == nullptr )
                {
                    continue;
                }

                if( animator.mAnimation != nullptr && meshRenderer.mMesh->mSkeleton.mNumBones == animator.mAnimation->mNumBones )
                {
                    Skeleton & skeleton  = meshRenderer.mMesh->mSkeleton;
                    Animation& animation = *animator.mAnimation;

                    {
                        struct BoneData
                        {
                            int     mIndex;
                            Matrix4 mParentAbsoluteTransform;
                        };

                        BoneData             root = { skeleton.mNumBones - 1, Matrix4::sIdentity };
                        std::stack<BoneData> stack;
                        stack.push( root );
                        while( !stack.empty() )
                        {
                            BoneData boneData = stack.top();
                            stack.pop();

                            Vector3    scale    = animation.SampleScale( boneData.mIndex, animator.mTime );
                            Vector3    position = animation.SamplePosition( boneData.mIndex, animator.mTime );
                            Quaternion rotation = animation.SampleRotation( boneData.mIndex, animator.mTime );
                            Matrix4 animatedRelativeTransform = Matrix4(Quaternion::sIdentity, position) * Matrix4( rotation, Vector3::sZero ) * Matrix4(Quaternion::sIdentity, Vector3::sZero, scale);
                            animatedRelativeTransform = Matrix4(rotation, position, scale );

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
                            if( animator.mTime > animation.mDuration )
                            {
                                animator.mTime = 0;
                            }
                        }
                    }
                }
                else
                {
                    Skeleton& skeleton = meshRenderer.mMesh->mSkeleton;
                    for( int i = 0; i < RenderGlobal::sMaxBones; i++ )
                    {
                        skeleton.mOffsetMatrix[i] = Matrix4::sIdentity;
                    }
                }
            }
        }
    };
}