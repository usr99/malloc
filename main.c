/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/20 22:25:09 by mamartin          #+#    #+#             */
/*   Updated: 2022/11/05 21:13:46 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "include/libft_malloc.h"
#include "include/mem_allocator.h"

int main()
{
	// {
	// 	int* p1 = malloc(80);
	// 	int* p2 = malloc(240);
	// 	int* p3 = malloc(16);
	// 	int* p4 = malloc(424);
	// 	int* p5 = malloc(64);
	// 	int* p6 = malloc(128);

	// 	free(p1);
	// 	free(p2);
	// 	free(p3);
	// 	free(p4);
	// 	free(p5);
	// 	free(p6);		
	// }

	// {
	// 	int* p1 = malloc(1024 * 80);
	// 	int* p2 = malloc(1024 * 240);
	// 	int* p3 = malloc(1024 * 16);
	// 	int* p4 = malloc(1024 * 424);
	// 	int* p5 = malloc(1024 * 64);
	// 	int* p6 = malloc(1024 * 128);

	// 	free(p1);
	// 	free(p2);
	// 	free(p3);
	// 	free(p4);
	// 	free(p5);
	// 	free(p6);
	// }

	// {
	// 	int* p1 = malloc(1024 * 1024 * 80);
	// 	int* p2 = malloc(1024 * 1024 * 240);
	// 	int* p3 = malloc(1024 * 1024 * 16);
	// 	int* p4 = malloc(1024 * 1024 * 424);
	// 	int* p5 = malloc(1024 * 1024 * 64);
	// 	int* p6 = malloc(1024 * 1024 * 128);

	// 	free(p1);
	// 	free(p2);
	// 	free(p3);
	// 	free(p4);
	// 	free(p5);
	// 	free(p6);
	// }

	/* Fill first arena */
	int i;
	char* t[102];
	for (i = 0; i < 102; i++)
		t[i] = malloc(1024);
	int* p1 = malloc(368);

	int* p2 = malloc(104);
	free(p2); // free first of arena 2

	for (i = 0; i < 101; i++)
		free(t[i]);
	free(t[101]);
	free(p1); // free last of first arena 1

	print_memory_diagram();

	return 0;
}
