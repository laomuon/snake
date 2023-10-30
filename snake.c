#include <curses.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <unistd.h>
#include <time.h>

#define WIDTH 40
#define HEIGHT 10
#define TOP_LEFT_X (COLS - WIDTH)/2
#define TOP_LEFT_Y (LINES - HEIGHT)/2
#define INITIAL_LEN 3
#define LEFT 1
#define RIGHT -1
#define UP 2
#define DOWN -2

int food_posx, food_posy;
int score=0;

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
    int orientation;
};

WINDOW *create_window(int height, int width, int top_left_y, int top_left_x)
{
    WINDOW *local_win;

    local_win = newwin(height, width, top_left_y, top_left_x);
    wborder(local_win, '|', '|', '-', '-', '+', '+', '+', '+');
    wrefresh(local_win);

    return local_win;
}

void init_snake(struct SnakeWatcher *snake)
{
    struct SnakeBit *head = malloc(sizeof(struct SnakeBit));
    head->x = WIDTH/2;
    head->y = HEIGHT/2;
    head->next = NULL;
    snake->head = head;
    snake->tail = head;
    snake->orientation = RIGHT;
    for (int i = 0; i < INITIAL_LEN - 1; i++)
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

void draw_food(WINDOW *win)
{
    generate_food_position();
    mvwaddch(win, food_posy, food_posx, 'k');
    wrefresh(win);
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

bool is_food_eaten(struct SnakeWatcher *snake)
{
    return ((snake->head->x == food_posx) && (snake->head->y == food_posy));
}

void draw_snake(struct SnakeWatcher *snake, WINDOW *win)
{
    struct SnakeBit *temp = snake->head;
    while (temp != NULL)
    {
        mvwaddch(win, temp->y, temp->x, '*');
        temp = temp->next;
    }
    if (is_food_eaten(snake)) 
    {
        draw_food(win);
        score++;
    }
    wrefresh(win);
}

int move_snake(struct SnakeWatcher *snake, int orientation, WINDOW* win)
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

    if (!is_food_eaten(snake))
    {
        mvwaddch(win, snake->tail->y, snake->tail->x, ' ');
        wrefresh(win);
        struct SnakeBit *temp = snake->head;
        while (temp->next->next != NULL)
        {
            temp = temp->next;
        }
        temp->next = NULL;
        snake->tail = temp;
    }

    return 1;
}

void update_score(WINDOW* window)
{
    mvwprintw(window, 1, 1, "Score: %d", score);
    wrefresh(window);
}

int main(int argc, char *argv[])
{
    WINDOW *snake_window, *score_window;
    srand(time(NULL));
    initscr();
    raw();
    noecho();
    curs_set(0);
    timeout(1000);
    refresh();
    snake_window = create_window(HEIGHT, WIDTH, TOP_LEFT_Y, TOP_LEFT_X);
    score_window = create_window(3, WIDTH, TOP_LEFT_Y+HEIGHT-1, TOP_LEFT_X);
    struct SnakeWatcher *snake = malloc(sizeof(struct SnakeWatcher));
    init_snake(snake);
    draw_snake(snake, snake_window);
    draw_food(snake_window);
    update_score(score_window);
    while (true)
    {
        int c;
        c = getch();
        if (c == 'q') break;
        int ret;
        switch (c) {
            case 'h':
                ret = move_snake(snake, LEFT, snake_window);
                break;
            case 'l':
                ret =  move_snake(snake, RIGHT, snake_window);
                break;
            case 'j':
                ret = move_snake(snake, DOWN, snake_window);
                break;
            case 'k':
                ret = move_snake(snake, UP, snake_window);
                break;
            default:
                ret = move_snake(snake, snake->orientation, snake_window);
        }
        if (!ret) break;
        draw_snake(snake, snake_window);
        update_score(score_window);
    }
    endwin();
    return 0;
}
