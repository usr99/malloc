/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/20 22:26:01 by mamartin          #+#    #+#             */
/*   Updated: 2022/11/04 19:11:08 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/mman.h>
#include <stdio.h>

#include "libft_malloc.h"
#include "libft.h"

#include "mem_allocator.h"
#include "utils.h"

void* g_arenas[3] = { NULL };

static int init_arena(t_arena_index aridx)
{
	size_t size = (aridx == TINY_ARENA) ? TINY_ARENA_SIZE : SMALL_ARENA_SIZE;

	/* Preallocate a pool of memory */
	g_arenas[aridx] = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (g_arenas[aridx] == MAP_FAILED)
		return -1;

	/* Initialize wilderness chunk */
	t_chunk* chunk = g_arenas[aridx];
	SETSIZE(chunk, size - CHUNK_OVERHEAD);
	chunk->next = NULL;
	chunk->prev = NULL;
	set_chunk_footer(chunk);

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
		size_t allocated = ALIGN(size + sizeof(t_large_chunk), getpagesize());
		t_large_chunk* new = mmap(NULL, allocated, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		if (new == MAP_FAILED)
			return NULL;

		SETSIZE(new, allocated - sizeof(t_large_chunk));
		new->next = NULL;
		new->prev = NULL;

		if (g_arenas[LARGE_ARENA])
		{
			new->next = g_arenas[LARGE_ARENA];
			((t_large_chunk*)g_arenas[LARGE_ARENA])->prev = new;
		}
		g_arenas[LARGE_ARENA] = new;
		return (void *)new + sizeof(t_large_chunk);
	}
	else
	{
		/* Initialize arena if empty */
		if (!g_arenas[aridx] && init_arena(aridx) == -1)
			return NULL;

		/* Find the first chunk of memory that can fit the request */
		t_chunk* current = g_arenas[aridx];
		while (current && GETSIZE(current->header) < size)
			current = current->next;

		if (!current) // none of the available chunks can satisfy the allocation request
			return NULL; // not implemented yet...
		else // split chunk found to only return the requested size
		{
			t_chunk** arena = (t_chunk**)&g_arenas[aridx];
			if (GETSIZE(current->header) - size < MIN_CHUNK_SIZE) // not enough space to perform chunk splitting
				update_freelist(arena, current, current->next, current->prev);
			else // split the chunk to avoid wasting free memory
			{
				t_chunk *splitted = (void *)current + size + CHUNK_OVERHEAD;
				*splitted = *current;

				SETSIZE(splitted, GETSIZE(splitted->header) - (size + CHUNK_OVERHEAD));
				SETSTATE(splitted, LEFT_CHUNK);
				set_chunk_footer(splitted);
				
				SETSIZE(current, size);
				SETSTATE(current, RIGHT_CHUNK);
				update_freelist(arena, current, splitted, splitted);
			}

			SETSTATE(current, IN_USE);
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

	t_arena_index aridx = choose_arena(GETSIZE(freed->header));
	if (aridx == LARGE_ARENA)
	{
		t_large_chunk* chk = ptr - sizeof(t_chunk);
		// update_freelist(arena, chk, chk->next, chk->prev);

		if (g_arenas[aridx] == chk)
			g_arenas[aridx] = chk->next;
		if (chk->prev)
			chk->prev->next = chk->next;
		if (chk->next)
			chk->next->prev = chk->prev;

		if (munmap(chk, GETSIZE(chk->header) + sizeof(t_large_chunk)) == -1)
			perror("munmap");
	}
	else
	{
		t_chunk** arena = (t_chunk**)&g_arenas[aridx];
		
		/* Merge the next chunk if it is also free */
		t_chunk *chk = get_near_chunk(freed, RIGHT_CHUNK);
		if (chk && !(chk->header & IN_USE))
			merge_chunks(arena, freed, chk, RIGHT_CHUNK);

		/* Also perform the merge with previous chunk if necessary */
		chk = get_near_chunk(freed, LEFT_CHUNK);
		if (chk && !(chk->header & IN_USE))
			merge_chunks(arena, chk, freed, LEFT_CHUNK);
		else
		{
			/* Set chunk as free */
			CLEARSTATE(freed, IN_USE);
			set_chunk_footer(freed);

			/* Insert freed chunk at the beginning of the freelist */
			freed->next = g_arenas[aridx];
			if (g_arenas[aridx])
				((t_chunk*)g_arenas[aridx])->prev = freed;
			g_arenas[aridx] = freed;
			freed->prev = NULL;
		}
	}
}
