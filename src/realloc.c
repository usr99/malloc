/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   realloc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/06 01:11:58 by mamartin          #+#    #+#             */
/*   Updated: 2022/11/07 14:05:16 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <pthread.h>
#include <sys/mman.h>

#include "libft_malloc.h"
#include "libft.h"

#include "mem_allocator.h"
#include "utils.h"

extern t_mem_tracker g_memory;
extern pthread_mutex_t g_mutex;

static void* _realloc(void* ptr, size_t oldsize, size_t newsize)
{
	void* new = malloc(newsize);
	if (!new)
		return unlock_mutex();
	ft_memcpy(new, ptr, oldsize);
	free(ptr);
	unlock_mutex();
	return new;
}

static bool enlarge_chunk(t_arena* arena, t_chunk** dest, t_chunk* src, size_t extension)
{
	size_t available = GETSIZE(src->header) + CHUNK_OVERHEAD;
	if (available < extension)
		return false;

	size_t left = available - extension;
	if (left >= MIN_CHUNK_SIZE) // split source chunk to take some memory from it
	{
		t_chunk* tmp;
		if (*dest < src)
		{
			/* Shift source chunk to the right and update pointers */
			tmp = src;
			src = (void*)src + extension;
			update_freelist(arena, tmp, src, src);
			*src = *tmp;
		}
		else
		{
			/* Extend destination chunk to the left */
			tmp = *dest;
			*dest = (void*)*dest - extension;
			(*dest)->header = tmp->header;
		}

		/* Set new chunks sizes */
		SETSIZE((*dest), GETSIZE((*dest)->header + extension));
		set_chunk_footer(*dest);
		SETSIZE(src, GETSIZE(src->header) - extension);
		set_chunk_footer(src);
	}
	else // not enough space left for a chunk
	{
		/* Remove source chunk from the list */
		update_freelist(arena, src, src->next, src->prev);

		if (*dest < src)
		{
			/* Extends destination chunk to the right */
			SETSIZE((*dest), GETSIZE((*dest)->header) + available);
			COPYSTATE((*dest), src, RIGHT_CHUNK);
		}
		else
		{
			/* Extends source chunk to absorb destination */
			SETSIZE(src, GETSIZE((*dest)->header) + available);
			SETSTATE(src, IN_USE);
			COPYSTATE(src, (*dest), RIGHT_CHUNK);
			set_chunk_footer(src);
			*dest = src;
		}
	}
	return true;
}	

void *realloc(void *ptr, size_t size)
{
	if (!ptr)
		return malloc(size);
	else if (!size)
	{
		free(ptr);
		return NULL;
	}

	lock_mutex();

	void* oldptr = ptr;
	size_t* header = ptr - sizeof(size_t);
	size_t current_size = GETSIZE(*header);
	size = align(size);

	if (!IS_ALIGNED(header, __SIZEOF_POINTER__) || !(*header & IN_USE))
		return unlock_mutex();

	/* Check that the size requested should not be managed in a different arena category */
	t_arena_index aridx = choose_arena(current_size);
	if (aridx != choose_arena(size)) // make a new allocation in order to preserve coherence
		return _realloc(ptr, current_size, size);

	if (aridx == LARGE)
	{		
		t_arena* arena = ptr - sizeof(t_arena);
		if (size > current_size)
			return _realloc(ptr, current_size - sizeof(t_arena), size);
		else
		{
			size_t npages = (arena->size - (size + sizeof(t_arena))) / getpagesize();
			size_t rm = npages * getpagesize();

			if (munmap((void*)arena + arena->size - rm, rm) == 0)
			{
				g_memory.total_mem_usage -= rm;
				arena->size -= rm;
			}
		}
	}
	else
	{
		t_chunk* chk = ptr - sizeof(size_t);
		t_arena *arena = find_arena(g_memory.arenas[aridx], chk);

		/* Find contiguous chunk on the right (upper addresses) */
		t_chunk* near = get_near_chunk(chk, RIGHT_CHUNK);
		if (size < current_size)
		{
			size_t diff = current_size - size;
			if (near && !(near->header & IN_USE))
			{
				/* Next chunk in memory is widen */
				t_chunk* tmp = near;
				near = (void*)near - diff;
				update_freelist(arena, tmp, near, near);
				*near = *tmp;
				SETSIZE(near, GETSIZE(tmp->header) + diff);
				set_chunk_footer(near);
			}
			else
			{
				if (diff >= MIN_CHUNK_SIZE)
				{
					/* Create a new free chunk from released memory */
					t_chunk* new = (void*)chk + size + CHUNK_OVERHEAD;
					ft_memset(new, 0, sizeof(t_chunk));
					SETSIZE(new, diff - CHUNK_OVERHEAD);
					SETSTATE(new, LEFT_CHUNK);
					if (near)
						SETSTATE(new, RIGHT_CHUNK);
					set_chunk_footer(new);
					freelist_push_front(arena, new);
				}
				else
				{
					unlock_mutex();
					return ptr; // allocation is left unmodified
				}
			}
			SETSIZE(chk, size);
			set_chunk_footer(chk);
		}
		else
		{
			size_t diff = size - current_size;
			if (near && !(near->header & IN_USE))
			{
				if (!enlarge_chunk(arena, &chk, near, diff))
					return _realloc(ptr, current_size, size);
			}
			else if ((near = get_near_chunk(chk, LEFT_CHUNK)) && !(near->header & IN_USE))
			{
				if (enlarge_chunk(arena, &chk, near, diff))
				{
					void* tmp = ptr;
					ptr = (void*)chk + sizeof(size_t);
					ft_memcpy(ptr, tmp, current_size);
				}
				else
					return _realloc(ptr, current_size, size);
			}
			else
				return _realloc(ptr, current_size, size);
		}		
	}

	if (getenv("FT_MALLOC_DEBUG"))
		update_history(oldptr, ptr, size);
	unlock_mutex();
	return ptr;
}
