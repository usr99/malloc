/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/25 22:45:15 by mamartin          #+#    #+#             */
/*   Updated: 2022/10/29 01:13:58 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdbool.h>
#include <stdio.h>

#include "mem_allocator.h"

extern t_arena_hdr* g_arenas[3];

void print_memory_diagram()
{
	static const char* arena_names[3] = { "TINY", "SMALL", "LARGE" };
	enum e_arena_index i;

	for (i = TINY_ARENA; i < LARGE_ARENA; i++)
	{
		if (g_arenas[i])
		{
			t_chunk *chk = (void *)g_arenas[i] + sizeof(t_arena_hdr);

			printf("%s:\n8 + 8|", arena_names[i]);

			while (chk && (void *)chk < (void *)g_arenas[i] + g_arenas[i]->size)
			{
				size_t chunksize = GETCHUNKSIZE(chk->header);
				bool in_use = GETCHUNKSTATE(chk->header);

				printf("8 + %s%ld%s + 8|", in_use ? "\x1b[31m" : "\x1b[32m", chunksize, "\x1b[00m"); // print size in header
				// printf("8 + %s%ld%s + 8|", in_use ? "\x1b[31m" : "\x1b[32m", *(size_t*)((void*)chk + chunksize + sizeof(size_t)) & (~1), "\x1b[00m"); // print size in footer
				chk = (void *)chk + chunksize + sizeof(size_t) * 2;
			}
			printf("\nTotal: %ld bytes\n", g_arenas[i]->size);
		}
		else
			printf("%s arena is empty\n", arena_names[i]);
	}

	if (!g_arenas[LARGE_ARENA])
		return ;

	size_t total = 0;
	t_large_chunk *chk = (void *)g_arenas[LARGE_ARENA];
	printf("%s:\n", arena_names[LARGE_ARENA]);
	while (chk)
	{
		printf("16 + %s%ld%s|", "\x1b[31m", chk->size, "\x1b[00m"); // print size in header
		
		total += chk->size + sizeof(t_large_chunk);
		chk = chk->next;
	}
	printf("\nTotal: %ld bytes\n", total);
}
