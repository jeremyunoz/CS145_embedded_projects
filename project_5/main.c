#include <stdio.h>
#include "lcd.h"
#include "avr.h"


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


typedef struct
{
    int year;
    signed char month;
    signed char day;
    signed char hour;
    unsigned char minute;
    unsigned char second;
} DateTime;


// Pause the alarm
void pause_alarm();

// Check whether the key has been pressed
int is_pressed(int r, int c);

// Return the position of the pressed key
int get_key();

// Initialize the DateTime with the default value
void init_dt(DateTime *dt);

// Initialize the alarm with the default information
void init_alarm(DateTime *dt);

// Check whether the current year is leap-year
int isLeapYear(int year);

// Process time ticking
void advance_dt(DateTime *dt);

// Print date time on the LCD
void print_dt(const DateTime *dt);

// Formatting current date time within bound
void correct_time(DateTime *dt);

// Function to play note
void play_note(const PlayingNote* note, DateTime *dt);

// Function to play song based on its notes
void play_song(const PlayingNote song[], int length, DateTime *dt);

// Function to check if it is the alarm time
void is_alarm(DateTime *dt);

// Function to enter the alarm setting mode
void enter_alarm_mode();

// Function to reset the alarm(s)
void reset_alarms();

// Function to output the corresponding days based on a specific month and year
int getDaysInMonth(int year, int month);

// Function to enter the time zone mode
void enter_time_zone_mode(DateTime *dt);

static volatile	int military_disable = 0; // by default, not using military time
static volatile	int ticking_enable = 0; // by default, not ticking
static volatile int add_alarm = 0; // enable user to add alarm(s)
static volatile int pauseAlarm = 0; // pause the alarm
static int num_alarm = 0; // number of current saved alarms
static int displacement = 0;	// number of hours ahead

static const int frequencies[12] = {110, 116, 123, 131, 138, 147, 155, 165, 174, 185, 196, 207};

DateTime alarmContainer[5]; // store 5 alarms (max 5)

// Alarm Music
PlayingNote shooting_stars[] = {
        {Ds, H},
        {Ds, H},
        {Ee, H},
        {B, H},
        {Gs, H}
};

