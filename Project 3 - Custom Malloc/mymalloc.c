//Author Jake Halloran
//CS 0449 Project 3
//Last Edited: 3/20/16

#include<stddef.h>
#include<stdio.h>
#include "mymalloc.h"

static struct node * first; //head of list
static struct node * last; //tail of list
static struct node * current; //current position for nextfit finding
static void * origin_brk; //automatic decrementing variable

//16 byte linked list node
struct node{
	char inUse; //0 if free, 1 if used
	int sizeBytes; //size in bytes of data portion
	struct node *next; //next node, default is null
	struct node *previous; //previous node default is null
};

//Merges nodes with surrounding free space
struct node *merge_node(struct node *a, struct node *b){
	void * reset_brk = (void *)sbrk(0);
	
	//early exits if it can't pull it off
	if(a==NULL || b == NULL){
		return a;
	}
	
	//This stops a wierd seg fualt that happened in like 1 out of every 30 runs
	if(last == NULL){
		sbrk(-(reset_brk-origin_brk));
		current = last = first = NULL;
		return NULL;
	}
	
	//if protection checks are passed, merge the nodes
	a->sizeBytes += b->sizeBytes + sizeof(struct node);
	a->next = b->next;
	if(b == last){
		a = last;
		current = a;
		
	}
	else{
		a->next->previous = a;
	}
	return a; //returns merged amalgamation node
}

//Makes a new node if needed for allocation
void *make_new_node(int size){
	void* temp_void = (void *)(sbrk(size+sizeof(struct node)));
	
	//catches sbrk issues
	if(temp_void==NULL){
		return NULL;
	}
	
	//Sets new pointer to a node reference and initializes the node's value fields
	struct node *temp_node = temp_void;
	temp_node->sizeBytes = size;
	temp_node->inUse = 1;
	temp_node->next = NULL;
	temp_node->previous = NULL;
	return temp_node;
}

//Uses next fit algorithm to return a pointer to the middle of a memory block for user to write to
void *my_nextfit_malloc(int size){
	char flag = 0; //selects when to create a new node
	int temp = 0; //holder for sizes
	struct node *temp_node = NULL; //holds intermediate nodes
	struct node * start_marker; //holds a copy of current for next fit looping
	void * return_ptr; //Pointer for return value to avoid weird pointer math issue
	
	//First node allocation
	if(current==NULL){ 
		first = make_new_node(size); //creates a new node
		
		//if node creation fails, early exit
		if(first==NULL){
			return NULL;
		}
		
		//Set all global references to the first existing node
		current = last = first;
		
		//Initialize all of its data
		current->previous = NULL;
		current->next = NULL;
		flag = 1; //do not create another new node later on
		return_ptr = current; //copy address for pointer math reasons
		origin_brk = return_ptr; //maintain copy of original brk point for resetting later
	}
	//If it is not the first node to be created, start the next fit algorithm loop
	else{
		start_marker = current; //track copy of marker to tell when fully looped
		do{
			
			//If current space is not in use, proceed to check its size
			if(current->inUse == 0){
				//If perfect fit, use this space
				if(current->sizeBytes == size){
					current->inUse = 1;
					flag = 1;
					break;
				}
				//If the space is big enough to hold the new data and another node, use it
				else if(current->sizeBytes > (size+sizeof(struct node))){
					temp = current->sizeBytes - size - sizeof(struct node);
					current->inUse = 1;
					current->sizeBytes = size;
					temp_node = (current+size+sizeof(struct node)); //create a new node from the space of the old
					temp_node->inUse = 0;
					temp_node->previous = current;
					temp_node->next = current->next;
					current->next = temp_node;
					flag = 1;
					break;
				}
			}
			//Reset to the first node if reached the end of the chain
			if(current==last){
				current = first;
			}
			//Continue to the next node otherwise
			else{
				current = current->next;
			}
			
		}while(start_marker!=current); //loop until all the way around the chain
	}
	
	//Create a new node that is not the first node
	if(!flag){
		temp_node = make_new_node(size); //make the new node
		//Initialize all of its data points
		last->next = temp_node;
		temp_node->previous = last;
		last = temp_node; //new node is automatically last by definition
		current = temp_node; //set current position marker to the new node
	}
	//Create a pointer copy of the node address to prevent wierd addressing issue
	return_ptr = current;
	
	//Return pointer to middle of newly allocated node's space
	return (void *)(return_ptr+sizeof(struct node));
}

//Frees a ptr if it points to a valid nextfit node
void my_free(void * ptr){
	//Throw out invalid memory locations
	if(ptr==NULL){
		return;
	}
	
	struct node *freenode = ptr - sizeof(struct node); //move pointer to node before it
	struct node *next = freenode->next; //lazinesss enabler
	struct node *prev = freenode->previous; //too lazy to type out each time
	struct node *temp; //holder node
	void * reset_brk;
	
	//Free the current node
	freenode->inUse = 0;
	
	//If next is also free, merge
	if(next!=NULL&&!next->inUse){
		temp = next->next;
		freenode = merge_node(freenode,next);
		next = temp;
	}
	
	//if previous is also free, merge
	if(prev!=NULL && !prev->inUse){
		temp = prev->previous;
		freenode = merge_node(prev,freenode);
		prev = temp;
	}
	
	//If against break, decrement heap
	if(next == NULL && freenode != NULL){
		sbrk(-(freenode->sizeBytes + sizeof(struct node))); //Decrement to head of this node
		if(first == last&& last == freenode){
			current = first = last = NULL;
		}
		else{
			last = prev;
			if((void *)last==origin_brk){
				first = last = current = NULL;
				return;
			}
			if(last!=NULL){
				last->next=NULL;
			}
			//Another wierd 1 in 10 seg fault preventation
			else{
				reset_brk = (void*)sbrk(0);
				sbrk(-(reset_brk-origin_brk));
				current = first = last =NULL;
				return;
			}
		}
	}
}