/* Program to print and play checker games.

  Skeleton program written by Artem Polyvyanyy, artem.polyvyanyy@unimelb.edu.au,
  September 2021, with the intention that it be modified by students
  to add functionality, as required by the assignment specification.

  Student Authorship Declaration:

  (1) I certify that except for the code provided in the initial skeleton file,
  the program contained in this submission is completely my own individual
  work, except where explicitly noted by further comments that provide details
  otherwise. I understand that work that has been developed by another student,
  or by me in collaboration with other students, or by non-students as a result
  of request, solicitation, or payment, may not be submitted for assessment in
  this subject. I understand that submitting for assessment work developed by
  or in collaboration with other students or non-students constitutes Academic
  Misconduct, and may be penalized by mark deductions, or by other penalties
  determined via the University of Melbourne Academic Honesty Policy, as
  described at https://academicintegrity.unimelb.edu.au.

  (2) I also certify that I have not provided a copy of this work in either
  softcopy or hardcopy or any other form to any other student, and nor will I
  do so until after the marks are released. I understand that providing my work
  to other students, regardless of my intention or any undertakings made to me
  by that other student, is also Academic Misconduct.

  (3) I further understand that providing a copy of the assignment specification
  to any form of code authoring or assignment tutoring service, or drawing the
  attention of others to such services and code that may have been made
  available via such a service, may be regarded as Student General Misconduct
  (interfering with the teaching activities of the University and/or inciting
  others to commit Academic Misconduct). I understand that an allegation of
  Student General Misconduct may arise regardless of whether or not I personally
  make use of such solutions or sought benefit from such actions.

  Signed by: [Stefan Temelkovski 1270368]
  Dated:     [03/10/21]

*/

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <assert.h>

/* some #define's from my sample solution ------------------------------------*/
#define BOARD_SIZE          8       // board size
#define ROWS_WITH_PIECES    3       // number of initial rows with pieces
#define CELL_EMPTY          '.'     // empty cell character
#define CELL_BPIECE         'b'     // black piece character
#define CELL_WPIECE         'w'     // white piece character
#define CELL_BTOWER         'B'     // black tower character
#define CELL_WTOWER         'W'     // white tower character
#define COST_PIECE          1       // one piece cost
#define COST_TOWER          3       // one tower cost
#define TREE_DEPTH          3       // minimax tree depth
#define COMP_ACTIONS        10     // number of computed actions
/* ---------------------------------------------------------------------------*/

#define WHITE_TURN 0 /* Whites plays on odd turns */
#define BLACK_TURN 1 /* Black plays on odd turns */

#define ROW_SEPERATOR "+---+---+---+---+---+---+---+---+"
#define BOARD_SEPERATOR "====================================="
#define DIVIDER "|"
#define APOSTROPHE 39 /* ASCI code for apostrophe */
#define FIRST_COL 'A' /* Value of first column */

/* one type definition from my sample solution -------------------------------*/
typedef unsigned char board_t[BOARD_SIZE][BOARD_SIZE]; // board type
/* ---------------------------------------------------------------------------*/

typedef struct node node_t;

struct node {
	board_t board;
	node_t *next;
	int cost;
};

typedef struct list {
	node_t *head;
	node_t *foot;
} list_t;

void fill_array(board_t board);
void print_info(board_t board);
int count_pieces(board_t board, char character);
void print_board(board_t board);
void print_input(board_t board);
void copy_board(board_t board, board_t new_board);
void valid_move(board_t board, int oldcol, int oldrow, int newcol,
	int newrow, int turn);
int within_range(int row, int col);
int take_piece(board_t board, int oldcol, int oldrow, int newcol, int newrow);
void move_piece(board_t board, int oldcol, int oldrow, int newcol, int newrow);
void become_tower(board_t board, int oldpos, int newrow, int newcol);
void print_game_info(board_t board, board_t new_board, int turn, int AI);
void action_taken(board_t board, board_t new_board);
int check_winner(board_t board, int turn);
void winner_info(int turn);
list_t* make_empty_list(void);
int possible_moves(board_t board, list_t* parent, int turn, int count);
int save_possible_move(board_t board, int row, int col,
	list_t* parent, int count);
