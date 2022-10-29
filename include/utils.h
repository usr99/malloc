/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/25 22:52:07 by mamartin          #+#    #+#             */
/*   Updated: 2022/10/28 21:02:27 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _UTILS_H_
# define _UTILS_H_

# include <stdbool.h>
# include "mem_allocator.h"

/*
** Define some common utilities functions
*/
void set_chunk_footer(t_chunk* chk);
size_t align(size_t size);
t_arena_index choose_arena(size_t size);
void update_freelist(t_arena_hdr* arena, t_chunk* current, t_chunk* next, t_chunk* previous);
t_chunk* get_next_chunk(t_chunk* current);
t_chunk* get_previous_chunk(t_arena_hdr* arena, t_chunk* current);
void merge_chunks(t_arena_hdr* arena, t_chunk* dest, t_chunk* src, bool update_links);

#endif
