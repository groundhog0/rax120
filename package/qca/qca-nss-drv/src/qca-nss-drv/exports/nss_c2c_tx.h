/*
 **************************************************************************
 * Copyright (c) 2018, The Linux Foundation. All rights reserved.
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all copies.
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 **************************************************************************
 */

/**
 * @file nss_c2c_tx.h
 *	NSS core-to-core transmission interface definitions.
 */

#ifndef __NSS_C2C_TX_H
#define __NSS_C2C_TX_H

/**
 * @addtogroup nss_c2c_tx_subsystem
 * @{
 */

/**
 * nss_c2c_tx_msg_type
 *	Supported message types.
 */
enum nss_c2c_tx_msg_type {
	NSS_C2C_TX_MSG_TYPE_STATS,		/**< Statistics synchronization. */
	NSS_C2C_TX_MSG_TYPE_TX_MAP,		/**< Open engine synchronization. */
	NSS_C2C_TX_MSG_TYPE_MAX			/**< Maximum message type. */
};

/**
 * nss_c2c_tx_map
 *	Core-to-core transmission queue address and interrupt address.
 */
struct nss_c2c_tx_map {
	uint32_t tx_map;		/**< Peer core core-to-core receiver queue start address. */
	uint32_t c2c_intr_addr;		/**< Peer core core-to-core interrupt register address. */
};

/**
 * nss_c2c_tx_stats
 *	The NSS core-to-core transmission node statistics structure.
 */
struct nss_c2c_tx_stats {
	struct nss_cmn_node_stats node_stats;
					/**< Common node statistics for core-to-core transmissions. */
	uint32_t pbuf_simple;		/**< Number of received simple pbuf. */
	uint32_t pbuf_sg;		/**< Number of scattered/gathered pbuf received. */
	uint32_t pbuf_returning;	/**< Number of returning scattered/gathered pbuf. */
};

/**
 * nss_c2c_tx_msg
 *	Message structure to send/receive core-to-core transmission commands.
 */
struct nss_c2c_tx_msg {
	struct nss_cmn_msg cm;			/**< Common message header. */

	/**
	 * Payload of a NSS core-to-core transmission rule or statistics message.
	 */
	union {
		struct nss_c2c_tx_map map;	/**< Core-to-core transmissions memory map. */
		struct nss_c2c_tx_stats stats;	/**< Core-to-core transmissions statistics. */
	} msg;					/**< Message payload. */
};

/**
 * nss_c2c_tx_register_handler
 *	Registers the core-to-core transmissions message handler.
 *
 * @datatypes
 * nss_ctx_instance
 *
 * @param[in] nss_ctx  Pointer to the NSS context.
 *
 * @return
 * None.
 */
void nss_c2c_tx_register_handler(struct nss_ctx_instance *nss_ctx);

/**
 * Callback function for receiving core-to-core transmissions messages.
 *
 * @datatypes
 * nss_c2c_tx_msg
 *
 * @param[in] app_data  Pointer to the application context of the message.
 * @param[in] msg       Pointer to the message data.
 */
typedef void (*nss_c2c_tx_msg_callback_t)(void *app_data, struct nss_c2c_tx_msg *msg);

/**
 * nss_c2c_tx_tx_msg
 *	Transmits a core-to-core transmissions message to the NSS.
 *
 * @datatypes
 * nss_ctx_instance \n
 * nss_c2c_tx_msg
 *
 * @param[in] nss_ctx   Pointer to the NSS context.
 * @param[in] nctm      Pointer to the message data.
 *
 * @return
 * Status of the transmit operation.
 */
extern nss_tx_status_t nss_c2c_tx_tx_msg(struct nss_ctx_instance *nss_ctx, struct nss_c2c_tx_msg *nctm);

/**
 * nss_c2c_tx_msg_init
 *	Initializes core-to-core transmissions messages.
 *
 * @datatypes
 * nss_c2c_tx_msg \n
 * nss_c2c_tx_msg_callback_t
 *
 * @param[in]     nct       Pointer to the NSS interface message.
 * @param[in]     if_num    NSS interface number.
 * @param[in]     type      Type of message.
 * @param[in]     len       Size of the payload.
 * @param[in]     cb        Callback function for the message.
 * @param[in]     app_data  Pointer to the application context of the message.
 *
 * @return
 * None.
 */
extern void nss_c2c_tx_msg_init(struct nss_c2c_tx_msg *nct, uint16_t if_num, uint32_t type, uint32_t len,
			nss_c2c_tx_msg_callback_t cb, void *app_data);

/**
 * nss_c2c_tx_notify_register
 *	Registers a notifier callback for core-to-core transmission messages with the NSS.
 *
 * @datatypes
 * nss_c2c_tx_msg_callback_t
 *
 * @param[in] core      NSS core number index to the notifier callback table.
 * @param[in] cb        Callback function for the message.
 * @param[in] app_data  Pointer to the application context of the message.
 *
 * @return
 * Pointer to the NSS core context.
 */
struct nss_ctx_instance *nss_c2c_tx_notify_register(int core, nss_c2c_tx_msg_callback_t cb, void *app_data);

/**
 * nss_c2c_tx_notify_unregister
 *	Deregisters a core-to-core transmission message notifier callback from the NSS.
 *
 * @param[in] core NSS core number index to the notifier callback table.
 *
 * @return
 * None.
 *
 * @dependencies
 * The notifier callback must have been previously registered.
 */
void nss_c2c_tx_notify_unregister(int core);

/**
 * nss_c2c_tx_msg_cfg_map
 *	Sends core-to-core transmissions map to NSS
 *
 * @datatypes
 * nss_ctx_instance \n
 *
 * @param[in] nss_ctx   Pointer to the NSS context.
 * @param[in] tx_map    Peer core core-to-core receiver queue start address.
 * @param[in] c2c_addr  Peer core core-to-core interrupt register address.
 *
 * @return
 * Status of the transmit operation.
 */
extern nss_tx_status_t nss_c2c_tx_msg_cfg_map(struct nss_ctx_instance *nss_ctx, uint32_t tx_map, uint32_t c2c_addr);

/**
 * nss_c2c_tx_init
 *	Initializes the core-to-core transmission.
 *
 * @return
 * None.
 */
void nss_c2c_tx_init(void);

/**
 * @}
 */

#endif /* __NSS_C2C_TX_H */
