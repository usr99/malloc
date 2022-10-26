/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/25 22:52:32 by mamartin          #+#    #+#             */
/*   Updated: 2022/10/25 23:05:51 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "mem_allocator.h"
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
