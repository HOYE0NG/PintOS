#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "userprog/process.h"
#include "threads/vaddr.h"
#include "filesys/off_t.h"
#include "filesys/file.h"


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

    /* user prog 2 */
    for(int i=3; i<128; i++){
        if(thread_current()->fd[i]) {
            close(i);
        }
    }

    thread_exit();
}

tid_t exec(const char* command) {
    return process_execute(command);
}

int wait(tid_t tid) {
    return process_wait(tid);
}





/* addtional user prog 2 fucntion start */
bool create (const char* file, unsigned size) {
    if(!file){
        exit(-1);
    }else{
        checking_valid_address(file);
        return filesys_create(file, size);
    }
}

bool remove (const char* file) {
    if(!file){
        exit(-1);
    }else{
        checking_valid_address(file);
        return filesys_remove(file);
    }    
}

int open (const char* file) {
    if(!file){
        exit(-1);
    }else{
        checking_valid_address(file);
        lock_acquire(&f_lock);
        struct file* fp = filesys_open(file);
        if (!fp) {
            lock_release(&f_lock);
            return -1;
        } 
        for (int i = 3; i < 128; i++) {
            if (!(thread_current()->fd[i])) {
                if (!strcmp(thread_current()->name, file)) {
                    file_deny_write(fp);
                    }   
                thread_current()->fd[i] = fp; 
                lock_release(&f_lock);
                return i;
            }   
        }   
        lock_release(&f_lock);
        return -1; 
    }
}

int filesize (int fd) {
    if(!(thread_current()->fd[fd])){
        exit(-1);
    }else{
        return file_length(thread_current()->fd[fd]);
    }
}

int read(int fd, const void* buff, unsigned size) {
    // if (size == 0){ // read-zero
    //     return 0;
    // }
    lock_acquire(&f_lock);
    int result;
    if(fd==0){
        for(int i=0; i<size; i++){
            uint8_t c = input_getc();
            if(c=='\0') {
               result = i;
               break; 
            }
        }
    } else if(fd > 2){
        if(!thread_current()->fd[fd]){
            lock_release(&f_lock);
            exit(-1);
        }
        lock_release(&f_lock);
        return file_read(thread_current()->fd[fd], buff, size);
    } else {
        lock_release(&f_lock);
        return -1;
    }
}

int write(int fd, const void* buff, unsigned size) {
    checking_valid_address(buff);
    lock_acquire(&f_lock);
    if (fd == 1) {
        putbuf(buff, size);
        lock_release(&f_lock);
        return size;
    }else if (fd > 2){
        if (!(thread_current()->fd[fd])) {
            lock_release(&f_lock);
            exit(-1);
        }
        if (thread_current()->fd[fd]->deny_write) {
            file_deny_write(thread_current()->fd[fd]);
    }
        lock_release(&f_lock);
        return file_write(thread_current()->fd[fd], buff, size);
    }
    lock_release(&f_lock);
    return -1;
}


void seek (int fd, unsigned place) {
    if(!(thread_current()->fd[fd])){
        exit(-1);
    }else{
        file_seek(thread_current()->fd[fd], place);
    }
}

unsigned tell (int fd) {
    if(!(thread_current()->fd[fd])){
        exit(-1);
    }else{
        return file_tell(thread_current()->fd[fd]);
    }

}

void close (int fd) {
    if(!(thread_current()->fd[fd])){
        exit(-1);
    }else{
        struct file* fp = thread_current()->fd[fd];
        thread_current()->fd[fd] = NULL;
        return file_close(fp);
    }
}

/* user prog 2 function end*/


// check valid address 
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
        checking_valid_address((void*)*(uint32_t*)(f->esp + 4));
        exit(*(uint32_t*)(f->esp + 4));
        break;
    case SYS_EXEC:
        checking_valid_address((void*)*(uint32_t*)(f->esp + 4));
        f->eax = exec((const char*)*(uint32_t*)(f->esp + 4));
        break;
    case SYS_WAIT:
        checking_valid_address((void*)*(uint32_t*)(f->esp + 4));
        f->eax = wait((tid_t) * (uint32_t*)(f->esp + 4));
        break;
    case SYS_CREATE: /* user prog 2*/
        checking_valid_address((void*)*(uint32_t*)(f->esp + 4));
        checking_valid_address((void*)*(uint32_t*)(f->esp + 8));
        f->eax = create((const char*)*(uint32_t*)(f->esp + 4), (unsigned)*(uint32_t*)(f->esp + 8));
        break;
    case SYS_REMOVE:
        checking_valid_address((void*)*(uint32_t*)(f->esp + 4));
        f->eax = remove((const char*)*(uint32_t*)(f->esp + 4));
        break;
    case SYS_OPEN:
        checking_valid_address((void*)*(uint32_t*)(f->esp + 4));
        f->eax = open((const char*)*(uint32_t*)(f->esp + 4));
        break;
    case SYS_FILESIZE:
        checking_valid_address((void*)*(uint32_t*)(f->esp + 4));
        f->eax = filesize((int)*(uint32_t*)(f->esp + 4));
        break;
    case SYS_READ:
        /* 수정 for read-bad-ptr */
        checking_valid_address((void*)*(uint32_t*)(f->esp + 8));
        checking_valid_address((void*)*(uint32_t*)(f->esp + 4));
        checking_valid_address((void*)*(uint32_t*)(f->esp + 12));
        // if(!is_user_vaddr((void*)*(uint32_t*)(f->esp+8))) {
        //     exit(-1);}
        f->eax = read((int)*(uint32_t*)(f->esp+4), (void*)*(uint32_t*)(f->esp+8) , (unsigned)*(uint32_t*)(f->esp+12));
        break;
    case SYS_WRITE:
        checking_valid_address((void*)*(uint32_t*)(f->esp + 20));
        checking_valid_address((void*)*(uint32_t*)(f->esp + 24));
        checking_valid_address((void*)*(uint32_t*)(f->esp + 28));
        f->eax = write((int)*(uint32_t*)(f->esp + 20), (void*)*(uint32_t*)(f->esp + 24), (unsigned)*((uint32_t*)(f->esp + 28)));
        break;
    case SYS_SEEK:
        checking_valid_address((void*)*(uint32_t*)(f->esp + 4));
        checking_valid_address((void*)*(uint32_t*)(f->esp + 8));
        seek((int)*(uint32_t*)(f->esp + 4), (unsigned)*(uint32_t*)(f->esp + 8));
    case SYS_TELL:
        checking_valid_address((void*)*(uint32_t*)(f->esp + 4));
        f->eax = tell((int)*(uint32_t *)(f->esp + 4));
        break;
    case SYS_CLOSE:
        checking_valid_address((void*)*(uint32_t*)(f->esp + 4));
        close((int)*(uint32_t *)(f->esp + 4));
        break;
    case SYS_FIBONACCI:
        //printf("\n\nthis is fibo\n\n");
        checking_valid_address((void*)*(uint32_t*)(f->esp + 4));
        f->eax = fibonacci(*(int*)(f->esp + 4));
        break;
    case SYS_MAX_FOUR_INT:
        checking_valid_address((void*)*(uint32_t*)(f->esp + 4));
        f->eax = max_of_four_int(*(int*)(f->esp + 4), *(int*)(f->esp + 8), *(int*)(f->esp + 12), *(int*)(f->esp + 16));
        break;
    }
    return;
}
