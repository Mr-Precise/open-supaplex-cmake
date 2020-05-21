/*
* This file is part of the OpenSupaplex distribution (https://github.com/sergiou87/open-supaplex).
* Copyright (c) 2020 Sergio Padrino
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, version 3.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "graphics.h"

#include <assert.h>
#include <string.h>

#include "globals.h"
#include "utils.h"

uint8_t gChars6BitmapFont[kBitmapFontLength];
uint8_t gChars8BitmapFont[kBitmapFontLength];
uint8_t gFixedDecodedBitmapData[kFixedBitmapWidth * kFixedBitmapHeight];
uint8_t gMovingDecodedBitmapData[kMovingBitmapWidth * kMovingBitmapHeight];
uint8_t gPanelDecodedBitmapData[kPanelBitmapWidth * kPanelBitmapHeight];
uint8_t gPanelRenderedBitmapData[kPanelBitmapWidth * kPanelBitmapHeight];
uint8_t gCurrentPanelHeight = kPanelBitmapHeight;
uint8_t gMenuBitmapData[kFullScreenBitmapLength]; // 0x4D34 ??
uint8_t gControlsBitmapData[kFullScreenBitmapLength];
uint8_t gBackBitmapData[kFullScreenBitmapLength];
uint8_t gGfxBitmapData[kFullScreenBitmapLength];
uint8_t gTitle2DecodedBitmapData[kFullScreenFramebufferLength];
uint8_t gScrollDestinationScreenBitmapData[kFullScreenFramebufferLength];
uint8_t gLevelBitmapData[kLevelBitmapWidth * kLevelBitmapHeight];

void readMenuDat() // proc near       ; CODE XREF: readEverything+9p
{
    if (gFastMode == FastModeTypeUltra)
    {
        return;
    }

    FILE *file = openReadonlyFile("MENU.DAT", "r");
    if (file == NULL)
    {
        exitWithError("Error opening MENU.DAT\n");
    }

//loc_4AAED:             // ; CODE XREF: readMenuDat+8j

    size_t bytes = fread(gMenuBitmapData, sizeof(uint8_t), sizeof(gMenuBitmapData), file);
    if (bytes < sizeof(gMenuBitmapData))
    {
        exitWithError("Error reading MENU.DAT\n");
    }

//loc_4AB0B:              // ; CODE XREF: readMenuDat+25j
    if (fclose(file) != 0)
    {
        exitWithError("Error closing MENU.DAT\n");
    }
}

void loadMurphySprites() // readMoving  proc near       ; CODE XREF: start:isFastModep
                    //; start+382p ...
{
    if (gFastMode == FastModeTypeUltra)
    {
        return;
    }

    // 01ED:0D84

//loc_479ED:              // ; CODE XREF: loadMurphySprites+27j

// IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT
// IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT
// MOVING.DAT bitmap size is 320x462
// IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT
// IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT

    FILE *file = openReadonlyFile("MOVING.DAT", "r");

    if (file == NULL)
    {
//    loc_47A10:              ; CODE XREF: loadMurphySprites+13j
//                    ; loadMurphySprites+25j
        exitWithError("Error opening MOVING.DAT\n");
    }

//loc_47A13:              ; CODE XREF: loadMurphySprites+Ej

    // There is an error in the original code, it uses height 464 (which should be
    // harmless in the original implementation since fread doesn't fail there, just
    // returns 0. The new implementation will fail on partial reads.
    for (int y = 0; y < kMovingBitmapHeight; ++y)
    {
        uint8_t fileData[kMovingBitmapWidth / 2];

        size_t bytes = fread(fileData, 1, sizeof(fileData), file);
        if (bytes < sizeof(fileData))
        {
            exitWithError("Error reading MOVING.DAT\n");
        }

//loc_47A40:              ; CODE XREF: loadMurphySprites+BAj
        for (int x = 0; x < kMovingBitmapWidth; ++x)
        {
//loc_47A45:              ; CODE XREF: loadMurphySprites+AEj
            uint32_t destPixelAddress = y * kMovingBitmapWidth + x;

            uint8_t sourcePixelAddress = x / 8;
            uint8_t sourcePixelBitPosition = 7 - (x % 8);

            uint8_t b = (fileData[sourcePixelAddress + 0] >> sourcePixelBitPosition) & 0x1;
            uint8_t g = (fileData[sourcePixelAddress + 40] >> sourcePixelBitPosition) & 0x1;
            uint8_t r = (fileData[sourcePixelAddress + 80] >> sourcePixelBitPosition) & 0x1;
            uint8_t i = (fileData[sourcePixelAddress + 120] >> sourcePixelBitPosition) & 0x1;

            uint8_t finalColor = ((b << 0)
                                  | (g << 1)
                                  | (r << 2)
                                  | (i << 3));

            // Store a copy of the decoded value in a buffer with 4bit per pixel
            gMovingDecodedBitmapData[destPixelAddress] = finalColor;
        }
    }
    if (fclose(file) != 0)
    {
        exitWithError("Error closing MOVING.DAT\n");
    }

//loc_47AB1:              ; CODE XREF: loadMurphySprites+C5j
    // IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT
    // IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT
    // FIXED.DAT bitmap size is 640x16
    // IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT
    // IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT

    file = openReadonlyFile("FIXED.DAT", "r");
    if (file == NULL)
    {
        exitWithError("Error opening FIXED.DAT\n");
    }

//loc_47ABE:              //; CODE XREF: loadMurphySprites+D2j
    uint8_t bitmapData[kFixedBitmapWidth * kFixedBitmapHeight / 2];
    size_t bytes = fread(bitmapData, 1, sizeof(bitmapData), file);
    if (bytes < sizeof(bitmapData))
    {
        exitWithError("Error reading FIXED.DAT\n");
    }

    for (int y = 0; y < kFixedBitmapHeight; ++y)
    {
        for (int x = 0; x < kFixedBitmapWidth; ++x)
        {
            uint32_t destPixelAddress = y * kFixedBitmapWidth + x;

            uint16_t sourcePixelAddress = y * kFixedBitmapWidth / 2 + x / 8;
            uint8_t sourcePixelBitPosition = 7 - (x % 8);

            uint8_t b = (bitmapData[sourcePixelAddress + 0] >> sourcePixelBitPosition) & 0x1;
            uint8_t g = (bitmapData[sourcePixelAddress + 80] >> sourcePixelBitPosition) & 0x1;
            uint8_t r = (bitmapData[sourcePixelAddress + 160] >> sourcePixelBitPosition) & 0x1;
            uint8_t i = (bitmapData[sourcePixelAddress + 240] >> sourcePixelBitPosition) & 0x1;

            uint8_t finalColor = ((b << 0)
                                  | (g << 1)
                                  | (r << 2)
                                  | (i << 3));

            // Store a copy of the decoded value in a buffer with 4bit per pixel
            gFixedDecodedBitmapData[destPixelAddress] = finalColor;
        }
    }

//loc_47AD5:              //; CODE XREF: loadMurphySprites+E9j
    if (fclose(file) != 0)
    {
        exitWithError("Error closing FIXED.DAT\n");
    }
}

void readPanelDat() //    proc near       ; CODE XREF: readPanelDat+14j
                    // ; readEverything+6p
{
    if (gFastMode == FastModeTypeUltra)
    {
        return;
    }

    FILE *file = openReadonlyFile("PANEL.DAT", "r");
    if (file == NULL)
    {
        exitWithError("Error opening PANEL.DAT\n");
    }

//loc_47B0A:             // ; CODE XREF: readPanelDat+8j
    uint8_t bitmapData[kPanelBitmapWidth * kPanelBitmapHeight / 2];
    size_t bytes = fread(bitmapData, 1, sizeof(bitmapData), file);
    if (bytes < sizeof(bitmapData))
    {
        exitWithError("Error reading PANEL.DAT\n");
    }

    for (int y = 0; y < kPanelBitmapHeight; ++y)
    {
        for (int x = 0; x < kPanelBitmapWidth; ++x)
        {
            uint32_t destPixelAddress = y * kPanelBitmapWidth + x;

            uint16_t sourcePixelAddress = y * kPanelBitmapWidth / 2 + x / 8;
            uint8_t sourcePixelBitPosition = 7 - (x % 8);

            uint8_t b = (bitmapData[sourcePixelAddress + 0] >> sourcePixelBitPosition) & 0x1;
            uint8_t g = (bitmapData[sourcePixelAddress + 40] >> sourcePixelBitPosition) & 0x1;
            uint8_t r = (bitmapData[sourcePixelAddress + 80] >> sourcePixelBitPosition) & 0x1;
            uint8_t i = (bitmapData[sourcePixelAddress + 120] >> sourcePixelBitPosition) & 0x1;

            uint8_t finalColor = ((b << 0)
                                  | (g << 1)
                                  | (r << 2)
                                  | (i << 3));

            // Store a copy of the decoded value in a buffer with 4bit per pixel
            gPanelDecodedBitmapData[destPixelAddress] = finalColor;
        }
    }

//loc_47B21:              // ; CODE XREF: readPanelDat+2Bj
    if (fclose(file) != 0)
    {
        exitWithError("Error closing PANEL.DAT\n");
    }
}

void readBackDat() // proc near       ; CODE XREF: readBackDat+14j
                    // ; readEverything+15p
{
    if (gFastMode == FastModeTypeUltra)
    {
        return;
    }

    // address: 01ED:0ECD
    FILE *file = openReadonlyFile("BACK.DAT", "r");
    if (file == NULL)
    {
        exitWithError("Error opening BACK.DAT\n");
    }

//loc_47B49:             // ; CODE XREF: readBackDat+8j
    size_t bytes = fread(gBackBitmapData, 1, sizeof(gBackBitmapData), file);
    if (bytes < sizeof(gBackBitmapData))
    {
        exitWithError("Error reading BACK.DAT\n");
    }

//loc_47B67:              //; CODE XREF: readBackDat+31j
    if (fclose(file) != 0)
    {
        exitWithError("Error closing BACK.DAT\n");
    }
}

// The bitmap fonts are bitmaps of 512x8 pixels. The way they're encoded is each bit is a pixel that
// is either on or off, so this is encoded in exactly 512 bytes.
// These bits are used as bitmasks to render on top of an existing image combining:
// - enabling the corresponding pixels in the frame buffer
// - setting those pixels from the font in the bitmask to prevent the VGA controller from overriding
//   other pixels (i.e. preserving the content of the original image in the pixels that are "empty"
//   in the font).
//
void readBitmapFonts() //   proc near       ; CODE XREF: readBitmapFonts+14j
                    // ; readEverything+3p
{
    if (gFastMode == FastModeTypeUltra)
    {
        return;
    }

    FILE *file = openReadonlyFile("CHARS6.DAT", "r");
    if (file == NULL)
    {
        exitWithError("Error opening CHARS6.DAT\n");
    }

//loc_47B90:              // ; CODE XREF: readBitmapFonts+8j
    size_t bytes = fread(gChars6BitmapFont, sizeof(uint8_t), kBitmapFontLength, file);
    if (bytes < kBitmapFontLength)
    {
        exitWithError("Error reading CHARS6.DAT\n");
    }

//loc_47BA7:              // ; CODE XREF: readBitmapFonts+2Bj
    if (fclose(file) != 0)
    {
        exitWithError("Error closing CHARS6.DAT\n");
    }

//loc_47BB5:              //; CODE XREF: readBitmapFonts+39j
    file = openReadonlyFile("CHARS8.DAT", "r");
    if (file == NULL)
    {
        exitWithError("Error opening CHARS8.DAT\n");
    }

//loc_47BC2:              //; CODE XREF: readBitmapFonts+46j
    bytes = fread(gChars8BitmapFont, sizeof(uint8_t), kBitmapFontLength, file);
    if (bytes < kBitmapFontLength)
    {
        exitWithError("Error reading CHARS8.DAT\n");
    }

//loc_47BD9:             // ; CODE XREF: readBitmapFonts+5Dj
    if (fclose(file) != 0)
    {
        exitWithError("Error closing CHARS8.DAT\n");
    }
}

void readAndRenderTitleDat() // proc near  ; CODE XREF: start+2BBp
{
    FILE *file = openReadonlyFile("TITLE.DAT", "r");

    if (file == NULL)
    {
        exitWithError("Error opening TITLE.DAT\n");
    }

//loc_47C18:              // ; CODE XREF: readAndRenderTitleDat+2Bj

    const uint8_t kBytesPerRow = kScreenWidth / 2;
    uint8_t fileData[kBytesPerRow];

    for (int y = 0; y < kScreenHeight; y++)
    {
//loc_47C3F:              //; CODE XREF: readAndRenderTitleDat+8Ej
        // read 160 bytes from title.dat
        size_t bytesRead = fread(fileData, sizeof(uint8_t), kBytesPerRow, file);

        if (bytesRead < kBytesPerRow)
        {
            exitWithError("Error reading TITLE.DAT\n");
        }

        for (int x = 0; x < kScreenWidth; ++x)
        {
//loc_47C54:             // ; CODE XREF: readAndRenderTitleDat+67j
            uint16_t destPixelAddress = y * kScreenWidth + x;

            uint8_t sourcePixelAddress = x / 8;
            uint8_t sourcePixelBitPosition = 7 - (x % 8);

//loc_47C59:              ; CODE XREF: readAndRenderTitleDat+88j
            uint8_t b = (fileData[sourcePixelAddress + 0] >> sourcePixelBitPosition) & 0x1;
            uint8_t g = (fileData[sourcePixelAddress + 40] >> sourcePixelBitPosition) & 0x1;
            uint8_t r = (fileData[sourcePixelAddress + 80] >> sourcePixelBitPosition) & 0x1;
            uint8_t i = (fileData[sourcePixelAddress + 120] >> sourcePixelBitPosition) & 0x1;

            uint8_t finalColor = ((b << 0)
                                  | (g << 1)
                                  | (r << 2)
                                  | (i << 3));

            gScreenPixels[destPixelAddress] = finalColor;
        }
    }

    if (fclose(file) != 0)
    {
        exitWithError("Error closing TITLE.DAT\n");
    }
}

void readAndRenderTitle1Dat(void)
{
//loc_478C0:              // ; CODE XREF: loadScreen2+8j
    FILE *file = openReadonlyFile("TITLE1.DAT", "r");
    if (file == NULL)
    {
        exitWithError("Error opening TITLE1.DAT\n");
    }

    const uint8_t kBytesPerRow = kScreenWidth / 2;
    uint8_t fileData[kBytesPerRow];

    for (int y = 0; y < kScreenHeight; y++)
    {
//loc_478E7:              //; CODE XREF: loadScreen2+6Bj
        // read 160 bytes from title.dat
        size_t bytesRead = fread(fileData, 1, kBytesPerRow, file);

        if (bytesRead < kBytesPerRow)
        {
            exitWithError("Error reading TITLE1.DAT\n");
        }

        for (int x = 0; x < kScreenWidth; ++x)
        {
            uint16_t destPixelAddress = y * kScreenWidth + x;

            uint8_t sourcePixelAddress = x / 8;
            uint8_t sourcePixelBitPosition = 7 - (x % 8);

            uint8_t b = (fileData[sourcePixelAddress + 0] >> sourcePixelBitPosition) & 0x1;
            uint8_t g = (fileData[sourcePixelAddress + 40] >> sourcePixelBitPosition) & 0x1;
            uint8_t r = (fileData[sourcePixelAddress + 80] >> sourcePixelBitPosition) & 0x1;
            uint8_t i = (fileData[sourcePixelAddress + 120] >> sourcePixelBitPosition) & 0x1;

            uint8_t finalColor = ((b << 0)
                                  | (g << 1)
                                  | (r << 2)
                                  | (i << 3));

            // Copy directly to the screen too
            gScreenPixels[destPixelAddress] = finalColor;
        }
    }

    if (fclose(file) != 0)
    {
        exitWithError("Error closing TITLE1.DAT\n");
    }
}

void readTitle2Dat()
{
    FILE *file = openReadonlyFile("TITLE2.DAT", "r");
    if (file == NULL)
    {
        exitWithError("Error opening TITLE2.DAT\n");
    }

    const uint8_t kBytesPerRow = kScreenWidth / 2;
    uint8_t fileData[kBytesPerRow];

// loc_47978:              //; CODE XREF: loadScreen2+C0j
    for (int y = 0; y < kScreenHeight; y++)
    {
        // loc_47995:              //; CODE XREF: loadScreen2+119j
        // read 160 bytes from title.dat
        size_t bytesRead = fread(fileData, sizeof(uint8_t), kBytesPerRow, file);

        if (bytesRead < kBytesPerRow)
        {
            exitWithError("Error reading TITLE2.DAT\n");
        }

        for (int x = 0; x < kScreenWidth; ++x)
        {
//loc_479AF:              ; CODE XREF: loadScreen2+113j
            uint16_t destPixelAddress = y * kScreenWidth + x;

            uint8_t sourcePixelAddress = x / 8;
            uint8_t sourcePixelBitPosition = 7 - (x % 8);

            uint8_t b = (fileData[sourcePixelAddress + 0] >> sourcePixelBitPosition) & 0x1;
            uint8_t g = (fileData[sourcePixelAddress + 40] >> sourcePixelBitPosition) & 0x1;
            uint8_t r = (fileData[sourcePixelAddress + 80] >> sourcePixelBitPosition) & 0x1;
            uint8_t i = (fileData[sourcePixelAddress + 120] >> sourcePixelBitPosition) & 0x1;

            uint8_t finalColor = ((b << 0)
                                  | (g << 1)
                                  | (r << 2)
                                  | (i << 3));

            // Store a copy of the decoded value in a buffer with 4bit per pixel
            gTitle2DecodedBitmapData[destPixelAddress] = finalColor;
        }
    }

    if (fclose(file) != 0)
    {
        exitWithError("Error closing TITLE2.DAT\n");
    }
}

void drawLevelViewport(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
    if (gFastMode == FastModeTypeUltra)
    {
        return;
    }

    int scrollX = MAX(0, MIN(x, kLevelBitmapWidth - width));
    int scrollY = MAX(0, MIN(y, kLevelBitmapHeight - height));

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            gScreenPixels[y * kScreenWidth + x] = gLevelBitmapData[(scrollY + y) * kLevelBitmapWidth + x + scrollX];
        }
    }
}

void readGfxDat() //  proc near       ; CODE XREF: readGfxDat+14j
                   // ; readEverything+1Ep
{
    if (gFastMode == FastModeTypeUltra)
    {
        return;
    }

    FILE *file = openReadonlyFile("GFX.DAT", "r");
    if (file == NULL)
    {
        exitWithError("Error opening GFX.DAT\n");
    }

//loc_47DB5:             // ; CODE XREF: readGfxDat+8j
    size_t bytes = fread(gGfxBitmapData, 1, sizeof(gGfxBitmapData), file);
    if (bytes < sizeof(gGfxBitmapData))
    {
        exitWithError("Error reading GFX.DAT\n");
    }

//loc_47DD3:             // ; CODE XREF: readGfxDat+31j
    if (fclose(file) != 0)
    {
        exitWithError("Error closing GFX.DAT\n");
    }

// readGfxDat  endp
}

void readControlsDat() // proc near       ; CODE XREF: readControlsDat+14j
                    // ; readEverything+Cp
{
    if (gFastMode == FastModeTypeUltra)
    {
        return;
    }

    FILE *file = openReadonlyFile("CONTROLS.DAT", "r");
    if (file == NULL)
    {
        exitWithError("Error opening CONTROLS.DAT\n");
    }

//loc_47DFC:             // ; CODE XREF: readControlsDat+8j
    size_t bytes = fread(gControlsBitmapData, 1, sizeof(gControlsBitmapData), file);
    if (bytes < sizeof(gControlsBitmapData))
    {
        exitWithError("Error reading CONTROLS.DAT\n");
    }

//loc_47E1A:             // ; CODE XREF: readControlsDat+31j
    if (fclose(file) != 0)
    {
        exitWithError("Error closing CONTROLS.DAT\n");
    }
}

// Draws the fixed stuff from the level (edges of the screen + tiles from FIXED.DAT)
void drawFixedLevel() // sub_48F6D   proc near       ; CODE XREF: start+335p runLevel+AAp ...
{
    if (gFastMode == FastModeTypeUltra)
    {
        return;
    }

    // 01ED:230A

    static const uint16_t kMovingBitmapTopLeftCornerX = 288;
    static const uint16_t kMovingBitmapTopLeftCornerY = 388;
    static const uint16_t kMovingBitmapTopRightCornerX = 296;
    static const uint16_t kMovingBitmapTopRightCornerY = 388;
    static const uint16_t kMovingBitmapBottomRightCornerX = 296;
    static const uint16_t kMovingBitmapBottomRightCornerY = 396;
    static const uint16_t kMovingBitmapBottomLeftCornerX = 288;
    static const uint16_t kMovingBitmapBottomLeftCornerY = 396;
    static const uint16_t kMovingBitmapTopEdgeX = 304;
    static const uint16_t kMovingBitmapTopEdgeY = 396;
    static const uint16_t kMovingBitmapRightEdgeX = 304;
    static const uint16_t kMovingBitmapRightEdgeY = 388;
    static const uint16_t kMovingBitmapBottomEdgeX = 304;
    static const uint16_t kMovingBitmapBottomEdgeY = 396;
    static const uint16_t kMovingBitmapLeftEdgeX = 312;
    static const uint16_t kMovingBitmapLeftEdgeY = 388;

    // Draws top-left corner
    for (int y = 0; y < kLevelEdgeSize; ++y)
    {
        for (int x = 0; x < kLevelEdgeSize; ++x)
        {
//loc_48F86:              ; CODE XREF: drawFixedLevel+20j
            size_t srcAddress = (kMovingBitmapTopLeftCornerY + y) * kMovingBitmapWidth + kMovingBitmapTopLeftCornerX + x;
            size_t dstAddress = (y * kLevelBitmapWidth) + x;
            gLevelBitmapData[dstAddress] = gMovingDecodedBitmapData[srcAddress];
        }
    }

    // Draws top edge
    for (int y = 0; y < kLevelEdgeSize; ++y)
    {
//loc_48FA0:              ; CODE XREF: drawFixedLevel+41j
        for (int x = kLevelEdgeSize - 1; x < kLevelBitmapWidth - kLevelEdgeSize; ++x)
        {
//loc_48FA3:              ; CODE XREF: drawFixedLevel+38j
            size_t srcAddress = (kMovingBitmapTopEdgeY + y) * kMovingBitmapWidth + kMovingBitmapTopEdgeX + (x % kLevelEdgeSize);
            size_t dstAddress = (y * kLevelBitmapWidth) + x;
            gLevelBitmapData[dstAddress] = gMovingDecodedBitmapData[srcAddress];
        }
    }

    // Top-right corner
    for (int y = 0; y < kLevelEdgeSize; ++y)
    {
        for (int x = kLevelBitmapWidth - 1; x >= kLevelBitmapWidth - kLevelEdgeSize; --x)
        {
//loc_48FC8:              ; CODE XREF: drawFixedLevel+62j
            int srcX = x - kLevelBitmapWidth + kLevelEdgeSize;
            size_t srcAddress = (kMovingBitmapTopRightCornerY + y) * kMovingBitmapWidth + kMovingBitmapTopRightCornerX + srcX;
            size_t dstAddress = (y * kLevelBitmapWidth) + x;
            gLevelBitmapData[dstAddress] = gMovingDecodedBitmapData[srcAddress];
        }
    }

    // Right edge
    for (int y = kLevelEdgeSize - 1; y < kLevelBitmapHeight - kLevelEdgeSize; ++y)
    {
//loc_48FA0:              ; CODE XREF: drawFixedLevel+41j
        for (int x = kLevelBitmapWidth - 1; x >= kLevelBitmapWidth - kLevelEdgeSize; --x)
        {
//loc_48FA3:              ; CODE XREF: drawFixedLevel+38j
            int srcX = x - kLevelBitmapWidth + kLevelEdgeSize;
            int srcY = y % kLevelEdgeSize;
            size_t srcAddress = (kMovingBitmapRightEdgeY + srcY) * kMovingBitmapWidth + kMovingBitmapRightEdgeX + srcX;
            size_t dstAddress = (y * kLevelBitmapWidth) + x;
            gLevelBitmapData[dstAddress] = gMovingDecodedBitmapData[srcAddress];
        }
    }

    // Bottom-right corner
    for (int y = kLevelBitmapHeight - 1; y >= kLevelBitmapHeight - kLevelEdgeSize; --y)
    {
        for (int x = kLevelBitmapWidth - 1; x >= kLevelBitmapWidth - kLevelEdgeSize; --x)
        {
//loc_48FFE:              ; CODE XREF: drawFixedLevel+98j
            int srcX = x - kLevelBitmapWidth + kLevelEdgeSize;
            int srcY = y - kLevelBitmapHeight + kLevelEdgeSize;
            size_t srcAddress = (kMovingBitmapBottomRightCornerY + srcY) * kMovingBitmapWidth + kMovingBitmapBottomRightCornerX + srcX;
            size_t dstAddress = (y * kLevelBitmapWidth) + x;
            gLevelBitmapData[dstAddress] = gMovingDecodedBitmapData[srcAddress];
        }
    }

    // Bottom edge
    for (int y = kLevelBitmapHeight - 1; y >= kLevelBitmapHeight - kLevelEdgeSize; --y)
    {
//loc_4901C:              ; CODE XREF: drawFixedLevel+BDj
        for (int x = kLevelEdgeSize - 1; x < kLevelBitmapWidth - kLevelEdgeSize; ++x)
        {
//loc_4901F:              ; CODE XREF: drawFixedLevel+B4j
            int srcX = x % kLevelEdgeSize;
            int srcY = y - kLevelBitmapHeight + kLevelEdgeSize;
            size_t srcAddress = (kMovingBitmapBottomEdgeY + srcY) * kMovingBitmapWidth + kMovingBitmapBottomEdgeX + srcX;
            size_t dstAddress = (y * kLevelBitmapWidth) + x;
            assert(dstAddress < kLevelBitmapWidth * kLevelBitmapHeight);
            gLevelBitmapData[dstAddress] = gMovingDecodedBitmapData[srcAddress];
        }
    }

    // Draws left edge
    for (int y = kLevelEdgeSize - 1; y < kLevelBitmapHeight - kLevelEdgeSize; ++y)
    {
//loc_49047:              ; CODE XREF: drawFixedLevel+EBj
        for (int x = 0; x < kLevelEdgeSize; ++x)
        {
//loc_4904A:              ; CODE XREF: drawFixedLevel+E4j
            int srcY = y % kLevelEdgeSize;

            size_t srcAddress = (kMovingBitmapLeftEdgeY + srcY) * kMovingBitmapWidth + kMovingBitmapLeftEdgeX + x;
            size_t dstAddress = (y * kLevelBitmapWidth) + x;
            assert(dstAddress < kLevelBitmapWidth * kLevelBitmapHeight);
            gLevelBitmapData[dstAddress] = gMovingDecodedBitmapData[srcAddress];
        }
    }

    // Bottom-left corner
    for (int y = kLevelBitmapHeight - 1; y >= kLevelBitmapHeight - kLevelEdgeSize; --y)
    {
        for (int x = 0; x < kLevelEdgeSize; ++x)
        {
//loc_49067:              ; CODE XREF: drawFixedLevel+101j
            int srcY = y - kLevelBitmapHeight + kLevelEdgeSize;
            size_t srcAddress = (kMovingBitmapBottomLeftCornerY + srcY) * kMovingBitmapWidth + kMovingBitmapBottomLeftCornerX + x;
            size_t dstAddress = (y * kLevelBitmapWidth) + x;
            assert(dstAddress < kLevelBitmapWidth * kLevelBitmapHeight);
            gLevelBitmapData[dstAddress] = gMovingDecodedBitmapData[srcAddress];
        }
    }

    for (int tileY = 1; tileY < kLevelHeight - 1; ++tileY)
    {
        for (int tileX = 1; tileX < kLevelWidth - 1; ++tileX)
        {
            int bitmapTileX = tileX - 1;
            int bitmapTileY = tileY - 1;

            size_t startDstX = kLevelEdgeSize + bitmapTileX * kTileSize;
            size_t startDstY = kLevelEdgeSize + bitmapTileY * kTileSize;
            uint16_t tileValue = gCurrentLevelState[tileY * kLevelWidth + tileX].tile;
            size_t startSrcX = tileValue * kTileSize;

            for (int y = 0; y < kTileSize; ++y)
            {
                for (int x = 0; x < kTileSize; ++x)
                {
                    size_t dstAddress = (startDstY + y) * kLevelBitmapWidth + startDstX + x;
                    size_t srcAddress = (y * kFixedBitmapWidth) + startSrcX + x;
                    gLevelBitmapData[dstAddress] = gFixedDecodedBitmapData[srcAddress];
                }
            }
        }
    }
}

void drawCurrentLevelViewport(uint16_t panelHeight)
{
    if (gFastMode == FastModeTypeUltra)
    {
        return;
    }

    uint16_t viewportHeight = kScreenHeight - panelHeight;

    drawLevelViewport(gScrollOffsetX, gScrollOffsetY, kScreenWidth, viewportHeight);

    for (int y = 0; y < panelHeight; ++y)
    {
        uint32_t srcAddress = y * kPanelBitmapWidth;
        uint32_t dstAddress = (viewportHeight + y) * kScreenWidth;
        memcpy(&gScreenPixels[dstAddress], &gPanelRenderedBitmapData[srcAddress], kPanelBitmapWidth);
    }
}

void drawMovingSpriteFrameInLevel(uint16_t srcX, uint16_t srcY, uint16_t width, uint16_t height, int16_t dstX, int16_t dstY)
{
    if (gFastMode == FastModeTypeUltra)
    {
        return;
    }

    assert((width % kTileSize) == 0);

    for (int y = 0; y < height; ++y)
    {
        int16_t finalY = dstY + y - kLevelEdgeSize;

        if (finalY < 0 || finalY >= kLevelBitmapHeight)
        {
            continue;
        }

        for (int x = 0; x < width; ++x)
        {
            int16_t finalX = dstX + x - kLevelEdgeSize;

            if (finalX < 0 || finalX >= kLevelBitmapWidth)
            {
                continue;
            }

            size_t srcAddress = (srcY + y) * kMovingBitmapWidth + srcX + x;
            size_t dstAddress = finalY * kLevelBitmapWidth + finalX;
            gLevelBitmapData[dstAddress] = gMovingDecodedBitmapData[srcAddress];
        }
    }
}

// srcX and srcY are the coordinates of the frame to draw in MOVING.DAT
void drawMovingFrame(uint16_t srcX, uint16_t srcY, uint16_t destPosition) // sub_4F200   proc near       ; CODE XREF: scrollToMurphy+26p
                   // ; updatePlantedRedDisk+2Ap ...
{
    if (gFastMode == FastModeTypeUltra)
    {
        return;
    }

    // 01ED:859D
    // Draws a frame from MOVING.DAT on the screen
    // Parameters:
    // - di: coordinates on the screen
    // - si: coordinates on the MOVING.DAT bitmap to draw from?

    int16_t destX = (destPosition % kLevelWidth) * kTileSize;
    int16_t destY = (destPosition / kLevelWidth) * kTileSize;

    drawMovingSpriteFrameInLevel(srcX, srcY, kTileSize, kTileSize, destX, destY);
}

void scrollTerminalScreen(int16_t position)
{
    // From this point it's all rendering, nothing needed in ultra fast mode
    if (gFastMode == FastModeTypeUltra)
    {
        return;
    }

    // This code basically simulates a scroll effect in the terminal:
    // copies the row 2 in the row 10, and then row 3 in 2, 4 in 3...
    //
    uint16_t tileX = (position % kLevelWidth) - 1;
    uint16_t tileY = (position / kLevelWidth) - 1;

    uint16_t x = kLevelEdgeSize + tileX * kTileSize;
    uint16_t y = kLevelEdgeSize + tileY * kTileSize;

    uint32_t source = 0;
    uint32_t dest = 0;

    source = dest = y * kLevelBitmapWidth + x;

    dest += kLevelBitmapWidth * 10;
    source += kLevelBitmapWidth * 2;
    memcpy(&gLevelBitmapData[dest], &gLevelBitmapData[source], kTileSize);

    dest -= kLevelBitmapWidth * 8;
    source += kLevelBitmapWidth;

    for (int i = 0; i < 9; ++i)
    {
        memcpy(&gLevelBitmapData[dest], &gLevelBitmapData[source], kTileSize);
        dest += kLevelBitmapWidth;
        source += kLevelBitmapWidth;
    }
}