/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/20 22:26:01 by mamartin          #+#    #+#             */
/*   Updated: 2022/10/28 21:34:39 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/mman.h>
#include <stdio.h>

#include "libft_malloc.h"
#include "libft.h"

#include "mem_allocator.h"
#include "utils.h"

t_arena_hdr* g_arenas[3] = {0};

static int init_arena(t_arena_index aridx)
{
	size_t size = (aridx == TINY_ARENA) ? TINY_ARENA_SIZE : SMALL_ARENA_SIZE;

	/* Preallocate a pool of memory */
	g_arenas[aridx] = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (g_arenas[aridx] == MAP_FAILED)
		return -1;

	/* Initialize first chunk */
	t_chunk* chunk = (void*)g_arenas[aridx] + sizeof(t_arena_hdr);
	SETCHUNKSIZE(chunk, size - sizeof(t_arena_hdr) - sizeof(size_t) * 2);
	SETCHUNKSTATE(chunk, FREE);
	chunk->prev = NULL;
	chunk->next = NULL;
	set_chunk_footer(chunk);

	/* Initialize g_arena header */
	g_arenas[aridx]->size = size;
	g_arenas[aridx]->root = chunk;
	return 0;
}

void* malloc(size_t size)
{
	if (!size)
		return NULL;
	size = align(size);
	
	t_arena_index aridx = choose_arena(size);
	if (aridx == LARGE_ARENA)
	{
		// not implemented yet...
		return NULL;
	}
	else
	{
		/* Initialize arena if empty */
		if (!g_arenas[aridx] && init_arena(aridx) == -1)
			return NULL;
		t_arena_hdr* arena = g_arenas[aridx];

		/* Find the first chunk of memory that can fit the request */
		t_chunk *current = arena->root;
		while (current && GETCHUNKSIZE(current->header) < size)
			current = current->next;

		if (!current) // none of the available chunks can satisfy the allocation request
			return NULL; // not implemented yet...
		else // split chunk found to only return the requested size
		{
			if (GETCHUNKSIZE(current->header) - size < MIN_CHUNK_SIZE) // not enough space to perform chunk splitting
				update_freelist(arena, current, current->next, current->prev);
			else // split the chunk to avoid wasting free memory
			{
				t_chunk *splitted = (void *)current + size + sizeof(size_t) * 2;
				*splitted = *current;

				SETCHUNKSIZE(splitted, GETCHUNKSIZE(splitted->header) - (size + sizeof(size_t) * 2));
				set_chunk_footer(splitted);
				SETCHUNKSIZE(current, size);
				update_freelist(arena, current, splitted, splitted);
			}

			SETCHUNKSTATE(current, IN_USE);
			set_chunk_footer(current);
			return (void *)current + sizeof(size_t);
		}
	}
}

void free(void *ptr)
{
	if (!ptr)
		return ;

	t_chunk* freed = ptr - sizeof(size_t);
	if (!IS_ALIGNED(freed, __SIZEOF_POINTER__))
		return ; // pointer is not aligned on 8-bytes so this cannot be one of our chunks
	t_arena_hdr* arena = g_arenas[choose_arena(GETCHUNKSIZE(freed->header))];

	/* Merge the next chunk if it is also free */
	t_chunk* chk = get_next_chunk(freed);
	if (GETCHUNKSTATE(chk->header) == FREE)
		merge_chunks(arena, freed, chk, true);

	/* Also perform the merge with previous chunk if necessary */
	chk = get_previous_chunk(arena, freed);
	if (chk && GETCHUNKSTATE(chk->header) == FREE)
	{
		/*
		** Since we merge the newly freed chunk into an existing one
		** no need to update pointers between chunks
		*/
		merge_chunks(arena, chk, freed, false);
	}
	else
	{
		/* Set chunk as free */
		SETCHUNKSTATE(freed, FREE);
		set_chunk_footer(freed);

		/* Insert freed chunk at the beginning of the freelist */
		freed->next = arena->root;
		if (arena->root)
			arena->root->prev = freed;
		arena->root = freed;
		freed->prev = NULL;
	}
}
