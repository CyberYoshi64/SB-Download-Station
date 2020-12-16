/*
*   This file is part of Universal-Updater
*   Copyright (C) 2019-2020 DeadPhoenix8091, Epicpkmn11, Flame, RocketRobz, StackZ, TotallyNotGuy
*
*   This program is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*   Additional Terms 7.b and 7.c of GPLv3 apply to this file:
*       * Requiring preservation of specified reasonable legal notices or
*         author attributions in that material or in the Appropriate Legal
*         Notices displayed by works containing it.
*       * Prohibiting misrepresentation of the origin of that material,
*         or requiring that modified versions of such material be marked in
*         reasonable ways as different from the original version.
*/

#include "animation.hpp"
#include "common.hpp"
#include "gui.hpp"

extern C2D_SpriteSheet sprites;

float animProgBarTimer=0.0f;

// Draws a Rectangle as the progressbar.
void Animation::DrawProgressBar(float currentProgress, float totalProgress) {
	if (animProgBarTimer > 0.99f) {
		animProgBarTimer=0.0f;
	} else {
		animProgBarTimer=(animProgBarTimer * 15 + 1.0f) / 16;
	}
	Draw_Rect(61, 170, (int)(((float)currentProgress / (float)totalProgress) * 200.0f), 28, 0xff008000);
	Draw_Rect(61, 170, (int)((((float)currentProgress / (float)totalProgress) * 200.0f)*animProgBarTimer), 28, C2D_Color32(0,255-animProgBarTimer*127,0,255));
}
