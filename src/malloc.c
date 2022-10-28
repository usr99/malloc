/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/20 22:26:01 by mamartin          #+#    #+#             */
/*   Updated: 2022/10/28 17:10:38 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/mman.h>

#include "libft_malloc.h"
#include "libft.h"

#include "mem_allocator.h"
#include "utils.h"

t_arena_hdr* g_arena = NULL;

static int init_g_arena()
{
	/* Preallocate a pool of memory */
	g_arena = mmap(NULL, TINY_ARENA_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (g_arena == MAP_FAILED)
		return -1;

	/* Initialize first chunk */
	t_chunk* chunk = (void*)g_arena + sizeof(t_arena_hdr);
	SETCHUNKSIZE(chunk, TINY_ARENA_SIZE - sizeof(t_arena_hdr) - sizeof(size_t) * 2);
	SETCHUNKSTATE(chunk, FREE);
	chunk->prev = NULL;
	chunk->next = NULL;
	set_chunk_footer(chunk);

	/* Initialize g_arena header */
	g_arena->size = TINY_ARENA_SIZE;
	g_arena->root = chunk;
	return 0;
}

void* malloc(size_t size)
{
	/* Initialize g_arena when called for the first time */
	if (!g_arena && init_g_arena() == -1)
		return NULL;

	if (!size)
		return NULL;
	size = align(size);

	/* Find the first chunk of memory that can fit the request */
	t_chunk* current = g_arena->root;
	while (current && GETCHUNKSIZE(current->header) < size)
		current = current->next;

	if (!current) // none of the available chunks can satisfy the allocation request
		return NULL; // not implemented yet...
	else // split chunk found to only return the requested size
	{
		if (GETCHUNKSIZE(current->header) - size < MIN_CHUNK_SIZE) // not enough space to perform chunk splitting
			update_freelist(current, current->next, current->prev);
		else // split the chunk to avoid wasting free memory
		{
			t_chunk* splitted = (void*)current + size + sizeof(size_t) * 2;
			*splitted = *current;

			SETCHUNKSIZE(splitted, GETCHUNKSIZE(splitted->header) - (size + sizeof(size_t) * 2));
			set_chunk_footer(splitted);
			SETCHUNKSIZE(current, size);
			update_freelist(current, splitted, splitted);
		}

		SETCHUNKSTATE(current, IN_USE);
		set_chunk_footer(current);
		return (void*)current + sizeof(size_t);
	}
}

void free(void *ptr)
{
	if (!ptr)
		return ;

	t_chunk* freed = ptr - sizeof(size_t);
	if (((size_t)freed & (8 - 1)) != 0)
		return ; // pointer is not aligned on 8-bytes so this cannot be one of our chunks

	/* Merge the next chunk if it is also free */
	t_chunk* chk = get_next_chunk(freed);
	if (GETCHUNKSTATE(chk->header) == FREE)
		merge_chunks(freed, chk, true);

	/* Also perform the merge with previous chunk if necessary */
	chk = get_previous_chunk(freed);
	if (chk && GETCHUNKSTATE(chk->header) == FREE)
	{
		/*
		** Since we merge the newly freed chunk into an existing one
		** no need to update pointers between chunks
		*/
		merge_chunks(chk, freed, false);
	}
	else
	{
		/* Set chunk as free */
		SETCHUNKSTATE(freed, FREE);
		set_chunk_footer(freed);

		/* Insert freed chunk at the beginning of the freelist */
		freed->next = g_arena->root;
		if (g_arena->root)
			g_arena->root->prev = freed;
		g_arena->root = freed;
		freed->prev = NULL;
	}
}
