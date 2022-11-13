/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/25 22:45:15 by mamartin          #+#    #+#             */
/*   Updated: 2022/11/07 13:58:46 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>

#include "mem_allocator.h"
#include "libft.h"
#include "utils.h"

#define GREEN "\x1b[1;32m"
#define RED "\x1b[1;31m"
#define YELLOW "\x1b[33m"
#define BLUE "\x1b[34m"
#define RESET "\x1b[0m"

extern t_mem_tracker g_memory;
extern pthread_mutex_t g_mutex;

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

static void ft_putnbr_hex(size_t nbr)
{
	static const char* digits = "0123456789ABCDEF";

	if (nbr >= 16)
	{
		ft_putnbr_hex(nbr / 16);
		ft_putchar(digits[nbr % 16]);
	}
	else
		ft_putchar(digits[nbr % 16]);
}

static void show_mem(void (*debug_fn)(void*, size_t))
{
	static const char* arena_names[3] = { "\x1b[1;32mTINY\x1b[0;0m", "\x1b[1;33mSMALL\x1b[0;0m", "\x1b[1;31mLARGE\x1b[0;0m" };
	t_arena* lastptr = NULL;
	t_arena_index aridx;

	size_t total = 0;
	size_t sz;

	lock_mutex();
	while ((lastptr = lower_bound_addr(lastptr, &aridx)))
	{
		/* Print arena category and address */
		ft_putstr(arena_names[aridx]);
		ft_putstr(" : 0x");
		ft_putnbr_hex((size_t)lastptr);
		ft_putchar('\n');

		if (aridx != LARGE)
		{
			/* Go through each chunk of the arena */
			t_chunk* chk = (void*)lastptr + sizeof(t_arena);
			while (chk)
			{
				sz = GETSIZE(chk->header);
				if (chk->header & IN_USE) // allocated chunk
				{
					debug_fn((void*)chk + sizeof(size_t), sz);
					total += sz;
				}

				/* Shift pointer to the next chunk (if any) */
				if (chk->header & RIGHT_CHUNK)
					chk = (void*)chk + sz + CHUNK_OVERHEAD;
				else
					chk = NULL;
			}
		}
		else
		{
			sz = GETSIZE(lastptr->size) - sizeof(t_arena);
			debug_fn((void*)lastptr + sizeof(t_arena), sz);
			total += sz;
		}
	}

	unlock_mutex();
	ft_putstr("Total : ");
	ft_putnbr(total);
	ft_putstr(" bytes\n");	
}

static void print_alloc_info(void* start, size_t size)
{
	ft_putstr("0x");
	ft_putnbr_hex((size_t)start);
	ft_putstr(" - ");
	ft_putstr("0x");
	ft_putnbr_hex((size_t)start + size);
	ft_putstr(" : ");
	ft_putnbr(size);
	ft_putstr(" bytes\n");
}

static void print_hexdump(void* start, size_t size)
{
	unsigned char* buffer = start;
	size_t head = 0;
	size_t lower_bits;

	/* Print chunk's size and address */
	ft_putstr(YELLOW);
	ft_putstr("0x");
	ft_putnbr_hex((size_t)start);
	ft_putstr(RESET);
	ft_putstr(" : ");
	ft_putnbr(size);
	ft_putstr(" bytes\n");

	/* Read the entire chunk's data */
	while (head < size)
	{
		/* Print row offset */
		lower_bits = head & 0xF;
		if (lower_bits == 0) // multiple of 16
		{
			size_t x = 0x1000000;
			ft_putstr(BLUE);
			while (x > head & x > 1)
			{
				ft_putchar('0');
				x >>= 4; // divide by 16
			}
			ft_putnbr_hex(head);
			ft_putchar('\t');
			ft_putstr(RESET);
		}

		/* Print byte as hexadecimal */
		if (buffer[head] < 0x10)
			ft_putchar('0'); // padding
		ft_putnbr_hex(buffer[head]);
		
		/* Print separator */
		if (lower_bits == 0xF) // head = k * 16 - 1, its the last byte for the current row
			ft_putchar('\n');
		else
			ft_putchar(' ');
		head++;
	}
	if (head & 0xf)
		ft_putchar('\n');
}

void show_alloc_mem()
{
	show_mem(&print_alloc_info);
}

void show_alloc_mem_hex()
{
	show_mem(&print_hexdump);
}

void show_alloc_history()
{
	lock_mutex();
	if (g_memory.history)
	{
		size_t i;
		size_t nallocs = *(size_t*)(g_memory.history + sizeof(size_t));
		t_alloc_history* history = g_memory.history + sizeof(size_t) * 2;

		for (i = 0; i < nallocs; i++)
		{
			ft_putstr(history[i].free_time ? GREEN : RED);
			ft_putchar('[');
			ft_putnbr(history[i].alloc_time);
			ft_putchar(']');
			ft_putstr(RESET);
			ft_putstr(" malloc'd ");
			ft_putstr(BLUE);
			ft_putnbr(history[i].size);
			ft_putstr(RESET);
			ft_putstr(" bytes at");
			ft_putstr(BLUE);
			ft_putstr(" 0x");
			ft_putnbr_hex((size_t)history[i].address);
			ft_putstr(RESET);

			if (history[i].free_time)
			{
				ft_putstr(", ");
				ft_putstr(GREEN);
				ft_putstr("freed");
				ft_putstr(RESET);
				ft_putstr(" at [");
				ft_putnbr(history[i].free_time);
				ft_putstr("]\n");
			}
			else
			{
				ft_putstr(", still ");
				ft_putstr(RED);
				ft_putstr("in use\n");
				ft_putstr(RESET);
			}
		}
	}
	else
		ft_putstr("History is empty or disabled, please set FT_MALLOC_DEBUG to turn this feature on.\n");
	unlock_mutex();
}
