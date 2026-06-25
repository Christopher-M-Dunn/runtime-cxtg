#include "common/uart.h"
#include "common/test.h"
#include "../include/utils.h"

int main(void)
{
    unsigned long prev, cur;

    /* Case 1: swap from 0 -- prev=0, cxsel=0 */
    CXSETSEL(prev, 0UL);
    CHECK(prev == 0UL);
    __asm__ volatile ("csrr %0, 0xCA0" : "=r"(cur));
    CHECK(cur == 0UL);

    /* Case 2: swap in 0x0102 -- prev=0, cxsel=0x0102 */
    CXSETSEL(prev, 0x0102UL);
    CHECK(prev == 0UL);
    __asm__ volatile ("csrr %0, 0xCA0" : "=r"(cur));
    CHECK(cur == 0x0102UL);

    /* Case 3: swap in ~0 -- prev=0x0102, cxsel=~0 (no clamping in stub) */
    CXSETSEL(prev, ~0UL);
    CHECK(prev == 0x0102UL);
    __asm__ volatile ("csrr %0, 0xCA0" : "=r"(cur));
    CHECK(cur == ~0UL);

    /* Case 4: rd temp used, effectively discarding return; cxsel=0x55 */
    {
        unsigned long discard;
        CXSETSEL(discard, 0x55UL);
        __asm__ volatile ("csrr %0, 0xCA0" : "=r"(cur));
        CHECK(cur == 0x55UL);
        (void)discard;
    }

    /* Case 5: round-trip */
    {
        unsigned long old1, old2;
        CXSETSEL(old1, 0xABCDUL);
        CHECK(old1 == 0x55UL);
        CXSETSEL(old2, old1);
        CHECK(old2 == 0xABCDUL);
        __asm__ volatile ("csrr %0, 0xCA0" : "=r"(cur));
        CHECK(cur == 0x55UL);
    }

    test_pass();
    return 0;
}
