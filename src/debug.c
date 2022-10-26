/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/25 22:45:15 by mamartin          #+#    #+#             */
/*   Updated: 2022/10/25 23:06:04 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdbool.h>
#include <stdio.h>

#include "mem_allocator.h"

extern t_arena_hdr* g_arena;

void print_memory_diagram()
{
	if (g_arena)
	{
		t_chunk *chk = (void*)g_arena + sizeof(t_arena_hdr);
	
		printf("8 + 8|");
	
		while (chk && (void*)chk < (void*)g_arena + g_arena->size)
		{
			size_t chunksize = GETCHUNKSIZE(chk->header);
			bool in_use = GETCHUNKSTATE(chk->header);

			printf("8 + %s%ld%s + 8|", in_use ? "\x1b[31m" : "\x1b[32m", chunksize, "\x1b[00m"); // print size in header
			// printf("8 + %s%ld%s + 8|", in_use ? "\x1b[31m" : "\x1b[32m", *(size_t*)((void*)chk + chunksize + sizeof(size_t)) & (~1), "\x1b[00m"); // print size in footer
			chk = (void*)chk + chunksize + sizeof(size_t) * 2;
		}
		printf("\nTotal: %ld bytes\n", g_arena->size);
	}
	else
		printf("Arena is empty\n");
}
