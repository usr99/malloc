/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/25 22:45:15 by mamartin          #+#    #+#             */
/*   Updated: 2022/11/06 17:44:25 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdbool.h>
#include <stdio.h>

#include "mem_allocator.h"
#include "libft.h"

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

static t_arena* lower_bound_addr(t_arena* addr, t_arena_index* aridx)
{
	t_arena* smallest = NULL;
	int i;

	for (i = TINY; i <= LARGE; i++)
	{
		t_arena* ar;
		for (ar = g_memory.arenas[i]; ar; ar = ar->next)
		{
			if (addr < ar && (!smallest || ar < smallest))
			{
				smallest = ar;
				*aridx = i;
			}
		}
	}
	return smallest;
}

static void ft_putaddr(size_t addr)
{
	static const char* digits = "0123456789ABCDEF";

	if (addr >= 16)
	{
		ft_putaddr(addr / 16);
		ft_putchar(digits[addr % 16]);
	}
	else
		ft_putchar(digits[addr % 16]);
}

static void print_alloc_info(void* start, size_t size)
{
	ft_putstr("0x");
	ft_putaddr(start);
	ft_putstr(" - ");
	ft_putstr("0x");
	ft_putaddr(start + size);
	ft_putstr(" : ");
	ft_putnbr(size);
	ft_putstr(" bytes\n");
}

void show_alloc_mem()
{
	static const char* arena_names[3] = { "TINY", "SMALL", "LARGE" };
	t_arena* lastptr = NULL;
	t_arena_index aridx;

	size_t total = 0;
	size_t sz;
	while ((lastptr = lower_bound_addr(lastptr, &aridx)))
	{
		ft_putstr(arena_names[aridx]);
		ft_putstr(" : 0x");
		ft_putaddr(lastptr);
		ft_putchar('\n');

		if (aridx != LARGE)
		{
			t_chunk* chk = (void*)lastptr + sizeof(t_arena);
			while (chk)
			{
				sz = GETSIZE(chk->header);
				if (chk->header & IN_USE)
				{
					print_alloc_info((void*)chk + sizeof(size_t), sz);
					total += sz;
				}

				if (chk->header & RIGHT_CHUNK)
					chk = (void*)chk + sz + CHUNK_OVERHEAD;
				else
					chk = NULL;
			}
		}
		else
		{
			sz = GETSIZE(lastptr->size) - sizeof(t_arena);
			print_alloc_info((void*)lastptr + sizeof(t_arena), sz);
			total += sz;
		}
	}
	ft_putstr("Total : ");
	ft_putnbr(total);
	ft_putstr(" bytes\n");
}

void show_alloc_mem_hex()
{

}

void show_alloc_history()
{
}