int save_possible_capture(board_t board, int row, int col,
	list_t* parent, int count);
list_t* insert_at_foot(list_t *list, board_t board);
int board_cost(board_t board);
void next_move(board_t board, int turn);
void create_child_list(board_t board, int turn, list_t* parent, int depth);
list_t* get_tail(list_t *list);
void free_list(list_t *list);

/*********************************************************************/

int
main(int argc, char *argv[]) {
    // YOUR IMPLEMENTATION OF STAGES 0-2
    board_t board;
    
    fill_array(board);
    
    /* Print starting info/board */
    print_info(board);
    print_board(board); 
    
    /* Play the game */
    print_input(board);

    return EXIT_SUCCESS;            // exit program with the success code
}

/*********************************************************************/

/* Fill board array with gamepieces */

void
fill_array(board_t board) {
	int row, col;
	
	for(row=0; row<BOARD_SIZE; row++) {
		for(col=0; col<BOARD_SIZE; col++) {
			/* Even rows have white on odd columns */
			if (col%2 == 0 && row%2 == 1 &&
				row<ROWS_WITH_PIECES) {
				board[row][col] = CELL_WPIECE;	
			}
			/* Odd rows have white on even columns */
			else if (col%2 == 1 && row%2 == 0 &&
				row<ROWS_WITH_PIECES) {
				board[row][col] = CELL_WPIECE;	
			}
			/* Even rows have black on odd columns */
			else if (col%2 == 0 && row%2 == 1 
				&& row>=BOARD_SIZE - ROWS_WITH_PIECES) {
				board[row][col] = CELL_BPIECE;	
			}
			/* Odd rows have black on even columns */
			else if (col%2 == 1 && row%2 == 0
				&& row>=BOARD_SIZE - ROWS_WITH_PIECES) {
				board[row][col] = CELL_BPIECE;	
			}
			/* Rest of the positions are empty */
			else {
				board[row][col] = CELL_EMPTY;
			}
		}
	}
}

/*********************************************************************/

/* Display inital info about the game */

void
print_info(board_t board) {
	int wpcs = 0, bpcs = 0;
	
	wpcs = count_pieces(board, CELL_WPIECE);
	bpcs = count_pieces(board, CELL_BPIECE);
	
	printf("BOARD SIZE: %dx%d\n", BOARD_SIZE, BOARD_SIZE);
	printf("#BLACK PIECES: %2d\n", bpcs);
	printf("#WHITE PIECES: %2d\n", wpcs);
		
}

/*********************************************************************/

/* Count how many pieces of a certain type are on the board */

int
count_pieces(board_t board, char character) {
	int row, col;
	int count = 0;

	for(row=0; row<BOARD_SIZE; row++) {
		for(col=0; col<BOARD_SIZE; col++) {
			if (board[row][col] == character) {
				count++;	
			}
		}
	}	
	return count;
}

/*********************************************************************/

/* Format and print board with gamepieces */

void
print_board(board_t board) {
	int row, col;
		
	/* Print coloumn letters */
	for(col=FIRST_COL; col<FIRST_COL+BOARD_SIZE; col++) {
		if (col == FIRST_COL) {
			printf("     %c", col);
		}
		else {
			printf("   %c", col);	
		}
	}
	
	printf("\n   "ROW_SEPERATOR"\n");
	
	/* Print row numbers */
	for(row=0; row<BOARD_SIZE; row++) {
		printf(" %d ", row+1);
		/* Print board pieces */
		for(col=0; col<BOARD_SIZE; col++) {
			printf(DIVIDER);
			printf(" %c ", board[row][col]);
		}
		printf(DIVIDER);
		printf("\n   "ROW_SEPERATOR"\n");
	}
}
	
