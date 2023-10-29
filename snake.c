#include <curses.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <unistd.h>
#include <time.h>

#define WIDTH 40
#define HEIGHT 10
#define INITIAL_LEN 3
#define LEFT 1
#define RIGHT -1
#define UP 2
#define DOWN -2

int food_posx, food_posy;

struct SnakeBit
{
    int x;
    int y;
    struct SnakeBit *next;
};

struct SnakeWatcher
{
    struct SnakeBit *head;
    struct SnakeBit *tail;
    int len;
    int orientation;
};

void create_table(void)
{
    for (int i = 0; i < HEIGHT; i++ )
    {
        for(int j = 0; j < WIDTH; j++)
        {
            if (j == 0 || j == WIDTH - 1)
            {
                mvaddch(i, j, '#');
                continue;
            }
            if (i == 0 || i == HEIGHT - 1)
            {
                mvaddch(i, j, '#');
                continue;
            }
        }
    }
}

void init_snake(struct SnakeWatcher *snake)
{
    struct SnakeBit *head = malloc(sizeof(struct SnakeBit));
    head->x = WIDTH/2;
    head->y = HEIGHT/2;
    head->next = NULL;
    snake->head = head;
    snake->tail = head;
    snake->len = INITIAL_LEN;
    snake->orientation = RIGHT;
    for (int i = 0; i < snake->len - 1; i++)
    {
        struct SnakeBit * temp = malloc(sizeof(struct SnakeBit));
        temp->x = snake->tail->x -1;
        temp->y = snake->tail->y;
        temp->next = NULL;
        snake->tail->next = temp;
        snake->tail = temp;
    }
}


int _generate_random_in_range(int upper, int lower)
{
    return (rand() % (upper-lower+1)) + lower;
}
void generate_food_position(void)
{
    food_posx = _generate_random_in_range(WIDTH - 2, 1);
    food_posy = _generate_random_in_range(HEIGHT - 2, 1);
}

void draw_food(void)
{
    generate_food_position();
    mvaddch(food_posy, food_posx, 'k');
}
void draw_snake(struct SnakeWatcher *snake)
{
    struct SnakeBit *temp = snake->head;
    while (temp != NULL)
    {
        mvaddch(temp->y, temp->x, '*');
        temp = temp->next;
    }
    if ((snake->head->x == food_posx) && (snake->head->y == food_posy)) draw_food();
    refresh();
}

bool is_collided(struct SnakeWatcher *snake)
{
    struct SnakeBit *temp = snake->head->next;
    if (temp->x == 0 || temp->x == WIDTH - 1 ||
    temp->y == 0 || temp->y == HEIGHT - 1)
    {
        return true;
    }
    while (temp != NULL)
    {
        if ((temp->x == snake->head->x) && (temp->y == snake->head->y)) return true;
        temp = temp->next;
    }
    return false;
}

int move_snake(struct SnakeWatcher *snake, int orientation)
{
    struct SnakeBit *new_head = malloc(sizeof(struct SnakeBit));
    if ((snake->orientation + orientation) != 0)
    {
        snake->orientation = orientation;
    }
    switch (snake->orientation) {
        case RIGHT:
            new_head->x = snake->head->x + 1;
            new_head->y = snake->head->y;
            break;
        case LEFT:
            new_head->x = snake->head->x - 1;
            new_head->y = snake->head->y;
            break;
        case DOWN:
            new_head->x = snake->head->x;
            new_head->y = snake->head->y + 1;
            break;
        case UP:
            new_head->x = snake->head->x;
            new_head->y = snake->head->y - 1;
            break;
    }
    new_head->next = snake->head;
    snake->head = new_head;
    if (is_collided(snake)) return 0;

    mvaddch(snake->tail->y, snake->tail->x, ' ');
    struct SnakeBit *temp = snake->head;
    while (temp->next->next != NULL)
    {
        temp = temp->next;
    }
    temp->next = NULL;
    snake->tail = temp;

    return 1;
}

int main(int argc, char *argv[])
{
    srand(time(NULL));
    initscr();
    raw();
    noecho();
    curs_set(0);
    timeout(1000);
    create_table();
    struct SnakeWatcher *snake = malloc(sizeof(struct SnakeWatcher));
    init_snake(snake);
    draw_snake(snake);
    draw_food();
    while (true)
    {
        int c;
        c = getch();
        if (c == 'q') break;
        int ret;
        switch (c) {
            case 'h':
                ret = move_snake(snake, LEFT);
                break;
            case 'l':
                ret =  move_snake(snake, RIGHT);
                break;
            case 'j':
                ret = move_snake(snake, DOWN);
                break;
            case 'k':
                ret = move_snake(snake, UP);
                break;
            default:
                ret = move_snake(snake, snake->orientation);
        }
        if (!ret) break;
        draw_snake(snake);
    }
    endwin();
    return 0;
}
