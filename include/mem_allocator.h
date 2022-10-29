/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mem_allocator.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/25 22:42:33 by mamartin          #+#    #+#             */
/*   Updated: 2022/10/28 21:34:30 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _MEM_ALLOCATOR_H_
# define _MEM_ALLOCATOR_H_

# include <stdlib.h>

/*
** Alignment-related macros
*/
# define IS_ALIGNED(size, boundary) (((size_t)size & (boundary - 1)) == 0)
# define ALIGN(size, boundary) ((size + boundary - 1) & ~(boundary - 1))

/*
** Implementation specific definitions
*/
# define TINY_MAX_ALLOC 256
# define TINY_ARENA_SIZE ALIGN(100 * (TINY_MAX_ALLOC + sizeof(size_t) * 2), getpagesize())

# define SMALL_MAX_ALLOC 1024
# define SMALL_ARENA_SIZE ALIGN(100 * (SMALL_MAX_ALLOC + sizeof(size_t) * 2), getpagesize())

# define MIN_ALLOC_SIZE __SIZEOF_POINTER__ * 2
# define MIN_CHUNK_SIZE MIN_ALLOC_SIZE + sizeof(size_t) * 2

typedef enum e_arena_index { TINY_ARENA, SMALL_ARENA, LARGE_ARENA } t_arena_index;

/*
** Useful macros to manage memory chunks
*/
#define GETCHUNKSTATE(header)		(header & 0x1)
#define GETCHUNKSIZE(header)		(header & (~0x1))
#define SETCHUNKSTATE(chk, state)	chk->header ^= (GETCHUNKSTATE(chk->header) ^ state)
#define SETCHUNKSIZE(chk, newsize)	chk->header = newsize | GETCHUNKSTATE(chk->header)

/*
** Data structures
*/
typedef struct s_chunk
{
	size_t header;
	struct s_chunk* prev;
	struct s_chunk* next;
} t_chunk;

typedef struct s_arena_hdr
{
	size_t size;
	t_chunk* root;
} t_arena_hdr;

enum e_chunk_state { FREE, IN_USE };

/*
** Debug functions
*/
void print_memory_diagram();

#endif
