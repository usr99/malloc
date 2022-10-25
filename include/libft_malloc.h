/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libft_malloc.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/20 22:25:31 by mamartin          #+#    #+#             */
/*   Updated: 2022/10/24 19:59:24 by mamartin         ###   ########.fr       */
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

# define MIN_ALLOC_SIZE sizeof(t_chunk)

typedef struct s_chunk
{
	size_t size;
	struct s_chunk* prev;
	struct s_chunk* next;
} t_chunk;

typedef struct s_arena_hdr
{
	size_t size;
	t_chunk* root;
} t_arena_hdr;

void*	malloc(size_t size);
void*	realloc(void *ptr, size_t size);
void	free(void *ptr);
void	show_alloc_mem();

#endif