/*********************************************************************/

/* Read and print input from the user */

void
print_input(board_t board) {
	int oldrow, newpos, oldcol, newcol;
	int turn = 1, game = 0, AI = 0;
	char oldletter, newletter; 
	board_t old_board;
		
	/* Read in input */
	while((scanf(" %c%d-%c%d", 
		&oldletter, &oldrow, &newletter, &newpos)) != EOF) {
		oldcol = oldletter - FIRST_COL;
		newcol = newletter - FIRST_COL;
		
		copy_board(board, old_board);
		
		/* Play AI game */
		if ((oldletter == 'A' || oldletter == 'P') && newletter == ' ') {
			/* Play next move */
			if (oldletter == 'A') {
				/* Only report winner if part of machine game */
				if (check_winner(board,turn)) {
					winner_info(turn);
				}
				next_move(board, turn++);			
			}
			/* Play next ten moves */
			else {
				while(game != COMP_ACTIONS) {
					if (check_winner(board, turn)) {
						winner_info(turn);
						break;		
					}
					next_move(board, turn++);
					game++;
				}
			}
			
			return;
		}
		
		valid_move(board, oldcol, oldrow-1, newcol, newpos-1, turn);
		move_piece(board, oldcol, oldrow-1, newcol, newpos-1);		
		print_game_info(old_board, board, turn, AI);
		turn++;
		
		/* clear last input */
		newletter = ' ';
	}	
}

/*********************************************************************/

/* Copy current board into new board to perform inconsequential actions */

void
copy_board(board_t board, board_t new_board) {
	int row, col;
		
	for(row=0; row<BOARD_SIZE; row++) {
		for(col=0; col<BOARD_SIZE; col++) {	
			new_board[row][col] = board[row][col]; 
		}
	}
}

/*********************************************************************/

/* Check if the move is valid */

void
valid_move(board_t board, int oldcol, int oldrow, int newcol, 
	int newrow, int turn){
	char oldpos = board[oldrow][oldcol];
	char newpos = board[newrow][newcol];

	if (!within_range(oldrow, oldcol)) {
		printf("ERROR: Source cell is outside of the board.\n");
		exit(EXIT_FAILURE); 
	}
	else if (!within_range(newrow, newcol)) {
		printf("ERROR: Target cell is outside of the board.\n");
		exit(EXIT_FAILURE); 
	}
	else if (oldpos == CELL_EMPTY) {
		printf("ERROR: Source cell is empty.\n");	
		exit(EXIT_FAILURE); 
	}
	else if (newpos != CELL_EMPTY) {
		printf("ERROR: Target cell is not empty.\n");
		exit(EXIT_FAILURE); 
	}
	else if ((turn%2 == BLACK_TURN && oldpos == CELL_WPIECE)
		|| (turn%2 == WHITE_TURN && oldpos == CELL_BPIECE)) {
		printf("ERROR: Source cell holds opponent%cs piece/tower.\n"
			, APOSTROPHE);
		exit(EXIT_FAILURE); 
	}
	/* Piece moves within same row/column, moves too far diagonally or moves
	backwards and is not a tower */
	else if (oldpos == CELL_BPIECE && 
		(newrow != oldrow-1 || newcol == oldcol)) {
		/* valid if piece moves two rows, but also makes a capture */
		if(!take_piece(board, oldcol, oldrow, newcol, newrow)){
			printf("ERROR: Illegal action.\n");
			exit(EXIT_FAILURE); 
		}		
	}
	else if (oldpos == CELL_WPIECE && 
		(newrow != oldrow+1 || newcol == oldcol)) {	
		if (!take_piece(board, oldcol, oldrow, newcol, newrow)){
			printf("ERROR: Illegal action.\n");
			exit(EXIT_FAILURE); 		
		}		
	}
	/* Piece moves within same row/column or moves too far diagonally */
	else if ((oldpos == CELL_BTOWER || oldpos == CELL_WTOWER) && 
		(newrow == oldrow || newcol == oldcol || abs(newrow-oldrow) > 1)) {
		if (!take_piece(board, oldcol, oldrow, newcol, newrow)){
			printf("ERROR: Illegal action.\n");
			exit(EXIT_FAILURE); 		
		}		
	}
}
	
