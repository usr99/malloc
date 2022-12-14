/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/25 22:52:07 by mamartin          #+#    #+#             */
/*   Updated: 2022/11/08 18:53:02 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _UTILS_H_
# define _UTILS_H_

# include <sys/resource.h>
# include <stdbool.h>
# include "mem_allocator.h"

/*
** Define some common utilities functions
*/
void set_chunk_footer(t_chunk* chk);
size_t align(size_t size);
t_arena_index choose_arena(size_t size);
void update_freelist(t_arena* arena, t_chunk* current, t_chunk* next, t_chunk* previous);
void freelist_push_front(t_arena* arena, t_chunk* to_insert);
t_chunk* get_near_chunk(t_chunk* current, t_chunk_state side);
void merge_chunks(t_arena* arena, t_chunk* dest, t_chunk* src, t_chunk_state direction);
bool memory_available(size_t total_usage);
t_arena* find_arena(t_arena* root, void* ref);

/*
** Multithreading utilities
*/
void lock_mutex();
void* unlock_mutex();

#endif
