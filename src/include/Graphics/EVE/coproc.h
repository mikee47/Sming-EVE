/****
 * EVE_coproc.h
 *
 * Initially pulled in from https://github.com/RudolphRiedel/FT800-FT813/EVE_commands.c
 *
 * To be replaced by code generated from schema. See `eve.py`.
 *
 ****/

#pragma once

#include "EVE.h"

namespace Graphics::EVE::CoProc
{
/**
 * @brief Returns the source address and size of the bitmap loaded by the previous CMD_LOADIMAGE.
 *
 * Interesting one to start with since this pulls values into RAM. Steps:
 *
 *   1. Send CMD_GETPROPS (take note of FIFO write address)
 *   2. Wait for command to complete
 *   3. Read result data back from stored FIFO address
 *
 * This is basically a WRITE followed by a READ.
 * When the WRITE completes we could wait for interrupt, read status, check for FIFO_EMPTY then act on that.
 * If this is a blocking call then we'd need to block interrupts for the duration.
 * The interrupt handler logic needs a flag to tell it we're blocking so a status query doesn't need to be queued,
 * we'll call it directly.
 *
 * This command would be used during asset loading, which would definitely be asynchronous.
 * Assets could be quite large so we'd need to fetch the GETPROPS result immediately as the next LOADIMAGE
 * would likely overwrite the FIFO.
 *
 * Note that generally assets are pre-built so we should already know what width/height are.
 *
 */
void EVE_cmd_getprops(uint32_t* p_pointer, uint32_t* p_width, uint32_t* p_height)
{
	uint16_t cmdoffset;

	eve_begin_cmd(CMD_GETPROPS);
	spi_transmit_32(0UL);
	spi_transmit_32(0UL);
	spi_transmit_32(0UL);
	EVE_cs_clear();
	EVE_execute_cmd();
	cmdoffset = EVE_memRead16(REG_CMD_WRITE); /* read the coprocessor write pointer */

	if(p_pointer != NULL) {
		*p_pointer = EVE_memRead32(EVE_RAM_CMD + ((cmdoffset - 12UL) & 0xfffUL));
	}
	if(p_width != NULL) {
		*p_width = EVE_memRead32(EVE_RAM_CMD + ((cmdoffset - 8UL) & 0xfffUL));
	}
	if(p_height != NULL) {
		*p_height = EVE_memRead32(EVE_RAM_CMD + ((cmdoffset - 4UL) & 0xfffUL));
	}
}

/**
 * @brief Returns the next address after a CMD_INFLATE and other commands.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
uint32_t EVE_cmd_getptr()
{
	uint16_t cmdoffset;

	eve_begin_cmd(CMD_GETPTR);
	spi_transmit_32(0UL);
	EVE_cs_clear();
	EVE_execute_cmd();
	cmdoffset = EVE_memRead16(REG_CMD_WRITE); /* read the coprocessor write pointer */
	cmdoffset -= 4U;
	cmdoffset &= 0x0fffU;
	return (EVE_memRead32(EVE_RAM_CMD + cmdoffset));
}

/**
 * @brief Decompress data into RAM_G.
 * @note - The data must be correct and complete.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
void EVE_cmd_inflate(uint32_t ptr, const uint8_t* p_data, uint32_t len)
{
	eve_begin_cmd(CMD_INFLATE);
	spi_transmit_32(ptr);
	EVE_cs_clear();
	if(p_data != NULL) {
		block_transfer(p_data, len);
	}
}

/**
 * @brief Trigger interrupt INT_CMDFLAG.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
void EVE_cmd_interrupt(uint32_t msec)
{
	eve_begin_cmd(CMD_INTERRUPT);
	spi_transmit_32(msec);
	EVE_cs_clear();
	EVE_execute_cmd();
}

/**
 * @brief Loads and decodes a JPEG/PNG image into RAM_G.
 * @note - Decoding PNG images takes significantly more time than decoding JPEG images.
 * @note - In doubt use the EVE Asset Builder to check if PNG/JPEG files are compatible.
 * @note - If the image is in PNG format, the top 42kiB of RAM_G will be overwritten.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
void EVE_cmd_loadimage(uint32_t ptr, uint32_t options, const uint8_t* p_data, uint32_t len)
{
	eve_begin_cmd(CMD_LOADIMAGE);
	spi_transmit_32(ptr);
	spi_transmit_32(options);
	EVE_cs_clear();

#if EVE_GEN > 2
	if((0UL == (options & EVE_OPT_MEDIAFIFO)) &&
	   (0UL == (options & EVE_OPT_FLASH))) /* direct data, neither by Media-FIFO or from Flash */
#else
	if(0UL == (options & EVE_OPT_MEDIAFIFO)) /* direct data, not by Media-FIFO */
#endif
	{
		if(p_data != NULL) {
			block_transfer(p_data, len);
		}
	}
}

/**
 * @brief Set up a streaming media FIFO in RAM_G.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
void EVE_cmd_mediafifo(uint32_t ptr, uint32_t size)
{
	eve_begin_cmd(CMD_MEDIAFIFO);
	spi_transmit_32(ptr);
	spi_transmit_32(size);
	EVE_cs_clear();
	EVE_execute_cmd();
}

/**
 * @brief Copy a block of RAM_G.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
void EVE_cmd_memcpy(uint32_t dest, uint32_t src, uint32_t num)
{
	eve_begin_cmd(CMD_MEMCPY);
	spi_transmit_32(dest);
	spi_transmit_32(src);
	spi_transmit_32(num);
	EVE_cs_clear();
	EVE_execute_cmd();
}

/**
 * @brief Compute a CRC-32 for RAM_G.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
uint32_t EVE_cmd_memcrc(uint32_t ptr, uint32_t num)
{
	uint16_t cmdoffset;

	eve_begin_cmd(CMD_MEMCRC);
	spi_transmit_32(ptr);
	spi_transmit_32(num);
	spi_transmit_32(0UL);
	EVE_cs_clear();
	EVE_execute_cmd();
	cmdoffset = EVE_memRead16(REG_CMD_WRITE); /* read the coprocessor write pointer */
	cmdoffset -= 4U;
	cmdoffset &= 0x0fffU;
	return (EVE_memRead32(EVE_RAM_CMD + cmdoffset));
}

/**
 * @brief Fill RAM_G with a byte value.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
void EVE_cmd_memset(uint32_t ptr, uint8_t value, uint32_t num)
{
	eve_begin_cmd(CMD_MEMSET);
	spi_transmit_32(ptr);
	spi_transmit_32((uint32_t)value);
	spi_transmit_32(num);
	EVE_cs_clear();
	EVE_execute_cmd();
}

/**
 * @brief Write bytes into RAM_G using the coprocessor.
 * @note - Commented out, just use one of the EVE_memWrite* helper functions to directly write to EVEs memory.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
/*
void EVE_cmd_memwrite(uint32_t dest, uint32_t num, const uint8_t *p_data)
{
    eve_begin_cmd(CMD_MEMWRITE);
    spi_transmit_32(dest);
    spi_transmit_32(num);

    num = (num + 3U) & (~3U);

    for (uint32_t count = 0U; count<len; count++)
    {
        spi_transmit(pgm_read_byte_far(p_data + count));
    }

    EVE_cs_clear();
    EVE_execute_cmd();
}
*/