/*********************************************************************/

/* Check if input is within the range of the board */

int
within_range(int row, int col) {
	
	if (col<0 || col>BOARD_SIZE-1 
		|| row<0 || row>BOARD_SIZE-1) {
		return 0;
	}
	return 1;
}

/*********************************************************************/

/* Check if move results in a capture and, if so, remove captured piece */

int
take_piece(board_t board, int oldcol, int oldrow, int newcol, int newrow){			
	char oldpos = board[oldrow][oldcol];
	char neast = board[oldrow-1][oldcol+1];
	char nwest = board[oldrow-1][oldcol-1];
	char seast = board[oldrow+1][oldcol+1];
	char swest = board[oldrow+1][oldcol-1];
	
	/* Black piece captures */
	if (oldpos == CELL_BPIECE &&
		(nwest == CELL_WPIECE || nwest == CELL_WTOWER) && oldcol-2 == newcol){
		board[oldrow-1][oldcol-1] = CELL_EMPTY;
	}
	else if (oldpos == CELL_BPIECE &&
		(neast == CELL_WPIECE || neast == CELL_WTOWER) && oldcol+2 == newcol) {
		board[oldrow-1][oldcol+1] = CELL_EMPTY;
	}	
	/* White piece captures */
	else if (oldpos == CELL_WPIECE &&
		(seast == CELL_BPIECE || seast == CELL_BTOWER) && oldcol+2 == newcol) {
		board[oldrow+1][oldcol+1] = CELL_EMPTY;
	}
	else if (oldpos == CELL_WPIECE &&
		(swest == CELL_BPIECE || swest == CELL_BTOWER) && oldcol-2 == newcol) {
		board[oldrow+1][oldcol-1] = CELL_EMPTY;
	}	
	/* Black tower captures */
	else if (oldpos == CELL_BTOWER &&
		(neast == CELL_WPIECE || neast == CELL_WTOWER)
		&& oldcol+2 == newcol && oldrow-2 == newrow) {
		board[oldrow-1][oldcol+1] = CELL_EMPTY;
	}	
	else if (oldpos == CELL_BTOWER &&
		(seast == CELL_WPIECE || seast == CELL_WTOWER) 
		&& oldcol+2 == newcol && oldrow+2 == newrow) {
		board[oldrow+1][oldcol+1] = CELL_EMPTY;
	}
	else if (oldpos == CELL_BTOWER &&
		(swest == CELL_WPIECE || swest == CELL_WTOWER)
		&& oldcol-2 == newcol && oldrow+2 == newrow) {
		board[oldrow+1][oldcol-1] = CELL_EMPTY;
	}	
	else if (oldpos == CELL_BTOWER &&
		(nwest == CELL_WPIECE || nwest == CELL_WTOWER)
		&& oldcol-2 == newcol && oldrow-2 == newrow){
		board[oldrow-1][oldcol-1] = CELL_EMPTY;
	}
	/* White tower captures */
	else if (oldpos == CELL_WTOWER &&
		(neast == CELL_BPIECE || neast == CELL_BTOWER)
		&& oldcol+2 == newcol && oldrow-2 == newrow) {
		board[oldrow-1][oldcol+1] = CELL_EMPTY;
	}	
	else if (oldpos == CELL_WTOWER &&
		(seast == CELL_BPIECE || seast == CELL_BTOWER)
		&& oldcol+2 == newcol && oldrow+2 == newrow) {
		board[oldrow+1][oldcol+1] = CELL_EMPTY;
	}
	else if (oldpos == CELL_WTOWER &&
		(swest == CELL_BPIECE || swest == CELL_BTOWER)
		&& oldcol-2 == newcol && oldrow+2 == newrow) {
		board[oldrow+1][oldcol-1] = CELL_EMPTY;
	}
	else if (oldpos == CELL_WTOWER &&
		(nwest == CELL_BPIECE || nwest == CELL_BTOWER)
		&& oldcol-2 == newcol && oldrow-2 == newrow){
		board[oldrow-1][oldcol-1] = CELL_EMPTY;
	}
	else {
		return 0;	
	}
	return 1;		
}

