
#include "mbed.h"
#include "uLCD_4DGL.h"
#include "rtos.h"
#include "SDFileSystem.h"
#include "wave_player.h"
#include <queue>

DigitalIn  up(p9);
DigitalIn  down(p12);
DigitalIn  left(p11);
DigitalIn  right(p13);
DigitalIn  center(p10);
AnalogOut DACout(p18);
SDFileSystem sd(p5, p6, p7, p8, "sd");
wave_player waver(&DACout);

#define snake_green 0x00ff00
#define pink 0xff69b4
uLCD_4DGL uLCD(p28,p27,p30);
int DEFAULT_MAP[32][32] = {
    {0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0},
    {0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0},
    {0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0},
    {0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0},
    {1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1, 1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1},
    {1,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   1},
    {1,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   1},
    {1,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   1},
    {1,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   1},
    {1,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   1},
    {1,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   1},
    {1,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   1},
    {1,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0,   0,   0,   0,   0,   0,   0,   0,   0,   3,   0,   0,   0,   0,   0,   1},
    {1,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   1},
    {1,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   1},
    {1,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   1},
    {1,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   1},
    {1,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   1},
    {1,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   1},
    {1,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   1},
    {1,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   1},
    {1,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   1},
    {1,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   1},
    {1,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 5,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   1},
    {1,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 2,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   1},
    {1,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 2,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   1},
    {1,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 2,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   1},
    {1,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 2,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   1},
    {1,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 6,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   1},
    {1,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   1},
    {1,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   1},
    {1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1, 1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1}
}; //head = 5, body = 2, food = 3, wall = 1, tail = 6

int score = 0;
int playerx = 0;
int playery = 0;
int tailx = 0;
int taily = 0;
double speed = 0.2;
char dir = 'n';
int grow_counter = 0;
bool endgame;
bool startgame;

struct map {
    int status;
    int x, y;
    char direction;
};

void title_screen()
{
    uLCD.background_color(BLACK);
    uLCD.cls();
    //draw brder
    uLCD.filled_rectangle(2, 2, 125, 4, WHITE);
    uLCD.filled_rectangle(125, 4, 123, 125, WHITE);
    uLCD.filled_rectangle(123, 125, 2, 123, WHITE);
    uLCD.filled_rectangle(2, 123, 4, 4, WHITE);
    uLCD.filled_rectangle(6, 6, 8, 8, snake_green);
    uLCD.filled_rectangle(119, 6, 121, 8, snake_green);
    uLCD.filled_rectangle(6, 115, 121, 118, snake_green);
    uLCD.text_width(1);
    uLCD.text_height(1);
    uLCD.color(WHITE);
    uLCD.locate(2,10);
    uLCD.set_font(FONT_8X8);
    uLCD.printf("Hold C to Play");
    uLCD.text_width(2); //4X size text
    uLCD.text_height(4);
    uLCD.color(WHITE);
    uLCD.locate(2,1);
    uLCD.printf("SNAKE");
}

void map_draw(map status)
{
    if(status.status == 1) {
        uLCD.filled_rectangle(status.y*4, status.x*4, status.y*4 + 3, status.x*4 + 3, WHITE);
    } else if (status.status == 2) {
        uLCD.filled_rectangle(status.y*4, status.x*4, status.y*4 + 3, status.x*4 + 3, snake_green);
    } else if (status.status == 5) {
        uLCD.filled_rectangle(status.y*4, status.x*4, status.y*4 + 3, status.x*4 + 3, snake_green);
        uLCD.filled_rectangle(status.y*4,status.x*4, status.y*4+3,status.x*4+1, BLACK);
        uLCD.pixel(status.y*4, status.x*4, pink);
        uLCD.pixel(status.y*4+3, status.x*4, pink);
        uLCD.pixel(status.y*4+1, status.x*4+1, pink);
        uLCD.pixel(status.y*4+2, status.x*4+1, pink);
        uLCD.pixel(status.y*4, status.x*4+3, RED);
        uLCD.pixel(status.y*4+3, status.x*4+3, RED);
    } else if (status.status == 6) {
        uLCD.filled_rectangle(status.y*4, status.x*4, status.y*4 + 3, status.x*4 + 3, snake_green);
        uLCD.filled_rectangle(status.y*4, status.x*4 + 1, status.y*4, status.x*4 + 3, BLACK);
        uLCD.filled_rectangle(status.y*4 + 3, status.x*4 + 1, status.y*4 + 3, status.x*4 + 3, BLACK);
    } else if (status.status == 3) {
        uLCD.filled_rectangle(status.y*4, status.x*4, status.y*4 + 3, status.x*4 + 3, RED);
        uLCD.pixel(status.y*4, status.x*4, BLACK);
        uLCD.pixel(status.y*4 + 3, status.x*4, BLACK);
        uLCD.pixel(status.y*4, status.x*4 + 3, BLACK);
        uLCD.pixel(status.y*4 + 3, status.x*4 + 3, BLACK);
    }
}

