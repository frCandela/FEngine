#include "editor/fanModals.hpp"

#include <sstream>
#include <core/fanDebug.hpp>
#include "core/input/fanKeyboard.hpp"
#include "imgui/imgui_internal.h"

//============================================================================================================
// Extracts the file name of a path without the extension
//============================================================================================================
std::string std::filesystem::file_name( const std::filesystem::path& _path )
{
	if ( std::filesystem::is_directory( _path ) == true || _path.has_filename() == false )
	{
		return "";
	}
	std::string filename = _path.filename().string();
	if ( _path.has_extension() )
	{
		const size_t lastindexBuffer = filename.find_last_of( "." );
		filename = filename.substr( 0, lastindexBuffer );
	}
	return filename;
}


namespace ImGui
{
	//========================================================================================================
	//========================================================================================================
	void FanShowHelpMarker( const char* _desc )
	{
		ImGui::TextDisabled( "(?)" );
		FanToolTip( _desc );
	}

	//========================================================================================================
	//========================================================================================================
	void FanToolTip( const char* _desc )
	{
		if ( ImGui::IsItemHovered() )
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos( ImGui::GetFontSize() * 35.0f );
			ImGui::TextUnformatted( _desc );
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
	}

	//========================================================================================================
	//========================================================================================================
	bool impl::FilesSelector(
		const std::set< std::string >& _extensionWhiteList
		, std::filesystem::path& _path )
	{
		bool returnValue = false;

		//std::filesystem::directory_entry _nextPath = std::filesystem::directory_entry(_path);

		ImGui::SetNextItemOpen( true );
		if ( ImGui::TreeNode( "File Selection" ) )
		{

			// '.' Selectable to go to the parent 
            if( _path != "." &&
                ImGui::Selectable( ".",
                                   false,
                                   ImGuiSelectableFlags_DontClosePopups |
                                   ImGuiSelectableFlags_AllowDoubleClick ) )
			{
				_path = std::filesystem::directory_entry( _path.parent_path() );
				if ( _path.empty() )
				{
					_path = std::filesystem::path( "." );
				}
			}

			// Lists all directories
            const std::filesystem::path dir = std::filesystem::is_directory( _path )
                    ? _path
                    : _path.parent_path(); // iterates on a dir, bot a file
			for ( const std::filesystem::path& childPath : std::filesystem::directory_iterator( dir ) )
			{

				std::string extension = childPath.extension().string();
				const bool isFile = _extensionWhiteList.find( extension ) != _extensionWhiteList.end();
				const bool isDirectory = std::filesystem::is_directory( childPath );
				// Filter
				if ( isFile || isDirectory )
				{
					const std::string filename = childPath.filename().generic_string();

					if ( isFile )
					{
						ImGui::PushStyleColor( ImGuiCol_Text, ( ImVec4 ) ( ImColor( 1.0f, 0.712f, 0.0f ) ) );
					}
                    if( ImGui::Selectable( filename.c_str(),
                                           false,
                                           ImGuiSelectableFlags_DontClosePopups |
                                           ImGuiSelectableFlags_AllowDoubleClick ) )
					{
						_path = std::filesystem::directory_entry( childPath );

						if ( ImGui::IsMouseDoubleClicked( 0 ) && isFile )
						{
							returnValue = true;
						}
					}
					if ( isFile )
					{
						ImGui::PopStyleColor();
					}
				}
			}

			ImGui::TreePop();
		}

		return returnValue;
	}

