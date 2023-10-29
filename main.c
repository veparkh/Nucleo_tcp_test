#include <common.h>


static inline void testRoutines(void) {

#if(PROGRAM_ROUTINE == SERVER_TEST)
	server_test();
#elif(PROGRAM_ROUTINE == CLIENT_TEST)
	client_test();
#endif
}



int main(void) {
#if(PROGRAM_ROUTINE != MAIN_ROUTINE)
    testRoutines();

#else
    chSysInit();
    halInit();

    while(1);

#endif
}
