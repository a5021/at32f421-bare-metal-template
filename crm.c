/**
 * AT32F421 CRM (Clock and Reset Management) Implementation
 */

#include "crm.h"

/**
 * @brief Configure CRM system clock and enable all used peripheral clocks
 * 
 * @return CRM_OK if successful, or specific error code if failed
 */
crm_status_t crm_config(void) {
    uint32_t timeout;

#ifdef HEXT_FREQUENCY
    /* Step 0: Enable and wait for HEXT (external crystal) to stabilize */
    CRM->ctrl |= CRM_CTRL_HEXTEN;
    timeout = CRM_HEXT_TIMEOUT;
    while (!(CRM->ctrl & CRM_CTRL_HEXTSTBL)) {
        if (--timeout == 0) {
            return CRM_ERR_HEXT_TIMEOUT;
        }
    }
#endif

    /* Step 1: Configure PLL multiplication factor and source in CFG register */
    CRM->cfg = CRM_CFG_PLLMULT_L |          /* PLL multiplication factor - low bits */
#ifdef HEXT_FREQUENCY
               CRM_CFG_PLLRCS_SEL |         /* PLL source selection (HEXT) */
               CRM_CFG_PLLHEXTDIV_SEL |     /* HEXT divider */
#endif
               CRM_CFG_PLLMULT_H;           /* PLL multiplication factor - high bits */

    /* Step 2: Enable PLL */
    CRM->ctrl |= CRM_CTRL_PLLEN;

    /* Step 3: Wait for PLL to become ready */
    timeout = CRM_PLL_TIMEOUT;
    while (!(CRM->ctrl & CRM_CTRL_PLLSTBL)) {
        if (--timeout == 0) {
            return CRM_ERR_PLL_TIMEOUT;
        }
    }

    /* Step 4: Enable auto-step mode for smooth clock switching (>108MHz) */
    CRM->misc2 |= CRM_MISC2_AUTO_STEP_EN;

    /* Step 5: Configure Flash for high-frequency operation with prefetch enabled */
    FLASH->psr = FLASH_PSR_WTCYC_3 |        /* 3 wait cycles for high frequency */
                 FLASH_PSR_PFT_EN |         /* Enable main prefetch buffer */
                 FLASH_PSR_PFT_EN2;         /* Enable prefetch buffer block 2 */

    /* Step 6: Switch system clock to PLL */
    CRM->cfg |= CRM_CFG_SCLKSEL_PLL;

    /* Step 7: Wait for system clock switch to complete */
    timeout = CRM_SWITCH_TIMEOUT;
    while ((CRM->cfg & CRM_CFG_SCLKSTS_Msk) != CRM_CFG_SCLKSTS_PLL) {
        if (--timeout == 0) {
            return CRM_ERR_SWITCH_TIMEOUT;
        }
    }

    /* Step 8: Disable auto-step mode after successful switch */
    CRM->misc2 &= ~CRM_MISC2_AUTO_STEP_EN_Msk;

    /* Step 9: Enable all peripheral clocks used in this project */
    CRM->ahben  = CRM_AHBEN_GPIOAEN;       /* Enable GPIOA clock */
    CRM->apb1en = CRM_APB1EN_TMR14EN;      /* Enable TMR14 clock */
    CRM->apb2en = CRM_APB2EN_USART1EN;     /* Enable USART1 clock */

    return CRM_OK;
}