/**
 * @brief Read a register value using the coprocessor.
 * @note - Commented out, just read the register directly.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
/*
uint32_t EVE_cmd_regread(uint32_t ptr)
{
    uint16_t cmdoffset;

    eve_begin_cmd(CMD_REGREAD);
    spi_transmit_32(ptr);
    spi_transmit_32(0UL);
    EVE_cs_clear();
    EVE_execute_cmd();
    cmdoffset = EVE_memRead16(REG_CMD_WRITE); // read the coprocessor write pointer
    cmdoffset -= 4U;
    cmdoffset &= 0x0fffU;
    return (EVE_memRead32(EVE_RAM_CMD + cmdoffset));
}
*/

/**
 * @brief Write zero to RAM_G.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
void EVE_cmd_memzero(uint32_t ptr, uint32_t num)
{
	eve_begin_cmd(CMD_MEMZERO);
	spi_transmit_32(ptr);
	spi_transmit_32(num);
	EVE_cs_clear();
	EVE_execute_cmd();
}

/**
 * @brief Play back motion-JPEG encoded AVI video.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command.
 * @note - Does not support burst-mode.
 * @note - Does not wait for completion in order to allow the video to be paused or terminated by REG_PLAY_CONTROL
 */
void EVE_cmd_playvideo(uint32_t options, const uint8_t* p_data, uint32_t len)
{
	eve_begin_cmd(CMD_PLAYVIDEO);
	spi_transmit_32(options);
	EVE_cs_clear();

#if EVE_GEN > 2
	if((0UL == (options & EVE_OPT_MEDIAFIFO)) &&
	   (0UL == (options & EVE_OPT_FLASH))) /* direct data, neither by Media-FIFO or from Flash */
#else
	if(0UL == (options & EVE_OPT_MEDIAFIFO)) /* direct data, not by Media-FIFO */
#endif
	{
		if(p_data != NULL) {
			block_transfer(p_data, len);
		}
	}
}

/**
 * @brief Rotate the screen and set up transform matrix accordingly.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
void EVE_cmd_setrotate(uint32_t rotation)
{
	eve_begin_cmd(CMD_SETROTATE);
	spi_transmit_32(rotation);
	EVE_cs_clear();
	EVE_execute_cmd();
}

/**
 * @brief Take a snapshot of the current screen.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
void EVE_cmd_snapshot(uint32_t ptr)
{
	eve_begin_cmd(CMD_SNAPSHOT);
	spi_transmit_32(ptr);
	EVE_cs_clear();
	EVE_execute_cmd();
}

/**
 * @brief Take a snapshot of part of the current screen with format option.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
void EVE_cmd_snapshot2(uint32_t fmt, uint32_t ptr, int16_t xc0, int16_t yc0, uint16_t wid, uint16_t hgt)
{
	eve_begin_cmd(CMD_SNAPSHOT2);
	spi_transmit_32(fmt);
	spi_transmit_32(ptr);

	spi_transmit((uint8_t)((uint16_t)xc0));
	spi_transmit((uint8_t)(((uint16_t)xc0) >> 8U));
	spi_transmit((uint8_t)((uint16_t)yc0));
	spi_transmit((uint8_t)(((uint16_t)yc0) >> 8U));

	spi_transmit((uint8_t)(wid));
	spi_transmit((uint8_t)(wid >> 8U));
	spi_transmit((uint8_t)(hgt));
	spi_transmit((uint8_t)(hgt >> 8U));

	EVE_cs_clear();
	EVE_execute_cmd();
}

/**
 * @brief Track touches for a graphics object.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
void EVE_cmd_track(int16_t xc0, int16_t yc0, uint16_t wid, uint16_t hgt, uint16_t tag)
{
	eve_begin_cmd(CMD_TRACK);

	spi_transmit((uint8_t)((uint16_t)xc0));
	spi_transmit((uint8_t)(((uint16_t)xc0) >> 8U));
	spi_transmit((uint8_t)((uint16_t)yc0));
	spi_transmit((uint8_t)(((uint16_t)yc0) >> 8U));

	spi_transmit((uint8_t)(wid));
	spi_transmit((uint8_t)(wid >> 8U));
	spi_transmit((uint8_t)(hgt));
	spi_transmit((uint8_t)(hgt >> 8U));

	spi_transmit((uint8_t)(tag));
	spi_transmit((uint8_t)(tag >> 8U));
	spi_transmit(0U);
	spi_transmit(0U);

	EVE_cs_clear();
	EVE_execute_cmd();
}

/**
 * @brief Load the next frame of a video.
 * @note - Meant to be called outside display-list building.
 * @note - Includes executing the command and waiting for completion.
 * @note - Does not support burst-mode.
 */
void EVE_cmd_videoframe(uint32_t dest, uint32_t result_ptr)
{
	eve_begin_cmd(CMD_VIDEOFRAME);
	spi_transmit_32(dest);
	spi_transmit_32(result_ptr);
	EVE_cs_clear();
	EVE_execute_cmd();
}

/* ##################################################################
    functions for display lists
##################################################################### */

/**
 * @brief Appends commands from RAM_G to the display list.
 */
void EVE_cmd_append(uint32_t ptr, uint32_t num)
{
	if(0U == cmd_burst) {
		eve_begin_cmd(CMD_APPEND);
		spi_transmit_32(ptr);
		spi_transmit_32(num);
		EVE_cs_clear();
	} else {
		spi_transmit_burst(CMD_APPEND);
		spi_transmit_burst(ptr);
		spi_transmit_burst(num);
	}
}

/**
 * @brief Appends commands from RAM_G to the display list, only works in burst-mode.
 */
void EVE_cmd_append_burst(uint32_t ptr, uint32_t num)
{
	spi_transmit_burst(CMD_APPEND);
	spi_transmit_burst(ptr);
	spi_transmit_burst(num);
}

/**
 * @brief Set the background color.
 */
void EVE_cmd_bgcolor(uint32_t color)
{
	if(0U == cmd_burst) {
		eve_begin_cmd(CMD_BGCOLOR);
		spi_transmit((uint8_t)(color));
		spi_transmit((uint8_t)(color >> 8U));
		spi_transmit((uint8_t)(color >> 16U));
		spi_transmit(0U);
		EVE_cs_clear();
	} else {
		spi_transmit_burst(CMD_BGCOLOR);
		spi_transmit_burst(color);
	}
}

/**
 * @brief Set the background color, only works in burst-mode.
 */
void EVE_cmd_bgcolor_burst(uint32_t color)
{
	spi_transmit_burst(CMD_BGCOLOR);
	spi_transmit_burst(color);
}

/**
 * @brief Draw a button with a label.
 */