/*********************************************************************/

/* Move piece based on input and check if it becomes a tower */

void 
move_piece(board_t board, int oldcol, int oldrow, int newcol, int newrow) {
	char oldpos = board[oldrow][oldcol];
	
	become_tower(board, oldpos, newrow, newcol);
	board[oldrow][oldcol] = CELL_EMPTY;
	
}

/*********************************************************************/

/* Check if piece becomes a tower */

void
become_tower(board_t board, int oldpos, int newrow, int newcol) {
	
	/* Reaches top of board */
	if (oldpos == CELL_BPIECE && newrow == 0) {
		board[newrow][newcol] = CELL_BTOWER;		
	}
	/* Reaches bottom of board */
	else if (oldpos == CELL_WPIECE && newrow == BOARD_SIZE-1) {
		board[newrow][newcol] = CELL_WTOWER;		
	}
	/* Does not become a tower */
	else {
		board[newrow][newcol] = oldpos;
	}
}

/*********************************************************************/

/* Print info about each turn taken */

void
print_game_info(board_t board, board_t new_board, int turn, int AI) {

	printf(BOARD_SEPERATOR"\n");

	if (turn%2 == BLACK_TURN) {
		if (AI) {
			printf("*** ");	
		}
		printf("BLACK ACTION #%d: ", turn);
		action_taken(board, new_board);
	}
	else {
		if (AI) {
			printf("*** ");
		}
		printf("WHITE ACTION #%d: ", turn);
		action_taken(board, new_board);
	}	
		
	/* Account for minus character */
	if (board_cost(new_board) < 0) {
		printf("BOARD COST: %2d\n", board_cost(new_board));
	}
	else{
		printf("BOARD COST: %d\n", board_cost(new_board));
	}
	print_board(new_board);
	
}

/*********************************************************************/

/* Display what action was taken in a given a turn */

void
action_taken(board_t board, board_t new_board) {
	int row, col;
	int count = 0, oldrow = 0, newrow = BOARD_SIZE;
	char oldcol = 0, newcol = 0;

		
	for(row=0; row<BOARD_SIZE; row++) {
		for(col=0; col<BOARD_SIZE; col++) {	
			/* Piece has been moved */
			if (board[row][col] != new_board[row][col]) {
				/* Where it moved from */
				if ((new_board[row][col] == CELL_EMPTY && count == 0) ||
					(new_board[row][col] == CELL_EMPTY && row > newrow)){
					oldrow = row+1;
					oldcol = col + FIRST_COL;
					count++;
				}
				/* Where it moved to */
				else {
					newrow = row+1;
					newcol = col + FIRST_COL;		
				}
			}
		}
	}
	printf("%c%d-%c%d\n", oldcol, oldrow, newcol, newrow);
}

/*********************************************************************/

/* Check if current board configuration results in a winner */

int
check_winner(board_t board, int turn) {
	list_t* move;
	int row, col;
	int black = 0, white = 0, count = 0;
	
	move = make_empty_list();
	/* Can the player make a move? */
	count = possible_moves(board, move, ++turn, count);
		
	for(row=0; row<BOARD_SIZE; row++) {
		for(col=0; col<BOARD_SIZE; col++) {	
			/* Check if there are any black or white pieces left on the board */
			if (board[row][col] == CELL_BPIECE ||
				board[row][col] == CELL_BTOWER) {
				black++;
			}	
			if (board[row][col] == CELL_WPIECE ||
				board[row][col] == CELL_WTOWER) {
				white++;
			}				
		}				
	}
	
	free_list(move);
	move = NULL;

	/* If no moves available or black/white pieces left, there is a winner */
	if (white == 0 || black == 0 || count == 0) {
		return 1;	
	}
	
	return 0;
	
}

