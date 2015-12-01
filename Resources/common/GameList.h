#ifndef GameList_h
#define GameList_h

#include <stdlib.h>
#include <stdio.h>

#define MAXLEN 80 /* Maximum size in the world of Any string */

typedef enum{I, W, L} GameState;

struct GameNode;

// structure for each node in the list
typedef struct GameNode {
	// game related data
	int fd;	// socket file descriptor for TCP connections
	char* username;	// name of user playing this game
	char* part_word[MAXLEN]; // portion of word guessed by the client
	char* correctWord;	// the correct word that needs to be guessed by the client
	int numOfLives;		// number of lives left to the client
	GameState state;	// the state of the game (either Incomplete, Win, Lost)

	// node related pointers
	struct GameNode *next;	// next node in the list
	struct GameNode *prev;	// previous node in the list
}GameNode;

GameNode *GameNode_create(); // basic node constructor and detailed constructor
GameNode *GameNode_create(int fd, char* username, char* correctWord, int numOfLives);

// structure for whole list
typedef struct GameList {
	int count;			// total number of nodes in the list
	GameNode *first;	// the head of the list
	GameNode *last;		// the tail of the list
}GameList;

GameList *GameList_create();	// basic list constructor
void GameList_destroy(GameList *list);	// basic list destructor
void GameList_clear(GameList *list);	// empties the list
void GameList_clear_destroy(GameList *list); // empties the list and destroys it

// macro functions for getting count
#define GameList_count(A) ((A)->count)


void GameList_push(GameList *list, GameNode *node); // add a GameNode at last position
GameNode* GameList_pop(GameList *list);	// remove last element

void GameList_insertAtHead(GameList *list, GameNode *node);
GameNode* GameList_removeHead(GameList *list);

GameNode* GameList_remove(GameList *list, GameNode *node); // remove a particlar node

// SEARCH functions //
// search a game node by looking at file descriptor
GameNode* GameList_searchGameNodeByFd(GameList *list, int fd);
// search a game node by looking at username
GameNode* GameList_searchGameNodeByUsername(GameList *list, char* username);

#endif