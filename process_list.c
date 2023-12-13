#include "shell.h"

process_list *create_process_list()
{
	process_list *new_list = (process_list *)malloc(sizeof(process_list));
	if (new_list == NULL)
	{
		perror("Malloc error while creating list.");
		return NULL;
	}
	new_list->top = NULL;

	return new_list;
}

process_block *create_process_block(pid_t pid, char *cmd[])
{
	process_block *new_block = (process_block *)malloc(sizeof(process_block));
	if (new_block == NULL)
	{
		perror("Memory allocation failed");
		return NULL;
	}

	new_block->pid = pid;
	store_command(new_block, cmd);
	new_block->owner_id = getuid();
	new_block->status = ACTIVE;
	new_block->ret = 0;
	// new_block->time = strdup(current_time());
	time(&new_block->time);
	return new_block;
}

int insert_process_block(process_list *list, pid_t pid, char *cmd[])
{
	process_block *new_block = create_process_block(pid, cmd);
	if (new_block != NULL)
	{
		new_block->next = list->top;
		list->top = new_block;
		return 1;
	}
	return 0;
}

void empty_process_list(process_list *list)
{
	process_block *current = list->top;
	process_block *next;

	while (current != NULL)
	{
		next = current->next;
		if (current->cmd)
			free(current->cmd);
		free(current);
		current = next;
	}
	list->top = NULL;
}

void destroy_process_list(process_list *list)
{
	empty_process_list(list);
	free(list);
}

void remove_process_block(process_list *list, process_block *block)
{	
	process_block *tmp = list->top;
	process_block *prev = NULL;

	if (list == NULL || block == NULL)
		return;
	while (tmp != block)
	{
		prev = tmp;
		tmp = tmp->next;
	}
	if (!prev)
		list->top = tmp->next;
	else
		prev->next = tmp->next;
	if (tmp->cmd)
		free(tmp->cmd);
	free(tmp);
}

void remove_process_blocks(process_list *list, int term, int sig)
{
	process_block *tmp = list->top;
	process_block *prev = NULL;
	process_block *next;

	while (tmp != NULL)
	{
		check_status(tmp);
		if((term && tmp->status == FINISHED) || (sig && tmp->status == SIGNALED))
		{
			next = tmp->next;
			if (!prev)
				list->top = tmp->next;
			else
				prev->next = tmp->next;
			if (tmp->cmd)
				free(tmp->cmd);
			free(tmp);
			tmp = next;
		}
		else
		{
			prev = tmp;
			tmp = tmp->next;
		}
	}
}

// void remove_prblock_helper(process_list *list, process_block *tmp, process_block *prev)
// {
// 	if (tmp) {
// 		pid_t p = tmp->pid;
// 		if (!prev)
// 			list->top = tmp->next;
// 		else
// 			prev->next = tmp->next;
// 		if (tmp->cmd)
// 			free(tmp->cmd);
// 		free(tmp);
// 		printf("Process with pid %ld removed\n", (long)p);
// 	}
// }

void print_process_list(process_list *list)
{
	if (list == NULL)
		return;
	process_block *tmp = list->top; 
	while (tmp != NULL)
	{
		check_status(tmp);
		process_info(tmp);
		tmp = tmp->next;
	}
}

process_block *find_process_block(process_list *list, pid_t pid)
{
	if (list == NULL)
		return NULL;
	process_block *tmp = list->top;
	while (tmp != NULL && tmp->pid != pid)
		tmp = tmp->next;
	return tmp;
}


