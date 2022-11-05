/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/20 22:25:09 by mamartin          #+#    #+#             */
/*   Updated: 2022/11/05 18:10:44 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "include/libft_malloc.h"
#include "include/mem_allocator.h"

int main()
{
	// int* p1 = malloc(80);
	// int* p2 = malloc(256);
	// int* p3 = malloc(256);
	// int* p4 = malloc(256);
	// int* p5 = malloc(64);
	// int* p6 = malloc(121);

	// int i;
	// char* q = malloc(80);
	// char* t[297];
	// for (i = 0; i < 297; i++)
	// 	t[i] = malloc(80);
	// int* p1 = malloc(48);
	// free(q);

	// int* p2 = malloc(104);
	// free(p2); // free first of arena 2

	// for (i = 0; i < 296; i++)
	// 	free(t[i]);
	// free(t[296]);
	// free(p1); // free last of first arena 1

	// free(p1);
	// free(p3);
	// free(p2);
	// free(p4);
	// free(p5);
	// free(p6);

	print_memory_diagram();

	return 0;
}
