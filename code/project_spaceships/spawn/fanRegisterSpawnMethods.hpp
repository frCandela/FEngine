#pragma once

#include "network/singletons/fanSpawnManager.hpp"
#include "project_spaceships/spawn/fanSpawnShip.hpp"
#include "project_spaceships/spawn/fanSpawnBullet.hpp"
#include "project_spaceships/spawn/fanSpawnSolarEruption.hpp"

namespace fan
{
    inline void RegisterGameSpawnMethods( SpawnManager& _spawner )
    {
        _spawner.RegisterSpawnMethod( spawn::SpawnBullet::sID, &spawn::SpawnBullet::Spawn );
        _spawner.RegisterSpawnMethod( spawn::SpawnShip::sID, &spawn::SpawnShip::Spawn );
        _spawner.RegisterSpawnMethod( spawn::SpawnSolarEruption::sID, &spawn::SpawnSolarEruption::Spawn );
    }
}