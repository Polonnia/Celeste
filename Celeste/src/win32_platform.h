#ifndef SM_ASSERT
#define SM_ASSERT(x, msg) if(!(x)) { printf("Error: %s\n", msg); }
#endif 