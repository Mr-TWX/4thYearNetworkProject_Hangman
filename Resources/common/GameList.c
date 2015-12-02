#include "GameList.h"
#include <string.h>

//return pointer to new node in memory
GameNode *GameNode_createEmpty()
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
        free(cur->part_word);
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

GameNode *GameList_removeHead(GameList *list)
{
    GameNode *node = list->first;
    return node != NULL ? GameList_remove(list, node) : NULL;
}

GameNode *GameList_remove(GameList *list, GameNode *node)
{
    GameNode *result = NULL;

    if((list->first == NULL) && (list->last == NULL))
        {
            perror("List is empty.");
            return NULL;
        }

    if(node == NULL)
    {
        perror("node can't be NULL");
        return NULL;
    }

    if(node == list->first && node == list->last) {
        list->first = NULL;
        list->last = NULL;
    } else if(node == list->first) {
        list->first = node->next;
        if(list->first == NULL)
        {
            perror("Invalid list, first found NULL.");
            return NULL;
        } 
        list->first->prev = NULL;
    } else if (node == list->last) {
        list->last = node->prev;
        if(list->last == NULL)
        {
            perror("Invalid list, next found NULL.");
            return NULL;
        } 
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
        printf("checking %s against %s\n", result->username, username);
        if(strcmp(result->username, username) == 0)
            break;
    }
error:
    return result;
}

 /*//Uncomment for testing
int main(int argc, char *argv[])
{
    GameList* gList = GameList_create();
    GameNode* gNode = GameNode_create(1, "Tom\0", "apple\0", 4);
    GameList_push(gList, gNode);

    gNode = GameNode_create(2, "John", "carrot", 3);
    GameList_push(gList, gNode);

    for(gNode = gList->first; gNode != NULL; gNode = gNode->next)
    {
        printf("fd %d username %s, word %s, lives %d\n", gNode->fd, gNode->username, gNode->correctWord, gNode->numOfLives);
    }

    GameNode* uNode = GameList_searchGameNodeByUsername(gList, "John\0");
    if(uNode != NULL)
        printf("username retrieved %s\n", uNode->username);
    else
        printf("returned null\n");

    return(0);
}
*/