#include "core/ecs/fanEcsSystem.hpp"

namespace fan
{
    //========================================================================================================
    // Draw the bounds of all scene nodes
    //========================================================================================================
    struct STestSystem : EcsSystem
    {
        static EcsSignature GetSignature( const EcsWorld& _world )
        {
            return _world.GetSignature<FxRigidbody>() | _world.GetSignature<FxTransform>();
        }

        static void Run( EcsWorld& _world, const EcsView& _view, Fixed _delta )
        {
            static std::vector<Contact> tmpTest;
            if( _delta == 0)
            {
                for( Contact c : tmpTest )
                {
                    RenderDebug& rd = _world.GetSingleton<RenderDebug>();
                    rd.DebugPoint(ToBullet(Math::ToGLM(c.position)), Color::sRed );
                    rd.DebugLine( ToBullet(Math::ToGLM(c.position)), ToBullet(Math::ToGLM(c.position+ c.normal)), Color::sYellow, false );
                }
                return;
            }

            FxPhysicsWorld physicsWorld = _world.GetSingleton<FxPhysicsWorld>();
            std::set<Contact> mContacts;

            auto rbIt0        = _view.begin<FxRigidbody>();
            auto transformIt0 = _view.begin<FxTransform>();
            for( ; rbIt0 != _view.end<FxRigidbody>(); ++rbIt0, ++transformIt0 )
            {
                FxRigidbody& rb0        = *rbIt0;
                FxTransform& transform0 = *transformIt0;
                // contact with the ground
                if( transform0.mPosition.y - FIXED( 0.5 ) < 0 )
                {
                    Contact contact;
                    contact.rb0         = &rb0;
                    contact.transform0  = &transform0;
                    contact.normal      = Vector3::sUp;
                    Vector3::MakeOrthonormalBasis( contact.normal, contact.tangents[0], contact.tangents[1]);
                    contact.contactToWorld = Matrix3( contact.normal, contact.tangents[0], contact.tangents[1]);
                    contact.position = Vector3( transform0.mPosition.x, transform0.mPosition.y - FIXED( 0.5 ), transform0.mPosition.z );
                    contact.relativeContactPosition0 = contact.position - contact.transform0->mPosition;
                    contact.restitution = physicsWorld.mRestitution;
                    contact.penetration = FIXED( 0.5 ) - transform0.mPosition.y;
                    contact.rb1         = nullptr;
                    contact.transform1 = nullptr;
                    mContacts.insert( contact );
                }

                // contact with other spheres
                auto rbIt1        = _view.begin<FxRigidbody>();
                auto transformIt1 = _view.begin<FxTransform>();
                for( ; rbIt1 != _view.end<FxRigidbody>(); ++rbIt1, ++transformIt1 )
                {
                    FxRigidbody& rb1        = *rbIt1;
                    FxTransform& transform1 = *transformIt1;
                    if( &rb0 != &rb1 &&( transform0.mPosition - transform1.mPosition ).SqrMagnitude() < Fixed( 1 ) )
                    {
                        Contact contact;
                        contact.rb0         = &rb0;
                        contact.rb1         = &rb1;
                        contact.transform0  = &transform0;
                        contact.transform1 = &transform1;
                        contact.normal      = (transform0.mPosition - transform1.mPosition).Normalized();
                        Vector3::MakeOrthonormalBasis( contact.normal, contact.tangents[0], contact.tangents[1]);
                        contact.contactToWorld = Matrix3( contact.normal, contact.tangents[0], contact.tangents[1]);
                        contact.position = transform0.mPosition - FIXED( 0.5 ) * contact.normal;
                        contact.relativeContactPosition0 = contact.position - contact.transform0->mPosition;
                        contact.relativeContactPosition1 = contact.position - contact.transform1->mPosition;
                        contact.restitution = physicsWorld.mRestitution;
                        contact.penetration = 1 - ( transform0.mPosition - transform1.mPosition ).Magnitude();
                        mContacts.insert( contact );
                    }
                }
            }

            RenderDebug& rd = _world.GetSingleton<RenderDebug>();
            for( Contact c : mContacts )
            {
                rd.DebugPoint(ToBullet(Math::ToGLM(c.position)), Color::sRed );
                rd.DebugLine( ToBullet(Math::ToGLM(c.position)), ToBullet(Math::ToGLM(c.position+ c.normal)), Color::sYellow, false );
            }
            tmpTest = {mContacts.begin(), mContacts.end()};
            physicsWorld.mContactSolver.ResolveContacts( {mContacts.begin(), mContacts.end()}, _delta );
        }
    };
}