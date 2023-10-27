#include "shell.h"

t_node* create_node(void *data)
{
    t_node *new_node = (t_node *)malloc(sizeof(t_node));
    if (new_node == NULL)
    {
        ft_perror("Memory allocation failed\n", EXIT_FAILURE);
        return NULL;
    }
    new_node->data = data;
    // new_node->data = strdup(data); //?
    new_node->next = NULL;
    return new_node;
}

// t_node* create_node(t_file *file)
// {
//     t_node *new_node = (t_node *)malloc(sizeof(t_node));
//     if (new_node == NULL)
//     {
//         ft_perror("Memory allocation failed\n", EXIT_FAILURE);
//         return NULL;
//     }
//     // new_node->data = data;
//     new_node->data = file;
//     new_node->next = NULL;
//     return new_node;
// }


void *create_list()
{
    t_list *new_list = (t_list *)malloc(sizeof(t_list));
    if (new_list == NULL)
    {
        perror("Malloc error while creating list.");
        return NULL;
    }
    new_list->top = NULL;

    return new_list;
}

void insert_element(t_list *list, void *data)
{
    t_node *new_node = create_node(data);

    if (!list->top)
        list->top = new_node;
    else
    {
        t_node *tmp = list->top;
        while (tmp->next != NULL)
            tmp = tmp->next;
        tmp->next = new_node;
    }
}

void insert_open_file(t_list *list, t_file *file)
{
    t_node *new_node = create_node(file);
    t_node *prev = NULL;

    if (list->top == NULL)
        list->top = new_node;
    else
    {
        t_node *tmp = list->top;
        while(tmp)
        {
            t_file *f = (t_file *)tmp->data;
            if (!f)
            {
                perror("Wrong list to insert an open file");
                return;
            }
            if (f->fd > file->fd)
            {
                if (!prev)
                {
                    new_node->next = list->top;
                    list->top = new_node;
                }
                else
                {
                    prev->next = new_node;
                    new_node->next = tmp;
                }
                return;
            }
            prev = tmp;
            tmp = tmp->next;
        }
        if (!tmp && prev)
            prev->next = new_node;
    }

}

void remove_element(t_list *list, char *data)
{
    t_node *tmp = list->top;
    t_node *prev = NULL;

    while (tmp != NULL && strcmp(tmp->data, data) != 0)
    {
        prev = tmp;
        tmp = tmp->next;
    }
    if (!tmp)
        printf("Not such element in the list");
    else
    {
        if (!prev)
            list->top = tmp->next;
        else
            prev->next = tmp->next;
        free(tmp->data);
        free(tmp);
    }
}

void remove_open_file(t_list *list, int fd)
{
    t_node *curr = list->top;
    t_node *prev = NULL;

    while (curr)
    {
        t_file *f = (t_file *)curr->data;
        if (!f)
        {
            perror("Wrong list given to remove an open file");
            return ;
        }
        if (f->fd == fd)
        {
            if (!prev)
                list->top = curr->next;
            else
                prev->next = curr->next;
            free(f->file_name);
            free(curr->data);
            free(curr);
            break;
        }
        prev = curr;
        curr = curr->next;
    }
}

void remove_top(t_list *list)
{
    t_node *tmp = list->top;

    if (list->top != NULL) {
        list->top = tmp->next;
        free(tmp->data);
        free(tmp);
    }

}

void destroy_list(t_list *list, int file)
{
    t_node *current = list->top;
    t_node *next;

    while (current != NULL)
    {
        next = current->next;
        if (file)
            free(((t_file *)current->data)->file_name);
        free(current->data);
        free(current);
        current = next;
    }
    free(list);
}

char *file_name(t_list *list, int fd)
{
    t_node *tmp = list->top;

    while (tmp)
    {
        t_file *file = tmp->data;
        if (!file)
            return NULL;
        if (file->fd == fd)
            return file->file_name;
        tmp = tmp->next;
    }
    return NULL;
}