void EVE_cmd_button(int16_t xc0, int16_t yc0, uint16_t wid, uint16_t hgt, uint16_t font, uint16_t options,
					const char* p_text)
{
	if(0U == cmd_burst) {
		eve_begin_cmd(CMD_BUTTON);
		spi_transmit((uint8_t)((uint16_t)xc0));
		spi_transmit((uint8_t)(((uint16_t)xc0) >> 8U));
		spi_transmit((uint8_t)((uint16_t)yc0));
		spi_transmit((uint8_t)(((uint16_t)yc0) >> 8U));
		spi_transmit((uint8_t)(wid));
		spi_transmit((uint8_t)(wid >> 8U));
		spi_transmit((uint8_t)(hgt));
		spi_transmit((uint8_t)(hgt >> 8U));
		spi_transmit((uint8_t)(font));
		spi_transmit((uint8_t)(font >> 8U));
		spi_transmit((uint8_t)(options));
		spi_transmit((uint8_t)(options >> 8U));
		private_string_write(p_text);
		EVE_cs_clear();
	} else {
		spi_transmit_burst(CMD_BUTTON);
		spi_transmit_burst(((uint32_t)((uint16_t)xc0)) + (((uint32_t)((uint16_t)yc0)) << 16U));
		spi_transmit_burst(((uint32_t)wid) + (((uint32_t)hgt) << 16U));
		spi_transmit_burst(((uint32_t)font) + (((uint32_t)options) << 16U));
		private_string_write(p_text);
	}
}

/**
 * @brief Draw a button with a label, only works in burst-mode.
 */
void EVE_cmd_button_burst(int16_t xc0, int16_t yc0, uint16_t wid, uint16_t hgt, uint16_t font, uint16_t options,
						  const char* p_text)
{
	spi_transmit_burst(CMD_BUTTON);
	spi_transmit_burst(((uint32_t)((uint16_t)xc0)) + (((uint32_t)((uint16_t)yc0)) << 16U));
	spi_transmit_burst(((uint32_t)wid) + (((uint32_t)hgt) << 16U));
	spi_transmit_burst(((uint32_t)font) + (((uint32_t)options) << 16U));
	private_string_write(p_text);
}

/**
 * @brief Execute the touch screen calibration routine.
 * @note - does not support burst-mode
 */
void EVE_cmd_calibrate()
{
	if(0U == cmd_burst) {
		eve_begin_cmd(CMD_CALIBRATE);
		spi_transmit_32(0UL);
		EVE_cs_clear();
	}
}

/**
 * @brief Draw an analog clock.
 */
void EVE_cmd_clock(int16_t xc0, int16_t yc0, uint16_t rad, uint16_t options, uint16_t hours, uint16_t mins,
				   uint16_t secs, uint16_t msecs)
{
	if(0U == cmd_burst) {
		eve_begin_cmd(CMD_CLOCK);
		spi_transmit((uint8_t)((uint16_t)xc0));
		spi_transmit((uint8_t)(((uint16_t)xc0) >> 8U));
		spi_transmit((uint8_t)((uint16_t)yc0));
		spi_transmit((uint8_t)(((uint16_t)yc0) >> 8U));
		spi_transmit((uint8_t)(rad));
		spi_transmit((uint8_t)(rad >> 8U));
		spi_transmit((uint8_t)(options));
		spi_transmit((uint8_t)(options >> 8U));
		spi_transmit((uint8_t)(hours));
		spi_transmit((uint8_t)(hours >> 8U));
		spi_transmit((uint8_t)(mins));
		spi_transmit((uint8_t)(mins >> 8U));
		spi_transmit((uint8_t)(secs));
		spi_transmit((uint8_t)(secs >> 8U));
		spi_transmit((uint8_t)(msecs));
		spi_transmit((uint8_t)(msecs >> 8U));
		EVE_cs_clear();
	} else {
		spi_transmit_burst(CMD_CLOCK);
		spi_transmit_burst(((uint32_t)((uint16_t)xc0)) + (((uint32_t)((uint16_t)yc0)) << 16U));
		spi_transmit_burst(((uint32_t)rad) + (((uint32_t)options) << 16U));
		spi_transmit_burst(((uint32_t)hours) + (((uint32_t)mins) << 16U));
		spi_transmit_burst(((uint32_t)secs) + (((uint32_t)msecs) << 16U));
	}
}

/**
 * @brief Draw an analog clock, only works in burst-mode.
 */
void EVE_cmd_clock_burst(int16_t xc0, int16_t yc0, uint16_t rad, uint16_t options, uint16_t hours, uint16_t mins,
						 uint16_t secs, uint16_t msecs)
{
	spi_transmit_burst(CMD_CLOCK);
	spi_transmit_burst(((uint32_t)((uint16_t)xc0)) + (((uint32_t)((uint16_t)yc0)) << 16U));
	spi_transmit_burst(((uint32_t)rad) + (((uint32_t)options) << 16U));
	spi_transmit_burst(((uint32_t)hours) + (((uint32_t)mins) << 16U));
	spi_transmit_burst(((uint32_t)secs) + (((uint32_t)msecs) << 16U));
}

/**
 * @brief Draw a rotary dial control.
 */
void EVE_cmd_dial(int16_t xc0, int16_t yc0, uint16_t rad, uint16_t options, uint16_t val)
{
	if(0U == cmd_burst) {
		eve_begin_cmd(CMD_DIAL);
		spi_transmit((uint8_t)((uint16_t)xc0));
		spi_transmit((uint8_t)(((uint16_t)xc0) >> 8U));
		spi_transmit((uint8_t)((uint16_t)yc0));
		spi_transmit((uint8_t)(((uint16_t)yc0) >> 8U));
		spi_transmit((uint8_t)(rad));
		spi_transmit((uint8_t)(rad >> 8U));
		spi_transmit((uint8_t)(options));
		spi_transmit((uint8_t)(options >> 8U));
		spi_transmit((uint8_t)(val));
		spi_transmit((uint8_t)(val >> 8U));
		spi_transmit(0U);
		spi_transmit(0U);
		EVE_cs_clear();
	} else {
		spi_transmit_burst(CMD_DIAL);
		spi_transmit_burst(((uint32_t)((uint16_t)xc0)) + (((uint32_t)((uint16_t)yc0)) << 16U));
		spi_transmit_burst(((uint32_t)rad) + (((uint32_t)options) << 16U));
		spi_transmit_burst(val);
	}
}

/**
 * @brief Draw a rotary dial control, only works in burst-mode.
 */
void EVE_cmd_dial_burst(int16_t xc0, int16_t yc0, uint16_t rad, uint16_t options, uint16_t val)
{
	spi_transmit_burst(CMD_DIAL);
	spi_transmit_burst(((uint32_t)((uint16_t)xc0)) + (((uint32_t)((uint16_t)yc0)) << 16U));
	spi_transmit_burst(((uint32_t)rad) + (((uint32_t)options) << 16U));
	spi_transmit_burst(val);
}

/**
 * @brief Set the foreground color.
 */
void EVE_cmd_fgcolor(uint32_t color)
{
	if(0U == cmd_burst) {
		eve_begin_cmd(CMD_FGCOLOR);
		spi_transmit((uint8_t)(color));
		spi_transmit((uint8_t)(color >> 8U));
		spi_transmit((uint8_t)(color >> 16U));
		spi_transmit(0U);
		EVE_cs_clear();
	} else {
		spi_transmit_burst(CMD_FGCOLOR);
		spi_transmit_burst(color);
	}
}

/**
 * @brief Set the foreground color, only works in burst-mode.
 */
void EVE_cmd_fgcolor_burst(uint32_t color)
{
	spi_transmit_burst(CMD_FGCOLOR);
	spi_transmit_burst(color);
}

