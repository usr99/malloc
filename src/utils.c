/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/25 22:52:32 by mamartin          #+#    #+#             */
/*   Updated: 2022/11/04 21:17:21 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>

#include "utils.h"
#include "libft.h"

extern void* g_arenas[3];

size_t align(size_t size)
{
	if (size < MIN_ALLOC_SIZE)
		size = MIN_ALLOC_SIZE;

	/* Align size on a 8-byte boundary (or 4-byte on 32-bits architecture) */
	return ALIGN(size, __SIZEOF_POINTER__);
}

void set_chunk_footer(t_chunk* chk)
{
	/* Copy chunk's header in its last bytes */
	size_t chunksize = GETSIZE(chk->header);
	ft_memcpy((void*)chk + chunksize + sizeof(size_t), (void*)chk, sizeof(size_t));
}

t_arena_index choose_arena(size_t size)
{
	return (size > SMALL_MAX_ALLOC) ? LARGE_ARENA : ((size > TINY_MAX_ALLOC) ? SMALL_ARENA : TINY_ARENA);
}

void update_freelist(t_chunk** arena, t_chunk* current, t_chunk* next, t_chunk* previous)
{
	/* Update logical relations between neighbours chunks */
	if (*arena == current)
		*arena = next;
	if (current->prev)
		current->prev->next = next;
	if (current->next)
		current->next->prev = previous;
}

t_chunk* get_near_chunk(t_chunk* current, t_chunk_state side)
{
	if (current->header & side)
	{
		if (side == LEFT_CHUNK)
		{
			size_t *footer = (void*)current - sizeof(size_t);
			return (void*)current - CHUNK_OVERHEAD - GETSIZE(*footer);
		}
		else
			return (void*)current + CHUNK_OVERHEAD + GETSIZE(current->header);
	}
	else
		return NULL;
}

void merge_chunks(t_chunk** arena, t_chunk* dest, t_chunk* src, t_chunk_state direction)
{
	/* Update destination size */
	SETSIZE(dest, GETSIZE(dest->header) + CHUNK_OVERHEAD + GETSIZE(src->header));
	COPYSTATE(dest, src, RIGHT_CHUNK);
	set_chunk_footer(dest);

	if (direction == RIGHT_CHUNK)
	{
		/*
		** Since the newly freed chunk is the destination for the merge
		** we need to update pointers
		*/
		dest->next = src->next;
		dest->prev = src->prev;
		update_freelist(arena, src, dest, dest);
	}
}
