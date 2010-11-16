/* Copyright (C) 2001  David Decotigny
 
   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.
    
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
    
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
   USA.
*/
#ifndef _SOS_LIST_H_
#define _SOS_LIST_H_

/**
 * @file list.h
 *
 * Circular doubly-linked lists implementation entirely based on C
 * macros
 */


/* *_named are used when next and prev links are not exactly next
   and prev. For instance when we have next_in_team, prev_in_team,
   prev_global and next_global */

#define list_init_named(list,prev,next) \
  ((list) = NULL)

#define list_singleton_named(list,item,prev,next) ({ \
  (item)->next = (item)->prev = (item); \
  (list) = (item); \
})

#define list_is_empty_named(list,prev,next) \
  ((list) == NULL)

#define list_is_singleton_named(list,prev,next) \
  ( ((list) != NULL) && ((list)->prev == (list)->next) && ((list) == (list)->next) )

#define list_get_head_named(list,prev,next) \
  (list)

#define list_get_tail_named(list,prev,next) \
  ((list)?((list)->prev):NULL)

/* Internal macro : insert before the head == insert at tail */
#define __list_insert_atleft_named(before_this,item,prev,next) ({ \
   (before_this)->prev->next = (item); \
   (item)->prev = (before_this)->prev; \
   (before_this)->prev = (item); \
   (item)->next = (before_this); \
})

/* @note Before_this and item are expected to be valid ! */
#define list_insert_before_named(list,before_this,item,prev,next) ({ \
   __list_insert_atleft_named(before_this,item,prev,next); \
   if ((list) == (before_this)) (list) = (item); \
})    

/** @note After_this and item are expected to be valid ! */
#define list_insert_after_named(list,after_this,item,prev,next) ({ \
   (after_this)->next->prev = (item); \
   (item)->next = (after_this)->next; \
   (after_this)->next = (item); \
   (item)->prev = (after_this); \
})

#define list_add_head_named(list,item,prev,next) ({ \
  if (list) \
    list_insert_before_named(list,list,item,prev,next); \
  else \
    list_singleton_named(list,item,prev,next); \
  (list) = (item); \
})

#define list_add_tail_named(list,item,prev,next) ({ \
  if (list) \
    __list_insert_atleft_named(list,item,prev,next); \
  else \
    list_singleton_named(list,item,prev,next); \
})

/** @note NO check whether item really is in list ! */
#define list_delete_named(list,item,prev,next) ({ \
  if ( ((item)->next == (item)) && ((item)->prev == (item)) ) \
    (item)->next = (item)->prev = (list) = NULL; \
  else { \
    (item)->prev->next = (item)->next; \
    (item)->next->prev = (item)->prev; \
    if ((item) == (list)) (list) = (item)->next; \
    (item)->prev = (item)->next = NULL; \
  } \
})

#define list_pop_head_named(list,prev,next) ({ \
  typeof(list) __ret_elt = (list); \
  list_delete_named(list,__ret_elt,prev,next); \
  __ret_elt; })

/** Loop statement that iterates through all of its elements, from
    head to tail */
#define list_foreach_forward_named(list,iterator,nb_elements,prev,next) \
        for (nb_elements=0, (iterator) = (list) ; \
             (iterator) && (!nb_elements || ((iterator) != (list))) ; \
             nb_elements++, (iterator) = (iterator)->next )

/** Loop statement that iterates through all of its elements, from
    tail back to head */
#define list_foreach_backward_named(list,iterator,nb_elements,prev,next) \
        for (nb_elements=0, (iterator) = list_get_tail_named(list,prev,next) ; \
             (iterator) && (!nb_elements || \
               ((iterator) != list_get_tail_named(list,prev,next))) ; \
             nb_elements++, (iterator) = (iterator)->prev )

#define list_foreach_named list_foreach_forward_named

/** True when we exitted early from the foreach loop (ie break) */
#define list_foreach_early_break(list,iterator,nb_elements) \
  ((list) && ( \
    ((list) != (iterator)) || \
    ( ((list) == (iterator)) && (nb_elements == 0)) ))

/** Loop statement that also removes the item at each iteration. The
    body of the loop is allowed to delete the iterator element from
    memory. */
#define list_collapse_named(list,iterator,prev,next) \
        for ( ; ({ ((iterator) = (list)) ; \
                   if (list) list_delete_named(list,iterator,prev,next) ; \
                   (iterator); }) ; )


/*
 * the same macros : assume that the prev and next fields are really
 * named "prev" and "next"
 */

#define list_init(list) \
  list_init_named(list,prev,next)

#define list_singleton(list,item) \
  list_singleton_named(list,item,prev,next)

#define list_is_empty(list) \
  list_is_empty_named(list,prev,next)

#define list_is_singleton(list) \
  list_is_singleton_named(list,prev,next)

#define list_get_head(list) \
  list_get_head_named(list,prev,next) \

#define list_get_tail(list) \
  list_get_tail_named(list,prev,next) \

/* @note Before_this and item are expected to be valid ! */
#define list_insert_after(list,after_this,item) \
  list_insert_after_named(list,after_this,item,prev,next)

/* @note After_this and item are expected to be valid ! */
#define list_insert_before(list,before_this,item) \
  list_insert_before_named(list,before_this,item,prev,next)

#define list_add_head(list,item) \
  list_add_head_named(list,item,prev,next)

#define list_add_tail(list,item) \
  list_add_tail_named(list,item,prev,next)

/* @note NO check whether item really is in list ! */
#define list_delete(list,item) \
  list_delete_named(list,item,prev,next)

#define list_pop_head(list) \
  list_pop_head_named(list,prev,next)

#define list_foreach_forward(list,iterator,nb_elements) \
  list_foreach_forward_named(list,iterator,nb_elements,prev,next)

#define list_foreach_backward(list,iterator,nb_elements) \
  list_foreach_backward_named(list,iterator,nb_elements,prev,next)

#define list_foreach list_foreach_forward

#define list_collapse(list,iterator) \
  list_collapse_named(list,iterator,prev,next)

#endif /* _SOS_LIST_H_ */
