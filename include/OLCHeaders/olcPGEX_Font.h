#pragma once

#ifndef __OLC_PGEX_FONT__
#define __OLC_PGEX_FONT__

#include <vector>
#include <memory>
#include "olcPixelGameEngine.h"

namespace olc
{
	class Font : public olc::PGEX
	{
	public:
        Font() = delete;
	    Font(const std::string& sFontFile, olc::ResourcePack* pack = nullptr);
		
		olc::vi2d GetTextSize(const std::string& s);
		olc::vi2d GetTextSizeProp(const std::string& s);
        
        void DrawStringDecal(const olc::vf2d& pos, const std::string& sText, const Pixel col = olc::WHITE, const olc::vf2d& scale = { 1.0f, 1.0f });
		void DrawRotatedStringDecal(const olc::vf2d& pos, const std::string& sText, const float fAngle, const olc::vf2d& center = {0.0f, 0.0f},  
                                    const Pixel col = olc::WHITE, const olc::vf2d& scale = { 1.0f, 1.0f });
		void DrawStringPropDecal(const olc::vf2d& pos, const std::string& sText, const Pixel col = olc::WHITE, const olc::vf2d& scale = { 1.0f, 1.0f });
		void DrawRotatedStringPropDecal(const olc::vf2d& pos, const std::string& sText, const float fAngle, const olc::vf2d& center = {0.0f, 0.0f},  
                                        const Pixel col = olc::WHITE, const olc::vf2d& scale = { 1.0f, 1.0f });

    private:
        std::unique_ptr<olc::Sprite>    fontSprite;
        std::unique_ptr<olc::Decal>     fontDecal;

        float                           fCharWidth;
        float                           fCharHeight;
        std::vector<std::pair<olc::vi2d, olc::vi2d> > vGlyphPositionsMono;
        std::vector<std::pair<olc::vi2d, olc::vi2d> > vGlyphPositionsProp;
    };
}


#ifdef OLC_PGEX_FONT
#undef OLC_PGEX_FONT

namespace olc
{
	Font::Font(const std::string& sFontFile, olc::ResourcePack* pack)
	{
        fontSprite = std::make_unique<olc::Sprite>( sFontFile, pack );
        fontDecal = std::make_unique<olc::Decal>( fontSprite.get() );

        // Get the font information embedded in the last row of pixels
        auto lastRow = fontSprite->height - 1;
        auto pixelValue = fontSprite->GetPixel( 0, lastRow );
        int nCharWidth = fontSprite->width / 16;
        int nCharHeight = (fontSprite->height-1) / 6;
        
        if( pixelValue == olc::Pixel('F','O','N','T') )
        {
            // FONT signature found - presume the pixels contain valid font information
            uint32_t nFormatVersion = fontSprite->GetPixel( 1, lastRow ).n;
            int nOffset             = fontSprite->GetPixel( 2, lastRow ).n;
           
            for( auto i=0; i < 96; i++ )
            {
                int nWidth  = fontSprite->GetPixel( 3+i, lastRow ).n;
                int x       = i % 16 * nCharWidth + nOffset;
                int y       = i / 16 * nCharHeight;
                vGlyphPositionsMono.push_back( std::make_pair<olc::vi2d, olc::vi2d>( {x,y}, {nCharWidth, nCharHeight-1}) );
                vGlyphPositionsProp.push_back( std::make_pair<olc::vi2d, olc::vi2d>( {x,y}, {nWidth, nCharHeight-1}) );
            }
        }
        else
        {
            // No "FONT" signature found on last pixel row : presume this is simply a png
            // of 96 evenly spaced character images in 6 rows of 16 chars.
            nCharHeight = fontSprite->height / 6;
            for( auto i=0; i < 96; i++ )
            {
                int x = i % 16 * nCharWidth;
                int y = i / 16 * nCharHeight;
                vGlyphPositionsMono.push_back( std::make_pair<olc::vi2d, olc::vi2d>( {x,y}, {nCharWidth, nCharHeight-1}) );
                vGlyphPositionsProp.push_back( std::make_pair<olc::vi2d, olc::vi2d>( {x,y}, {nCharWidth, nCharHeight-1}) );
            }
        }

        fCharWidth  = float(nCharWidth);
        fCharHeight = float(nCharHeight);
    }