/*********************************************************************/

/* Print out the winner */

void
winner_info(int turn) {
	
	if (turn%2 == WHITE_TURN) {
		printf("BLACK WINS!\n");	
	}
	else {
		printf("WHITE WINS!\n");
	}
	
}

/*********************************************************************/

/* "make_empty_list" from "listops.c", Alistair Moffat, December 2012 */

list_t
*make_empty_list(void) {
	list_t *list;
	list = (list_t*)malloc(sizeof(*list));
	assert(list!=NULL);
	list->head = list->foot = NULL;
	return list;
}

/*********************************************************************/

/* Determine all possible moves from a given board configuration */

int
possible_moves(board_t board, list_t* list, int turn, int count) {
	int row, col;	
	
	for(row=0; row<BOARD_SIZE; row++) {
		for(col=0; col<BOARD_SIZE; col++) {
			/* Possible moves on black turn */
			if (((board[row][col] == CELL_BPIECE ||
				board[row][col] == CELL_BTOWER) && turn%2==BLACK_TURN)) { 
				count = save_possible_move(board, row, col, list, count);	
				count = save_possible_capture(board, row, col, list, count);		
			}
			/* Possible moves on white turn */
			else if ((board[row][col] == CELL_WPIECE ||
				board[row][col] == CELL_WTOWER) && turn%2==WHITE_TURN) {
				count = save_possible_move(board, row, col, list, count);		
				count = save_possible_capture(board, row, col, list, count);	
			}
		}
	}
	return count;	
}

/*********************************************************************/

/* Store all possible moves (not including captures) for given board
configuration in list in row major order and from north-east clockwise*/

int
save_possible_move(board_t board, int row, int col, list_t* parent, int count) {
	board_t new_board;
	
	copy_board(board, new_board);
	
	/* North-East */
	if (board[row-1][col+1] == CELL_EMPTY && (board[row][col] == CELL_BPIECE 
		|| board[row][col] == CELL_WTOWER || board[row][col] == CELL_BTOWER)
		&& within_range(row-1, col+1)) {	
	
		move_piece(new_board, col, row, col+1, row-1);
		/* Add to end of list */
		insert_at_foot(parent, new_board);
		/* Reset board to find next move for same piece with the same board */
		copy_board(board, new_board);
		count++;
		
	}		
	/* South-East */
	if (board[row+1][col+1] == CELL_EMPTY && (board[row][col] == CELL_WPIECE 
		|| board[row][col] == CELL_WTOWER || board[row][col] == CELL_BTOWER)
		&& within_range(row+1, col+1)) {	
	
		move_piece(new_board, col, row, col+1, row+1);
		insert_at_foot(parent, new_board);
		copy_board(board, new_board);
		count++;
		
	}	
	/* South-West */
	if (board[row+1][col-1] == CELL_EMPTY && (board[row][col] == CELL_WPIECE 
		|| board[row][col] == CELL_WTOWER || board[row][col] == CELL_BTOWER)
		&& within_range(row+1, col-1)) {	
	
		move_piece(new_board, col, row, col-1, row+1);
		insert_at_foot(parent, new_board);
		copy_board(board, new_board);
		count++;
	}	
	/* North-West */
	if (board[row-1][col-1] == CELL_EMPTY && (board[row][col] == CELL_BPIECE 
		|| board[row][col] == CELL_WTOWER || board[row][col] == CELL_BTOWER)
		&& within_range(row-1, col-1)) {	
	
		move_piece(new_board, col, row, col-1, row-1);
		insert_at_foot(parent, new_board);
		copy_board(board, new_board);
		count++;
	}	
	
	/* Return the number of possible moves */
	return count;
		
}

