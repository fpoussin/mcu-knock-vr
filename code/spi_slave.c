#include "hal.h"
#include "spi_slave.h"

typedef uint8_t cmd_t;

enum {
  STATUS = 0u
} cmd_enum;

static const SPIConfig spicfg = {
  false,
  NULL,
  PORT_SPI1_NSS,
  PAD_SPI1_NSS,
  0,
  0
};

cmd_t readCmd(uint8_t* buf, size_t len)
{

}

void prepareReply(uint8_t* buf, size_t len)
{

}

static uint8_t rxbuf[8], txbuf[8];
static THD_WORKING_AREA(waSpiThread, 128);
static THD_FUNCTION(SpiThread, arg)
{
  (void) arg;
  while (TRUE)
  {
    spiAcquireBus(&SPID1);
    spiReceive(&SPID1, 2, rxbuf);
    switch (readCmd(rxbuf, 2))
    {
      default:
        break;
    }

    prepareReply(rxbuf, 8);
    spiSend(&SPID1, 8, txbuf);

    spiReleaseBus(&SPID1);
    chThdSleepMilliseconds(10);
  }
}

void createSpiThreads(void)
{
  spiStart(&SPID1, &spicfg);
  SPID1.spi->CR1 &= ~SPI_CR1_SPE; // Disable
  SPID1.spi->CR1 = SPID1.config->cr1 & ~(SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI); // Remove the SPI master stuff
  SPID1.spi->CR2 = SPI_CR2_FRXTH | SPI_CR2_RXDMAEN | SPI_CR2_TXDMAEN; // Remove the SPI slave select stuff
  SPID1.spi->CR2 |= SPI_CR2_RXNEIE | SPI_CR2_ERRIE; // enable receive, error irq
  SPID1.spi->CR1 |= SPI_CR1_SPE; // Enable

  chThdCreateStatic(waSpiThread, sizeof(waSpiThread), NORMALPRIO, SpiThread, NULL);
}
