/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/20 22:26:01 by mamartin          #+#    #+#             */
/*   Updated: 2022/10/25 19:47:27 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/mman.h>
#include <stdio.h>

#include "libft_malloc.h"
#include "libft.h"

#define GETCHUNKSTATE(chk)			(chk->size & 0x1)
#define SETCHUNKSTATE(chk, state)	chk->size ^= (GETCHUNKSTATE(chk) ^ state)

#define GETCHUNKSIZE(chk)			(chk->size & (~0x1))
#define SETCHUNKSIZE(chk, newsize)	chk->size = newsize | GETCHUNKSTATE(chk)

enum e_chunk_state { FREE, IN_USE };

static t_arena_hdr* arena = NULL;

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
	SETCHUNKSTATE(chunk, FREE);
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
	return size;
}

static void update_freelist(t_chunk* current, t_chunk* next, t_chunk* previous)
{
	/* Update logical relations between neighbours chunks */
	if (arena->root == current)
		arena->root = next;
	if (current->prev)
		current->prev->next = next;
	if (current->next)
		current->next->prev = previous;
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
		if (GETCHUNKSIZE(current) - size < MIN_CHUNK_SIZE) // not enough space to perform chunk splitting
			update_freelist(current, current->next, current->prev);
		else // split the chunk to avoid wasting free memory
		{
			t_chunk* splitted = (void*)current + size + sizeof(size_t) * 2;
			*splitted = *current;

			SETCHUNKSIZE(splitted, GETCHUNKSIZE(splitted) - (size + sizeof(size_t) * 2));
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
	t_chunk* freed = ptr - sizeof(size_t);
	SETCHUNKSTATE(freed, FREE);
	set_chunk_footer(freed);

	freed->next = arena->root;
	if (arena->root)
		arena->root->prev = freed;
	arena->root = freed;
	freed->prev = NULL;
}

void show_alloc_mem()
{
	if (arena)
	{
		t_chunk *chk = (void*)arena + sizeof(t_arena_hdr);
	
		printf("8 + 8|");
	
		while (chk && (void*)chk < (void*)arena + arena->size)
		{
			size_t chunksize = GETCHUNKSIZE(chk);
			bool in_use = GETCHUNKSTATE(chk);

			printf("8 + %s%ld%s + 8|", in_use ? "\x1b[31m" : "\x1b[32m", chunksize, "\x1b[00m"); // print size in header
			// printf("8 + %s%ld%s + 8|", in_use ? "\x1b[31m" : "\x1b[32m", *(size_t*)((void*)chk + chunksize + sizeof(size_t)) & (~1), "\x1b[00m"); // print size in footer
			chk = (void*)chk + chunksize + sizeof(size_t) * 2;
		}
		printf("\nTotal: %ld bytes\n", arena->size);
	}
	else
		printf("Arena is empty\n");
}
