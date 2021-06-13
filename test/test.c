#include <stdio.h>
#include <assert.h>
#include <stddef.h>

#include <setjmp.h>
#include <stdlib.h>

#include <sys/resolve_symbol.h>

#include <buildin/list.h>

jmp_buf buf;

void enumerate(struct list_node_t* node) {
	printf("Node: 0x%x, %d\n", node, node->data);
}

void main() {
	printf("Hello world %d\n", 100);

	printf("task_entry.exit: 0x%x\n", resolve_symbol("task_entry.exit"));
	void* m1 = malloc(100);
	void* m2 = malloc(100);
	void* m3 = malloc(100);
	void* m4 = malloc(100);
	void* m5 = malloc(100);
	void* m6 = malloc(100);
	printf("Malloc test: 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n", m1, m2, m3, m4, m5, m6);

	free(m1);
	free(m6);
	free(m4);

	struct list_node_t* head = NULL;

	head = __libc_list_create(0, NULL);
	__libc_list_append(1, head);
	__libc_list_append(2, head);
	__libc_list_append(3, head);

	__libc_list_traverse(head, enumerate);
	printf("\n\n");

	head = __libc_list_remove(head, __libc_list_search(head, 2));
	__libc_list_traverse(head, enumerate);
	printf("\n\n");

	head = __libc_list_remove(head, __libc_list_search(head, 0));
	__libc_list_traverse(head, enumerate);
	printf("\n\n");

	head = __libc_list_remove(head, __libc_list_search(head, 3));
	__libc_list_traverse(head, enumerate);
	printf("\n\n");

	__libc_list_dispose(head);

	int x = 5;

	setjmp(buf);

	printf("Loop %d!\n", x);
	x--;

	assert(x != 0);

	longjmp(buf, 1);
}