void pause_alarm() {
    pauseAlarm = 1;
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

void init_dt(DateTime *dt)
{
    dt->year = 2024;
    dt->month = 11;
    dt->day = 3;
    dt->hour = 1;
    dt->minute = 59;
    dt->second = 58;
}

void init_alarm(DateTime *dt)
{
    dt->year = 2024;
    dt->month = 11;
    dt->day = 3;
    dt->hour = 2;
    dt->minute = 0;
    dt->second = 0;
}


int isLeapYear(int year){
    return (year % 4 == 0) && (year % 100 != 0 || year % 400 == 0);
}

void advance_dt(DateTime *dt)
{
    ++dt->second;
    if (dt->second >= 60)
    {
        ++dt->minute;
        dt->second = 0;
    }
    if (dt->minute >= 60)
    {
        ++dt->hour;
        dt->minute = 0;
    }
    if (dt->hour >= 24)
    {
        ++dt->day;
        dt->hour = 0;
    }
    if ((isLeapYear(dt->year) && dt->month == 2 && dt->day == 30) ||
        (!(isLeapYear(dt->year)) && dt->month == 2 && dt->day == 29)||
        ((dt->month == 1 || dt->month == 3 || dt->month == 5 || dt->month == 7 || dt->month == 8 || dt->month == 10 || dt->month == 12) && dt->day >= 32)||
        ((dt->month == 4 || dt->month == 6 || dt->month == 9 || dt->month == 11) && dt->day >= 31))
    {
        ++dt->month;
        dt->day = 1;
    }
    if (dt->month >= 13)
    {
        ++dt->year;
        dt->month = 1;
    }
}

void print_dt(const DateTime *dt)
{
    // print date on top row
    char buf1[16];
    lcd_pos(0, 0);
    sprintf(buf1, "%02d-%02d-%04d",
            dt->month, dt->day, dt->year);
    lcd_puts2(buf1);

    // print time on bottom row
    char buf2[16];
    if(!military_disable) { // enable military time
        sprintf(buf2, "%02d:%02d:%02d",
                dt->hour, dt->minute, dt->second);
    } else { // disable military time
        int non_military_time = dt->hour < 12 ? dt->hour :(dt->hour % 12);
        non_military_time = non_military_time == 0 ? 12 : non_military_time;
        sprintf(buf2, "%02d:%02d:%02d %s",
                non_military_time, dt->minute, dt->second, non_military_time < 12 ? "AM" : "PM");
    }

    lcd_pos(1, 0);
    lcd_puts2(buf2);
}

void correct_time(DateTime *dt){
    if (dt->year < 0) {
        dt->year = 2024;
    }
    if(dt->month > 12){
        dt->month = 1;
    }
    if(((dt->day > 29 && isLeapYear(dt->year))|| (dt->day > 28 && !(isLeapYear(dt->year)))) && dt->month == 2){
        dt->day = 1;
    }
    if ((dt->month == 1 || dt->month == 3 || dt->month == 5 || dt->month == 7 || dt->month == 8 || dt->month == 10 || dt->month == 12) && dt->day > 31){
        dt->day = 1;
    }
    if ((dt->month == 4 || dt->month == 6 || dt->month == 9 || dt->month == 11) && dt->day >= 31){
        dt->day = 1;
    }
    if(dt->minute >= 60) {
        dt->minute = 0;
    }
    if(dt->hour >= 24){
        dt->hour = 0;
    }
    if(dt->second >= 60){
        dt->second = 0;
    }
}

void play_note(const PlayingNote* note, DateTime *dt) {
    int new_k = get_key();
    if (new_k == 12) {
        pause_alarm();
        return;
    }

    int i;
    unsigned int k;
    int frequency = frequencies[note->note] ;
    unsigned int period = (unsigned int) (100000 / frequency);
    k = frequency * (note->duration+1);


    for (i = 0; i < k; i++) {
        SET_BIT(PORTB, 3);
        avr_wait_2(period/2);
        CLR_BIT(PORTB, 3);
        avr_wait_2(period/2);
    }
}

void play_song(const PlayingNote song[], int length, DateTime *dt) {
    int i;
    for (i = 0; i < length; i++) {
        if(pauseAlarm){
            break;
        }
        play_note(&song[i], dt);

        print_dt(dt);
        advance_dt(dt);
    }
}

void is_alarm(DateTime *dt) {
    for (int i=0; i < num_alarm; i++) {
        DateTime curr_alarm = alarmContainer[i];
        if (dt->minute == curr_alarm.minute && dt->hour == curr_alarm.hour && dt->second == curr_alarm.second) {
            while(!pauseAlarm) {
                play_song(shooting_stars, 5, dt);
            }
        }
        pauseAlarm = 0;
    }
}

void enter_alarm_mode() {
    DateTime alarm;

    char buf[16];
    char buf2[16];

    init_alarm(&alarm);
    lcd_clr();
    print_dt(&alarm);

    while (1) {
        avr_wait(250);
        int key = get_key();
        if (key) {
            switch (key) {
                case 1: // Button 1
                    alarm.month++;
                    break;
                case 2: // Button 2
                    alarm.day++;
                    break;
                case 3: // Button 3
                    alarm.year++;
                    break;
                case 5: // Button 4
                    alarm.hour++;
                    break;
                case 6: // Button 5
                    alarm.minute++;
                    break;
                case 7: // Button 6
                    alarm.second++;
                    break;
                case 9: // Button 6
                    alarm.hour--;
                    break;
                case 10: // Button 6
                    alarm.minute--;
                    break;
                case 11: // Button 6
                    alarm.second--;
                    break;
                case 15: // Button #
                    correct_time(&alarm);
                    add_alarm = 1;
                    break;
                default:
                    break;
            }
            lcd_clr();
            print_dt(&alarm);
            avr_wait(100);
        }

        if (add_alarm && num_alarm < 5) {
            alarmContainer[num_alarm] = alarm;
            ++num_alarm;
            lcd_clr();
            lcd_pos(0, 0);
            sprintf(buf, "%01d Alarms", num_alarm);
            lcd_puts2(buf);
            lcd_pos(1, 0);
            sprintf(buf2, "Added");
            lcd_puts2(buf2);
            add_alarm = 0;
            break;
        } else if (add_alarm && num_alarm >= 5) {
            lcd_clr();
            lcd_pos(0, 0);
            sprintf(buf, "Not Added");
            lcd_puts2(buf);
            lcd_pos(1, 0);
            sprintf(buf2, "Reset Alarms?");
            lcd_puts2(buf2);
            add_alarm = 0;
            break;
        }
    }
    avr_wait(1000);	// time to allow message to display
}

void reset_alarms() {
    for (int i = 0; i < 5; i++) {
        alarmContainer[i].year = 0;
        alarmContainer[i].month = 0;
        alarmContainer[i].day = 0;
        alarmContainer[i].hour = 0;
        alarmContainer[i].minute = 0;
        alarmContainer[i].second = 0;
    }
    num_alarm = 0;
}

int getDaysInMonth(int year, int month) {
    switch (month) {
        case 2:
            return isLeapYear(year) ? 29 : 28;
        case 4: case 6: case 9: case 11:
            return 30;
        default:
            return 31;
    }
}

void enter_time_zone_mode(DateTime *dt) {
    int should_exit = 0;
    char buf[16];
    char buf2[16];

    lcd_clr();
    lcd_pos(0, 0);
    sprintf(buf, "New-Y Paris Mos");
    lcd_puts2(buf);
    lcd_pos(1, 0);
    sprintf(buf2, "Select 1 2 3 A");
    lcd_puts2(buf2);

    avr_wait(1000);	// time to allow message to display

    while (1) {
        avr_wait(250);
        int key = get_key();
        if (key) {
            switch (key) {
                case 1: // Button 1 // New York
                    dt->hour -= displacement;
                    displacement = 3;
                    break;
                case 2: // Button 2 // Paris
                    dt->hour -= displacement;
                    displacement = 9;
                    break;
                case 3: // Button 3 // Moscow
                    dt->hour -= displacement;
                    displacement = 10;
                    break;
                case 4: // Button A // Back to the default time zone
                    dt->hour -= displacement;
                    displacement = 0;
                    break;
                default:
                    break;
            }
            should_exit = 1;
            dt->hour += displacement;
        }
        if (should_exit) {
            if (dt->hour > 23) {
                dt->hour -= 24;
                dt->day++;
            }
            if (dt->hour < 0) {
                dt->hour += 24;
                dt->day--;
            }

            if (dt->day > getDaysInMonth(dt->year, dt->month)) {
                dt->day = 1;
                dt->month++;
            }
            if (dt->day < 1) {
                dt->month--;
                if (dt->month < 1) {
                    dt->month = 12;
                    dt->year--;
                }
                dt->day = getDaysInMonth(dt->year, dt->month);
            }

            if (dt->month > 12) {
                dt->month = 1;
                dt->year++;
            }
            if (dt->month < 1) {
                dt->month = 12;
                dt->year--;
            }
            lcd_clr();
            print_dt(dt);
            break;
        }
    }
    avr_wait(1000);	// prevent unnecessary input
}

int main(void)
{
    DateTime dt;

    lcd_init();
    init_dt(&dt);
    SET_BIT(DDRB, 3);

    while (1)
    {
        avr_wait(100);
        int k = get_key();
        if(k) {
            switch(k){
                case 1: // Button 1
                    dt.month++;
                    break;
                case 2: // Button 2
                    dt.day++;
                    break;
                case 3: // Button 3
                    dt.year++;
                    break;
                case 4: // Button A
                    military_disable = !(military_disable);
                    break;
                case 5: // Button 4
                    dt.hour++;
                    break;
                case 6: // Button 5
                    dt.minute++;
                    break;
                case 7: // Button 6
                    dt.second++;
                    break;
                case 8: // Button B
                    ticking_enable = !(ticking_enable);
                    break;
                case 9: // Button 7
                    dt.year--;
                    break;
                case 10: // Button 8
                    ticking_enable = 0;
                    enter_alarm_mode();
                    break;
                case 11: // Button 9
                    reset_alarms();
                    lcd_clr();
                    lcd_pos(0, 0);
                    lcd_puts2("Alarms Reset");
                    avr_wait(500);
                    break;
                case 12: // Button C
                    pause_alarm();
                    break;
                case 13: // Button *
                    enter_time_zone_mode(&dt);
                    break;
                case 15: // Button #
                    correct_time(&dt);
                    break;
                case 16: // Button D
                    ticking_enable = 0;
                    pauseAlarm = 0;
                    init_dt(&dt);
                default:
                    break;
            }
            lcd_clr();
            print_dt(&dt);
            avr_wait(100);
        }

        if (ticking_enable) {
            print_dt(&dt);
            avr_wait(1000); // advance 1 second
            advance_dt(&dt);
            if (num_alarm > 0)
                is_alarm(&dt);
        }
    }
    return 0;
}
