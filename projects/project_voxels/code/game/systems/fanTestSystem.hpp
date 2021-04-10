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
            if( _delta == 0){ return; }

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
                    contact.restitution = FIXED( 0.9);
                    contact.penetration = FIXED( 0.5 ) - transform0.mPosition.y;
                    contact.rb1         = nullptr;
                    contact.transform1 = nullptr;
                    float pos = transform0.mPosition.y.ToFloat();(void)pos;
                    float p = contact.penetration.ToFloat();(void)p;
                    glm::vec3  p1 = Math::ToGLM(contact.transform0->mPosition);(void)p1;
                    glm::vec3  n = Math::ToGLM(contact.normal);(void)n;
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
                        contact.transform0  = &transform0;
                        contact.normal      = (transform0.mPosition - transform1.mPosition).Normalized();
                        contact.restitution = FIXED( 0.9);
                        contact.penetration = 1 - ( transform0.mPosition - transform1.mPosition ).Magnitude();
                        contact.rb1         = &rb1;
                        contact.transform1 = &transform1;
                        float p = contact.penetration.ToFloat();(void)p;
                        glm::vec3  p1 = Math::ToGLM(contact.transform0->mPosition);(void)p1;
                        glm::vec3  p2 = Math::ToGLM(contact.transform1->mPosition);(void)p2;
                        glm::vec3  n = Math::ToGLM(contact.normal);(void)n;
                        mContacts.insert( contact );
                    }
                }
            }



            physicsWorld.mContactSolver.ResolveContacts( {mContacts.begin(), mContacts.end()}, _delta );
        }
    };
}