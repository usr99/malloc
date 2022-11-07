/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/20 22:25:09 by mamartin          #+#    #+#             */
/*   Updated: 2022/11/07 12:03:02 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "libft/libft.h"
#include "include/libft_malloc.h"
#include "include/mem_allocator.h"

#if 0
int main()
{
	// int *p1 = malloc(80);
	// int *p2 = malloc(1024 * 1024);
	// int *p3 = malloc(1024 * 1024 * 32);
	// int *p3 = malloc(16);
	// int *p4 = malloc(424);
	// int *p5 = malloc(64);
	// int *p6 = malloc(128);

	// print_memory_diagram();

	// void* p = malloc(1024);
	// p = malloc(1024 * 32);
	// p = malloc(1024 * 1024);
	// p = malloc(1024 * 1024 * 16);
	// p = malloc(1024 * 1024 * 128);
	// show_alloc_mem();

	// free(p1);
	// free(p2);
	// free(p3);
	// free(p4);
	// free(p5);
	// free(p6);

	return 0;
}
#endif

#include <string.h>

int main()
{
	// long* l = malloc(16);
	// *l = 0xFEEDBEEF;

	// char* str = malloc(32);
	// strcpy(str, "Hello, World!\n");

	// int* aint = malloc(24);
	// aint[0] = 5;
	// aint[1] = 0;
	// aint[2] = 32;
	// aint[3] = 64;
	// aint[4] = 128;
	// aint[5] = 72;

	// show_alloc_mem_hex();

	void* p = malloc(1024);
	p = malloc(1024 * 32);
	p = malloc(1024 * 1024);
	p = malloc(1024 * 1024 * 16);
	p = malloc(1024 * 1024 * 128);
	show_alloc_mem();	
	return (0);
}