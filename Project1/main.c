#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h> 
#include <time.h>  
#include <locale.h>

#define MAX_WIDTH 50
#define MAX_HEIGHT 50
#define UP 'w'
#define DOWN 's'
#define LEFT 'a'
#define RIGHT 'd'

#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define PURPLE "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"

int CurrentLevel = 1;
int TotalLevels = 5;

#define LOSE 1
#define WIN 2

typedef struct {
	int x , y;
} Point;

const int max_size = MAX_WIDTH * MAX_HEIGHT;

int GlobalScore;

Point * snake = NULL; 
int snake_capacity = 0; // Capacidade atual da array
int snake_length = 1;  // Tamanho atual da cobra


Point food;
char direction = RIGHT;
int game_over = 0;
int score = 0; // Variável de pontuação

typedef struct {
	float Speed;
	int width;
	int height;
} LevelInfo;

void saveToFile( const char * filename , LevelInfo * data ) {
	FILE * file = fopen( filename , "wb" );
	if ( !file ) {
		perror( "Erro ao abrir o arquivo para escrita" );
		exit( EXIT_FAILURE );
	}

	fwrite( data , sizeof( LevelInfo ) , 1 , file );
	fclose( file );
	printf( "Dados salvos com sucesso no arquivo '%s'.\n" , filename );
}

void readFromFile( const char * filename , LevelInfo * data ) {
	FILE * file = fopen( filename , "rb" );
	if ( !file ) {
		perror( "Erro ao abrir o arquivo para leitura" );
		exit( EXIT_FAILURE );
	}

	fread( data , sizeof( LevelInfo ) , 1 , file );
	fclose( file );
	printf( "Dados lidos com sucesso do arquivo '%s'.\n" , filename );
}

void hideCursor( ) {
	HANDLE hConsole = GetStdHandle( STD_OUTPUT_HANDLE );
	CONSOLE_CURSOR_INFO cursorInfo;

	GetConsoleCursorInfo( hConsole , &cursorInfo );
	cursorInfo.bVisible = FALSE;  // Define a visibilidade como falsa
	SetConsoleCursorInfo( hConsole , &cursorInfo );
}

void UpdateConsoleCursorPosition( ) {
	HANDLE hConsole = GetStdHandle( STD_OUTPUT_HANDLE );
	COORD cursorPosition = { 0, 0 }; // Define o cursor na posição inicial
	SetConsoleCursorPosition( hConsole , cursorPosition ); // Move o cursor
}

void draw_level_info( int currentLevel , int totalLevels ) {
	printf( "Nível: %d | Faltam %d níveis\n" , currentLevel , totalLevels - currentLevel );
}




void draw_board( int BoardWidth , int BoardHeight ) {

	UpdateConsoleCursorPosition( );

	draw_level_info( CurrentLevel , TotalLevels );

	for ( int i = 0; i < BoardHeight; i++ ) {
		for ( int j = 0; j < BoardWidth; j++ ) {
			if ( i == 0 || i == BoardHeight - 1 || j == 0 || j == BoardWidth - 1 ) {
				// Parede
				printf( "\033[0;100m \033[0m" );
			}
			else if ( i == food.y && j == food.x ) {
				// Comida
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
					// Espaço em branco
					printf( "\033[0;47m \033[0m" );
			}
		}
		printf( "\n" );
	}
	printf( "Pontuação: %d/%d\n" , score , CurrentLevel * 100 ); // Exibe a pontuação na tela
}

