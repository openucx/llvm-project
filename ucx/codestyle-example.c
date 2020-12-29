
struct foo {
    int field1;
    int *field2;
};

struct bar {
    char field1;
    int  *field2;
    struct {
        int  f1;
        long f2;
    } s1;
};

struct struct_with_comments {
    /* these are the first fields */
    integer_t int_field;
    boolean_t bool_field;

    /* these are more fields */
    int       a; /* this is the a-field */
    void      *b; /* this is the b-field */
};

enum {
    FIRST_BIT    = UCS_BIT(0),
    SECOND_BITTY = UCS_BIT(1),
    THIRD_BIT    = UCS_BIT(1) | LALALALALALALALALA | FOOOOOOOOOOOOO |
                   BOOOOOOOOOOOOOOOOOO,
};

typedef int integer_t;

static ucm_event_installer_t ucm_cuda_initializer = {
    .install            = ucm_cudamem_install,
    .get_existing_alloc = ucm_cudamem_get_existing_alloc,
};

struct small_struct var = {A, B};

const char *vfs_action_names[] = {
    [UCS_VFS_SOCK_ACTION_START] = "start",
    [UCS_VFS_SOCK_ACTION_STOP]  = "stop",
    [UCS_VFS_SOCK_ACTION_MOUNT] = "mount"
};

struct {
    ucs_spinlock_t    lock;
    ucs_vfs_node_t    root;
    khash_t(vfs_path) path_hash;
    khash_t(vfs_obj)  obj_hash;
} ucs_vfs_obj_context = {};

int main(int argc, char **argv)
{
    static const ucm_event_type_t memtype_events = UCM_EVENT_MEM_TYPE_ALLOC |
                                                   UCM_EVENT_MEM_TYPE_FREE;
    static const int num_expected_events         = 2;
    ucm_bistro_jmp_r11_patch_t jmp_r11           = {
        .mov_r11 = {0x49, 0xbb},
        .jmp_r11 = {0x41, 0xff, 0xe3}
    };

    int a             = 0;
    char b            = 6; /* b-variable */
    pointer_type_t *p = 17;
    long longer       = 0; /* l-variable */
    void *page        = mmap(NULL, length, MAP_PRIVATE | MAP_ANONYMOUS,
                             PROT_READ | PROT_WRITE, -1, 0);

    function(arg1, arg2, arg3, asdasd, sadsa, sadsd, sdsad, sadsa, asdsa, asdsd,
             asds);

    vfs_server_context.fd_state[dest_idx] = vfs_server_context.fd_state[src_idx];

    {
        struct sigaction sigact;

        sigact.sa_handler = vfs_server_sighandler;
        sigact.sa_flags   = 0;
    }

    a       = *(integer_t*)&b;
    p       = (void*)&b;
    longer += a * b;

    return 0;
}
