#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "linked_list.h"
#include "packet.h"

// add to the end of the list
int add_node(linked_list* ls, tcp_packet* packet) {
    if(is_empty(ls)) {
        ls->head = malloc(sizeof(struct node));
        ls->head->p = packet;
        ls->tail = ls->head;
        ls->head->prev = NULL;
    }
    else {
        struct node* newNode = malloc(sizeof(struct node));
        newNode->p = packet;
        newNode->prev = ls->tail;
        ls->tail->next = newNode;
        ls->tail = newNode;
    }
    ls->tail->next = NULL;
    ls->tail->is_resend = 0;
    (ls->size)++;

    gettimeofday(&(ls->tail->time_sent), 0);

    return 0;
}

int is_empty(linked_list* ls) {
    return ls->size == 0;
}

// free all memory 
int delete_list(linked_list* ls) {
    remove_node(ls, ls->size);
    return 0;
}

tcp_packet *get_head(linked_list* TCP_window){
    // if(TCP_window->head == TCP_window->tail){
    //     free(TCP_window->head);
    //     TCP_window->head = NULL;
    //     return
    // }
    return TCP_window-> head -> p;
}


int remove_node(linked_list* TCP_window, int num_nodes) {

    for(int i = 0; i < num_nodes; i++ ){
        if(is_empty(TCP_window)){
            return -1;
        }
        struct node *temp_pointer = TCP_window->head;
        TCP_window->head = temp_pointer->next;
        free(temp_pointer->p); //Commented to run rdt_receiver 
        free(temp_pointer);
        TCP_window->size--;
        if(!is_empty(TCP_window)) {
            TCP_window->head->prev = NULL;
        }
    }
    return 0;
}

int remove_back(linked_list* ls, int num_nodes) {

    for(int i = 0; i < num_nodes; ++i) {
        if(is_empty(ls)) {
            return -1;
        }

        struct node* temp_pointer = ls->tail;
        ls->tail = temp_pointer->prev;

        free(temp_pointer->p);
        free(temp_pointer);
        ls->size--;
        if(!is_empty(ls)) {
            ls->tail->next = NULL;
        }
    }

    return 0;
}

// debug methods
void print(linked_list* ls) {
    printf("List with %d items\n", ls->size);

    struct node* curr = ls->head;
    for(int i = 0; i<ls->size; ++i) {
        tcp_packet* pct = curr->p;
        printf("Packet %d:\n\tSeqNo: %d\n\tLen: %d\n", i, pct->hdr.seqno, pct->hdr.data_size);

        curr = curr->next;
    }
}