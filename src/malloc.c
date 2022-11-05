/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/20 22:26:01 by mamartin          #+#    #+#             */
/*   Updated: 2022/11/05 18:31:50 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/mman.h>
#include <stdio.h>

#include "libft_malloc.h"
#include "libft.h"

#include "mem_allocator.h"
#include "utils.h"

t_mem_tracker g_memory = {0};

static void* allocate_arena(t_arena_index aridx, size_t size)
{
	if (aridx != LARGE) // for LARGE allocs size is defined by the corresponding argument
		size = (aridx == TINY) ? TINY_ARENA_SIZE : SMALL_ARENA_SIZE;
	
	/* Check that enough memory is available in the virtual address space to avoid being killed by the OS */
	if (!memory_available(g_memory.total_mem_usage + size))
		return NULL;

	/* Preallocate a pool of memory */
	void* arena = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (arena == MAP_FAILED)
		return NULL;
	g_memory.total_mem_usage += size;
	ft_memset(arena, 0, sizeof(t_chunk));

	if (aridx == LARGE)
	{
		t_large_chunk* ar = arena;

		/* Initialize chunk and insert it at the beginning of the list */
		SETSIZE(ar, size - sizeof(t_large_chunk));
		if (g_memory.arenas[LARGE])
		{
			ar->next = g_memory.arenas[LARGE];
			((t_large_chunk*)g_memory.arenas[LARGE])->prev = ar;
		}
		g_memory.arenas[LARGE] = ar;
	}
	else
	{
		t_chunk* ar = arena;

		/* Initialize wilderness chunk */
		SETSIZE(ar, size - CHUNK_OVERHEAD);
		set_chunk_footer(ar);
	}
	return arena;
}

static void unmap_arena(void* ptr, size_t size)
{
	if (munmap(ptr, size) == 0)
		g_memory.total_mem_usage -= size;
}

void* malloc(size_t size)
{
	if (!size)
		return NULL;
	size = align(size);
	
	t_arena_index aridx = choose_arena(size);
	if (aridx == LARGE)
	{
		size_t allocated = ALIGN(size + sizeof(t_large_chunk), getpagesize());
		t_large_chunk* new = allocate_arena(LARGE, allocated);
		
		if (!new)
			return NULL;
		return (void *)new + sizeof(t_large_chunk);
	}
	else
	{
		/* Initialize arena if empty */
		if (!g_memory.arenas[aridx] && !(g_memory.arenas[aridx] = allocate_arena(aridx, 0)))
			return NULL;

		/* Find the first chunk of memory that can fit the request */
		t_chunk* current = g_memory.arenas[aridx];
		while (current && GETSIZE(current->header) < size)
			current = current->next;

		if (!current) // none of the available chunks can satisfy the allocation request
		{
			t_chunk* extension = allocate_arena(aridx, 0);
			if (!extension)
				return NULL;
			t_chunk** arena = (t_chunk**)&g_memory.arenas[aridx];
			extension->next = *arena;
			(*arena)->prev = extension;
			*arena = extension;
			current = extension;
		}

		t_chunk **arena = (t_chunk **)&g_memory.arenas[aridx];
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

void free(void *ptr)
{
	if (!ptr)
		return ;

	t_chunk* freed = ptr - sizeof(size_t);
	if (!IS_ALIGNED(freed, __SIZEOF_POINTER__) || !(freed->header & IN_USE))
		return ;

	t_arena_index aridx = choose_arena(GETSIZE(freed->header));
	if (aridx == LARGE)
	{
		/* Remove the chunk from the list */
		t_large_chunk* chk = ptr - sizeof(t_chunk);
		if (g_memory.arenas[aridx] == chk)
			g_memory.arenas[aridx] = chk->next;
		if (chk->prev)
			chk->prev->next = chk->next;
		if (chk->next)
			chk->next->prev = chk->prev;
		
		/* Release memory */
		unmap_arena(chk, GETSIZE(chk->header) + sizeof(t_large_chunk));
	}
	else
	{
		t_chunk** arena = (t_chunk**)&g_memory.arenas[aridx];
		bool merged = false;

		/* Set chunk as free */
		CLEARSTATE(freed, IN_USE);
		set_chunk_footer(freed);

		/* Merge the next chunk if it is also free */
		t_chunk *chk = get_near_chunk(freed, RIGHT_CHUNK);
		if (chk && !(chk->header & IN_USE))
		{
			merge_chunks(arena, freed, chk, RIGHT_CHUNK);
			merged = true;
		}

		/* Also perform the merge with previous chunk if necessary */
		chk = get_near_chunk(freed, LEFT_CHUNK);
		if (chk && !(chk->header & IN_USE))
		{
			merge_chunks(arena, chk, freed, LEFT_CHUNK);
			if (merged)
				update_freelist(arena, freed, freed->next, freed->prev);
			freed = chk;
		}
		else if (!merged)
		{
			/* Insert freed chunk at the beginning of the freelist */
			freed->next = *arena;
			if (*arena)
				(*arena)->prev = freed;
			*arena = freed;
			freed->prev = NULL;
		}

		/* Unmap an arena if it is now empty */
		size_t init_sz = (aridx == TINY) ? TINY_ARENA_SIZE : SMALL_ARENA_SIZE;
		size_t chunk_sz = GETSIZE(freed->header);
		if (chunk_sz == init_sz - CHUNK_OVERHEAD)
		{
			update_freelist(arena, freed, freed->next, freed->prev);
			unmap_arena(freed, chunk_sz + CHUNK_OVERHEAD);
		}
	}
}

#if 0
void *realloc(void *ptr, size_t size)
{
/*
	if ptr == NULL
		return malloc(size)
	else if size == 0
		return ???

	if ptr & RIGHT_CHUNK
		if !(right_chunk & IN_USE)
			make the current chunk bigger
		else
			new = malloc(size)
			copy(ptr, new)
			free(ptr)
	else
		new = malloc(size)
		copy(ptr, new)
		free(ptr)
*/
}
#endif