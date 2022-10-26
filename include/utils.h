/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/25 22:52:07 by mamartin          #+#    #+#             */
/*   Updated: 2022/10/25 22:57:41 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _UTILS_H_
# define _UTILS_H_

/*
** Define some common utilities functions
*/
void set_chunk_footer(t_chunk* chk);
size_t align(size_t size);
void update_freelist(t_chunk* current, t_chunk* next, t_chunk* previous);

#endif
