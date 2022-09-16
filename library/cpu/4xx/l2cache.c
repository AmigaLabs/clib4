/*
 * $Id: l2cache.c,v 1.00 2022-09-16 12:04:27 clib2devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#include "dcr-regs.h"

#define mtspr(reg, val) \
    asm volatile ("mtspr %0, %1" :: "i" (reg), "r" (val))

#define mfspr(reg) \
    ({ uint32 ret; asm volatile ("mfspr %0, %1" : "=r" (ret) : "i" (reg)); \
        ret;})

void enableL2Cache(BOOL disable) {
    uint32 cputype;
    uint32 r1, r2, r3, r4;
    APTR oldstack = NULL;

    /* Check that we are using a 460, otherwise exit silently.. */
    GetCPUInfoTags(GCIT_Model, &cputype, TAG_DONE);
    if (cputype != CPUTYPE_PPC460EX)
        return;

    oldstack = SuperState();

    /* stop the L2 controller */
    asm volatile ("sync" ::: "memory");
    r1 = 0;
    mtdcr(0x0030 + DCRN_L2C0_CFG, r1);
    asm volatile ("sync" ::: "memory");

    /* Disable SRAM */
    mtdcr(0x0020 + DCRN_SRAM0_DPC, mfdcr(0x0020 + DCRN_SRAM0_DPC) & ~SRAM_DPC_ENABLE);
    mtdcr(0x0020 + DCRN_SRAM0_SB0CR, mfdcr(0x0020 + DCRN_SRAM0_SB0CR) & ~SRAM_SBCR_BU_MASK);
    mtdcr(0x0020 + DCRN_SRAM0_SB1CR, mfdcr(0x0020 + DCRN_SRAM0_SB1CR) & ~SRAM_SBCR_BU_MASK);
    mtdcr(0x0020 + DCRN_SRAM0_SB2CR, mfdcr(0x0020 + DCRN_SRAM0_SB2CR) & ~SRAM_SBCR_BU_MASK);
    mtdcr(0x0020 + DCRN_SRAM0_SB3CR, mfdcr(0x0020 + DCRN_SRAM0_SB3CR) & ~SRAM_SBCR_BU_MASK);

    /* Enable L2_MODE without ICU/DCU */
    r1 = mfdcr(0x0030 + DCRN_L2C0_CFG) & ~(L2C_CFG_ICU | L2C_CFG_DCU | L2C_CFG_SS_MASK);
    r1 |= L2C_CFG_L2M | L2C_CFG_SS_256;
    mtdcr(0x0030 + DCRN_L2C0_CFG, r1);

    mtdcr(0x0030 + DCRN_L2C0_ADDR, 0);

    /* Hardware Clear Command */
    mtdcr(0x0030 + DCRN_L2C0_CMD, L2C_CMD_HCC);
    while (!(mfdcr(0x0030 + DCRN_L2C0_SR) & L2C_SR_CC))
        ;

    /* Clear Cache Parity and Tag Errors */
    mtdcr(0x0030 + DCRN_L2C0_CMD, L2C_CMD_CCP | L2C_CMD_CTE);

    /* Enable 64G snoop region starting at 0 */
    r2 = mfdcr(0x0030 + DCRN_L2C0_SNP0) & ~(L2C_SNP_BA_MASK | L2C_SNP_SSR_MASK);
    r2 |= L2C_SNP_SSR_32G | L2C_SNP_ESR;
    mtdcr(0x0030 + DCRN_L2C0_SNP0, r2);

    r3 = mfdcr(0x0030 + DCRN_L2C0_SNP1) & ~(L2C_SNP_BA_MASK | L2C_SNP_SSR_MASK);
    r3 |= 0x80000000 | L2C_SNP_SSR_32G | L2C_SNP_ESR;
    mtdcr(0x0030 + DCRN_L2C0_SNP1, r3);

    asm volatile ("sync" ::: "memory");

    /* Enable ICU/DCU ports */
    r4 = mfdcr(0x0030 + DCRN_L2C0_CFG);
    r4 &= ~(L2C_CFG_DCW_MASK | L2C_CFG_PMUX_MASK | L2C_CFG_PMIM | L2C_CFG_TPEI | L2C_CFG_CPEI | L2C_CFG_NAM | L2C_CFG_NBRM);
    r4 |= L2C_CFG_ICU | L2C_CFG_DCU | L2C_CFG_TPC | L2C_CFG_CPC | L2C_CFG_FRAN | L2C_CFG_CPIM | L2C_CFG_TPIM | L2C_CFG_LIM | L2C_CFG_SMCM;

    r4 &= ~L2C_CFG_SNPCI;
    if (disable)
        r4 |= L2C_CFG_SNP440;  // disable L2 write
    else
        r4 &= ~L2C_CFG_SNP440; // enable L2 write
    r4 |= L2C_CFG_RDBW;

    mtdcr(0x0030 + DCRN_L2C0_CFG, r4);

    asm volatile ("sync; isync" ::: "memory");

    UserState((APTR)oldstack);
}
