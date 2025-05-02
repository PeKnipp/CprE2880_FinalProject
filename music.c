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
    unsigned char driftVeil_note[] = {75, 82, 81, 80, 73, 75, 75, 85, 87, 87, 87, 85, 87, 87, 82, 85};
    //TODO: write the music notes with durations
    unsigned char driftVeil_duration[] = {16, 16, 16, 16, 16, 16, 8, 8, 16, 16, 16, 16, 16, 8, 8, 16};

    oi_loadSong(driftVeil_index, driftVeil_num, driftVeil_note, driftVeil_duration);

    int catsMars_index = 2;
        int catsMars_num = 16;
//        unsigned char catsMars_note[] = {76, 30, 79, 30, 86, 30, 83, 30, 79, 30, 76, 30, 76, 30, 79, 30};
//        //TODO: write the music notes with durations
//        unsigned char catsMars_duration[] = {16, 8, 16, 8, 32, 8, 16, 8, 16, 8, 32, 8, 16, 8, 16, 8};

        unsigned char catsMars_note[] = {76, 79, 86, 83, 79, 76, 76, 79, 83, 79, 74, 71, 71, 74, 78, 30};
                //TODO: write the music notes with durations
                unsigned char catsMars_duration[] = {16, 16, 32, 16, 16, 32, 16, 16, 32, 16, 16, 32, 16, 16, 32, 8};

        oi_loadSong(catsMars_index, catsMars_num, catsMars_note, catsMars_duration);
}

void play_song(){
    make_song();
    oi_play_song(1);//selects song to play
}