	//========================================================================================================
	//========================================================================================================
    bool FanSaveFileModal( const char* _popupName,
                           const std::set<std::string>& _extensionWhiteList,
                           std::filesystem::path& _outCurrentPath )
	{
		bool returnValue = false;

		static int extensionIndex = 0;

		ImGui::SetNextWindowSize( { 316,410 } );
		if ( ImGui::BeginPopupModal( _popupName ) )
		{
			// Files hierarchy
			bool itemDoubleClicked = false;
			ImGui::BeginChild( "load_scene_hierarchy", { 300,300 }, true );
			{
				itemDoubleClicked = impl::FilesSelector( _extensionWhiteList, _outCurrentPath );
			} ImGui::EndChild();


			// Input name
			const int bufferSize = 32;
			char buffer[ bufferSize ];
			strcpy_s( buffer, std::filesystem::file_name( _outCurrentPath ).c_str() );

			if ( ImGui::IsWindowAppearing() )
			{
				ImGui::SetKeyboardFocusHere();
				extensionIndex = 0;
			}
			bool enterPressed = false;
            if( ImGui::InputText( "name",
                                  buffer,
                                  IM_ARRAYSIZE( buffer ),
                                  ImGuiInputTextFlags_EnterReturnsTrue ) )
			{
				enterPressed = true;
			}
			ImGui::SetItemDefaultFocus();
			std::string stringBuffer = buffer;

			// Format selection
			std::stringstream extensions;
			std::vector<std::string> tmpExtensions;
			for ( const std::string& extension : _extensionWhiteList )
			{
				extensions << extension << '\0';
				tmpExtensions.push_back( extension );
			}
			if ( _extensionWhiteList.size() > 1 )
			{
				ImGui::Combo( "format", &extensionIndex, extensions.str().c_str() );
			}
			else
			{
				ImGui::TextDisabled( extensions.str().c_str() );
			}
			ImGui::Separator();

			// re format to path/.../filename.extension	
			if ( !std::filesystem::is_directory( _outCurrentPath ) )
			{
				_outCurrentPath.remove_filename();
			}
			if ( stringBuffer.length() > 0 )
			{
				_outCurrentPath.append( stringBuffer + tmpExtensions[ extensionIndex ] );
			}

			// Ok Button
			if ( itemDoubleClicked || ImGui::Button( "Ok" ) || enterPressed )
			{
				if ( !std::filesystem::is_directory( _outCurrentPath ) )
				{
					ImGui::CloseCurrentPopup();
					returnValue = true;
					_outCurrentPath.make_preferred();
				}
			}

			// Cancel button
			ImGui::SameLine();
			if ( ImGui::Button( "Cancel" ) || ImGui::IsKeyPressed( GLFW_KEY_ESCAPE, false ) )
			{
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
		return returnValue;
	}

	//========================================================================================================
	//========================================================================================================
	bool FanLoadFileModal(
		const char* _popupName,
		const std::set<std::string>& _extensionWhiteList,
		std::filesystem::path& _path )
	{

		bool returnValue = false;

		ImGui::SetNextWindowSize( { 316,400 } );

		if ( ImGui::BeginPopupModal( _popupName, NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove ) )
		{
            static char overrideTextBuffer[64];
            ImGui::PushItemWidth(220);
            ImGui::InputText("##override", overrideTextBuffer, 64 );
            ImGui::PopItemWidth();
            ImGui::SameLine();
            if( ImGui::Button("override"))
            {
                ImGui::CloseCurrentPopup();
                _path = overrideTextBuffer;
                fan::Debug::Log( _path.string() );
                ImGui::EndPopup();
                return true;
            }

			ImGui::BeginChild( "files_region", { 300,280 }, true );
			std::filesystem::directory_entry newEntry;
			bool itemDoubleClicked = impl::FilesSelector( _extensionWhiteList, _path );
			if ( std::filesystem::is_directory( newEntry ) )
			{
				_path = newEntry;
			}
			else if ( std::filesystem::is_regular_file( newEntry ) )
			{
				_path = newEntry;
			}
			ImGui::EndChild();


			ImGui::Text( _path.string().c_str() );
			ImGui::Separator();

            if( itemDoubleClicked == true ||
                ImGui::Button( "Ok" ) ||
                ImGui::IsKeyPressed( GLFW_KEY_ENTER, false ) )
			{
				if ( std::filesystem::is_regular_file( _path ) )
				{
					ImGui::CloseCurrentPopup();
					returnValue = true;
				}
			}
			ImGui::SameLine();
			if ( ImGui::Button( "Cancel" ) || ImGui::IsKeyPressed( GLFW_KEY_ESCAPE, false ) )
			{
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
		return returnValue;
	}

	//========================================================================================================
	//========================================================================================================
	void PushReadOnly()
	{
		ImGui::PushItemFlag( ImGuiItemFlags_Disabled, true );
		ImGui::PushStyleVar( ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f );
	}

	//========================================================================================================
	//========================================================================================================
	void PopReadOnly()
	{
		ImGui::PopItemFlag();
		ImGui::PopStyleVar();
	}
}
