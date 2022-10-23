/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/20 22:26:01 by mamartin          #+#    #+#             */
/*   Updated: 2022/10/23 01:18:37 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/mman.h>
#include <stdio.h>

#include "libft_malloc.h"
#include "libft.h"

t_arena_hdr* arena = NULL;

void* malloc(size_t size)
{
	/* Initialize arena when called for the first time */
	if (!arena)
	{
		ft_putstr_fd("WARNING: THIS ALLOCATOR IS POORLY IMPLEMENTED ! USE IT AT YOUR OWN RISKS ! thx tho :)\n", STDERR_FILENO);

		/* Preallocate a pool of memory */
		arena = mmap(NULL, TINY_ARENA_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		if (arena == MAP_FAILED)
			return NULL;

		/* Initialize first chunk */
		t_chunk* chunk = (void*)arena + sizeof(t_arena_hdr);
		chunk->size = TINY_ARENA_SIZE - sizeof(t_arena_hdr);
		chunk->prev = NULL;
		chunk->next = NULL;
		*(uint32_t*)((void*)chunk + chunk->size - sizeof(uint32_t)) = chunk->size;

		/* Initialize arena header */
		arena->size = TINY_ARENA_SIZE;
		arena->root = chunk;
	}

	/* Find the first chunk of memory that can fit the request */
	t_chunk* current = arena->root;
	while (current && current->size < size)
		current = current->next;

	if (!current) // none of the available chunks can satisfy the allocation request
		return NULL; // not implemented yet...
	else // split chunk found to only return the requested size
	{
		t_chunk* splitted = ((void*)current + size);
		*splitted = *current;
		splitted->size -= size;

		/* Update logical relations between neighbours chunks */
		if (arena->root == current)
			arena->root = splitted;
		if (splitted->prev)
			splitted->prev->next = splitted;
		if (splitted->next)
			splitted->next->prev = splitted;
	
		/* Set new footer */
		*(uint32_t*)((void*)splitted + splitted->size - sizeof(uint32_t)) = splitted->size;
		
		return current;
	}
}

void free(void *ptr)
{
	if (ptr < (void*)arena + 1 || ptr > (void*)arena + arena->size)
		abort();
}
