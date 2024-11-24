#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h> 
#include <time.h>  

#define WIDTH 50
#define HEIGHT 20
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

void draw_board( ) {
	system( "cls" );
	printf("Pontuação: %d\n", score); // Exibe a pontuação na tela
	for ( int i = 0; i < HEIGHT; i++ ) {
		for ( int j = 0; j < WIDTH; j++ ) {
			if ( i == 0 || i == HEIGHT - 1 || j == 0 || j == WIDTH - 1 ) {
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
}

void generate_food( ) {
	food.x = rand( ) % ( WIDTH - 2 ) + 1;
	food.y = rand( ) % ( HEIGHT - 2 ) + 1;
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

void check_collision( ) {
	if ( snake[ 0 ].x == 0 || snake[ 0 ].x == WIDTH - 1 ||
		snake[ 0 ].y == 0 || snake[ 0 ].y == HEIGHT - 1 ) {
		game_over = LOSE;
	}

	for ( int i = 1; i < snake_length; i++ ) {
		if ( snake[ 0 ].x == snake[ i ].x && snake[ 0 ].y == snake[ i ].y ) {
			game_over = LOSE;
		}
	}
}

void eat_food( ) {
	if ( snake[ 0 ].x == food.x && snake[ 0 ].y == food.y ) {
		snake_length++;
		score += 10; // Incrementa a pontuação
		if ( snake_length >= max_size ) {
			game_over = WIN;
		}
		generate_food( );
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

int main( ) {

	// Cores de texto
   /*
	printf( "\033[0;31mTexto vermelho\n" );
	printf( "\033[0;32mTexto verde\n" );
	printf( "\033[0;33mTexto amarelo\n" );
	printf( "\033[0;34mTexto azul\n" );
	printf( "\033[0;35mTexto magenta\n" );
	printf( "\033[0;36mTexto ciano\n" );
	printf( "\033[0;37mTexto branco\n" );*/

	// Resetar cor
	// printf( "\033\033[0mTexto com cor padrão\n" );



	hideCursor( );
	srand( time( 0 ) );
	snake[ 0 ].x = WIDTH / 2;
	snake[ 0 ].y = HEIGHT / 2;
	generate_food( );

	// Controle de tempo
	LARGE_INTEGER frequency , start , end;
	QueryPerformanceFrequency( &frequency );

	float speed = 10.0f; // Velocidade controlada por variável (quadros por segundo)
	double frame_duration = 1 / speed; // Duração de cada quadro em segundos
	QueryPerformanceCounter( &start );

	while ( !game_over ) {
		QueryPerformanceCounter( &end );
		double elapsed_time = ( double ) ( end.QuadPart - start.QuadPart ) / frequency.QuadPart;

		draw_board( );
		Sleep( 100 );

		if ( elapsed_time >= frame_duration ) {
			start = end;

			update_direction( );
			move_snake( );
			eat_food( );
			check_collision( );
		}
	}

	switch ( game_over ) {
	case WIN:
		printf( "Parabéns, voce ganhou! Pontuacao: %d\n" , score );
		break;
	case LOSE:
		printf( "Game Over! Pontuacao: %d\n" , score );
		break;
	default:
		break;
	}

	Sleep( 10000 );

	return 0;
}