/*********************************************************************/

/* Store all possible captures for given board configuration in list 
in row major order and from north-east clockwise */

int
save_possible_capture(board_t board, int row, int col,
	list_t* parent, int count){
	board_t new_board;
	
	copy_board(board, new_board);
	
	/* North-East */
	if (board[row-2][col+2] == CELL_EMPTY && (board[row][col] == CELL_BPIECE 
		|| board[row][col] == CELL_WTOWER || board[row][col] == CELL_BTOWER)
		&& within_range(row-2, col+2)
		&& take_piece(new_board, col, row, col+2, row-2)) {	
		
	  	move_piece(new_board, col, row, col+2, row-2);  
		/* Add to end of list */
		insert_at_foot(parent, new_board);
		/* Reset board to find next capture
		for same piece with the same board */
		copy_board(board, new_board);
		count++;
		
	}		
	/* South-East */
	if (board[row+2][col+2] == CELL_EMPTY && (board[row][col] == CELL_WPIECE 
		|| board[row][col] == CELL_WTOWER || board[row][col] == CELL_BTOWER)
		&& within_range(row+2, col+2)
		&& take_piece(new_board, col, row, col+2, row+2)) {	
		
		move_piece(new_board, col, row, col+2, row+2); 
		insert_at_foot(parent, new_board);
		copy_board(board, new_board);
		count++;
	}	
	/* South-West */
	if (board[row+2][col-2] == CELL_EMPTY && (board[row][col] == CELL_WPIECE 
		|| board[row][col] == CELL_WTOWER || board[row][col] == CELL_BTOWER)
		&& within_range(row+2, col-2)
		&& take_piece(new_board, col, row, col-2, row+2)) {	
	
		move_piece(new_board, col, row, col-2, row+2); 
		insert_at_foot(parent, new_board);
		copy_board(board, new_board);
		count++;
	}	
	/* North-West */
	if (board[row-2][col-2] == CELL_EMPTY && (board[row][col] == CELL_BPIECE 
		|| board[row][col] == CELL_WTOWER || board[row][col] == CELL_BTOWER)
		&& within_range(row-2, col-2)
		&& take_piece(new_board, col, row, col-2, row-2)) {	
		
		move_piece(new_board, col, row, col-2, row-2); 	
		insert_at_foot(parent, new_board);
		copy_board(board, new_board);
		count++;
	}	
	
	return count;
		
} 

/*********************************************************************/   

/* Modified "inset_at_foot" from "listops.c", Alistair Moffat, December 2012 */

list_t
*insert_at_foot(list_t *list, board_t board) {
	node_t *new;
	new = (node_t*)malloc(sizeof(*new));
	assert(list!=NULL && new!=NULL);
	copy_board(board, new->board);
	new->next = NULL;
	if (list->foot==NULL) {
		/* this is the first insertion into the list */
		list->head = list->foot = new;
	} else {
		list->foot->next = new;
		list->foot = new;
	}
	return list;
}

/*********************************************************************/


/* Calculate cost of the board */

int
board_cost(board_t board) {
	int wpcs = 0, bpcs = 0, btwpcs = 0, wtwpcs = 0;
	
	wpcs = count_pieces(board, CELL_WPIECE);
	bpcs = count_pieces(board, CELL_BPIECE);
	wtwpcs = count_pieces(board, CELL_WTOWER);
	btwpcs = count_pieces(board, CELL_BTOWER);
	
	return bpcs + 3*btwpcs - wpcs - 3*wtwpcs;
	
}

/*********************************************************************/

/* Determine best possible next move using minimax tree */

