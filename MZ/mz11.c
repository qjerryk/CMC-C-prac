#include <stdio.h>
#include <stdlib.h>
#include <string.h>



struct Node{
	char *value;
	struct Node *next;
};

void print_list(struct Node *list){
	while (list != NULL){
		printf("%s ", list -> value);
		list = list -> next;
	}
}

void free_list(struct Node **list){
	struct Node *tmp = NULL;
	while ((*list) -> next != NULL){
		tmp = *list;
		*list = (*list) -> next;
		free(tmp -> value);
		free(tmp);
	}
	free(*list);
}

char* read_word(char *c){
	char *word = (char*)calloc(21, sizeof(char));
    if (!word){
        return word;
    }
	size_t ind = 0;
	while ((*c = getchar()) != ' ' && *c != '\n' && *c != EOF){
		word[ind] = *c;
		ind++;
	}
	word[ind]='\0';
	return word;
}

struct Node* init_node(char *word){
	struct Node *list = (struct Node*)malloc(sizeof(struct Node));
    if (!list){
        return list;
    }
	list -> value = word;
	list -> next = NULL;
	return list;
}

struct Node* add_element(struct Node *list, char *word){
	struct Node *p;
	if (list == NULL){
		list = init_node(word);
	}
	else{
		p = list;
		while (p -> next != NULL){
			p = p -> next;
		}
		p -> next = init_node(word);

	}
	return list;
}

struct Node* Reading(){
	char c = ' ';
	char *word;
	struct Node *list = NULL;
	while (c != EOF && c != '\n'){
		word = read_word(&c);
		if(!strlen(word)){
			break;
		}
		list = add_element(list, word);
	}
	return list;
}

int delete_node(struct Node **list, char* badword){
	struct Node *tmp = *list, *pred = NULL;
	int is_del = 0;
	if (*list == NULL)
		return 1;
	while (tmp -> next != NULL){
		if (!strcmp(tmp -> value, badword) && !is_del){
			if (pred){
				pred -> next = tmp -> next;
				free(tmp -> value);
				free(tmp);
				tmp = pred -> next;
				is_del = 1;
				continue;
			}else{
				*list = tmp -> next;
				free(tmp -> value);
				free(tmp);
				tmp = *list;
				is_del = 1;
				continue;
			}
		}
		pred = tmp;
		tmp = tmp -> next;
	}
	if (!strcmp(tmp -> value, badword) && !is_del){
		if (pred){
			pred -> next = NULL;
			free(tmp);
		}
	}
	return is_del;
}

int main(){
	char trash = ' ';
	char *badword = read_word(&trash);
	if (badword[0] == '\0'){
		free(badword);
		badword = " ";
	}

	struct Node *list = Reading();
	if (list == NULL){
        if (!strcmp(badword, " "))
		    free(badword);
		printf("empty");
		return 0;
	}
	int is_del = delete_node(&list, badword);
//	if (list != NULL)
//		print_list(list);
	if (!strcmp(list -> value, badword) && !is_del){
		free(list);
		free(badword);
		printf("empty");
		return 0;
	}
	print_list(list);
//	if (list != NULL)
	free_list(&list);
	return 0;
}
