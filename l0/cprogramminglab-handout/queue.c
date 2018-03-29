/* 
 * Code for basic C skills diagnostic.
 * Developed for courses 15-213/18-213/15-513 by R. E. Bryant, 2017
 */

/*
 * This program implements a queue supporting both FIFO and LIFO
 * operations.
 *
 * It uses a singly-linked list to represent the set of queue elements
 */

#include <stdlib.h>
#include <stdio.h>

#include "harness.h"
#include "queue.h"

/*
  Create empty queue.
  Return NULL if could not allocate space.
*/
queue_t *q_new()
{
    queue_t *q =  malloc(sizeof(queue_t));
    /* What if malloc returned NULL? */
    if(q==NULL) return NULL;
    q->head = NULL;
    q->size=0;
    q->tail=q->head;
    return q;
}

/* Free all storage used by queue */
void q_free(queue_t *q)
{
    /* How about freeing the list elements? */
    if(q==NULL) return ;              //当空链表时，错误释放，You dereferenced a NULL or invalid pointer

    list_ele_t *p;
    while(q->head!=NULL){
      p=q->head;
      q->head=p->next;
      free(p);
      q->size--;
    }
    /* Free queue structure */
    free(q);
}

/*
  Attempt to insert element at head of queue.
  Return true if successful.
  Return false if q is NULL or could not allocate space.
 */
bool q_insert_head(queue_t *q, int v)
{
    if(q==NULL) return 0;
    list_ele_t *newh;
    /* What should you do if the q is NULL? */
    newh = malloc(sizeof(list_ele_t));
    /* What if malloc returned NULL? */

    if(newh==NULL) return 0;
    q->size++;
     newh->value = v;
    newh->next = q->head;
    q->head = newh;
    if(q->size==1) q->tail=newh;
    return true;
}


/*
  Attempt to insert element at tail of queue.
  Return true if successful.
  Return false if q is NULL or could not allocate space.
 */
bool q_insert_tail(queue_t *q, int v)
{
    /* You need to write the complete code for this function */
    /* Remember: It should operate in O(1) time */

    if(q==NULL) return 0;
    list_ele_t *newh;
    newh = malloc(sizeof(list_ele_t));
    if(newh==NULL) return false;
    q->size++;
    newh->value = v;
    newh->next=NULL;
    if(q->size==1){
      q->tail=newh;
      q->head=newh;
    }else{
    q->tail->next=newh;
    q->tail=newh;
  }
return 1;
}

/*
  Attempt to remove element from head of queue.
  Return true if successful.
  Return false if queue is NULL or empty.
  If vp non-NULL and element removed, store removed value at *vp.
  Any unused storage should be freed
*/
bool q_remove_head(queue_t *q, int *vp)
{
    /* You need to fix up this code. */
    list_ele_t *p;
    if(q==NULL||q->head==NULL||vp==NULL) return 0;      //要是给的参数地址不存在
    p=q->head;
    *vp=p->value;
    q->head = p->next;
    free(p);
    q->size--;
    return true;
}

/*
  Return number of elements in queue.
  Return 0 if q is NULL or empty
 */
int q_size(queue_t *q)
{
    /* You need to write the code for this function */
    /* Remember: It should operate in O(1) time */
  if(q==NULL||q->head==NULL) return 0;
    return q->size;
}

/*
  Reverse elements in queue.

  Your implementation must not allocate or free any elements (e.g., by
  calling q_insert_head or q_remove_head).  Instead, it should modify
  the pointers in the existing data structure.
 */
void q_reverse(queue_t *q)
{
    if(q==NULL||q->head==NULL) return ;         //q->head==NULL漏写，错误引用
    /* You need to write the code for this function */
    list_ele_t *s,*p,*t;      //指针指向时要防止断链
    p=q->head;
    q->head=q->tail;
    q->tail=p;
    t=p->next;
    while(t!=q->head&&t!=NULL){
      s=t->next;
      t->next=p;
      p=t;
      t=s;
    }
    q->head->next=p;
    q->tail->next=NULL;
}

