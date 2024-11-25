#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h> 
#include <time.h>  

#define WIDTH 50
#define HEIGHT 50
#define UP 'w'
#define DOWN 's'
#define LEFT 'a'
#define RIGHT 'd'

#define LOSE 1
#define WIN 2

typedef struct {
	int x , y;
} Point;

const int max_size = WIDTH * HEIGHT;

int GlobalScore;

Point snake[ WIDTH * HEIGHT ];
int snake_length = 1;
Point food;
char direction = RIGHT;
int game_over = 0;
int score = 0; // Variável de pontuação


void hideCursor( ) {
	HANDLE hConsole = GetStdHandle( STD_OUTPUT_HANDLE );
	CONSOLE_CURSOR_INFO cursorInfo;

	GetConsoleCursorInfo( hConsole , &cursorInfo );
	cursorInfo.bVisible = FALSE;  // Define a visibilidade como falsa
	SetConsoleCursorInfo( hConsole , &cursorInfo );
}

void draw_board( int BoardWidth, int BoardHeight ) {
	system( "cls" );

	for ( int i = 0; i < BoardHeight; i++ ) {
		for ( int j = 0; j < BoardWidth; j++ ) {
			if ( i == 0 || i == BoardHeight - 1 || j == 0 || j == BoardWidth - 1 ) {
				//Parede
				printf( "\033[0;100m \033[0m" );
			}
			else if ( i == food.y && j == food.x ) {
				//Comida
				printf( "\033[0;101m \033[0m" );
			}
			else {
				int is_snake = 0;
				for ( int k = 0; k < snake_length; k++ ) {
					if ( snake[ k ].x == j && snake[ k ].y == i ) {
						if ( !k ) { // Cabeça da cobra
							printf( "\033[0;42m \033[0m" );
						}
						else {
							printf( "\033[0;102m \033[0m" ); // Corpo da cobra
						}
						is_snake = 1;
						break;
					}
				}
				if ( !is_snake )
					//Espaço em branco
					printf( "\033[0;40m \033[0m" );
			}
		}
		printf( "\n" );
	}
	printf( "Pontuacao: %d\n" , score ); // Exibe a pontuação na tela
}

void generate_food(int BoardWidth, int BoardHeight ) {
	food.x = rand( ) % ( BoardWidth - 2 ) + 1;
	food.y = rand( ) % ( BoardHeight - 2 ) + 1;
}

void move_snake( ) {
	Point prev = snake[ 0 ];
	Point temp;

	switch ( direction ) {
	case UP:
		snake[ 0 ].y--;
		break;
	case DOWN:
		snake[ 0 ].y++;
		break;
	case LEFT:
		snake[ 0 ].x--;
		break;
	case RIGHT:
		snake[ 0 ].x++;
		break;
	default:
		break;
	}

	for ( int i = 1; i < snake_length; i++ ) {
		temp = snake[ i ];
		snake[ i ] = prev;
		prev = temp;
	}
}

void check_collision(int boardwidth, int boardheight ) {
	if ( snake[ 0 ].x == 0 || snake[ 0 ].x == boardwidth - 1 ||
		snake[ 0 ].y == 0 || snake[ 0 ].y == boardheight - 1 ) {
		game_over = LOSE;
	}

	for ( int i = 1; i < snake_length; i++ ) {
		if ( snake[ 0 ].x == snake[ i ].x && snake[ 0 ].y == snake[ i ].y ) {
			game_over = LOSE;
		}
	}
}

void eat_food( int BoardWidth , int BoardHeight ) {
	if ( snake[ 0 ].x == food.x && snake[ 0 ].y == food.y ) {
		snake_length++;
		score += 10; // Incrementa a pontuação
		GlobalScore += 10;
		if ( !( score % 100 ) ) {
			game_over = WIN;
			score = 0;
		}

		generate_food( BoardWidth, BoardHeight );
	}
}

char opposite_direction( char direction ) {
	switch ( direction ) {
	case UP:
		return DOWN;
	case DOWN:
		return UP;
	case LEFT:
		return RIGHT;
	case RIGHT:
		return LEFT;
	}

	return 0;
}

void update_direction( ) {
	if ( _kbhit( ) ) {
		char new_dir = _getch( );
		switch ( new_dir ) {
		case UP:
		case DOWN:
		case LEFT:
		case RIGHT:
			if ( direction != new_dir && new_dir != opposite_direction( direction ) )
				direction = new_dir;
			break;
		default:
			break;
		}
	}
}

int RunGame( float speed, int BoardWidth, int BoardHeight ) {
	snake[ 0 ].x = BoardWidth / 2;
	snake[ 0 ].y = BoardHeight / 2;
	generate_food(BoardWidth, BoardHeight );

	// Controle de tempo
	LARGE_INTEGER frequency , start , end , boardStart , boardEnd;
	QueryPerformanceFrequency( &frequency );

	double frame_duration = 1 / speed; // Duração de cada quadro em segundos
	double board_update_duration = 0.05; 
	QueryPerformanceCounter( &start );
	QueryPerformanceCounter( &boardStart );

	while ( !game_over ) {
		QueryPerformanceCounter( &end );
		QueryPerformanceCounter( &boardEnd );

		double elapsed_time = ( double ) ( end.QuadPart - start.QuadPart ) / frequency.QuadPart;
		double board_elapsed_time = ( double ) ( boardEnd.QuadPart - boardStart.QuadPart ) / frequency.QuadPart;

		// Atualiza o jogo
		if ( elapsed_time >= frame_duration ) {
			start = end;

			update_direction( );
			move_snake( );
			eat_food(BoardWidth, BoardHeight );
			check_collision( BoardWidth, BoardHeight);
		}

		// Atualiza o desenho da board 
		if ( board_elapsed_time >= board_update_duration ) {
			boardStart = boardEnd;
			draw_board(BoardWidth, BoardHeight );
		}

		Sleep( 10 ); 
	}

	switch ( game_over ) {
	case WIN:
		snake_length = 1;
		printf( "Parabens, voce passou de fase! Pontuacao: %d\n" , GlobalScore );
		game_over = 0;

		Sleep( 2500 );

		return 1;
		break;
	case LOSE:
		return 0;
		break;
	default:
		break;
	}

	return 0;
}



int main( ) {

	hideCursor( );
	srand( time( 0 ) );

	if ( !RunGame( 10.f , 40, 25) ) {
		printf( "Game Over! Pontuacao: %d\n" , GlobalScore );
	}
	else if ( !RunGame( 15.f, 30, 25) ) {
		printf( "Game Over! Pontuacao: %d\n" , GlobalScore );
	}
	else if ( !RunGame( 20.f , 20, 20) ) {
		printf( "Game Over! Pontuacao: %d\n" , GlobalScore );
	}
	else if ( !RunGame( 25.f , 15, 15) ) {
		printf( "Game Over! Pontuacao: %d\n" , GlobalScore );
	}
	else {
		system( "cls" );
		printf( "Parabéns, voce ganhou o jogo!\n" );
		Sleep( 10000 );
	}

	return 0;
}