void generate_food( int BoardWidth , int BoardHeight ) {
	int validPosition = 0;

	while ( !validPosition ) {
		food.x = rand( ) % ( BoardWidth - 2 ) + 1;
		food.y = rand( ) % ( BoardHeight - 2 ) + 1;

		validPosition = 1;
		for ( int i = 0; i < snake_length; i++ ) {
			if ( snake[ i ].x == food.x && snake[ i ].y == food.y ) {
				validPosition = 0;
				break;
			}
		}
	}
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

void check_collision( int boardwidth , int boardheight ) {
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
		char new_dir = tolower(_getch( ));
		switch ( new_dir  ) {
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


void grow_snake_capacity( ) {
	if ( snake_length >= snake_capacity ) {
		snake_capacity = snake_capacity == 0 ? 4 : snake_capacity * 2; // Dobra a capacidade
		snake = ( Point * ) realloc( snake , snake_capacity * sizeof( Point ) );
		if ( !snake ) {
			perror( "Erro ao alocar memória para a cobra" );
			exit( EXIT_FAILURE );
		}
	}
}

void initialize_snake( int BoardWidth , int BoardHeight ) {
	snake_capacity = 4; // Capacidade inicial
	snake_length = 1;
	snake = ( Point * ) malloc( snake_capacity * sizeof( Point ) );
	if ( !snake ) {
		perror( "Erro ao alocar memória para a cobra" );
		exit( EXIT_FAILURE );
	}
	snake[ 0 ].x = BoardWidth / 2;
	snake[ 0 ].y = BoardHeight / 2;
}

void eat_food( int BoardWidth , int BoardHeight ) {
	if ( snake[ 0 ].x == food.x && snake[ 0 ].y == food.y ) {
		grow_snake_capacity( );
		snake_length++;
		score += 10; // Incrementa a pontuação
		GlobalScore += 10;
		if ( !( score % ( CurrentLevel * 100 ) ) ) {
			game_over = WIN;
			score = 0;
		}
		generate_food( BoardWidth , BoardHeight );
	}
}

void free_snake( ) {
	if ( snake ) {
		free( snake );
		snake = NULL;
	}
}

int RunGame( int Level ) {
	LevelInfo level;
	char filename[ 60 ];

	snprintf( filename , sizeof( filename ) , "%s%d.bin" , "fase" , Level );

	readFromFile( filename , &level );

	int BoardWidth , BoardHeight;
	float speed;

	BoardWidth = level.width;
	BoardHeight = level.height;
	speed = level.Speed;

	system( "cls" );
	initialize_snake( BoardWidth , BoardHeight );
	game_over = 0;

	generate_food( BoardWidth , BoardHeight );

	LARGE_INTEGER frequency , start , end , boardStart , boardEnd;
	QueryPerformanceFrequency( &frequency );

	double frame_duration = 1 / speed; // Duração de cada quadro em segundos
	double board_update_duration = 0.01;
	QueryPerformanceCounter( &start );
	QueryPerformanceCounter( &boardStart );

	while ( !game_over ) {
		QueryPerformanceCounter( &end );
		QueryPerformanceCounter( &boardEnd );

		double elapsed_time = ( double ) ( end.QuadPart - start.QuadPart ) / frequency.QuadPart;
		double board_elapsed_time = ( double ) ( boardEnd.QuadPart - boardStart.QuadPart ) / frequency.QuadPart;

		if ( elapsed_time >= frame_duration ) {
			start = end;
			update_direction( );
			move_snake( );
			eat_food( BoardWidth , BoardHeight );
			check_collision( BoardWidth , BoardHeight );
		}

		if ( board_elapsed_time >= board_update_duration ) {
			boardStart = boardEnd;
			draw_board( BoardWidth , BoardHeight );
		}
	}

	free_snake( ); // Libera a memória ao final do jogo

	switch ( game_over ) {
	case WIN:
		system( "cls" );
		printf( "Parabens, voce passou de fase! Pontuacao: %d\n" , GlobalScore );
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




void exibirMenu( int opcao ) {
	UpdateConsoleCursorPosition( );
	printf( "%s==============================\n" , CYAN );
	printf( "         %sJOGO DA COBRINHA\n" , GREEN );
	printf( "%s==============================\n\n" , CYAN );

	printf( "%s%s1. Iniciar Jogo%s\n" , opcao == 0 ? YELLOW : WHITE , opcao == 0 ? "-> " : "   " , RESET );
	printf( "%s%s2. Instruções%s\n" , opcao == 1 ? YELLOW : WHITE , opcao == 1 ? "-> " : "   " , RESET );
	printf( "%s%s3. Sair%s\n" , opcao == 2 ? YELLOW : WHITE , opcao == 2 ? "-> " : "   " , RESET );

	printf( "\n%sUse W/S para navegar e Enter para selecionar.%s\n" , PURPLE , RESET );
}

void exibirInstrucoes( ) {
	system( "clear || cls" );
	printf( "\n%sINSTRUÇÕES DO JOGO\n" , BLUE );
	printf( "=======================\n" );
	printf( "%s- Use W/A/S/D para mover a cobrinha.\n" , WHITE );
	printf( "- Coma as frutas para crescer e ganhar pontos.\n" );
	printf( "- Evite colidir com as paredes ou com o próprio corpo.\n" );
	printf( "%sPressione qualquer tecla para voltar ao menu.%s\n" , GREEN , RESET );
	_getch( ); // Pausa para o usuário ler as instruções
}




void LoseExibirPontuacao( ) {
	system( "cls" );
	CurrentLevel = 1;
	printf( "Game Over! Pontuacao: %d\n" , GlobalScore );
	GlobalScore = 0;
	score = 0;
	Sleep( 5000 );
}

int startgame( ) {
	system( "cls" );

	srand( time( 0 ) );

	for ( int i = CurrentLevel; i <= TotalLevels; i++ ) {
		if ( RunGame( CurrentLevel ) )
			CurrentLevel++;
		else {
			LoseExibirPontuacao( );
			return 0;
		}
	}

	system( "cls" );
	printf( "Parabéns, voce ganhou o jogo!\n" );
	Sleep( 10000 );


	return 0;
}

int main( ) {
	setlocale( LC_ALL , "pt_BR.UTF-8" );
	hideCursor( );
	system( "cls" );

	int opcao = 0;
	char tecla;

	while ( 1 ) {

		exibirMenu( opcao );
		tecla = _getch( );

		switch ( tolower( tecla ) ) {
		case 'w':
			opcao = ( opcao - 1 + 3 ) % 3; // Volta ao último item se ultrapassar o primeiro
			break;
		case 's':
			opcao = ( opcao + 1 ) % 3; // Volta ao primeiro item se ultrapassar o último
			break;
		case '\r': // Enter seleciona a opção
			switch ( opcao ) {
			case 0:
				printf( "\n%sIniciando o jogo...\n%s" , GREEN , RESET );
				Sleep( 2500 );
				startgame( );
				system( "cls" );
				break;
			case 1:
				exibirInstrucoes( );
				system( "cls" );
				break;
			case 2:
				printf( "\n%sSaindo do jogo. Até a próxima!\n%s" , RED , RESET );
				Sleep( 2500 );
				return 0;
				break;
			}
			break;
		}

	}

	return 0;
}