    olc::vi2d Font::GetTextSize(const std::string& s)
	{
		olc::vi2d size = { 0,1 };
		olc::vi2d pos = { 0,1 };
		for (auto c : s)
		{
			if (c == '\n') { pos.y++;  pos.x = 0; }
			else pos.x++;
			size.x = std::max(size.x, pos.x);
			size.y = std::max(size.y, pos.y);
		}
		return size * vf2d( fCharWidth, fCharHeight );
	}


	olc::vi2d Font::GetTextSizeProp(const std::string& s)
	{
		olc::vi2d size = { 0,1 };
		olc::vi2d pos = { 0,1 };
		for (auto c : s)
		{
			if (c == '\n') { pos.y += 1;  pos.x = 0; }
			else pos.x += vGlyphPositionsProp[c-32].second.x;
			size.x = std::max(size.x, pos.x);
			size.y = std::max(size.y, pos.y);
		}

		size.y *= (int)fCharHeight;
		return size;
	}


	void Font::DrawStringDecal(const olc::vf2d& pos, const std::string& sText, const Pixel col, const olc::vf2d& scale)
	{
		olc::vf2d spos = { 0.0f, 0.0f };
		for (auto c : sText)
		{
			if (c == '\n')
			{
				spos.x = 0; spos.y += fCharHeight * scale.y;
			}
			else
			{
                auto& glyph = vGlyphPositionsMono[c-32];
				pge->DrawPartialDecal(pos + spos, fontDecal.get(), glyph.first, glyph.second, scale, col);
				spos.x += fCharWidth * scale.x;
			}
		}
	}


	void Font::DrawRotatedStringDecal(const olc::vf2d& pos, const std::string& sText, const float fAngle, const olc::vf2d& center, const Pixel col, const olc::vf2d& scale )
    {
		olc::vf2d spos = center;
		for (auto c : sText)
		{
			if (c == '\n')
			{
				spos.x = center.x; spos.y -= fCharHeight;
			}
			else
			{
                auto& glyph = vGlyphPositionsMono[c-32];
				pge->DrawPartialRotatedDecal(pos, fontDecal.get(), fAngle, spos, glyph.first, glyph.second, scale, col);
				spos.x -= fCharWidth;
			}
		}
    }


	void Font::DrawStringPropDecal(const olc::vf2d& pos, const std::string& sText, const Pixel col, const olc::vf2d& scale)
	{
		olc::vf2d spos = { 0.0f, 0.0f };
		for (auto c : sText)
		{
			if (c == '\n')
			{
				spos.x = 0; spos.y += fCharHeight * scale.y;
			}
			else
			{
                auto& glyph = vGlyphPositionsProp[c-32];
				pge->DrawPartialDecal(pos + spos, fontDecal.get(), glyph.first, glyph.second, scale, col);
				spos.x += glyph.second.x * scale.x;
			}
		}
	}


	void Font::DrawRotatedStringPropDecal(const olc::vf2d& pos, const std::string& sText, const float fAngle, const olc::vf2d& center, const Pixel col, const olc::vf2d& scale )
    {
		olc::vf2d spos = center;

		for (auto c : sText)
		{
			if (c == '\n')
			{
				spos.x = center.x; spos.y -= fCharHeight;
			}
			else
			{
                auto& glyph = vGlyphPositionsProp[c-32];
				pge->DrawPartialRotatedDecal(pos, fontDecal.get(), fAngle, spos, glyph.first, glyph.second, scale, col);
				spos.x -= glyph.second.x;
			}
		}
    }
}

#endif
#endif
