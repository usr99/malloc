/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/20 22:26:01 by mamartin          #+#    #+#             */
/*   Updated: 2022/10/23 01:18:37 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/mman.h>
#include <stdio.h>

#include "libft_malloc.h"
#include "libft.h"

#define GETCHUNKSTATE(chk)			(chk->size & 0x1)
#define SETCHUNKSTATE(chk, state)	chk->size |= state

#define GETCHUNKSIZE(chk)			(chk->size & (~0x1))
#define SETCHUNKSIZE(chk, newsize)	chk->size = newsize | GETCHUNKSTATE(chk)

t_arena_hdr* arena = NULL;

static void set_chunk_footer(t_chunk* chk)
{
	/* Copy chunk's header in its last bytes */
	size_t chunksize = GETCHUNKSIZE(chk);
	ft_memcpy((void*)chk + chunksize + sizeof(size_t), (void*)chk, sizeof(size_t));
}

static int init_arena()
	{
		/* Preallocate a pool of memory */
		arena = mmap(NULL, TINY_ARENA_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		if (arena == MAP_FAILED)
		return -1;

		/* Initialize first chunk */
		t_chunk* chunk = (void*)arena + sizeof(t_arena_hdr);
	SETCHUNKSIZE(chunk, TINY_ARENA_SIZE - sizeof(t_arena_hdr) - sizeof(size_t) * 2);
	SETCHUNKSTATE(chunk, false);
		chunk->prev = NULL;
		chunk->next = NULL;
	set_chunk_footer(chunk);

		/* Initialize arena header */
		arena->size = TINY_ARENA_SIZE;
		arena->root = chunk;
	return 0;
	}

static size_t check_alignment(size_t size)
{
	if (size < MIN_ALLOC_SIZE)
		size = MIN_ALLOC_SIZE;

	/* Align size on a 8-byte boundary (or 4-byte on 32-bits architecture) */
	size = (size + 8 - 1) & ~(8 - 1);
	size = size + size % 8;
	return size;
}

void* malloc(size_t size)
{
	/* Initialize arena when called for the first time */
	if (!arena && init_arena() == -1)
		return NULL;

	if (!size)
		return NULL;
	size = check_alignment(size);

	/* Find the first chunk of memory that can fit the request */
	t_chunk* current = arena->root;
	while (current && GETCHUNKSIZE(current) < size)
		current = current->next;

	if (!current) // none of the available chunks can satisfy the allocation request
		return NULL; // not implemented yet...
	else // split chunk found to only return the requested size
	{
		t_chunk* splitted = (void*)current + size + sizeof(size_t) * 2;
		*splitted = *current;

		SETCHUNKSIZE(splitted, GETCHUNKSIZE(splitted) - (size + sizeof(size_t) * 2));
		set_chunk_footer(splitted);

		SETCHUNKSIZE(current, size);
		SETCHUNKSTATE(current, true);
		set_chunk_footer(current);

		/* Update logical relations between neighbours chunks */
		if (arena->root == current)
			arena->root = splitted;
		if (splitted->prev)
			splitted->prev->next = splitted;
		if (splitted->next)
			splitted->next->prev = splitted;
	
		/* Set new footer */
		*(uint32_t*)((void*)splitted + splitted->size - sizeof(uint32_t)) = splitted->size;
		
		return current;
	}
}

void free(void *ptr)
{
	if (ptr < (void*)arena + 1 || ptr > (void*)arena + arena->size)
		abort();
}
