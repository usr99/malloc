/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/20 22:26:01 by mamartin          #+#    #+#             */
/*   Updated: 2022/11/06 16:26:30 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/mman.h>
#include <stdio.h>

#include "libft_malloc.h"
#include "libft.h"

#include "mem_allocator.h"
#include "utils.h"

t_mem_tracker g_memory = {0};

static t_arena* map_new_arena(t_arena_index aridx, size_t size)
{
	if (aridx != LARGE) // for LARGE allocs size is defined by the corresponding argument
		size = (aridx == TINY) ? TINY_ARENA_SIZE : SMALL_ARENA_SIZE;
	
	/* Check that enough memory is available in the virtual address space to avoid being killed by the OS */
	if (!memory_available(g_memory.total_mem_usage + size))
		return NULL;

	/* Preallocate a pool of memory */
	t_arena* arena = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (arena == MAP_FAILED)
		return NULL;
	g_memory.total_mem_usage += size;

	/* Initialize arena structure */
	ft_memset(arena, 0, sizeof(t_arena) + sizeof(t_chunk));
	arena->size = size;
	arena->root = (t_chunk*)(arena + 1);

	/* Push front new arena to the list */
	if (g_memory.arenas[aridx])
	{
		arena->next = g_memory.arenas[aridx];
		g_memory.arenas[aridx]->prev = arena;
	}
	g_memory.arenas[aridx] = arena;

	if (aridx != LARGE)
	{
		/* Initialize wilderness chunk */
		SETSIZE(arena->root, size - (CHUNK_OVERHEAD + sizeof(t_arena)));
		set_chunk_footer(arena->root);
	}
	return arena;
}

static void unmap_arena(t_arena* arena, t_arena_index aridx)
{
	/* Remove arena from the list */
	if (g_memory.arenas[aridx] == arena)
		g_memory.arenas[aridx] = arena->next;
	if (arena->prev)
		arena->prev->next = arena->next;
	if (arena->next)
		arena->next->prev = arena->prev;

	/* Release its memory back to the system */
	size_t size = GETSIZE(arena->size);
	if (munmap(arena, size) == 0)
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
		size_t allocated = ALIGN(size + sizeof(t_arena), getpagesize());
		t_arena* new = map_new_arena(LARGE, allocated);
		if (!new)
			return NULL;
		new->size |= IN_USE;
		return (void *)new + sizeof(t_arena);
	}
	else
	{
		/* Initialize arena if empty */
		if (!g_memory.arenas[aridx] && !(g_memory.arenas[aridx] = map_new_arena(aridx, 0)))
			return NULL;

		/* Find the first chunk of memory that can fit the request */
		t_arena* arena;
		t_chunk* current = NULL;
		for (arena = g_memory.arenas[aridx]; arena; arena = arena->next)
		{
			for (current = arena->root; current && GETSIZE(current->header) < size; current = current->next);
			if (current)
				break ;
		}
		
		/* Map a new arena if none of the currently allocated chunks can satisfy the request */
		if (!current)
		{
			arena = map_new_arena(aridx, 0);
			if (!arena)
				return NULL;
			current = arena->root;
		}

		if (GETSIZE(current->header) - size < MIN_CHUNK_SIZE) // not enough space to perform chunk splitting
			update_freelist(arena, current, current->next, current->prev);
		else // split chunk to avoid wasting too much free memory
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

	size_t* header = ptr - sizeof(size_t);
	if (!IS_ALIGNED(header, __SIZEOF_POINTER__) || !(*header & IN_USE))
		return ;

	t_arena_index aridx = choose_arena(GETSIZE(*header));
	if (aridx == LARGE)
		unmap_arena(ptr - sizeof(t_arena), LARGE);
	else
	{
		t_chunk* freed = (t_chunk*)header;
		t_arena* arena = find_arena(g_memory.arenas[aridx], freed);
		if (!arena)
			return ;

		/* Set chunk as free */
		CLEARSTATE(freed, IN_USE);
		set_chunk_footer(freed);

		/* Merge the next chunk if it is also free */
		bool merged = false;
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
			freelist_push_front(arena, freed);

		/* Unmap an arena if it is now empty */
		if (GETSIZE(arena->root->header) == arena->size - sizeof(t_arena) - CHUNK_OVERHEAD)
			unmap_arena(arena, aridx);
	}
}
