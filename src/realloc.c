/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   realloc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/06 01:11:58 by mamartin          #+#    #+#             */
/*   Updated: 2022/11/06 13:49:12 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft_malloc.h"
#include "libft.h"

#include "mem_allocator.h"
#include "utils.h"

extern t_mem_tracker g_memory;

static void* realloc_routine(void* ptr, size_t oldsize, size_t newsize)
{
	void* new = malloc(newsize);
	if (!new)
		return NULL;

	ft_memcpy(new, ptr, oldsize);
	free(ptr);
	return new;
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

	t_chunk* chk = ptr - sizeof(size_t);
	size_t current_size = GETSIZE(chk->header);
	size = align(size);
	
	/* Check that the size requested should not be managed in a different arena category */
	t_arena_index aridx = choose_arena(current_size);
	if (aridx != choose_arena(size)) // make a new allocation in order to preserve coherence
		return realloc_routine(ptr, current_size, size);

	/* Find contiguous chunk on the right (upper addresses) */
	t_chunk* near = get_near_chunk(chk, RIGHT_CHUNK);

	if (size < current_size) // shrink allocated chunk
	{
		size_t diff = current_size - size;
		t_arena *arena = find_arena(g_memory.arenas[aridx], chk);
		
		if (near && !(near->header & IN_USE))
		{
			// enlarge nearest chunk
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
				// create a new free chunk
				t_chunk* new = (void*)chk + size + CHUNK_OVERHEAD;
				ft_memset(new, 0, sizeof(t_chunk));
				SETSIZE(new, diff - CHUNK_OVERHEAD);
				SETSTATE(new, LEFT_CHUNK);
				if (near)
					SETSTATE(new, RIGHT_CHUNK);
				set_chunk_footer(new);

				// push front in the freelist (code duplicate in free())
				new->next = arena->root;
				if (arena->root)
					arena->root->prev = new;
				arena->root = new;
			}
			else
				return ptr;
		}
		SETSIZE(chk, size);
		set_chunk_footer(chk);
		return ptr;
	}
	else // enlarge allocated chunk
	{
		if (near && !(near->header & IN_USE))
		{
			size_t diff = size - current_size;
			size_t available = GETSIZE(near->header) + CHUNK_OVERHEAD;
			if (available >= diff)
			{
				t_arena *arena = find_arena(g_memory.arenas[aridx], chk);
				size_t left = available - diff;

				if (left >= MIN_CHUNK_SIZE) // split nearest chunk
				{
					SETSIZE(chk, size);

					t_chunk *tmp = near;
					near = (void *)near + diff;
					update_freelist(arena, tmp, near, near);

					*near = *tmp;
					SETSIZE(near, GETSIZE(tmp->header) - diff);
					set_chunk_footer(near);
				}
				else // absorbs the whole chunk
				{
					update_freelist(arena, near, near->next, near->prev);
					SETSIZE(chk, current_size + available);
					COPYSTATE(chk, near, RIGHT_CHUNK);
				}
				set_chunk_footer(chk);
				return ptr;
			}
			else
				return realloc_routine(ptr, current_size, size);
		}
		else if ((near = get_near_chunk(chk, LEFT_CHUNK)) && !(near->header & IN_USE))
		{
			size_t diff = size - current_size;
			size_t available = GETSIZE(near->header) + CHUNK_OVERHEAD;
			if (available >= diff)
			{
				t_arena *arena = find_arena(g_memory.arenas[aridx], chk);
				size_t left = available - diff;

				if (left >= MIN_CHUNK_SIZE) // split nearest chunk
				{
					t_chunk* tmp = chk;
					chk = (void *)chk - diff;
					chk->header = tmp->header;
					SETSIZE(chk, size);
					set_chunk_footer(chk);

					SETSIZE(near, GETSIZE(near->header) - diff);
					set_chunk_footer(near);

					ptr = (void*)chk + sizeof(size_t);
				}
				else // absorbs the whole chunk
				{
					update_freelist(arena, near, near->next, near->prev);
					SETSIZE(near, current_size + available);
					SETSTATE(near, IN_USE);
					COPYSTATE(near, chk, RIGHT_CHUNK);
					set_chunk_footer(near);

					ptr = (void*)near + sizeof(size_t);
				}
				return ptr;
			}
			else
				return realloc_routine(ptr, current_size, size);
		}
		else
			return realloc_routine(ptr, current_size, size);
	}
}