void map_init(map * map)
{
    int i;
    for(i=0; i<32; i++) {
        for(int j = 0; j < 32; j++) {
            int index = i*32+j;
            map[index].x = i;
            map[index].y = j;
            switch(DEFAULT_MAP[i][j]) {
                case 1:     // Wall
                    map[index].status = 1;
                    break;
                case 2:     // Snake
                    map[index].status = 2;
                    map[index].direction = 'n';
                    break;
                case 3:     // Food
                    map[index].status = 3;
                    break;
                case 5:     // Snake head
                    map[index].status = 5;
                    playerx = i;
                    playery = j;
                    map[index].direction = 'n';
                    break;
                case 6:     // Snake tail
                    map[index].status = 6;
                    tailx = i;
                    taily = j;
                    map[index].direction = 'n';
                    break;
                default:    // Empty
                    map[index].status = 0;
                    break;
            }
            map_draw(map[index]);
        }
    }
}

void update_score()
{
    uLCD.locate(14,1);
    uLCD.color(BLACK);
    uLCD.printf("%d", score);
    score++;
    uLCD.locate(14,1);
    uLCD.color(snake_green);
    uLCD.printf("%d", score);
    if (speed > .05 && score%3 == 0)
        speed-=0.025;
}

unsigned int m_z=12434,m_w=33254;

unsigned int rnd()
{
    m_z = 36969 * (m_z & 65535) + (m_z >>16);
    m_w = 18000 * (m_w & 65535) + (m_w >>16);
    return ((m_z <<16) + m_w);
}

void generate_food(map * map)
{
    int x, y, index;
    int z1 = rnd()%25;
    int z2 = rnd()%29;
    x = 5 + z1;
    y = 1 + z2;
    index = x*32+y;
    while(map[index].status != 0) {
        z1 = rnd()%25;
        z2 = rnd()%29;
        x = 5 + z2;
        y = 1 + z1;
        index = x*32+y;
    }
    map[index].status = 3;
    uLCD.filled_rectangle(y*4, x*4, y*4 + 3, x*4 + 3, RED);
    uLCD.pixel(y*4, x*4, BLACK);
    uLCD.pixel(y*4 + 3, x, BLACK);
    uLCD.pixel(y*4, x*4 + 3, BLACK);
    uLCD.pixel(y*4 + 3, x*4+ 3, BLACK);
}

