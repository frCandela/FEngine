#pragma warning( push )
#pragma warning( disable : 4005 ) // macro redefinition
#include "LivePP/API/LPP_API.h"
#pragma warning( pop )
/*#include "project_spaceships/fanLaunchArguments.h"
#include "project_spaceships/fanGameClient.hpp"
#include "project_spaceships/fanGameServer.hpp"
#include "engine/fanGameHolder.hpp"
#include "editor/fanEditorHolder.hpp"*/

/*namespace fan
{
    //========================================================================================================
    // Runs the engine and connects it to the Live++ hot reload
    //========================================================================================================
    class LPPMain
    {
    public:
        LPPMain( LaunchSettings& _settings )
        {
            // enables livepp
            if( _settings.enableLivepp )
            {
                mLivePP = lpp::lppLoadAndRegister( L"middleware/LivePP/", "fanEngine" );
                lpp::lppEnableAllCallingModulesSync( mLivePP );
            }

            // runs the game
            switch( _settings.launchMode )
            {
                case LaunchSettings::Mode::EditorClient:
                {
                    _settings.windowName = _settings.windowName + "_editor_client";

                    // attaches an editor to a game client and runs it
                    fan::GameClient client;
                    fan::EditorHolder editor( _settings, { &client } );
                    if( _settings.enableLivepp )
                    {
                        editor.mOnLPPSynch.Connect( &LPPMain::OnSync, this );
                    }
                    editor.Run();
                }
                    break;
                case LaunchSettings::Mode::EditorServer:
                {
                    _settings.windowName = _settings.windowName + "_editor_server";

                    // attaches an editor to a game server and runs it
                    fan::GameServer server;
                    fan::EditorHolder editor( _settings, { &server } );
                    if( _settings.enableLivepp )
                    {
                        editor.mOnLPPSynch.Connect( &LPPMain::OnSync, this );
                    }
                    editor.Run();
                }
                    break;
                case LaunchSettings::Mode::EditorClientServer:
                {
                    _settings.windowName = _settings.windowName + "_editor_client_server";
                    // attaches an editor to a game client and runs it
                    fan::GameClient client;
                    fan::GameServer server;
                    fan::EditorHolder editor( _settings, { &client, &server } );
                    if( _settings.enableLivepp )
                    {
                        editor.mOnLPPSynch.Connect( &LPPMain::OnSync, this );
                    }
                    editor.Run();
                } break;
                case LaunchSettings::Mode::Client:
                {
                    _settings.windowName = _settings.windowName + "_client";

                    // attaches an editor to a game client and runs it
                    fan::GameClient client;
                    fan::GameHolder gameHolder( _settings, client );
                    if( _settings.enableLivepp )
                    {
                        gameHolder.mOnLPPSynch.Connect( &LPPMain::OnSync, this );
                    }
                    gameHolder.Run();
                } break;
                case LaunchSettings::Mode::Server:
                {
                    _settings.windowName = _settings.windowName + "_server";

                    // attaches an editor to a game client and runs it
                    fan::GameServer server;
                    fan::GameHolder gameHolder( _settings, server );
                    if( _settings.enableLivepp )
                    {
                        gameHolder.mOnLPPSynch.Connect( &LPPMain::OnSync, this );
                    }
                    gameHolder.Run();
                } break;
                default:
                    fanAssert( false );
                    break;
            }

            // disables livepp
            if( _settings.enableLivepp )
            {
                ::FreeLibrary( mLivePP );
            }
        }

    private:
        void OnSync() { lpp::lppSyncPoint( mLivePP ); }

        HMODULE mLivePP;
    };
}*/