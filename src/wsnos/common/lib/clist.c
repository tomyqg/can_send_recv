/*
 * Copyright (c) 2004, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this clist of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this clist of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

/**
 * \file
 * Linked clist library implementation.
 *
 * \author Adam Dunkels <adam@sics.se>
 *
 */

/**
 * \addtogroup clist
 * @{
 */

#include "clist.h"

#define NULL 0

struct clist {
    struct clist *next;
};

/*---------------------------------------------------------------------------*/
/**
 * Initialize a clist.
 *
 * This function initalizes a clist. The clist will be empty after this
 * function has been called.
 *
 * \param clist The clist to be initialized.
 */
void
clist_init(clist_t clist)
{
    *clist = NULL;
}
/*---------------------------------------------------------------------------*/
/**
 * Get a pointer to the first element of a clist.
 *
 * This function returns a pointer to the first element of the
 * clist. The element will \b not be removed from the clist.
 *
 * \param clist The clist.
 * \return A pointer to the first element on the clist.
 *
 * \sa clist_tail()
 */
void *
clist_head(clist_t clist)
{
    return *clist;
}
/*---------------------------------------------------------------------------*/
/**
 * Duplicate a clist.
 *
 * This function duplicates a clist by copying the clist reference, but
 * not the elements.
 *
 * \note This function does \b not copy the elements of the clist, but
 * merely duplicates the pointer to the first element of the clist.
 *
 * \param dest The destination clist.
 * \param src The source clist.
 */
void
clist_copy(clist_t dest, clist_t src)
{
    *dest = *src;
}
/*---------------------------------------------------------------------------*/
/**
 * Get the tail of a clist.
 *
 * This function returns a pointer to the elements following the first
 * element of a clist. No elements are removed by this function.
 *
 * \param clist The clist
 * \return A pointer to the element after the first element on the clist.
 *
 * \sa clist_head()
 */
void *
clist_tail(clist_t clist)
{
    struct clist *l;
    if (*clist == NULL) {
        return NULL;
    }

    for (l = *clist; l->next != NULL; l = l->next);

    return l;
}
/*---------------------------------------------------------------------------*/
/**
 * Add an item at the end of a clist.
 *
 * This function adds an item to the end of the clist.
 *
 * \param clist The clist.
 * \param item A pointer to the item to be added.
 *
 * \sa clist_push()
 *
 */
void
clist_add_tail(clist_t clist, void *item)
{
    struct clist *l;
    /* Make sure not to add the same element twice */
    clist_remove(clist, item);

    ((struct clist *)item)->next = NULL;

    l = clist_tail(clist);

    if (l == NULL) {
        *clist = item;
    } else {
        l->next = item;
    }
}
/*---------------------------------------------------------------------------*/
/**
 * Add an item to the start of the clist.
 */
void
clist_add_head(clist_t clist, void *item)
{
    /*  struct clist *l;*/
    /* Make sure not to add the same element twice */
    clist_remove(clist, item);

    ((struct clist *)item)->next = *clist;
    *clist = item;
}
/*---------------------------------------------------------------------------*/
/**
 * Remove the last object on the clist.
 *
 * This function removes the last object on the clist and returns it.
 *
 * \param clist The clist
 * \return The removed object
 *
 */
void *
clist_remove_tail(clist_t clist)
{
    struct clist *l, *r;
    if (*clist == NULL) {
        return NULL;
    }
    if (((struct clist *)*clist)->next == NULL) {
        l = *clist;
        *clist = NULL;
        return l;
    }

    for (l = *clist; l->next->next != NULL; l = l->next);

    r = l->next;
    l->next = NULL;

    return r;
}
/*---------------------------------------------------------------------------*/
/**
 * Remove the first object on a clist.
 *
 * This function removes the first object on the clist and returns a
 * pointer to it.
 *
 * \param clist The clist.
 * \return Pointer to the removed element of clist.
 */
/*---------------------------------------------------------------------------*/
void *
clist_remove_head(clist_t clist)
{
    struct clist *l;
    l = *clist;
    if (*clist != NULL) {
        *clist = ((struct clist *)*clist)->next;
    }

    return l;
}
/*---------------------------------------------------------------------------*/
/**
 * Remove a specific element from a clist.
 *
 * This function removes a specified element from the clist.
 *
 * \param clist The clist.
 * \param item The item that is to be removed from the clist.
 *
 */
/*---------------------------------------------------------------------------*/
void
clist_remove(clist_t clist, void *item)
{
    struct clist *l, *r;
    if (*clist == NULL) {
        return;
    }

    r = NULL;
    for (l = *clist; l != NULL; l = l->next) {
        if (l == item) {
            if (r == NULL) {
                /* First on clist */
                *clist = l->next;
            } else {
                /* Not first on clist */
                r->next = l->next;
            }
            l->next = NULL;
            return;
        }
        r = l;
    }
}
/*---------------------------------------------------------------------------*/
/**
 * Get the length of a clist.
 *
 * This function counts the number of elements on a specified clist.
 *
 * \param clist The clist.
 * \return The length of the clist.
 */
/*---------------------------------------------------------------------------*/
int
clist_length(clist_t clist)
{
    struct clist *l;
    int n = 0;
    for (l = *clist; l != NULL; l = l->next) {
        ++n;
    }

    return n;
}
/*---------------------------------------------------------------------------*/
/**
 * \brief      Insert an item after a specified item on the clist
 * \param clist The clist
 * \param previtem The item after which the new item should be inserted
 * \param newitem  The new item that is to be inserted
 * \author     Adam Dunkels
 *
 *             This function inserts an item right after a specified
 *             item on the clist. This function is useful when using
 *             the clist module to ordered clists.
 *
 *             If previtem is NULL, the new item is placed at the
 *             start of the clist.
 *
 */
void
clist_insert(clist_t clist, void *previtem, void *newitem)
{
    if (previtem == NULL) {
        clist_add_head(clist, newitem);
    } else {

        ((struct clist *)newitem)->next = ((struct clist *)previtem)->next;
        ((struct clist *)previtem)->next = newitem;
    }
}
/*---------------------------------------------------------------------------*/
/**
 * \brief      Get the next item following this item
 * \param item A clist item
 * \returns    A next item on the clist
 *
 *             This function takes a clist item and returns the next
 *             item on the clist, or NULL if there are no more items on
 *             the clist. This function is used when iterating through
 *             clists.
 */
void *
clist_item_next(void *item)
{
    return item == NULL ? NULL : ((struct clist *)item)->next;
}
/*---------------------------------------------------------------------------*/
/** @} */
