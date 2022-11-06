/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/25 22:45:15 by mamartin          #+#    #+#             */
/*   Updated: 2022/11/05 21:05:10 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdbool.h>
#include <stdio.h>

#include "mem_allocator.h"

extern t_mem_tracker g_memory;

void print_memory_diagram()
{
	static const char* arena_names[3] = { "TINY", "SMALL", "LARGE" };
	enum e_arena_index i;

	for (i = TINY; i <= LARGE; i++)
	{
		if (g_memory.arenas[i])
		{
			t_arena* ar = g_memory.arenas[i];
			
			while (ar)
			{
				printf("\n%s: 0x%lX - 0x%lX : %ld bytes\n", arena_names[i], (intptr_t)ar, (intptr_t)((void*)ar + ar->size), ar->size);
				void* chk = (void*)ar + sizeof(t_arena);
				while (chk)
				{
					size_t header;
					if (i != LARGE)
						header = ((t_chunk*)chk)->header;
					else
						header = ar->size - sizeof(t_arena);

					printf("8 + %s%ld%s + 8|", (header & IN_USE) ? "\x1b[31m" : "\x1b[32m", GETSIZE(header), "\x1b[00m");
					if (header & RIGHT_CHUNK)
						chk = chk + GETSIZE(header) + CHUNK_OVERHEAD;
					else
						chk = NULL;
				}
				ar = ar->next;				
			}
			printf("\n");
		}
		else
			printf("%s arena is empty\n", arena_names[i]);
	}
	printf("TOTAL: %ld\n", g_memory.total_mem_usage);
}
