namespace fan
{

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    template< typename _SingletonType >
    void EcsWorld::AddSingletonType()
    {
        static_assert( std::is_base_of<EcsSingleton, _SingletonType>::value );
        fanAssert( mSingletons.find( _SingletonType::Info::sType ) == mSingletons.end() );

        // Creates the singleton component
        _SingletonType* singleton = new _SingletonType {};
        mSingletons[_SingletonType::Info::sType] = singleton;

        // Registers singleton info
        EcsSingletonInfo info;
        info.setInfo = &_SingletonType::SetInfo;
        info.mName   = _SingletonType::Info::sName;
        _SingletonType::SetInfo( info );
        info.init  = &_SingletonType::Init;
        info.mType = _SingletonType::Info::sType;
        mSingletonInfos[_SingletonType::Info::sType] = info;
        info.init( *this, *singleton );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    template< typename _SingletonType >
    _SingletonType& EcsWorld::GetSingleton()
    {
        static_assert( std::is_base_of<EcsSingleton, _SingletonType>::value );
        return static_cast<_SingletonType&>( GetSingleton( _SingletonType::Info::sType ) );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    template< typename _TagType >
    void EcsWorld::AddTagType()
    {
        static_assert( std::is_base_of<EcsTag, _TagType>::value );
        fanAssert( mNextTagIndex >= NumComponents() );
        fanAssert( mTypeToIndex.find( _TagType::Info::sType ) == mTypeToIndex.end() );
        const int newTagIndex = mNextTagIndex--;
        mTypeToIndex[_TagType::Info::sType] = newTagIndex;

        mTagsMask[newTagIndex] = 1;
        EcsTagInfo info;
        info.mName = _TagType::Info::sName;
        info.mType = _TagType::Info::sType;
        mTagsInfo.push_back( info );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    template< typename _TagType >
    void EcsWorld::AddTag( const EcsEntity _entity )
    {
        static_assert( std::is_base_of<EcsTag, _TagType>::value );
        AddTag( _entity, _TagType::Info::sType );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    template< typename _TagType >
    void EcsWorld::RemoveTag( const EcsEntity _entity )
    {
        static_assert( std::is_base_of<EcsTag, _TagType>::value );
        RemoveTag( _entity, _TagType::Info::sType );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    template< typename _TagType >
    bool EcsWorld::HasTag( const EcsEntity _entity )
    {
        static_assert( std::is_base_of<EcsTag, _TagType>::value );
        return HasTag( _entity, _TagType::Info::sType );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    template< typename _ComponentType >
    void EcsWorld::AddComponentType()
    {
        static_assert( std::is_base_of<EcsComponent, _ComponentType>::value );

        fanAssert( mTypeToIndex.find( _ComponentType::Info::sType ) == mTypeToIndex.end() );
        const int nextTypeIndex = NumComponents();
        fanAssert( mNextTagIndex >= nextTypeIndex );

        // Set component info
        EcsComponentInfo info;
        info.mName      = _ComponentType::Info::sName;
        info.construct  = &_ComponentType::Info::Instanciate;
        info.copy       = std::is_trivially_copyable<_ComponentType>::value
                ? &std::memcpy
                : &_ComponentType::Info::Memcpy;
        info.init       = &_ComponentType::Init;
        info.mSize      = _ComponentType::Info::sSize;
        info.mAlignment = _ComponentType::Info::sAlignment;
        info.mType      = _ComponentType::Info::sType;
        info.mIndex     = nextTypeIndex;
        info.setInfo    = &_ComponentType::SetInfo;
        _ComponentType::SetInfo( info );
        mComponentsInfo.push_back( info );

        mTypeToIndex[_ComponentType::Info::sType] = nextTypeIndex;

        mTransitionArchetype.AddComponentType( info );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    template< typename _ComponentType >
    _ComponentType& EcsWorld::AddComponent( const EcsEntity _entity )
    {
        static_assert( std::is_base_of<EcsComponent, _ComponentType>::value );
        return static_cast<_ComponentType&>( AddComponent( _entity, _ComponentType::Info::sType ) );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    template< typename _ComponentType >
    void EcsWorld::RemoveComponent( const EcsEntity _entity )
    {
        static_assert( std::is_base_of<EcsComponent, _ComponentType>::value );
        RemoveComponent( _entity, _ComponentType::Info::sType );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    template< typename _ComponentType >
    bool EcsWorld::HasComponent( const EcsEntity _entity )
    {
        static_assert( std::is_base_of<EcsComponent, _ComponentType>::value );
        return HasComponent( _entity, _ComponentType::Info::sType );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    template< typename _ComponentType >
    _ComponentType& EcsWorld::GetComponent( const EcsEntity _entity )
    {
        static_assert( std::is_base_of<EcsComponent, _ComponentType>::value );
        return static_cast<_ComponentType&> ( GetComponent( _entity, _ComponentType::Info::sType ) );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    template< typename _ComponentType >
    _ComponentType* EcsWorld::SafeGetComponent( const EcsEntity _entity )
    {
        static_assert( std::is_base_of<EcsComponent, _ComponentType>::value );
        return static_cast<_ComponentType*> ( SafeGetComponent( _entity, _ComponentType::Info::sType ) );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    template< typename _SystemType, typename... _Args >
    void EcsWorld::Run( _Args&& ... _args )
    {
        static_assert( std::is_base_of<EcsSystem, _SystemType>::value );
        EcsView view = Match<_SystemType>();
        if( !view.Empty() )
        {
            _SystemType::Run( *this, view, _args... );
        }
    }

    //==================================================================================================================================================================================================
    // runs even if the view is empty
    //==================================================================================================================================================================================================
    template< typename _SystemType, typename... _Args >
    void EcsWorld::ForceRun( _Args&& ... _args )
    {
        static_assert( std::is_base_of<EcsSystem, _SystemType>::value );
        EcsView view = Match<_SystemType>();
        _SystemType::Run( *this, view, _args... );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    template<typename... ComponentsOrTags >
    void EcsWorld::RunLambda( std::function<void( EcsWorld&, const EcsView& )> _lambda, const EcsSignature& _signatureExclude )
    {
        EcsView view = Match( GetSignature<ComponentsOrTags...>(), _signatureExclude );
        if( !view.Empty() )
        {
            _lambda( *this, view );
        }
    }


    //==================================================================================================================================================================================================
    // runs even if the view is empty
    //==================================================================================================================================================================================================
    template<typename... ComponentsOrTags >
    void EcsWorld::ForceRunLambda( std::function<void( EcsWorld&, const EcsView& )> _lambda, const EcsSignature& _signatureExclude )
    {
        EcsView view = Match( GetSignature<ComponentsOrTags...>(), _signatureExclude );
        _lambda( *this, view );
    }

    //==================================================================================================================================================================================================
    // creates an EcsSignature from a template list of EcsComponents
    //==================================================================================================================================================================================================
    namespace impl
    {
        template< class FirstType, class... NextTypes >
        struct AccumulateSignature;

        template< class FirstType, class... NextTypes >
        struct AccumulateSignature
        {
            static EcsSignature Get( const EcsWorld& _world ) { return _world.GetSignature<FirstType>() | AccumulateSignature<NextTypes...>::Get( _world ); }
        };

        template< class LastType >
        struct AccumulateSignature<LastType>
        {
            static EcsSignature Get( const EcsWorld& _world )
            {
                static_assert( std::is_base_of<EcsTag, LastType>::value || std::is_base_of<EcsComponent, LastType>::value );
                return EcsSignature( 1 ) << _world.GetIndex( LastType::Info::sType );
            }
        };
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    template< class FirstComponent, class... NextComponents >
    EcsSignature EcsWorld::GetSignature() const
    {
        return impl::AccumulateSignature<FirstComponent, NextComponents...>::Get( *this );
    }

    //==================================================================================================================================================================================================
    // returns an ecs view matching a system signature
    //==================================================================================================================================================================================================
    template< typename _SystemType >
    EcsView EcsWorld::Match() const
    {
        static_assert( std::is_base_of<EcsSystem, _SystemType>::value );
        const EcsSignature signature = _SystemType::GetSignature( *this );
        return Match( signature, EcsSignature( 0 ) );
    }
}