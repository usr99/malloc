/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mem_allocator.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/25 22:42:33 by mamartin          #+#    #+#             */
/*   Updated: 2022/11/05 17:19:40 by mamartin         ###   ########.fr       */
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
# define MIN_ALLOC_SIZE __SIZEOF_POINTER__ * 2
# define CHUNK_OVERHEAD sizeof(size_t) * 2
# define MIN_CHUNK_SIZE MIN_ALLOC_SIZE + CHUNK_OVERHEAD

# define TINY_MAX_ALLOC 256
# define TINY_ARENA_SIZE ALIGN(100 * (TINY_MAX_ALLOC + CHUNK_OVERHEAD), getpagesize())

# define SMALL_MAX_ALLOC 1024
# define SMALL_ARENA_SIZE ALIGN(100 * (SMALL_MAX_ALLOC + CHUNK_OVERHEAD), getpagesize())

/*
** Memory chunks implementation
*/

#define SETSIZE(chk, newsize)		chk->header = newsize | (chk->header & (__SIZEOF_POINTER__ - 1));
#define GETSIZE(header)				(header & ~(__SIZEOF_POINTER__ - 1))

#define SETSTATE(chk, state)		chk->header |= state
#define CLEARSTATE(chk, state)		chk->header &= ~state
#define COPYSTATE(dest, src, state) if (src->header & state) SETSTATE(dest, state); else CLEARSTATE(dest, state)

typedef enum e_arena_index { TINY, SMALL, LARGE } t_arena_index;

typedef struct s_alloc_history
{
	size_t alloc_time;
	size_t free_time;
	size_t size;
	void* address;
} t_alloc_history;

typedef struct s_mem_tracker
{
	void* arenas[3];
	size_t total_mem_usage;
	t_alloc_history* history;
} t_mem_tracker;

typedef enum e_chunk_states
{
	FREE		= 0,
	IN_USE		= 1,
	LEFT_CHUNK	= 2,
	RIGHT_CHUNK	= 4
} t_chunk_state;

typedef struct s_chunk
{
	size_t header;
	struct s_chunk* next;
	struct s_chunk* prev;
} t_chunk;

typedef struct s_large_chunk
{
	struct s_large_chunk* next;
	struct s_large_chunk* prev;
	size_t header;
} t_large_chunk;

/*
** Debug functions
*/
void print_memory_diagram();

#endif
