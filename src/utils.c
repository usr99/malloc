/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/25 22:52:32 by mamartin          #+#    #+#             */
/*   Updated: 2022/10/28 17:03:59 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.h"
#include "libft.h"

extern t_arena_hdr* g_arena;

size_t align(size_t size)
{
	if (size < MIN_ALLOC_SIZE)
		size = MIN_ALLOC_SIZE;

	/* Align size on a 8-byte boundary (or 4-byte on 32-bits architecture) */
	size = (size + 8 - 1) & ~(8 - 1);
	return size;
}

void set_chunk_footer(t_chunk* chk)
{
	/* Copy chunk's header in its last bytes */
	size_t chunksize = GETCHUNKSIZE(chk->header);
	ft_memcpy((void*)chk + chunksize + sizeof(size_t), (void*)chk, sizeof(size_t));
}

void update_freelist(t_chunk* current, t_chunk* next, t_chunk* previous)
{
	/* Update logical relations between neighbours chunks */
	if (g_arena->root == current)
		g_arena->root = next;
	if (current->prev)
		current->prev->next = next;
	if (current->next)
		current->next->prev = previous;
}

t_chunk* get_next_chunk(t_chunk* current)
{
	return (void*)current + sizeof(size_t) * 2 + GETCHUNKSIZE(current->header);
}

t_chunk* get_previous_chunk(t_chunk* current)
{
	if ((void*)g_arena + sizeof(t_arena_hdr) == current)
		return NULL; // current chunk is at the beginning of arena, thus there is no "previous" chunk

	size_t prev_footer = *(size_t*)((void*)current - sizeof(size_t));
	return (void*)current - sizeof(size_t) * 2 - GETCHUNKSIZE(prev_footer);
}

void merge_chunks(t_chunk* dest, t_chunk* src, bool update_links)
{
	/* Update destination size */
	SETCHUNKSIZE(dest, GETCHUNKSIZE(dest->header) + sizeof(size_t) * 2 + GETCHUNKSIZE(src->header));
	SETCHUNKSTATE(dest, FREE);
	set_chunk_footer(dest);

	if (update_links)
	{
		/* Update logical links between free chunks */
		dest->next = src->next;
		dest->prev = src->prev;
		update_freelist(src, dest, dest);
	}
}
