#pragma once

#include <vector>
#include <string>
#include <ft2build.h>
#include FT_FREETYPE_H

namespace fan
{
	class Font;

	//========================================================================================================
	// Owns all the fonts of the engine
	//========================================================================================================
	class FontManager
	{
	public:
        FontManager();
        ~FontManager();
        FontManager( FontManager const& ) = delete;
        FontManager& operator=( FontManager const& ) = delete;

		void Clear();
        Font* Find( const std::string& _path ) const;
        Font* Load(  const std::string& _path );
        void Remove( const std::string& _path );
        bool Empty() const { return mFonts.empty(); }
		int  Count() const { return (int)mFonts.size(); }

	private:
		std::vector< Font* > mFonts;
        FT_Library   mFreetypeLib;
	};
}