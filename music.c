/*
 * music.c
 *
 *  Created on: Apr 29, 2025
 *      Author: pknipper
 */

#include "open_interface.h"
#include "music.h"

void make_song()
{
    int driftVeil_index = 1;
    int driftVeil_num = 16;
    unsigned char driftVeil_note[] = {80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80};
    //TODO: write the music notes with durations
    unsigned char driftVeil_duration[] = {32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32};

    oi_loadSong(driftVeil_index, driftVeil_num, driftVeil_note, driftVeil_duration);
}

void play_song(){
    make_song();
    oi_play_song(1);
}
