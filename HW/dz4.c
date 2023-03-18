#include <stdio.h>
#include <limits.h>
#include <stdlib.h>

struct Node{
	FILE *file;
	int value;
	struct Node *next;
};

void init_node(struct Node **list, char *filename){
	*list = (struct Node*)malloc(sizeof(struct Node));
	(*list) -> file = fopen(filename, "r");
	if ((*list) -> file != NULL)
		fscanf((*list) -> file, "%d", &((*list) -> value));
	(*list) -> next = NULL;
}

struct Node *init_list(int argc, char **argv){
	size_t ind;
	struct Node *list = NULL;
	struct Node *ptr = NULL;
	for (ind =  1; ind < argc; ind++){
		if (!list){
			init_node(&list,argv[ind]);
			ptr = list;
		}else{
			init_node(&(ptr->next),argv[ind]);
			ptr = ptr->next;
		}
	}
	return list;
}


void free_list(struct Node **list){
	struct Node *ptr;
	if (*list == NULL){
		return;
	}
	while ((*list) -> next != NULL){
		ptr = *list;
		fclose(ptr -> file);
		(*list) = (*list) -> next;
		free(ptr);
	}
	if (*list != NULL){
		free(*list);
	}
}
/*
char reading(struct Node *list){
	char is_reading = 0;
	if (list == NULL){
		return is_reading;
	}
	while (list -> next != NULL){
		if (list -> file && !feof(list -> file)){
			is_reading = 1;
			break;
		}
		list = list -> next;
	}
	if (list -> file && !feof(list -> file)){
		is_reading = 1;
	}
	return is_reading;
}
*/
int find_min(struct Node **list){
	int res = INT_MAX;
	struct Node *min_ptr = NULL, *ptr = *list;
	while (ptr -> next != NULL){
		if (!feof(ptr -> file)){
			if ( res > (ptr -> value)){
				res = ptr -> value;
				min_ptr = ptr;
			}
		}
		ptr = ptr -> next;
	}
	if (!feof(ptr -> file)){
		if (res > (ptr -> value)){
			res = ptr -> value;
			min_ptr = ptr;
		}	
	}
    if (!min_ptr){
        return 0;
    }
	printf("%d ", res);
	fscanf(min_ptr -> file, "%d", &(min_ptr -> value));
    return 1;
}

void merging_files(int argc, char **argv){
	struct Node *list = init_list(argc, argv);
    int is_reading = 1;
	while(1){
		is_reading = find_min(&list);
        if (!is_reading)
            break;
	}
	free_list(&list);
}

int main(int argc, char **argv){
	merging_files(argc, argv);
	printf("\n");
	return 0;
}
