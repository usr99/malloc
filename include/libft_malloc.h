/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libft_malloc.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/20 22:25:31 by mamartin          #+#    #+#             */
/*   Updated: 2022/10/23 01:05:16 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _LIBFT_MALLOC_H_
# define _LIBFT_MALLOC_H_

# include <stdlib.h>
# include <stdint.h>

# define TINY_MAX_ALLOC 256
# define TINY_ARENA_SIZE getpagesize() * TINY_MAX_ALLOC

// # define m 1024
// # define M getpagesize() * m

typedef struct s_chunk
{
	uint32_t size;
	struct s_chunk* prev;
	/* payload data */
	struct s_chunk* next;
} t_chunk;

typedef struct s_arena_hdr
{
	uint32_t size;
	t_chunk* root;
} t_arena_hdr;

void*	malloc(size_t size);
void*	realloc(void *ptr, size_t size);
void	free(void *ptr);
void	show_alloc_mem();

#endif