/**
 * @brief Draw a gauge.
 */
void EVE_cmd_gauge(int16_t xc0, int16_t yc0, uint16_t rad, uint16_t options, uint16_t major, uint16_t minor,
				   uint16_t val, uint16_t range)
{
	if(0U == cmd_burst) {
		eve_begin_cmd(CMD_GAUGE);
		spi_transmit((uint8_t)((uint16_t)xc0));
		spi_transmit((uint8_t)(((uint16_t)xc0) >> 8U));
		spi_transmit((uint8_t)((uint16_t)yc0));
		spi_transmit((uint8_t)(((uint16_t)yc0) >> 8U));
		spi_transmit((uint8_t)(rad));
		spi_transmit((uint8_t)(rad >> 8U));
		spi_transmit((uint8_t)(options));
		spi_transmit((uint8_t)(options >> 8U));
		spi_transmit((uint8_t)(major));
		spi_transmit((uint8_t)(major >> 8U));
		spi_transmit((uint8_t)(minor));
		spi_transmit((uint8_t)(minor >> 8U));
		spi_transmit((uint8_t)(val));
		spi_transmit((uint8_t)(val >> 8U));
		spi_transmit((uint8_t)(range));
		spi_transmit((uint8_t)(range >> 8U));
		EVE_cs_clear();
	} else {
		spi_transmit_burst(CMD_GAUGE);
		spi_transmit_burst(((uint32_t)((uint16_t)xc0)) + (((uint32_t)((uint16_t)yc0)) << 16U));
		spi_transmit_burst(((uint32_t)rad) + (((uint32_t)options) << 16U));
		spi_transmit_burst(((uint32_t)major) + (((uint32_t)minor) << 16U));
		spi_transmit_burst(((uint32_t)val) + (((uint32_t)range) << 16U));
	}
}

/**
 * @brief Draw a gauge, only works in burst-mode.
 */
void EVE_cmd_gauge_burst(int16_t xc0, int16_t yc0, uint16_t rad, uint16_t options, uint16_t major, uint16_t minor,
						 uint16_t val, uint16_t range)
{
	spi_transmit_burst(CMD_GAUGE);
	spi_transmit_burst(((uint32_t)((uint16_t)xc0)) + (((uint32_t)((uint16_t)yc0)) << 16U));
	spi_transmit_burst(((uint32_t)rad) + (((uint32_t)options) << 16U));
	spi_transmit_burst(((uint32_t)major) + (((uint32_t)minor) << 16U));
	spi_transmit_burst(((uint32_t)val) + (((uint32_t)range) << 16U));
}

/**
 * @brief Retrieves the current matrix within the context of the coprocessor engine.
 * @note - waits for completion and reads values from RAM_CMD after completion
 * @note - can not be used with cmd-burst
 */
void EVE_cmd_getmatrix(int32_t* p_a, int32_t* p_b, int32_t* p_c, int32_t* p_d, int32_t* p_e, int32_t* p_f)
{
	if(0U == cmd_burst) {
		uint16_t cmdoffset;
		uint32_t address;

		eve_begin_cmd(CMD_GETMATRIX);
		spi_transmit_32(0UL);
		spi_transmit_32(0UL);
		spi_transmit_32(0UL);
		spi_transmit_32(0UL);
		spi_transmit_32(0UL);
		spi_transmit_32(0UL);
		EVE_cs_clear();
		EVE_execute_cmd();
		cmdoffset = EVE_memRead16(REG_CMD_WRITE);

		if(p_f != NULL) {
			address = EVE_RAM_CMD + ((cmdoffset - 4UL) & 0xfffUL);
			*p_f = (int32_t)EVE_memRead32(address);
		}
		if(p_e != NULL) {
			address = EVE_RAM_CMD + ((cmdoffset - 8UL) & 0xfffUL);
			*p_e = (int32_t)EVE_memRead32(address);
		}
		if(p_d != NULL) {
			address = EVE_RAM_CMD + ((cmdoffset - 12UL) & 0xfffUL);
			*p_d = (int32_t)EVE_memRead32(address);
		}
		if(p_c != NULL) {
			address = EVE_RAM_CMD + ((cmdoffset - 16UL) & 0xfffUL);
			*p_c = (int32_t)EVE_memRead32(address);
		}
		if(p_b != NULL) {
			address = EVE_RAM_CMD + ((cmdoffset - 20UL) & 0xfffUL);
			*p_b = (int32_t)EVE_memRead32(address);
		}
		if(p_a != NULL) {
			address = EVE_RAM_CMD + ((cmdoffset - 24UL) & 0xfffUL);
			*p_a = (int32_t)EVE_memRead32(address);
		}
	}
}

/**
 * @brief Set up the highlight color used in 3D effects for CMD_BUTTON and CMD_KEYS.
 */
void EVE_cmd_gradcolor(uint32_t color)
{
	if(0U == cmd_burst) {
		eve_begin_cmd(CMD_GRADCOLOR);
		spi_transmit((uint8_t)(color));
		spi_transmit((uint8_t)(color >> 8U));
		spi_transmit((uint8_t)(color >> 16U));
		spi_transmit(0U);
		EVE_cs_clear();
	} else {
		spi_transmit_burst(CMD_GRADCOLOR);
		spi_transmit_burst(color);
	}
}

/**
 * @brief Set up the highlight color used in 3D effects for CMD_BUTTON and CMD_KEYS, only works in burst-mode.
 */
void EVE_cmd_gradcolor_burst(uint32_t color)
{
	spi_transmit_burst(CMD_GRADCOLOR);
	spi_transmit_burst(color);
}

/**
 * @brief Draw a smooth color gradient.
 */
void EVE_cmd_gradient(int16_t xc0, int16_t yc0, uint32_t rgb0, int16_t xc1, int16_t yc1, uint32_t rgb1)
{
	if(0U == cmd_burst) {
		eve_begin_cmd(CMD_GRADIENT);
		spi_transmit((uint8_t)((uint16_t)xc0));
		spi_transmit((uint8_t)(((uint16_t)xc0) >> 8U));
		spi_transmit((uint8_t)((uint16_t)yc0));
		spi_transmit((uint8_t)(((uint16_t)yc0) >> 8U));
		spi_transmit((uint8_t)(rgb0));
		spi_transmit((uint8_t)(rgb0 >> 8U));
		spi_transmit((uint8_t)(rgb0 >> 16U));
		spi_transmit(0U);
		spi_transmit((uint8_t)((uint16_t)xc1));
		spi_transmit((uint8_t)(((uint16_t)xc1) >> 8U));
		spi_transmit((uint8_t)((uint16_t)yc1));
		spi_transmit((uint8_t)(((uint16_t)yc1) >> 8U));
		spi_transmit((uint8_t)(rgb1));
		spi_transmit((uint8_t)(rgb1 >> 8U));
		spi_transmit((uint8_t)(rgb1 >> 16U));
		spi_transmit(0U);
		EVE_cs_clear();
	} else {
		spi_transmit_burst(CMD_GRADIENT);
		spi_transmit_burst(((uint32_t)((uint16_t)xc0)) + (((uint32_t)((uint16_t)yc0)) << 16U));
		spi_transmit_burst(rgb0);
		spi_transmit_burst(((uint32_t)((uint16_t)xc1)) + (((uint32_t)((uint16_t)yc1)) << 16U));
		spi_transmit_burst(rgb1);
	}
}

