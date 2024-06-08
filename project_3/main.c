#include <stdio.h>
#include "avr.h"
#include "lcd.h"


volatile int pauseFlag = 0;
volatile float tempFactor = 1;
volatile float freqFactor = 1;


typedef enum {
    A, As, B, C, Cs, D, Ds, Ee, F, Fs, G, Gs
} Note;


typedef enum {
    Ei, Q, H, W
} Duration;


typedef struct {
    Note note;
    Duration duration;
} PlayingNote;


PlayingNote twinkle_twinkle[] = {
        {A, Q},
        {A, Q},
        {Ee, Q},
        {Ee, Q},
        {Fs, Q},
        {Fs, Q},
        {Ee, H},
        {D, Q},
        {D, Q},
        {Cs, Q},
        {Cs, Q},
        {B, Q},
        {B, Q},
        {A, H},
        {Ee, Q},
        {Ee, Q},
        {D, Q},
        {D, Q},
        {Cs, Q},
        {Cs, Q},
        {B, H},
        {Ee, Q},
        {Ee, Q},
        {D, Q},
        {D, Q},
        {Cs, Q},
        {Cs, Q},
        {B, H},
        {A, Q},
        {A, Q},
        {Ee, Q},
        {Ee, Q},
        {Fs, Q},
        {Fs, Q},
        {Ee, H},
        {D, Q},
        {D, Q},
        {Cs, Q},
        {Cs, Q},
        {B, Q},
        {B, Q},
        {A, H}
};


PlayingNote shooting_stars[] = {
        {Ds, W},
        {Ds, H},
        {Ee, H},
        {B, Q},
        {Gs, Q}
};


PlayingNote amazing_grace[] = {
        {A, Q},
        {D, H},
        {Fs, Ei},
        {D, Ei},
        {Fs, H},
        {Ee, Q},
        {D, H},
        {B, Q},
        {A, H},
        {A, Q},
        {D, H},
        {Fs, Ei},
        {D, Ei},
        {Fs, H},
        {Ee, Q},
        {A, H},
        {Fs, Q},
        {Fs, H},
        {Ee, Q},
        {Ee, H},
        {D, Q},
        {D, H},
        {B, Q},
        {A, H},
        {A, Q},
        {D, H},
        {Fs, Ei},
        {D, Ei},
        {Fs, H},
        {Ee, Q},
        {D, H}
};




char melodies[3][20] = {"twinkle star", "amazing grace", "shooting stars"};


static const int frequencies[12] = {440,466,493,523,554,587,622,659,698,739,783, 830};


void pause_song() {
    pauseFlag = 1;
}


void play_note(const PlayingNote* note) {
    int new_k = get_key();
    if (new_k == 4 || new_k == 2 || new_k == 3) {
        pause_song();
        return;
    }
    else if (new_k == 5) {
        tempFactor = 2;
    }
    else if (new_k == 6) {
        tempFactor = 0.5;
    }
    else if (new_k == 7) {
        tempFactor = 1;
    }
    else if (new_k == 9) {
        freqFactor = 2;
    }
    else if (new_k == 10) {
        freqFactor = 0.5;
    }
    else if (new_k == 11) {
        freqFactor = 1;
    }

    int i;
    unsigned int k;
    int frequency = frequencies[note->note] * freqFactor;
    unsigned int period = (unsigned int) (100000 / frequency);
    k = frequency * (note->duration*tempFactor+1);


    for (i = 0; i < k; i++) {
        SET_BIT(PORTB, 3);
        avr_wait_2(period/2);
        CLR_BIT(PORTB, 3);
        avr_wait_2(period/2);
    }
}


void play_song(const PlayingNote song[], int length) {
    int i;
    for (i = 0; i < length; i++) {
        if(pauseFlag){
            break;
        }
        play_note(&song[i]);
    }
}


int is_pressed(int r, int c)
{
    // all 8 GPIOs to N / C
    DDRC = 0;
    PORTC = 0;


    //set r to "0"
    SET_BIT(DDRC, r);
    CLR_BIT(PORTC, r);


    // set c to "1"
    CLR_BIT(DDRC, c+4);
    SET_BIT(PORTC, c+4);


    avr_wait(1);


    if (!GET_BIT(PINC, c+4)){
        return 1;
    }
    return 0;
}


int get_key()
{
    int i, j;
    for (i = 0; i < 4; ++i)
    {
        for (j = 0; j < 4; ++j){
            if (is_pressed(i, j)){
                return i*4 + j + 1;
            }
        }
    }
    return 0;
}


void print_song(const char* melody, const char* melodyIndex) {
    lcd_clr();
    char buf1[16];
    lcd_pos(0, 0);
    sprintf(buf1, "%s", melody);
    lcd_puts2(buf1);


    char buf2[16];
    lcd_pos(1, 0);
    sprintf(buf2, "%s", melodyIndex == 0 ? "Options: 1,2,3" : melodyIndex);
    lcd_puts2(buf2);
}


int main() {
    lcd_init();
    avr_init();
    SET_BIT(DDRB, 3);

    const char* message = "Pick a song";
    print_song(message, 0);


    while (1) {
        int k = get_key();
        if (k) {
            pauseFlag = 0;
            switch (k) {
                case 1: // Button 1
                    print_song(melodies[0], "1");
                    play_song(twinkle_twinkle, 42);
                    break;
                case 2: // Button 2
                    print_song(melodies[1], "2");
                    play_song(amazing_grace, 31);
                    break;
                case 3: // Button 3
                    print_song(melodies[2], "3");
                    play_song(shooting_stars, 5);
                    break;
                case 4: // Button A
                    print_song(message, 0);
                    pause_song();
                    break;
                case 5: // Button 4
                    tempFactor = 2;
                    break;
                case 6: // Button 5
                    tempFactor = 0.5;
                    break;
                case 7: // Button 6
                    tempFactor = 1;
                    break;
                case 8: // Button B
                    break;
                case 9: // Button 7
                    freqFactor = 2;
                    break;
                case 10: // Button 8
                    freqFactor = 0.5;
                    break;
                case 11: // Button 9
                    freqFactor = 1;
                    break;
                case 12: // Button C
                    break;
                case 13: // Button *
                    break;
                case 14: // Button 0
                    break;
                case 15: // Button #
                    break;
                case 16: // Button D
                    break;
                default:
                    pause_song();
                    break;
            }
        }


    }
    return 0;
}