void paint_snake(map status)
{
    uLCD.filled_rectangle(status.y*4, status.x*4, status.y*4 + 3, status.x*4 + 3, snake_green);
    if (status.status == 5) {
        if (dir == 'n') {
            uLCD.filled_rectangle(status.y*4,status.x*4, status.y*4+3,status.x*4+1, BLACK);
            uLCD.pixel(status.y*4, status.x*4, pink);
            uLCD.pixel(status.y*4+3, status.x*4, pink);
            uLCD.pixel(status.y*4+1, status.x*4+1, pink);
            uLCD.pixel(status.y*4+2, status.x*4+1, pink);
            uLCD.pixel(status.y*4, status.x*4+3, RED);
            uLCD.pixel(status.y*4+3, status.x*4+3, RED);
        } else if (dir == 's') {
            uLCD.filled_rectangle(status.y*4,status.x*4+2, status.y*4+3,status.x*4+3, BLACK);
            uLCD.pixel(status.y*4, status.x*4+3, pink);
            uLCD.pixel(status.y*4+3, status.x*4+3, pink);
            uLCD.pixel(status.y*4+1, status.x*4+2, pink);
            uLCD.pixel(status.y*4+2, status.x*4+2, pink);
            uLCD.pixel(status.y*4, status.x*4, RED);
            uLCD.pixel(status.y*4+3, status.x*4, RED);
        } else if (dir == 'w') {
            uLCD.filled_rectangle(status.y*4,status.x*4, status.y*4+1,status.x*4+3, BLACK);
            uLCD.pixel(status.y*4, status.x*4, pink);
            uLCD.pixel(status.y*4, status.x*4+3, pink);
            uLCD.pixel(status.y*4+1, status.x*4+1, pink);
            uLCD.pixel(status.y*4+1, status.x*4+2, pink);
            uLCD.pixel(status.y*4+3, status.x*4, RED);
            uLCD.pixel(status.y*4+3, status.x*4+3, RED);
        } else if (dir == 'e') {
            uLCD.filled_rectangle(status.y*4 + 2,status.x*4, status.y*4+3,status.x*4+3, BLACK);
            uLCD.pixel(status.y*4 +3, status.x*4, pink);
            uLCD.pixel(status.y*4 + 3, status.x*4+3, pink);
            uLCD.pixel(status.y*4+2, status.x*4+1, pink);
            uLCD.pixel(status.y*4+2, status.x*4+2, pink);
            uLCD.pixel(status.y*4, status.x*4, RED);
            uLCD.pixel(status.y*4, status.x*4+3, RED);
        }
    } else if (status.status == 6) {
        if (status.direction == 'n') {
            uLCD.filled_rectangle(status.y*4, status.x*4 + 1, status.y*4, status.x*4 + 3, BLACK);
            uLCD.filled_rectangle(status.y*4 + 3, status.x*4 + 1, status.y*4 + 3, status.x*4 + 3, BLACK);
        } else if (status.direction == 's') {
            uLCD.filled_rectangle(status.y*4, status.x*4, status.y*4, status.x*4 + 2, BLACK);
            uLCD.filled_rectangle(status.y*4 + 3, status.x*4, status.y*4 + 3, status.x*4 + 2, BLACK);
        } else if (status.direction == 'w') {
            uLCD.filled_rectangle(status.y*4 + 1, status.x*4, status.y*4 + 3, status.x*4, BLACK);
            uLCD.filled_rectangle(status.y*4 + 1, status.x*4 + 3, status.y*4 + 3, status.x*4 + 3, BLACK);
        } else if (status.direction == 'e') {
            uLCD.filled_rectangle(status.y*4, status.x*4, status.y*4 + 2, status.x*4, BLACK);
            uLCD.filled_rectangle(status.y*4, status.x*4 + 3, status.y*4 + 2, status.x*4 + 3, BLACK);
        }
    }

}
void t_music()
{
    FILE *wave_file;
    switch(grow_counter) {
        case 3:
            wave_file = fopen("/sd/wavfiles/first_grow_converted.wav", "r");
            waver.play(wave_file);
            fclose(wave_file);
            break;
        case 2:
            wave_file = fopen("/sd/wavfiles/second_grow_converted.wav", "r");
            waver.play(wave_file);
            fclose(wave_file);
            break;
        case 1:
            wave_file = fopen("/sd/wavfiles/third_grow_converted.wav", "r");
            waver.play(wave_file);
            fclose(wave_file);
            break;
    }

}

void title_music(void const*args)
{
    int p, q, exit;
    exit=0;
    FILE *wave_file;
    for(p=0; p<2 && exit==0; p++) {
        for(q=0; q<4 && exit==0; q++) {
            if(q==0)
                wave_file = fopen("/sd/wavfiles/split1_converted.wav","r");
            else if(q==1)
                wave_file = fopen("/sd/wavfiles/split2_converted.wav","r");
            else if(q==2)
                wave_file = fopen("/sd/wavfiles/split3_converted.wav","r");
            else if(q==3)
                wave_file = fopen("/sd/wavfiles/split4_converted.wav","r");
            waver.play(wave_file);
            fclose(wave_file);
            if(center==0) {
                exit=1;
                break;
            }
        }
        if(exit==1)
            break;
    }
    while(center==1 && exit==0) {
        for(p=0; p<4 && exit == 0; p++) {
            if(p==0)
                wave_file = fopen("/sd/wavfiles/split5_converted.wav","r");
            else if(p==1)
                wave_file = fopen("/sd/wavfiles/split6_converted.wav","r");
            else if(p==2)
                wave_file = fopen("/sd/wavfiles/split7_converted.wav","r");
            else if(p==3)
                wave_file = fopen("/sd/wavfiles/split8_converted.wav","r");
            waver.play(wave_file);
            fclose(wave_file);
            if(center==0) {
                exit=1;
                break;
            }
        }
    }
    Thread::wait(200);
}

