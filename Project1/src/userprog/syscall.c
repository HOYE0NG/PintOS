#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "userprog/process.h"
#include "threads/vaddr.h"


static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

// proj1 syscall func
int fibonacci(int n)
{
    int prev = 0;
    int curr = 1;
    int next = 0;
    if (n == 0) {
        return 0;
    }
    if (n == 1) {
        return 1;
    }

    for (int i = 2; i <= n; i++) {
        next = prev + curr;
        prev = curr;
        curr = next;
    }

    return curr;
}

int max_of_four_int(int a, int b, int c, int d)
{
    int arr[4];
    arr[0] = a;
    arr[1] = b;
    arr[2] = c;
    arr[3] = d;
    int max = a;
    for (int i = 1; i < 4; i++) {
        if (arr[i] > max) {
            max = arr[i];
        }
    }
    return max;
}


void halt() {
    shutdown_power_off();
}

void exit(int status) {
    printf("%s: exit(%d)\n", thread_name(), status);
    thread_current()->exit_stat = status;
    thread_exit();
}

tid_t exec(const char* command) {
    return process_execute(command);
}

int wait(tid_t tid) {
    return process_wait(tid);
}

int read(int fd, const void* buff, unsigned size) {

    int result;
    if (fd == 0) {
        for (int i = 0; i < size; i++) {
            if (((char*)buff)[i] == '\0') {
                result = i;
                break;
            }
        }
    }
    return result;
}

int write(int fd, const void* buff, unsigned size) {
    if (fd == 1) {
        putbuf(buff, size);
        return size;
    } else{
        return -1;
        }
}

void checking_valid_address(const void* addr) {

    if (!is_user_vaddr(addr)) {
        exit(-1);
    }
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
    //printf("syscall : %d\n", *(uint32_t*)(f->esp));
    //hex_dump(f->esp, f->esp, 100, 1);
    //printf("%p\n", f->esp);
    switch (*(uint32_t*)(f->esp)) {
    case SYS_HALT:
        halt();
        break;
    case SYS_EXIT:
        checking_valid_address(f->esp + 4);
        exit(*(uint32_t*)(f->esp + 4));
        break;
    case SYS_EXEC:
        checking_valid_address(f->esp + 4);
        f->eax = exec((const char*)*(uint32_t*)(f->esp + 4));
        break;
    case SYS_WAIT:
        checking_valid_address(f->esp + 4);
        f->eax = wait((tid_t) * (uint32_t*)(f->esp + 4));
        break;
    case SYS_READ:
        checking_valid_address(f->esp + 20);
        checking_valid_address(f->esp + 24);
        checking_valid_address(f->esp + 28);
        read((int)*(uint32_t*)(f->esp + 20), (void*)*(uint32_t*)(f->esp + 24), (unsigned)*((uint32_t*)(f->esp + 28)));
        break;
    case SYS_WRITE:
        checking_valid_address(f->esp + 20);
        checking_valid_address(f->esp + 24);
        checking_valid_address(f->esp + 28);
        f->eax = write((int)*(uint32_t*)(f->esp + 20), (void*)*(uint32_t*)(f->esp + 24), (unsigned)*((uint32_t*)(f->esp + 28)));
        break;
    case SYS_FIBONACCI:
        //printf("\n\nthis is fibo\n\n");
        checking_valid_address(f->esp + 4);
        f->eax = fibonacci(*(int*)(f->esp + 4));
        break;
    case SYS_MAX_FOUR_INT:
        checking_valid_address(f->esp + 4);
        f->eax = max_of_four_int(*(int*)(f->esp + 4), *(int*)(f->esp + 8), *(int*)(f->esp + 12), *(int*)(f->esp + 16));
        break;
    }
    return;
}
