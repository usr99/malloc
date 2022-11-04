/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/20 22:25:09 by mamartin          #+#    #+#             */
/*   Updated: 2022/11/04 19:14:15 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "include/libft_malloc.h"
#include "include/mem_allocator.h"

int main()
{
	int* p1 = malloc(2048);
	int* p2 = malloc(4096);
	int* p3 = malloc(8192);
	// int* p6 = malloc(256);
	// int* p7 = malloc(64);
	// int* p8 = malloc(121);
	// int* p9 = malloc(300);

	free(p1);
	free(p2);
	free(p3);
	// free(p4);
	// free(p5);
	// free(p6);
	// free(p7);

	print_memory_diagram();

	return 0;
}