void snake_move(map * map)
{
    int index = playerx*32 + playery;
    map[index].direction = dir;
    map[index].status = 2;
    paint_snake(map[index]);
    if(dir=='n')
        playerx--;
    else if(dir=='w')
        playery--;
    else if(dir == 'e')
        playery++;
    else if(dir == 's')
        playerx++;

    index = playerx*32 + playery;
    if(map[index].status==3) {
        update_score();
        grow_counter = 3;
        generate_food(map);
    }
    if (map[index].status==1 || map[index].status == 2 || map[index].status == 6) {
        endgame = true;
        return;
    }
    map[index].status = 5;
    paint_snake(map[index]);
    //t_music();


    if (grow_counter == 0) {
        index = tailx*32 + taily;
        char temp = map[index].direction;
        map[index].direction = 'x';
        map[index].status = 0;
        uLCD.filled_rectangle(taily*4, tailx*4, taily*4 + 3, tailx*4 + 3, BLACK);
        if(temp=='n')
            tailx--;
        else if(temp=='w')
            taily--;
        else if(temp == 'e')
            taily++;
        else if(temp == 's')
            tailx++;
        index = tailx*32 + taily;
        map[index].status = 6;
        paint_snake(map[index]);
    } else {
        grow_counter--;
    }

    wait(speed);
}

int main()
{
    up.mode(PullUp);
    center.mode(PullUp);
    down.mode(PullUp);
    left.mode(PullUp);
    right.mode(PullUp);
    startgame = true;
    FILE * sound;
    Thread thread(title_music);
    title_screen();
//snake animation
    int x1 = 110, y1 = 50, x2 = 113, y2 = 53; //White Snake
    int x3 = 110, y3 = 50, x4 = 113, y4 = 53; //Black Snake
    while(1) {
        //draw snake

        if (y1 > 20 && x1 == 110) {
            y1--;
            y2--;
            uLCD.filled_rectangle(x1, y1, x2, y2, snake_green);
        } else if (y1 == 20 && x1 > 18) {
            x1--;
            x2--;
            uLCD.filled_rectangle(x1, y1, x2, y2, snake_green);
        } else if (x1 == 18 && y1 < 67) {
            y1++;
            y2++;
            uLCD.filled_rectangle(x1, y1, x2, y2, snake_green);
        } else if (y1 == 67 && x1 < 110) {
            x1++;
            x2++;
            uLCD.filled_rectangle(x1, y1, x2, y2, snake_green);
        }
        if (y1 == 20 && y3 > 20 && x3 == 110) {
            y3--;
            y4--;
            uLCD.filled_rectangle(x3, y3, x4, y4, BLACK);
        } else if (y3 == 20 && x3 > 18) {
            x3--;
            x4--;
            uLCD.filled_rectangle(x3, y3, x4, y4, BLACK);
        } else if (x3 == 18 && y3 < 67) {
            y3++;
            y4++;
            uLCD.filled_rectangle(x3, y3, x4, y4, BLACK);
        } else if (y3 == 67 && x3 < 110) {
            x3++;
            x4++;
            uLCD.filled_rectangle(x3, y3, x4, y4, BLACK);
        }
        wait(.1);
        if (center == 0) {
            startgame = false;
            break;
        }
    }
    map * gamemap;
    gamemap = new map[1024];
Here:
    endgame = false;
    uLCD.cls();
    uLCD.background_color(BLACK);
    uLCD.color(snake_green);
    uLCD.text_width(2);
    uLCD.text_height(2);
    uLCD.locate(0,0);

    uLCD.printf("SNAKE");
    uLCD.text_width(1);
    uLCD.text_height(1);
    uLCD.locate(12,0);
    uLCD.printf("Score");
    uLCD.locate(14,1);
    uLCD.printf("%d", score);
    map_init(gamemap);
    wait(2);
    while(!endgame) {
        snake_move(gamemap);
        if(up==0 && dir != 's')
            dir = 'n';
        else if(down==0 && dir != 'n')
            dir = 's';
        else if(left==0 && dir != 'e')
            dir = 'w';
        else if(right==0 && dir != 'w')
            dir = 'e';
    }
    uLCD.cls();
    uLCD.locate(0,5);
    uLCD.text_width(2);
    uLCD.text_height(2);
    uLCD.color(RED);
    uLCD.printf("GAME OVER");
    sound = fopen("/sd/wavfiles/gameover3_converted.wav", "r");
    waver.play(sound);
    fclose(sound);
    uLCD.color(WHITE);
    uLCD.text_width(1);
    uLCD.text_height(1);
    uLCD.locate(2,9);
    uLCD.printf("Final Score: %d", score);
    uLCD.locate(3,12);
    uLCD.printf("Press to play");
    while(endgame) {
        if (center == 0) {
            score = 0;
            playerx = 0;
            playery = 0;
            tailx = 0;
            taily = 0;
            speed = 0.2;
            dir = 'n';
            grow_counter = 0;
            goto Here;
        }
    }
}
