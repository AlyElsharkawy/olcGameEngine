#include <memory>
#include <cmath>

#define OLC_IMAGE_STB
#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#define OLC_PGEX_FONT
#include "olcPGEX_Font.h"
#include "miscFunctions.h"

class Example : public olc::PixelGameEngine
{
private:
    std::unique_ptr<olc::Font> unseenFont;
    std::unique_ptr<olc::Font> underworldFont;
    std::unique_ptr<olc::Font> armaliteFont;
    std::unique_ptr<olc::Font> starsFont;

public:
	Example()
  {
		sAppName = "Font Extension Example";
	}

public:
	bool OnUserCreate() override
	{
        // Create the font objects now the engine is started
    /*unseenFont = std::make_unique<olc::Font>(GetFullPath("./the unseen.png"));
    underworldFont = std::make_unique<olc::Font>(GetFullPath("./underworld.png"));
    armaliteFont = std::make_unique<olc::Font>(GetFullPath("./armalite.png" ));
    starsFont = std::make_unique<olc::Font>(GetFullPath("./stars.png"));*/

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
        Clear( olc::WHITE );

        static float fAngle = 0.0f;
        fAngle += fElapsedTime;

        std::string underworld_text = "       Font\nUnderworld";
        std::string unseen_text     = "   Font\nThe Unseen";
        std::string armalite_text   = "    Font\nArmalite-Rifle";
        std::string stars_text      = "Font\nStars";

        // Get the string dimensions for rendering proportionally
        auto underworld_text_size   = underworldFont->GetTextSizeProp( underworld_text );
        auto unseen_text_size       = unseenFont->GetTextSizeProp( unseen_text );
        auto armalite_text_size     = unseenFont->GetTextSizeProp( armalite_text );
        auto stars_text_size        = starsFont->GetTextSizeProp( stars_text );

        // Compute the centre points so we can rotate about them
        auto underworld_centre      = underworld_text_size / 2.0f;
        auto unseen_centre          = unseen_text_size / 2.0f;
        auto armalite_centre        = armalite_text_size / 2.0f;
        auto stars_centre           = stars_text_size / 2.0f;

        auto fScale                 = 0.5f + std::abs( std::cos( fAngle ) );

        // Example of rendering proportionally, with a scale and a rotation
        unseenFont->DrawRotatedStringPropDecal( {200,150}, unseen_text, fAngle, unseen_centre, olc::RED, {fScale, fScale}  );
        underworldFont->DrawRotatedStringPropDecal( {500,400}, underworld_text, fAngle, underworld_centre, olc::BLACK, {fScale, fScale} );
        armaliteFont->DrawRotatedStringPropDecal( {200,400}, armalite_text, fAngle, armalite_centre, olc::DARK_GREEN, {fScale, fScale} );
        starsFont->DrawRotatedStringPropDecal( {500,150}, stars_text, fAngle, stars_centre, olc::DARK_GREY, {fScale, fScale} );

        return true;
	}
};


int main()
{
	Example demo;
	if (demo.Construct(800, 600, 1, 1))
		demo.Start();

	return 0;
}

