#pragma once

#include "network/singletons/fanSpawnManager.hpp"
#include "fanSpawnShip.hpp"
#include "fanSpawnBullet.hpp"
#include "fanSpawnSolarEruption.hpp"

namespace fan
{
    inline void RegisterGameSpawnMethods( SpawnManager& _spawner )
    {
        _spawner.RegisterSpawnMethod( spawn::SpawnBullet::sID, &spawn::SpawnBullet::Spawn );
        _spawner.RegisterSpawnMethod( spawn::SpawnShip::sID, &spawn::SpawnShip::Spawn );
        _spawner.RegisterSpawnMethod( spawn::SpawnSolarEruption::sID, &spawn::SpawnSolarEruption::Spawn );
    }
}