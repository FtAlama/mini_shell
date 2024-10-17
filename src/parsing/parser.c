/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hbutt <hbutt@student.s19.be>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/29 17:54:51 by hbutt             #+#    #+#             */
/*   Updated: 2024/10/17 18:17:46 by alama            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/mini_shell.h"

//je vais t'expliquer un jour mdr
// y en a des mots....

t_node	*str_node(t_token **token)
{
	t_node	*str_node;

	if (!token || (*token)->type != CHAR_TOKEN)
		return (NULL);
	str_node = malloc(sizeof(t_node));
	// if (!str_node)
	str_node->type = STR_NODE;
	str_node->data.str = (*token)->lexeme;
	str_node->nb = (*token)->nb;
	*token = (*token)->next;
	return (str_node);
}

t_node	*right_node(t_token *token)
{
	t_node	*str_node;

	str_node = malloc(sizeof(t_node));
	// if (!str_node)
	str_node->type = STR_NODE;
	str_node->data.str = token->lexeme;
	str_node->nb = token->nb;
	return (str_node);
}

t_node	*create_dir_node(t_node *s_node, t_token *token)
{
	t_node	*dir_node;

	dir_node = malloc(sizeof(t_node));
	// if (!dir_node)
	dir_node->type = PAIR_NODE;
	dir_node->data.pair.left = s_node;
	dir_node->data.pair.opera = token->lexeme;
	dir_node->nb = token->nb;
	return (dir_node);
}

t_node	*parse_redir(t_token **token)
{
	t_node	*s_node;
	t_node	*dir_node;
	char	*str;

	s_node = str_node(token);
	while ((*token)->type == STR_NODE)
	{
		*token = (*token)->next;
	}
	while (*token && ((*token)->type == O_DIR || (*token)->type == I_DIR
				|| (*token)->type == OA_DIR || (*token)->type == DI_DIR))
	{
		dir_node = create_dir_node(s_node, *token);
		*token = (*token)->next;
		dir_node->data.pair.right = right_node(*token);
		s_node = dir_node;
		*token = (*token)->next;
	}
	return (s_node);
}

t_node	*create_pipe_node(t_node *left, t_token *token)
{
	t_node	*pipe;

	pipe = malloc(sizeof(t_node));
	// if (!pipe)
	pipe->type = PAIR_NODE;
	pipe->data.pair.left = left;
	pipe->data.pair.opera = token->lexeme;
	pipe->nb = token->nb;
	return (pipe);
}

t_node	*parse_pipe(t_token **token)
{
	t_node	*left;
	t_node	*pipe;
	t_token	*tmp;
	char	*str;

	tmp = *token;
	left = parse_redir(&tmp);
	while (tmp->type != PIPE && tmp->type != END_TOKEN)
	{
		str = ft_strjoin(left->data.str, (*token)->lexeme);
		tmp = tmp->next;
		free(left->data.str);
		left->data.str = str;
	}
	while (tmp->type != END_TOKEN && tmp->type == PIPE)
	{
		pipe = create_pipe_node(left, tmp);
		tmp = tmp->next;
		pipe->data.pair.right = parse_redir(&tmp);
		left = pipe;
	}
	return (left);
}

t_node	*parse(t_token **token_list)
{
	t_node	*left;
	
	if (!*token_list || (*token_list)->type == END_TOKEN)
		return (NULL);
	left = parse_pipe(token_list);
	return (left);
}

void	execute_node(t_node *node)
{
	if (!node)
		return ;
	if (node->type == STR_NODE)
	{
		printf("Exécution : %s\n", node->data.str);
	}
	else if (node->type == PAIR_NODE)
	{
		if (node->data.pair.opera && node->data.pair.opera[0] == '|')
		{
			printf("Pipe entre deux commandes\n");
			execute_node(node->data.pair.left);
			execute_node(node->data.pair.right);
		}
		else if (node->data.pair.opera && (node->data.pair.opera[0] == '>' || node->data.pair.opera[0] == '<'))
		{
			printf("Redirection : %s\n", node->data.pair.opera);
			execute_node(node->data.pair.left);
			printf("Cible de la redirection : %s\n", node->data.pair.right->data.str);
		}
	}
}

static void	print_str_node(t_node *node)
{
	if (node->data.str)
		printf("STR_NODE:	%s	%d\n", node->data.str, node->nb);
}

static void	print_pair_node(t_node *node)
{
	if (node->data.pair.opera)
		printf("PAIR_NODE:	Operator %s	%d\n", node->data.pair.opera, node->nb);
}

void	print_node(t_node *node)
{

	if (!node)
		return;
	if (node->type == STR_NODE)
	{
		print_str_node(node);
	}
	else if (node->type == PAIR_NODE)
	{
		print_pair_node(node);
		if (node->data.pair.left)
		{
			printf("LEFT NODE:\n");
			print_node(node->data.pair.left);
		}
		if (node->data.pair.right)
		{
			printf("RIGHT NODE:\n");
			print_node(node->data.pair.right);
		}
	}
}
