/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mandatory.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamartin <mamartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/17 22:38:01 by mamartin          #+#    #+#             */
/*   Updated: 2022/11/06 17:21:21 by mamartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _MANDATORY_H_
# define _MANDATORY_H_

#include <unistd.h>
#include <stdlib.h>

typedef struct	s_list
{
	void			*content;
	struct s_list	*next;
}				t_list;

typedef struct	s_btree
{
	struct s_btree	*left;
	struct s_btree	*right;
	void			*item;
}				t_btree;

/*
**	LIBC FUNCTIONS
*/

void			*ft_memset(void *s, int c, size_t n);
void			ft_bzero(void *s, size_t n);
void			*ft_memcpy(void *dest, const void *src, size_t n);
void			*ft_memccpy(void *dest, const void *src, int c, size_t n);
void			*ft_memmove(void *dest, const void *src, size_t n);
void			*ft_memchr(const void *s, int c, size_t n);
int				ft_memcmp(const void *s1, const void *s2, size_t n);
size_t			ft_strlen(const char *s);
int				ft_isalpha(int c);
int				ft_isdigit(int c);
int				ft_isalnum(int c);
int				ft_isascii(int c);
int				ft_isprint(int c);
int				ft_toupper(int c);
int				ft_tolower(int c);
char			*ft_strchr(const char *s, int c);
char			*ft_strrchr(const char *s, int c);
int				ft_strncmp(const char *s1, const char *s2, size_t n);
size_t			ft_strlcpy(char *dst, const char *src, size_t size);
size_t			ft_strlcat(char *dst, const char *src, size_t size);
char			*ft_strnstr(const char *big, const char *little, size_t len);
int				ft_atoi(const char *nptr);
void			*ft_calloc(size_t nmenb, size_t size);
char			*ft_strdup(const char *s);

/*
**	MORE FUNCTIONS
*/

char			*ft_substr(char const *s, unsigned int start, size_t len);
char			*ft_strjoin(char const *s1, char const *s2);
char			*ft_strtrim(char const *s1, char const *set);
char			**ft_split(char const *s, char c);
char			*ft_itoa(int n);
char			*ft_strmapi(char const *s, char (*f)(unsigned int, char));
void			ft_putchar_fd(char c, int fd);
void			ft_putstr_fd(const char *s, int fd);
void			ft_putendl_fd(const char *s, int fd);
void			ft_putnbr_fd(int n, int fd);
void			ft_putchar(char c);
void			ft_putstr(const char *s);
void			ft_putendl(const char *s);
void			ft_putnbr(int n);

/*
**	LINKED LIST FUNCTIONS
*/

t_list			*ft_lstnew(void *content);
void			ft_lstadd_front(t_list **alst, t_list *new);
int				ft_lstsize(t_list *lst);
t_list			*ft_lstlast(t_list *lst);
void			ft_lstadd_back(t_list **alst, t_list *new);
void			ft_lstdelone(t_list *lst, void (*del)(void *));
void			ft_lstclear(t_list **lst, void (*del)(void *));
void			ft_lstiter(t_list *lst, void (*f)(void *));
t_list			*ft_lstmap(t_list *lst, void *(*f)(void *),
					void (*del)(void *));
t_list			*ft_lstdup(t_list *src, int length);
/*
**	BINARY TREES FUNCTIONS
*/

t_btree			*btree_create_node(void *item);
void			btree_apply_prefix(t_btree *root, void (*applyf)(void *));
void			btree_apply_suffix(t_btree *root, void (*applyf)(void *));
void			btree_apply_infix(t_btree *root, void (*applyf)
					(void *));
void			btree_insert_data(t_btree **root, void *item,
					int (*cmpf)(void *, void *));
void			*btree_search_item(t_btree *root, void *data_ref,
					int (*cmpf)(void *, void *));
void			btree_clear(t_btree **root, void (*del)(void *));

#endif