/**
 * @brief Draw a smooth color gradient, only works in burst-mode.
 */
void EVE_cmd_gradient_burst(int16_t xc0, int16_t yc0, uint32_t rgb0, int16_t xc1, int16_t yc1, uint32_t rgb1)
{
	spi_transmit_burst(CMD_GRADIENT);
	spi_transmit_burst(((uint32_t)((uint16_t)xc0)) + (((uint32_t)((uint16_t)yc0)) << 16U));
	spi_transmit_burst(rgb0);
	spi_transmit_burst(((uint32_t)((uint16_t)xc1)) + (((uint32_t)((uint16_t)yc1)) << 16U));
	spi_transmit_burst(rgb1);
}

/**
 * @brief Draw a row of key buttons with labels.
 * @note - The tag value of each button is set to the ASCII value of its label.
 * @note - Does not work with UTF-8.
 */
void EVE_cmd_keys(int16_t xc0, int16_t yc0, uint16_t wid, uint16_t hgt, uint16_t font, uint16_t options,
				  const char* p_text)
{
	if(0U == cmd_burst) {
		eve_begin_cmd(CMD_KEYS);
		spi_transmit((uint8_t)((uint16_t)xc0));
		spi_transmit((uint8_t)(((uint16_t)xc0) >> 8U));
		spi_transmit((uint8_t)((uint16_t)yc0));
		spi_transmit((uint8_t)(((uint16_t)yc0) >> 8U));
		spi_transmit((uint8_t)(wid));
		spi_transmit((uint8_t)(wid >> 8U));
		spi_transmit((uint8_t)(hgt));
		spi_transmit((uint8_t)(hgt >> 8U));
		spi_transmit((uint8_t)(font));
		spi_transmit((uint8_t)(font >> 8U));
		spi_transmit((uint8_t)(options));
		spi_transmit((uint8_t)(options >> 8U));
		private_string_write(p_text);
		EVE_cs_clear();
	} else {
		spi_transmit_burst(CMD_KEYS);
		spi_transmit_burst(((uint32_t)((uint16_t)xc0)) + (((uint32_t)((uint16_t)yc0)) << 16U));
		spi_transmit_burst(((uint32_t)wid) + (((uint32_t)hgt) << 16U));
		spi_transmit_burst(((uint32_t)font) + (((uint32_t)options) << 16U));
		private_string_write(p_text);
	}
}

/**
 * @brief Draw a row of key buttons with labels, only works in burst-mode.
 * @note - The tag value of each button is set to the ASCII value of its label.
 * @note - Does not work with UTF-8.
 */
void EVE_cmd_keys_burst(int16_t xc0, int16_t yc0, uint16_t wid, uint16_t hgt, uint16_t font, uint16_t options,
						const char* p_text)
{
	spi_transmit_burst(CMD_KEYS);
	spi_transmit_burst(((uint32_t)((uint16_t)xc0)) + (((uint32_t)((uint16_t)yc0)) << 16U));
	spi_transmit_burst(((uint32_t)wid) + (((uint32_t)hgt) << 16U));
	spi_transmit_burst(((uint32_t)font) + (((uint32_t)options) << 16U));
	private_string_write(p_text);
}

/**
 * @brief Draw a number.
 */
void EVE_cmd_number(int16_t xc0, int16_t yc0, uint16_t font, uint16_t options, int32_t number)
{
	if(0U == cmd_burst) {
		eve_begin_cmd(CMD_NUMBER);
		spi_transmit((uint8_t)((uint16_t)xc0));
		spi_transmit((uint8_t)(((uint16_t)xc0) >> 8U));
		spi_transmit((uint8_t)((uint16_t)yc0));
		spi_transmit((uint8_t)(((uint16_t)yc0) >> 8U));
		spi_transmit((uint8_t)(font));
		spi_transmit((uint8_t)(font >> 8U));
		spi_transmit((uint8_t)(options));
		spi_transmit((uint8_t)(options >> 8U));
		spi_transmit_32((uint32_t)number);
		EVE_cs_clear();
	} else {
		spi_transmit_burst(CMD_NUMBER);
		spi_transmit_burst(((uint32_t)((uint16_t)xc0)) + (((uint32_t)((uint16_t)yc0)) << 16U));
		spi_transmit_burst(((uint32_t)font) + (((uint32_t)options) << 16U));
		spi_transmit_burst((uint32_t)number);
	}
}

/**
 * @brief Draw a number, only works in burst-mode.
 */
void EVE_cmd_number_burst(int16_t xc0, int16_t yc0, uint16_t font, uint16_t options, int32_t number)
{
	spi_transmit_burst(CMD_NUMBER);
	spi_transmit_burst(((uint32_t)((uint16_t)xc0)) + (((uint32_t)((uint16_t)yc0)) << 16U));
	spi_transmit_burst(((uint32_t)font) + (((uint32_t)options) << 16U));
	spi_transmit_burst((uint32_t)number);
}

/**
 * @brief Draw a progress bar.
 */
void EVE_cmd_progress(int16_t xc0, int16_t yc0, uint16_t wid, uint16_t hgt, uint16_t options, uint16_t val,
					  uint16_t range)
{
	if(0U == cmd_burst) {
		eve_begin_cmd(CMD_PROGRESS);
		spi_transmit((uint8_t)((uint16_t)xc0));
		spi_transmit((uint8_t)(((uint16_t)xc0) >> 8U));
		spi_transmit((uint8_t)((uint16_t)yc0));
		spi_transmit((uint8_t)(((uint16_t)yc0) >> 8U));
		spi_transmit((uint8_t)(wid));
		spi_transmit((uint8_t)(wid >> 8U));
		spi_transmit((uint8_t)(hgt));
		spi_transmit((uint8_t)(hgt >> 8U));
		spi_transmit((uint8_t)(options));
		spi_transmit((uint8_t)(options >> 8U));
		spi_transmit((uint8_t)(val));
		spi_transmit((uint8_t)(val >> 8U));
		spi_transmit((uint8_t)(range));
		spi_transmit((uint8_t)(range >> 8U));
		spi_transmit(0U); /* dummy byte for 4-byte alignment */
		spi_transmit(0U); /* dummy byte for 4-byte alignment */
		EVE_cs_clear();
	} else {
		spi_transmit_burst(CMD_PROGRESS);
		spi_transmit_burst(((uint32_t)((uint16_t)xc0)) + (((uint32_t)((uint16_t)yc0)) << 16U));
		spi_transmit_burst(((uint32_t)wid) + (((uint32_t)hgt) << 16U));
		spi_transmit_burst(((uint32_t)options) + (((uint32_t)val) << 16U));
		spi_transmit_burst((uint32_t)range);
	}
}

/**
 * @brief Draw a progress bar, only works in burst-mode.
 */
