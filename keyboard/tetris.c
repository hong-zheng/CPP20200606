#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <keyboard.h>

#define FC 5
#define BC 7

#define W 10
#define H 20

struct data {
	int x;
	int y;
};

struct shape {
	int s[5][5];
};

int background[H][W] = {};

struct data t = {.x=5, .y=5};
struct shape shape_arr[7] = {
	{ 0,0,0,0,0, 0,0,1,0,0, 0,1,1,1,0, 0,0,0,0,0, 0,0,0,0,0, },
	{ 0,0,0,0,0, 0,0,1,1,0, 0,1,1,0,0, 0,0,0,0,0, 0,0,0,0,0, },
	{ 0,0,0,0,0, 0,1,1,0,0, 0,0,1,1,0, 0,0,0,0,0, 0,0,0,0,0, },
	{ 0,0,0,0,0, 0,0,1,0,0, 0,0,1,0,0, 0,0,1,1,0, 0,0,0,0,0, },
	{ 0,0,0,0,0, 0,0,1,0,0, 0,0,1,0,0, 0,1,1,0,0, 0,0,0,0,0, },
	{ 0,0,0,0,0, 0,1,1,0,0, 0,1,1,0,0, 0,0,0,0,0, 0,0,0,0,0, },
	{ 0,0,0,0,0, 0,0,1,0,0, 0,0,1,0,0, 0,0,1,0,0, 0,0,0,0,0, }
};

void drap_element(int x, int y, int c) {
	x = x * 2;
	x++;
	y++;
	printf("\033[%d;%dH\033[3%dm\033[4%dm[]\033[?25l\033[0m", y, x, c, c);
	fflush(stdout);
}

void draw_back() {
	int i, j;

	for (i=0; i<H; i++) {
		for (j=0; j<W; j++) {
			drap_element(j, i, BC);
		}
	}
}

void drap_shape(int x, int y, struct shape p, int c) {
	int i, j;
	
	for (i=0; i<5; i++) {
		for (j=0; j<5; j++) {
			if ( p.s[i][j] != 0 ) {
				drap_element(x+j, y+i, c);
			}
		}
	}
}

int can_move(int x, int y, struct shape p) {
	int i, j;
	
	for (i=0; i<5; i++) {
		for (j=0; j<5; j++) {
			if ( p.s[i][j] == 0 ) continue;
			if ( x+j >= W ) return 0;
			if ( x+j < 0 ) return 0;
			if ( y+i >= H ) return 0;
		}
	}

	return 1;
}


void tetris_timer(struct data *t) {
	drap_shape(t->x, t->y, shape_arr[0], BC);
	if ( can_move(t->x, t->y+1, shape_arr[0]) ) {
		t->y++;
		drap_shape(t->x, t->y, shape_arr[0], FC);
	} else {
		t->y = 0;
		t->x = 4;
		drap_shape(t->x, t->y, shape_arr[0], FC);
	}
}

void handler(int s) {
	tetris_timer(&t);
}

void handler_int(int s) {
	printf("\033[?25h");
	recover_keyboard();
	exit(0);
}

struct shape trun_90(struct shape p) {
	struct shape tmp;
	int i, j;

	for (i=0; i<5; i++) {
		for (j=0; j<5; j++) 
			tmp.s[i][j] = p.s[4-j][i];
	}

	return tmp;
}

void tetris(struct data *t) {
	int ch = get_key();
	if ( is_up(ch) ) {
		drap_shape(t->x, t->y, shape_arr[0], BC);
		shape_arr[0] = trun_90(shape_arr[0]);
		if ( can_move(t->x, t->y, shape_arr[0]) == 0 ) {
			shape_arr[0] = trun_90(shape_arr[0]);
			shape_arr[0] = trun_90(shape_arr[0]);
			shape_arr[0] = trun_90(shape_arr[0]);
		}
		drap_shape(t->x, t->y, shape_arr[0], FC);
	} else if ( is_down(ch) ) {
		drap_shape(t->x, t->y, shape_arr[0], BC);
		if ( can_move(t->x, t->y+1, shape_arr[0]) ) {
			t->y++;
			drap_shape(t->x, t->y, shape_arr[0], FC);
		} else {
			drap_shape(t->x, t->y, shape_arr[0], FC);
		}
	} else if ( is_left(ch) ) {
		drap_shape(t->x, t->y, shape_arr[0], BC);
		if ( can_move(t->x-1, t->y, shape_arr[0]) ) {
			t->x--;
			drap_shape(t->x, t->y, shape_arr[0], FC);
		} else {
			drap_shape(t->x, t->y, shape_arr[0], FC);
		}
	} else if ( is_right(ch) ) {
		drap_shape(t->x, t->y, shape_arr[0], BC);
		if ( can_move(t->x+1, t->y, shape_arr[0]) ) {
			t->x++;
			drap_shape(t->x, t->y, shape_arr[0], FC);
		} else {
			drap_shape(t->x, t->y, shape_arr[0], FC);
		}
	}
}

int main( void ) {
	int i;
	signal(SIGALRM, handler);
	signal(SIGINT, handler_int);
	draw_back();

	init_keyboard();
	struct itimerval it;
	it.it_value.tv_sec  = 0;
	it.it_value.tv_usec = 1;
	it.it_interval.tv_sec  = 1;
	it.it_interval.tv_usec = 0;
	setitimer(ITIMER_REAL, &it, NULL);

	for ( ; ; ) {
		tetris(&t);
	}

}

