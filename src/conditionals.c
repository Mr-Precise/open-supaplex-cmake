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

#include "conditionals.h"


uint8_t supportsRealKeyboard(void)
{
#if defined(__vita__) || defined(__SWITCH__) || defined(__PSP__) || defined(__NDS__) || defined(_3DS) || defined(__PSL1GHT__) || defined(__WII__) || defined(__WIIU__)
    return 0;
#else
    return 1;
#endif
}

uint8_t supportsVirtualKeyboard(void)
{
    return 0;
}

uint8_t supportsSPFileDemoPlayback(void)
{
    return 1;
}