void EVE_cmd_progress_burst(int16_t xc0, int16_t yc0, uint16_t wid, uint16_t hgt, uint16_t options, uint16_t val,
							uint16_t range)
{
	spi_transmit_burst(CMD_PROGRESS);
	spi_transmit_burst(((uint32_t)((uint16_t)xc0)) + (((uint32_t)((uint16_t)yc0)) << 16U));
	spi_transmit_burst(((uint32_t)wid) + (((uint32_t)hgt) << 16U));
	spi_transmit_burst(((uint32_t)options) + (((uint32_t)val) << 16U));
	spi_transmit_burst((uint32_t)range);
}

/**
 * @brief Load a ROM font into bitmap handle.
 * @note - generates display list commands, so it needs to be put in a display list
 */
void EVE_cmd_romfont(uint32_t font, uint32_t romslot)
{
	if(0U == cmd_burst) {
		eve_begin_cmd(CMD_ROMFONT);
		spi_transmit_32(font);
		spi_transmit_32(romslot);
		EVE_cs_clear();
	} else {
		spi_transmit_burst(CMD_ROMFONT);
		spi_transmit_burst(font);
		spi_transmit_burst(romslot);
	}
}

/**
 * @brief Load a ROM font into bitmap handle, only works in burst-mode.
 * @note - generates display list commands, so it needs to be put in a display list
 */
void EVE_cmd_romfont_burst(uint32_t font, uint32_t romslot)
{
	spi_transmit_burst(CMD_ROMFONT);
	spi_transmit_burst(font);
	spi_transmit_burst(romslot);
}

/**
 * @brief Apply a rotation to the current matrix.
 */
void EVE_cmd_rotate(uint32_t angle)
{
	if(0U == cmd_burst) {
		eve_begin_cmd(CMD_ROTATE);
		spi_transmit_32(angle & 0xFFFFUL);
		EVE_cs_clear();
	} else {
		spi_transmit_burst(CMD_ROTATE);
		spi_transmit_burst(angle & 0xFFFFUL);
	}
}

/**
 * @brief Apply a rotation to the current matrix, only works in burst-mode.
 */
void EVE_cmd_rotate_burst(uint32_t angle)
{
	spi_transmit_burst(CMD_ROTATE);
	spi_transmit_burst(angle & 0xFFFFUL);
}

/**
 * @brief Apply a scale to the current matrix.
 */
void EVE_cmd_scale(int32_t scx, int32_t scy)
{
	if(0U == cmd_burst) {
		eve_begin_cmd(CMD_SCALE);
		spi_transmit_32((uint32_t)scx);
		spi_transmit_32((uint32_t)scy);
		EVE_cs_clear();
	} else {
		spi_transmit_burst(CMD_SCALE);
		spi_transmit_burst((uint32_t)scx);
		spi_transmit_burst((uint32_t)scy);
	}
}

/**
 * @brief Apply a scale to the current matrix, only works in burst-mode.
 */
void EVE_cmd_scale_burst(int32_t scx, int32_t scy)
{
	spi_transmit_burst(CMD_SCALE);
	spi_transmit_burst((uint32_t)scx);
	spi_transmit_burst((uint32_t)scy);
}

/**
 * @brief Draw a scroll bar.
 */
void EVE_cmd_scrollbar(int16_t xc0, int16_t yc0, uint16_t wid, uint16_t hgt, uint16_t options, uint16_t val,
					   uint16_t size, uint16_t range)
{
	if(0U == cmd_burst) {
		eve_begin_cmd(CMD_SCROLLBAR);
		spi_transmit((uint8_t)((uint16_t)xc0));
		spi_transmit((uint8_t)(((uint16_t)xc0) >> 8U));
		spi_transmit((uint8_t)((uint16_t)yc0));
		spi_transmit((uint8_t)(((uint16_t)yc0) >> 8U));
		spi_transmit((uint8_t)(wid));
		spi_transmit((uint8_t)(wid >> 8U));
		spi_transmit((uint8_t)(hgt));
		spi_transmit((uint8_t)(hgt >> 8U));
		spi_transmit((uint8_t)(options));
		spi_transmit((uint8_t)(options >> 8U));
		spi_transmit((uint8_t)(val));
		spi_transmit((uint8_t)(val >> 8U));
		spi_transmit((uint8_t)(size));
		spi_transmit((uint8_t)(size >> 8U));
		spi_transmit((uint8_t)(range));
		spi_transmit((uint8_t)(range >> 8U));
		EVE_cs_clear();
	} else {
		spi_transmit_burst(CMD_SCROLLBAR);
		spi_transmit_burst(((uint32_t)((uint16_t)xc0)) + (((uint32_t)((uint16_t)yc0)) << 16U));
		spi_transmit_burst(((uint32_t)wid) + (((uint32_t)hgt) << 16U));
		spi_transmit_burst(((uint32_t)options) + (((uint32_t)val) << 16U));
		spi_transmit_burst(((uint32_t)size) + (((uint32_t)range) << 16U));
	}
}

/**
 * @brief Draw a scroll bar, only works in burst-mode.
 */
void EVE_cmd_scrollbar_burst(int16_t xc0, int16_t yc0, uint16_t wid, uint16_t hgt, uint16_t options, uint16_t val,
							 uint16_t size, uint16_t range)
{
	spi_transmit_burst(CMD_SCROLLBAR);
	spi_transmit_burst(((uint32_t)((uint16_t)xc0)) + (((uint32_t)((uint16_t)yc0)) << 16U));
	spi_transmit_burst(((uint32_t)wid) + (((uint32_t)hgt) << 16U));
	spi_transmit_burst(((uint32_t)options) + (((uint32_t)val) << 16U));
	spi_transmit_burst(((uint32_t)size) + (((uint32_t)range) << 16U));
}

/**
 * @brief Set the base for number output.
 */
void EVE_cmd_setbase(uint32_t base)
{
	if(0U == cmd_burst) {
		eve_begin_cmd(CMD_SETBASE);
		spi_transmit_32(base);
		EVE_cs_clear();
	} else {
		spi_transmit_burst(CMD_SETBASE);
		spi_transmit_burst(base);
	}
}

/**
 * @brief Set the base for number output, only works in burst-mode.
 */
void EVE_cmd_setbase_burst(uint32_t base)
{
	spi_transmit_burst(CMD_SETBASE);
	spi_transmit_burst(base);
}

/**
 * @brief Generate the corresponding display list commands for given bitmap information.
 */
void EVE_cmd_setbitmap(uint32_t addr, uint16_t fmt, uint16_t width, uint16_t height)
{
	if(0U == cmd_burst) {
		eve_begin_cmd(CMD_SETBITMAP);
		spi_transmit_32(addr);
		spi_transmit((uint8_t)(fmt));
		spi_transmit((uint8_t)(fmt >> 8U));
		spi_transmit((uint8_t)(width));
		spi_transmit((uint8_t)(width >> 8U));
		spi_transmit((uint8_t)(height));
		spi_transmit((uint8_t)(height >> 8U));
		spi_transmit(0U);
		spi_transmit(0U);
		EVE_cs_clear();
	} else {
		spi_transmit_burst(CMD_SETBITMAP);
		spi_transmit_burst(addr);
		spi_transmit_burst(((uint32_t)fmt) + (((uint32_t)width) << 16U));
		spi_transmit_burst((uint32_t)height);
	}
}

/**
 * @brief Generate the corresponding display list commands for given bitmap information, only works in burst-mode.
 */
