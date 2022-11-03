/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/20 22:25:09 by mamartin          #+#    #+#             */
/*   Updated: 2022/11/02 21:07:07 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdio.h>
#include "include/libft_malloc.h"
#include "include/mem_allocator.h"

int main()
{
	int* p1 = malloc(2048);
	int* p2 = malloc(4081);
	int* p3 = malloc(5000);
	int* p4 = malloc(1025);
	// int* p5 = malloc(128);
	// int* p6 = malloc(256);
	// int* p7 = malloc(572);
	// int* p8 = malloc(844);
	// int* p9 = malloc(300);

	free(p2);
	// free(p3);

	// free(p4);
	// free(p5);
	// free(p2);
	// free(p6);
	// free(p1);
	// free(p4);
	// free(p2);

	// p2 = malloc(16);
	// p3 = malloc(1024);
	// p4 = malloc(24);
	// p5 = malloc(128);
	// p6 = malloc(256);

	// p3 = malloc(1032);
	// free(p5);
	// p5 = malloc(160);

	// free(p1);
	// free(p2);
	// free(p4);
	// free(p6);

	// free(p3);
	// free(p5);	

	// p = malloc(1046777);
	// p = malloc(16);
	// p = malloc(16);
	// p = malloc(16);
	// p = malloc(16);
	// p = malloc(20);
	// p = malloc(20);
	// p = malloc(20);
	// p = malloc(20);
	// p = malloc(20);
	// p = malloc(20);
	// p = malloc(1000);
	// *p = 4242;

	print_memory_diagram();

	return 0;
}
