/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libft_malloc.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/20 22:25:31 by mamartin          #+#    #+#             */
/*   Updated: 2022/11/06 00:53:00 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _LIBFT_MALLOC_H_
# define _LIBFT_MALLOC_H_

# include <stdlib.h>
# include <stdint.h>

void*	malloc(size_t size);
void*	realloc(void *ptr, size_t size);
void	free(void *ptr);
void	show_alloc_mem();

void	show_alloc_mem_hex();
void	show_alloc_history();

#endif