void EVE_cmd_setbitmap_burst(uint32_t addr, uint16_t fmt, uint16_t width, uint16_t height)
{
	spi_transmit_burst(CMD_SETBITMAP);
	spi_transmit_burst(addr);
	spi_transmit_burst(((uint32_t)fmt) + (((uint32_t)width) << 16U));
	spi_transmit_burst((uint32_t)height);
}

/**
 * @brief Register one custom font into the coprocessor engine.
 * @note - does not set up the bitmap parameters of the font
 */
void EVE_cmd_setfont(uint32_t font, uint32_t ptr)
{
	if(0U == cmd_burst) {
		eve_begin_cmd(CMD_SETFONT);
		spi_transmit_32(font);
		spi_transmit_32(ptr);
		EVE_cs_clear();
	} else {
		spi_transmit_burst(CMD_SETFONT);
		spi_transmit_burst(font);
		spi_transmit_burst(ptr);
	}
}

/**
 * @brief Register one custom font into the coprocessor engine, only works in burst-mode.
 * @note - does not set up the bitmap parameters of the font
 */
void EVE_cmd_setfont_burst(uint32_t font, uint32_t ptr)
{
	spi_transmit_burst(CMD_SETFONT);
	spi_transmit_burst(font);
	spi_transmit_burst(ptr);
}

/**
 * @brief Set up a custom for use by the coprocessor engine.
 * @note - generates display list commands, so it needs to be put in a display list
 */
void EVE_cmd_setfont2(uint32_t font, uint32_t ptr, uint32_t firstchar)
{
	if(0U == cmd_burst) {
		eve_begin_cmd(CMD_SETFONT2);
		spi_transmit_32(font);
		spi_transmit_32(ptr);
		spi_transmit_32(firstchar);
		EVE_cs_clear();
	} else {
		spi_transmit_burst(CMD_SETFONT2);
		spi_transmit_burst(font);
		spi_transmit_burst(ptr);
		spi_transmit_burst(firstchar);
	}
}

/**
 * @brief Set up a custom for use by the coprocessor engine, only works in burst-mode.
 * @note - generates display list commands, so it needs to be put in a display list
 */
void EVE_cmd_setfont2_burst(uint32_t font, uint32_t ptr, uint32_t firstchar)
{
	spi_transmit_burst(CMD_SETFONT2);
	spi_transmit_burst(font);
	spi_transmit_burst(ptr);
	spi_transmit_burst(firstchar);
}

/**
 * @brief Set the scratch bitmap for widget use.
 */
void EVE_cmd_setscratch(uint32_t handle)
{
	if(0U == cmd_burst) {
		eve_begin_cmd(CMD_SETSCRATCH);
		spi_transmit_32(handle);
		EVE_cs_clear();
	} else {
		spi_transmit_burst(CMD_SETSCRATCH);
		spi_transmit_burst(handle);
	}
}

/**
 * @brief Set the scratch bitmap for widget use, only works in burst-mode.
 */
void EVE_cmd_setscratch_burst(uint32_t handle)
{
	spi_transmit_burst(CMD_SETSCRATCH);
	spi_transmit_burst(handle);
}

/**
 * @brief Start a continuous sketch update.
 */
void EVE_cmd_sketch(int16_t xc0, int16_t yc0, uint16_t wid, uint16_t hgt, uint32_t ptr, uint16_t format)
{
	if(0U == cmd_burst) {
		eve_begin_cmd(CMD_SKETCH);
		spi_transmit((uint8_t)((uint16_t)xc0));
		spi_transmit((uint8_t)(((uint16_t)xc0) >> 8U));
		spi_transmit((uint8_t)((uint16_t)yc0));
		spi_transmit((uint8_t)(((uint16_t)yc0) >> 8U));
		spi_transmit((uint8_t)((uint16_t)wid));
		spi_transmit((uint8_t)(((uint16_t)wid) >> 8U));
		spi_transmit((uint8_t)((uint16_t)hgt));
		spi_transmit((uint8_t)(((uint16_t)hgt) >> 8U));
		spi_transmit_32(ptr);
		spi_transmit((uint8_t)(format));
		spi_transmit((uint8_t)(format >> 8U));
		spi_transmit(0U);
		spi_transmit(0U);
		EVE_cs_clear();
	} else {
		spi_transmit_burst(CMD_SKETCH);
		spi_transmit_burst(((uint32_t)((uint16_t)xc0)) + (((uint32_t)((uint16_t)yc0)) << 16U));
		spi_transmit_burst(((uint32_t)((uint16_t)wid)) + (((uint32_t)((uint16_t)hgt)) << 16U));
		spi_transmit_burst(ptr);
		spi_transmit_burst((uint32_t)format);
	}
}

/**
 * @brief Start a continuous sketch update, only works in burst-mode.
 */
void EVE_cmd_sketch_burst(int16_t xc0, int16_t yc0, uint16_t wid, uint16_t hgt, uint32_t ptr, uint16_t format)
{
	spi_transmit_burst(CMD_SKETCH);
	spi_transmit_burst(((uint32_t)((uint16_t)xc0)) + (((uint32_t)((uint16_t)yc0)) << 16U));
	spi_transmit_burst(((uint32_t)((uint16_t)wid)) + (((uint32_t)((uint16_t)hgt)) << 16U));
	spi_transmit_burst(ptr);
	spi_transmit_burst((uint32_t)format);
}

/**
 * @brief Draw a slider.
 */
void EVE_cmd_slider(int16_t xc0, int16_t yc0, uint16_t wid, uint16_t hgt, uint16_t options, uint16_t val,
					uint16_t range)
{
	if(0U == cmd_burst) {
		eve_begin_cmd(CMD_SLIDER);
		spi_transmit((uint8_t)((uint16_t)xc0));
		spi_transmit((uint8_t)(((uint16_t)xc0) >> 8U));
		spi_transmit((uint8_t)((uint16_t)yc0));
		spi_transmit((uint8_t)(((uint16_t)yc0) >> 8U));
		spi_transmit((uint8_t)(wid));
		spi_transmit((uint8_t)(wid >> 8U));
		spi_transmit((uint8_t)(hgt));
		spi_transmit((uint8_t)(hgt >> 8U));
		spi_transmit((uint8_t)(options));
		spi_transmit((uint8_t)(options >> 8U));
		spi_transmit((uint8_t)(val));
		spi_transmit((uint8_t)(val >> 8U));
		spi_transmit((uint8_t)(range));
		spi_transmit((uint8_t)(range >> 8U));
		spi_transmit(0U); /* dummy byte for 4-byte alignment */
		spi_transmit(0U); /* dummy byte for 4-byte alignment */
		EVE_cs_clear();
	} else {
		spi_transmit_burst(CMD_SLIDER);
		spi_transmit_burst(((uint32_t)((uint16_t)xc0)) + (((uint32_t)((uint16_t)yc0)) << 16U));
		spi_transmit_burst(((uint32_t)wid) + (((uint32_t)hgt) << 16U));
		spi_transmit_burst(((uint32_t)options) + (((uint32_t)val) << 16U));
		spi_transmit_burst((uint32_t)range);
	}
}

/**
 * @brief Draw a slider, only works in burst-mode.
 */
