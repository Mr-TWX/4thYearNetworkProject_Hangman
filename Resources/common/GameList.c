#include "GameList.h"
#include <string.h>

//return pointer to new node in memory
GameNode *GameNode_create()
{
    return calloc(1, sizeof(GameNode));
}

GameNode *GameNode_create(int fd, char* username, char* correctWord, int numOfLives)
{
    GameNode* node = calloc(1, sizeof(GameNode));

    node->fd = fd;
    node->username = username;
    node->correctWord = correctWord;
    node->numOfLives = numOfLives;
    node->state = I;

    return node;
}

// return pointer to new list in memory
GameList *GameList_create()
{
    return calloc(1, sizeof(GameList));
}

// 
void GameList_destroy(GameList *list)
{
    GameNode* cur = NULL;

    for(cur = list->first; cur != NULL; cur = cur->next)  {
        if(cur->prev) {
            free(cur->prev);
        }
    }

    free(list->last);
    free(list);
}

void GameList_clear(GameList *list)
{
    GameNode* cur = NULL;

    for(cur = list->first; cur != NULL; cur = cur->next) {
        cur->fd = 0;
        free(cur->username);
        free(cur->part_Word);
        free(cur->correctWord);
        cur->numOfLives = 0;
    }
}

void GameList_clear_destroy(GameList *list)
{
    GameList_clear(list);
    GameList_destroy(list);
}

// insert a GameNode at last position of the list
void GameList_push(GameList *list, GameNode *node)
{
    //ListNode *node = calloc(1, sizeof(ListNode));
   // check_mem(node);

    //node->value = value;

    // no nodes added yet, insert at head
    if(list->last == NULL) {
        list->first = node;
        list->last = node;
    } else { 
        // not head node, insert at tail 
        list->last->next = node;
        node->prev = list->last;
        list->last = node;
    }

    list->count++;

error:
    return;
}

GameNode *GameList_pop(GameList *list)
{
    GameNode *node = list->last;
    return node != NULL ? GameList_remove(list, node) : NULL;
}

void GameList_insertAtHead(GameList *list, GameNode* node)
{
    //GameNode *node = calloc(1, sizeof(ListNode));
    //check_mem(node);

    //node->value = value;

    if(list->first == NULL) {
        list->first = node;
        list->last = node;
    } else {
        node->next = list->first;
        list->first->prev = node;
        list->first = node;
    }

    list->count++;

error:
    return;
}

void *GameList_removeHead(GameList *list)
{
    GameNode *node = list->first;
    return node != NULL ? GameList_remove(list, node) : NULL;
}

GameNode *GameList_remove(GameList *list, GameNode *node)
{
    GameNode *result = NULL;

    check(list->first && list->last, "List is empty.");
    check(node, "node can't be NULL");

    if(node == list->first && node == list->last) {
        list->first = NULL;
        list->last = NULL;
    } else if(node == list->first) {
        list->first = node->next;
        check(list->first != NULL, "Invalid list, first found NULL.");
        list->first->prev = NULL;
    } else if (node == list->last) {
        list->last = node->prev;
        check(list->last != NULL, "Invalid list, next found NULL.");
        list->last->next = NULL;
    } else {
        GameNode *after = node->next;
        GameNode *before = node->prev;
        after->prev = before;
        before->next = after;
    }

    list->count--;
    //result = node->value;
    free(node);

error:
    return result;
}

// search for a GameNode by looking at file descriptor
// return null if no matching GameNode is found
GameNode* GameList_searchGameNodeByFd(GameList *list, int fd)
{
    GameNode *result = NULL;
    for(result = list->first; result != NULL; result = result->next)
    {
        if(result->fd == fd)
            break;
    }

error:
    return result;
}
// search for a GameNode by looking at username
// return NULL if no matching GameNOde is found
GameNode* GameList_searchGameNodeByUsername(GameList *list, char* username)
{
    GameNode *result = NULL;
    for(result = list->first; result != NULL; result = result->next)
    {
        if(strcmp(result->username, username))
            break;
    }
error:
    return result;
}
