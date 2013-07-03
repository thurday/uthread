#include <malloc.h>
#include <ucontext.h>
#include <stdio.h>

// 64kB stack
#define FIBER_STACK 1024*64

ucontext_t child1, child2, parent;

// The child thread will execute this function
void threadFunction(ucontext_t* current)
{
    int i = 0 ;
    printf("Child :%d\n",i);
    i++;
    swapcontext(current, &parent);
    printf("Child :%d\n",i);
    i++;
    swapcontext(current, &parent);
    printf("Child :%d\n",i);
}

int main()
{
    // Get the current execution context
    getcontext(&child1);
    // Modify the context to a new stack
    child1.uc_link = &parent;//&parent;
    child1.uc_stack.ss_sp = malloc(FIBER_STACK);
    child1.uc_stack.ss_size = FIBER_STACK;
    child1.uc_stack.ss_flags = 0;
    if(child1.uc_stack.ss_sp == 0){
        perror("malloc: Could not allocate stack");
        return 1;
    }

    // Get the current execution context
    getcontext(&child2);
    // Modify the context to a new stack
    child2.uc_link = &parent;
    child2.uc_stack.ss_sp = malloc(FIBER_STACK);
    child2.uc_stack.ss_size = FIBER_STACK;
    child2.uc_stack.ss_flags = 0;
    if(child2.uc_stack.ss_sp == 0){
        perror("malloc: Could not allocate stack");
        return 1;
    }

    // Create the new context
    printf("Creating child1 fiber\n");
    makecontext(&child1, (void (*)(void))threadFunction, 1, &child1);
      // Create the new context
    printf("Creating child2 fiber\n");
    makecontext(&child2, (void (*)(void))threadFunction, 1, &child2);

    // Execute the child1 context
    printf("Switching to child1:1\n");
    swapcontext(&parent, &child1);

    printf("Switching to child1:2\n");
    swapcontext(&parent, &child1);

    // Execute the child2 context
    printf("Switching to child2 :1\n");
    swapcontext(&parent, &child2);

    // Execute the child1 context
    printf("Switching to child1:3\n");
    swapcontext(&parent, &child1);

    // Execute the child2 context
    printf("Switching to child2 :2\n");
    swapcontext(&parent, &child2);

    printf("Switching to child2 :3\n");
    swapcontext(&parent, &child2);

    // Free the stack
    free(child1.uc_stack.ss_sp);
    free(child2.uc_stack.ss_sp);
    printf("Childs fiber returned and stack freed\n");
    return 0;
}
