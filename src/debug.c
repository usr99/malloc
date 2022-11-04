/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/25 22:45:15 by mamartin          #+#    #+#             */
/*   Updated: 2022/11/04 19:14:06 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdbool.h>
#include <stdio.h>

#include "mem_allocator.h"

extern void* g_arenas[3];

void print_memory_diagram()
{
	static const char* arena_names[3] = { "TINY", "SMALL", "LARGE" };
	enum e_arena_index i;

	for (i = TINY_ARENA; i < LARGE_ARENA; i++)
	{
		if (g_arenas[i])
		{
			t_chunk *chk = g_arenas[i];
			size_t memory = 0;
			size_t overhead = 0;

			printf("%s:\n", arena_names[i]);

			while (chk->header & LEFT_CHUNK)
			{
				size_t* tag = (void*)chk - sizeof(size_t);
				chk = (void*)chk - (GETSIZE(*tag) + CHUNK_OVERHEAD);
			}

			while (chk)
			{
				size_t chunksize = GETSIZE(chk->header);

				printf("8 + %s%ld%s + 8|", (chk->header & IN_USE) ? "\x1b[31m" : "\x1b[32m", chunksize, "\x1b[00m"); // print size in header
				// printf("8 + %s%ld%s + 8|", in_use ? "\x1b[31m" : "\x1b[32m", *(size_t*)((void*)chk + chunksize + sizeof(size_t)) & (~1), "\x1b[00m"); // print size in footer
				
				memory += chunksize;
				overhead += CHUNK_OVERHEAD;
				if (chk->header & RIGHT_CHUNK)
					chk = (void *)chk + chunksize + CHUNK_OVERHEAD;
				else
					break ;
			}
			printf("\nTotal: %ld bytes + %ld overhead\n", memory, overhead);
		}
		else
			printf("%s arena is empty\n", arena_names[i]);
	}

	if (g_arenas[LARGE_ARENA])
	{
		size_t total = 0;
		t_large_chunk *chk = g_arenas[LARGE_ARENA];
		printf("%s:\n", arena_names[LARGE_ARENA]);
		while (chk)
		{
			printf("%lu + %s%ld%s|", sizeof(t_large_chunk), "\x1b[31m", GETSIZE(chk->header), "\x1b[00m");

			total += GETSIZE(chk->header) + sizeof(t_large_chunk);
			chk = chk->next;
		}
		printf("\nTotal: %ld bytes\n", total);
	}
	else
		printf("%s arena is empty\n", arena_names[i]);
}