void EVE_cmd_slider_burst(int16_t xc0, int16_t yc0, uint16_t wid, uint16_t hgt, uint16_t options, uint16_t val,
						  uint16_t range)
{
	spi_transmit_burst(CMD_SLIDER);
	spi_transmit_burst(((uint32_t)((uint16_t)xc0)) + (((uint32_t)((uint16_t)yc0)) << 16U));
	spi_transmit_burst(((uint32_t)wid) + (((uint32_t)hgt) << 16U));
	spi_transmit_burst(((uint32_t)options) + (((uint32_t)val) << 16U));
	spi_transmit_burst((uint32_t)range);
}

/**
 * @brief Start an animated spinner.
 */
void EVE_cmd_spinner(int16_t xc0, int16_t yc0, uint16_t style, uint16_t scale)
{
	if(0U == cmd_burst) {
		eve_begin_cmd(CMD_SPINNER);
		spi_transmit((uint8_t)((uint16_t)xc0));
		spi_transmit((uint8_t)(((uint16_t)xc0) >> 8U));
		spi_transmit((uint8_t)((uint16_t)yc0));
		spi_transmit((uint8_t)(((uint16_t)yc0) >> 8U));
		spi_transmit((uint8_t)(style));
		spi_transmit((uint8_t)(style >> 8U));
		spi_transmit((uint8_t)(scale));
		spi_transmit((uint8_t)(scale >> 8U));
		EVE_cs_clear();
	} else {
		spi_transmit_burst(CMD_SPINNER);
		spi_transmit_burst(((uint32_t)((uint16_t)xc0)) + (((uint32_t)((uint16_t)yc0)) << 16U));
		spi_transmit_burst(((uint32_t)style) + (((uint32_t)scale) << 16U));
	}
}

/**
 * @brief Start an animated spinner, only works in burst-mode.
 */
void EVE_cmd_spinner_burst(int16_t xc0, int16_t yc0, uint16_t style, uint16_t scale)
{
	spi_transmit_burst(CMD_SPINNER);
	spi_transmit_burst(((uint32_t)((uint16_t)xc0)) + (((uint32_t)((uint16_t)yc0)) << 16U));
	spi_transmit_burst(((uint32_t)style) + (((uint32_t)scale) << 16U));
}

/**
 * @brief Draw a text string.
 */
void EVE_cmd_text(int16_t xc0, int16_t yc0, uint16_t font, uint16_t options, const char* p_text)
{
	if(0U == cmd_burst) {
		eve_begin_cmd(CMD_TEXT);
		spi_transmit((uint8_t)((uint16_t)xc0));
		spi_transmit((uint8_t)(((uint16_t)xc0) >> 8U));
		spi_transmit((uint8_t)((uint16_t)yc0));
		spi_transmit((uint8_t)(((uint16_t)yc0) >> 8U));
		spi_transmit((uint8_t)(font));
		spi_transmit((uint8_t)(font >> 8U));
		spi_transmit((uint8_t)(options));
		spi_transmit((uint8_t)(options >> 8U));
		private_string_write(p_text);
		EVE_cs_clear();
	} else {
		spi_transmit_burst(CMD_TEXT);
		spi_transmit_burst(((uint32_t)((uint16_t)xc0)) + (((uint32_t)((uint16_t)yc0)) << 16U));
		spi_transmit_burst(((uint32_t)font) + (((uint32_t)options) << 16U));
		private_string_write(p_text);
	}
}

/**
 * @brief Draw a text string, only works in burst-mode.
 */
void EVE_cmd_text_burst(int16_t xc0, int16_t yc0, uint16_t font, uint16_t options, const char* p_text)
{
	spi_transmit_burst(CMD_TEXT);
	spi_transmit_burst(((uint32_t)((uint16_t)xc0)) + (((uint32_t)((uint16_t)yc0)) << 16U));
	spi_transmit_burst(((uint32_t)font) + (((uint32_t)options) << 16U));
	private_string_write(p_text);
}

/**
 * @brief Draw a toggle switch with labels.
 */
void EVE_cmd_toggle(int16_t xc0, int16_t yc0, uint16_t wid, uint16_t font, uint16_t options, uint16_t state,
					const char* p_text)
{
	if(0U == cmd_burst) {
		eve_begin_cmd(CMD_TOGGLE);
		spi_transmit((uint8_t)((uint16_t)xc0));
		spi_transmit((uint8_t)(((uint16_t)xc0) >> 8U));
		spi_transmit((uint8_t)((uint16_t)yc0));
		spi_transmit((uint8_t)(((uint16_t)yc0) >> 8U));
		spi_transmit((uint8_t)(wid));
		spi_transmit((uint8_t)(wid >> 8U));
		spi_transmit((uint8_t)(font));
		spi_transmit((uint8_t)(font >> 8U));
		spi_transmit((uint8_t)(options));
		spi_transmit((uint8_t)(options >> 8U));
		spi_transmit((uint8_t)(state));
		spi_transmit((uint8_t)(state >> 8U));
		private_string_write(p_text);
		EVE_cs_clear();
	} else {
		spi_transmit_burst(CMD_TOGGLE);
		spi_transmit_burst(((uint32_t)((uint16_t)xc0)) + (((uint32_t)((uint16_t)yc0)) << 16U));
		spi_transmit_burst(((uint32_t)wid) + (((uint32_t)font) << 16U));
		spi_transmit_burst(((uint32_t)options) + (((uint32_t)state) << 16U));
		private_string_write(p_text);
	}
}

/**
 * @brief Draw a toggle switch with labels, only works in burst-mode.
 */
void EVE_cmd_toggle_burst(int16_t xc0, int16_t yc0, uint16_t wid, uint16_t font, uint16_t options, uint16_t state,
						  const char* p_text)
{
	spi_transmit_burst(CMD_TOGGLE);
	spi_transmit_burst(((uint32_t)((uint16_t)xc0)) + (((uint32_t)((uint16_t)yc0)) << 16U));
	spi_transmit_burst(((uint32_t)wid) + (((uint32_t)font) << 16U));
	spi_transmit_burst(((uint32_t)options) + (((uint32_t)state) << 16U));
	private_string_write(p_text);
}

/**
 * @brief Apply a translation to the current matrix.
 */
void EVE_cmd_translate(int32_t tr_x, int32_t tr_y)
{
	if(0U == cmd_burst) {
		eve_begin_cmd(CMD_TRANSLATE);
		spi_transmit_32((uint32_t)tr_x);
		spi_transmit_32((uint32_t)tr_y);
		EVE_cs_clear();
	} else {
		spi_transmit_burst(CMD_TRANSLATE);
		spi_transmit_burst((uint32_t)tr_x);
		spi_transmit_burst((uint32_t)tr_y);
	}
}

/**
 * @brief Set the current color red, green and blue.
 */
void EVE_color_rgb(uint32_t color)
{
	EVE_cmd_dl(DL_COLOR_RGB | (color & 0x00ffffffUL));
}

/**
 * @brief Set the current color alpha, green and blue.
 */
void EVE_color_a(uint8_t alpha)
{
	EVE_cmd_dl(DL_COLOR_A | ((uint32_t)alpha));
}

} // namespace Graphics::EVE::CoProc
