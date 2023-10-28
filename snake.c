#include <curses.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <unistd.h>

#define WIDTH 40
#define HEIGHT 10
#define INITIAL_LEN 3
#define LEFT 1
#define RIGHT -1
#define UP 2
#define DOWN -2

struct SnakeBit
{
    int x;
    int y;
    struct SnakeBit *next;
    int orientation;
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

void draw_snake(struct SnakeWatcher *snake)
{
    struct SnakeBit *temp = snake->head;
    while (temp != NULL)
    {
        mvaddch(temp->y, temp->x, '*');
        temp = temp->next;
    }
    refresh();
}

bool is_collided(struct SnakeBit *snake)
{
    if (snake->x == 0 || snake->x == WIDTH - 1 ||
    snake->y == 0 || snake->y == HEIGHT - 1)
    {
        return true;
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
    if (is_collided(new_head)) return 0;
    new_head->next = snake->head;
    snake->head = new_head;

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
    initscr();
    raw();
    noecho();
    curs_set(0);
    timeout(1000);
    create_table();
    struct SnakeWatcher *snake = malloc(sizeof(struct SnakeWatcher));
    init_snake(snake);
    draw_snake(snake);
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
