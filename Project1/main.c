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

void draw_board( ) {
    system( "cls" );
    for ( int i = 0; i < HEIGHT; i++ ) {
        for ( int j = 0; j < WIDTH; j++ ) {
            if ( i == 0 || i == HEIGHT - 1 || j == 0 || j == WIDTH - 1 ) {
                printf( "#" ); // Parede do tabuleiro
            }
            else if ( i == food.y && j == food.x ) {
                printf( "*" ); // Comida
            }
            else {
                int is_snake = 0;
                for ( int k = 0; k < snake_length; k++ ) {
                    if ( snake[ k ].x == j && snake[ k ].y == i ) {
                        if ( k == 0 ) { // Cabeça da cobra
                            switch ( direction )
                            {
                            case UP:
                                printf( "^" );
                                break;

                            case DOWN:
                                printf( "v" );
                                break;

                            case LEFT:
                                printf( "<" );
                                break;

                            case RIGHT:
                                printf( ">" );
                                break;
                            default:
                                break;
                            }                                                      
                        }
                        else {
                            printf( "o" ); // Corpo da cobra
                        }
                        is_snake = 1;
                        break;
                    }
                }
                if ( !is_snake )
                    printf( " " ); // Espaço vazio
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

    switch ( direction )
    {
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
        if ( snake_length >= max_size ) {
            game_over = WIN;
        }
        generate_food( );
    }
}

void update_direction( ) {
    if ( _kbhit( ) ) {
        char new_dir = _getch( );
        switch ( new_dir )
        {
        case UP:    
        case DOWN:
        case LEFT:     
        case RIGHT:
          if(direction != new_dir )
              direction = new_dir;
        default:
            break;
        }
    }
}

int main( ) {
    srand( time( 0 ) );
    snake[ 0 ].x = WIDTH / 2;
    snake[ 0 ].y = HEIGHT / 2;
    generate_food( );

    while ( !game_over ) {
        draw_board( );
        update_direction( );
        move_snake( );
        eat_food( );
        check_collision( );
        Sleep( 50 );
    }

    switch ( game_over )
    {
    case WIN:
        printf( "Parabéns, voce ganhou! Pontuacao: %d\n" , snake_length - 1 );
        break;
    case LOSE:
        printf( "Game Over! Pontuacao: %d\n" , snake_length - 1 );
        break;
    default:
        break;
    }

   
    Sleep( 10000 );

    return 0;
}
