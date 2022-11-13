/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   history.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/07 12:17:15 by mamartin          #+#    #+#             */
/*   Updated: 2022/11/13 16:02:51 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/mman.h>
#include <sys/time.h>

#include "mem_allocator.h"
#include "libft.h"

#define HISTORY_INIT_SIZE ALIGN(sizeof(t_alloc_history) * 100 + sizeof(size_t) * 2, getpagesize())

extern t_mem_tracker g_memory;

static void* realloc_history(size_t oldsize)
{
	/* Map more pages to hold history data */
	size_t allocated = (oldsize == 0) ? HISTORY_INIT_SIZE : oldsize * 2;
	void* history = mmap(NULL, allocated, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (history == MAP_FAILED)
		return NULL;

	*(size_t*)history = allocated;
	if (g_memory.history)
	{
		/* Copy old history */
		ft_memcpy(history + sizeof(size_t), g_memory.history + sizeof(size_t), oldsize - sizeof(size_t));

		/* Unmap previous history buffer */
		if (munmap(g_memory.history, oldsize) == 0)
			g_memory.total_mem_usage += (allocated - oldsize);
	}
	else // init first history
	{
		*((size_t*)history + 1) = 0;
		g_memory.total_mem_usage += allocated;
	}
	return history;
}

static t_alloc_history* find_alloc(void* ptr)
{
	if (!g_memory.history)
		return NULL;
	
	size_t i = 0;
	size_t nallocs = *(size_t*)(g_memory.history + sizeof(size_t));
	t_alloc_history* history = g_memory.history + sizeof(size_t) * 2;

	while (i < nallocs && (history[i].address != ptr || history[i].free_time != 0))
		i++;
	return (i == nallocs) ? NULL : history + i;
}

void push_history(void* ptr, size_t size)
{
	struct timeval tv;
	size_t *nallocs = NULL;

	gettimeofday(&tv, NULL);
	if (!g_memory.history)
		g_memory.history = realloc_history(0);
	else
	{
		nallocs = g_memory.history + sizeof(size_t);
		size_t current_bufsize = *(size_t*)g_memory.history;
		if (*nallocs >= (current_bufsize - sizeof(size_t) * 2) / sizeof(t_alloc_history))
			g_memory.history = realloc_history(current_bufsize);
	}
	if (!g_memory.history)
		return ;
	nallocs = g_memory.history + sizeof(size_t);

	t_alloc_history* history = g_memory.history + sizeof(size_t) * 2;
	history[*nallocs].address = ptr;
	history[*nallocs].alloc_time = tv.tv_sec * 1000 + tv.tv_usec / 1000;
	history[*nallocs].free_time = 0;
	history[*nallocs].size = size;
	*nallocs += 1;
}

void set_free_history(void* ptr)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	
	t_alloc_history* alloc = find_alloc(ptr);
	if (alloc)
		alloc->free_time = tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

void update_history(void* oldptr, void* newptr, size_t newsize)
{
	t_alloc_history* alloc = find_alloc(oldptr);
	if (alloc)
	{
		alloc->address = newptr;
		alloc->size = newsize;
	}
}