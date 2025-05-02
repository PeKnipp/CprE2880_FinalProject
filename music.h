/*
 * music.h
 *
 *  Created on: Apr 29, 2025
 *      Author: pknipper
 */

#ifndef MUSIC_H_
#define MUSIC_H_

#include "open_interface.h"
/*Can have up to 4 songs with max sixteen notes
 * Lowest note the Roomba can play is Note #31
 * Anything outside of teh 31-127 range is considered a rest note
 * Duration of a musical nite is in increments of 1/64th of a second (ie half second is 32)
 */

void make_song();

void play_song(char songNo);

#endif /* MUSIC_H_ */