void
next_move(board_t board, int turn){
	list_t* parents;
	int count = 0, cost = 0, i = 0, depth = 1, AI = 1;
	int highcost = INT_MIN, lowcost = INT_MAX;
		
	/* Store all possible moves in list */
	parents = make_empty_list();
	count = possible_moves(board, parents, turn, count);
	
	/* Create list of potential moves for a specific next move (parent) */
	for(i=0; i<count; i++) {			
		create_child_list(parents->head->board, turn, parents, depth);
		cost = parents->head->cost;
		/* Store highest(lowest) potential board cost for black(white) */
		if (turn%2==BLACK_TURN && cost >= highcost) {
			highcost = cost;	
		}
		else if (turn%2==WHITE_TURN && cost <= lowcost){
			lowcost = cost;
		}
		get_tail(parents);
	}
	
	free_list(parents);
	parents = NULL;
	
	parents = make_empty_list();
	possible_moves(board, parents, turn, count);	
	
	for(i=0; i<count; i++) {
		create_child_list(parents->head->board, turn, parents, depth);
		cost = parents->head->cost;
		/* Find first parent in list that results in highest/lowest board cost
		at a given depth */
		if ((turn%2==BLACK_TURN && cost == highcost) ||
			(turn%2==WHITE_TURN && cost == lowcost)) {
			print_game_info(board, parents->head->board, turn, AI);
			copy_board(parents->head->board, board);
			break;
		}
		get_tail(parents);
	}
	
	free_list(parents);
	parents = NULL;
}

/*********************************************************************/

/* Recursively create branches that contain all possible moves from a given
board condiguration until tree depth is reached */

void
create_child_list(board_t board, int turn, list_t* parent, int depth) {
	list_t* children;
	int i; 
	int count = 0, cost = 0;
	int highcost = INT_MIN, lowcost = INT_MAX;
	
	depth++;
	
	children = make_empty_list();
	count = possible_moves(board, children, ++turn, count);
			
	for(i=0; i<count; i++) {	
		/* If tree depth reached, calculate board cost
		of all possible board configurations */
		if (depth == TREE_DEPTH) {	
			/* Board cost for winner becomes INT_MAX(INT_MIN)
			for black(white) */
			if (check_winner(children->head->board,turn)) {
				if (turn%2==BLACK_TURN) {
					cost = INT_MAX;	
				}
				else {
					cost = INT_MIN;	
				}
			}
			else {
				cost = board_cost(children->head->board);	
			}
			if (turn%2==BLACK_TURN && cost >= highcost) {
				highcost = cost;	
			}
			else if (turn%2==WHITE_TURN && cost <= lowcost){
				lowcost = cost;
			}
		}
		else {
			create_child_list(children->head->board, turn, children, depth);
			/* Store highest(lowest) potential board cost for black(white) */
			cost = children->head->cost;
			if (turn%2==BLACK_TURN && cost >= highcost) {
				highcost = cost;	
			}
			else if (turn%2==WHITE_TURN && cost <= lowcost){
				lowcost = cost;
			}	
		}
		get_tail(children);
	}
			
	free_list(children);
	children = NULL;
	
	/* Propogate cost up the tree switching between
	highest(lowest) for black(white) */
	if (turn%2==BLACK_TURN) {
		parent->head->cost = highcost;
	}
	else if (turn%2==WHITE_TURN){
		parent->head->cost = lowcost;
	}
	
} 
	
/*********************************************************************/

/* "get_tail" from "listops.c", Alistair Moffat, December 2012 */

list_t
*get_tail(list_t *list) {
	node_t *oldhead;
	assert(list!=NULL && list->head!=NULL);
	oldhead = list->head;
	list->head = list->head->next;
	if (list->head==NULL) {
		/* the only list node just got deleted */
		list->foot = NULL;
	}
	free(oldhead);
	return list;
}

/*********************************************************************/

/* "free_list" from "listops.c", Alistair Moffat, December 2012 */

void
free_list(list_t *list) {
	node_t *curr, *prev;
	assert(list!=NULL);
	curr = list->head;
	while (curr) {
		prev = curr;
		curr = curr->next;
		free(prev);
	}
	free(list);
}

/*********************************************************************/

/* algorithms are fun */
