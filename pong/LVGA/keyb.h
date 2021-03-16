#define e_Key 0x12
#define d_Key 0x20
#define o_Key 0x18
#define l_Key 0x26
#define a_Key 0x1E
#define s_Key 0x1F
#define q_Key 0x10
#define esc_Key 0x01
#define f1_Key 0x3B


//void interrupt keyb_int();
//void interrupt (*old_keyb_int)();
void hook_keyb_int(void);
void unhook_keyb_int(void);
int ctrlbrk_handler(void);
unsigned int get_key_status(unsigned char ch); // 1 pressed  0 not
