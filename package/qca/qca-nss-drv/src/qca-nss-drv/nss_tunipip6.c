/*
 **************************************************************************
 * Copyright (c) 2014-2018, The Linux Foundation. All rights reserved.
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

#include "nss_tx_rx_common.h"

/*
 * nss_tunipip6_verify_if_num
 *	Verify the interface is a valid interface
 */
static bool nss_tunipip6_verify_if_num(uint32_t if_num)
{
	return nss_dynamic_interface_get_type(nss_tunipip6_get_context(), if_num) == NSS_DYNAMIC_INTERFACE_TYPE_TUNIPIP6;
}

/*
 * nss_tunipip6_handler()
 *	Handle NSS -> HLOS messages for 6rd tunnel
 */
static void nss_tunipip6_handler(struct nss_ctx_instance *nss_ctx, struct nss_cmn_msg *ncm, __attribute__((unused))void *app_data)
{
	struct nss_tunipip6_msg *ntm = (struct nss_tunipip6_msg *)ncm;
	void *ctx;
	nss_tunipip6_msg_callback_t cb;

	BUG_ON(!nss_tunipip6_verify_if_num(ncm->interface));

	/*
	 * Is this a valid request/response packet?
	 */
	if (ncm->type >= NSS_TUNIPIP6_MAX) {
		nss_warning("%p: received invalid message %d for DS-Lite interface", nss_ctx, ncm->type);
		return;
	}

	if (nss_cmn_get_msg_len(ncm) > sizeof(struct nss_tunipip6_msg)) {
		nss_warning("%p: Length of message is greater than required: %d", nss_ctx, nss_cmn_get_msg_len(ncm));
		return;
	}

	/*
	 * Update the callback and app_data for NOTIFY messages, tun6rd sends all notify messages
	 * to the same callback/app_data.
	 */
	if (ncm->response == NSS_CMM_RESPONSE_NOTIFY) {
		ncm->cb = (nss_ptr_t)nss_ctx->nss_top->tunipip6_msg_callback;
	}

	/*
	 * Log failures
	 */
	nss_core_log_msg_failures(nss_ctx, ncm);

	/*
	 * Do we have a call back
	 */
	if (!ncm->cb) {
		return;
	}

	/*
	 * callback
	 */
	cb = (nss_tunipip6_msg_callback_t)ncm->cb;
	ctx = nss_ctx->subsys_dp_register[ncm->interface].ndev;

	/*
	 * call ipip6 tunnel callback
	 */
	if (!ctx) {
		 nss_warning("%p: Event received for DS-Lite tunnel interface %d before registration", nss_ctx, ncm->interface);
		return;
	}

	cb(ctx, ntm);
}

/*
 * nss_tunipip6_tx()
 * 	Transmit a tun6rd message to NSSFW
 */
nss_tx_status_t nss_tunipip6_tx(struct nss_ctx_instance *nss_ctx, struct nss_tunipip6_msg *msg)
{
	struct nss_cmn_msg *ncm = &msg->cm;

	/*
	 * Sanity check the message
	 */
	if (!nss_tunipip6_verify_if_num(ncm->interface)) {
		nss_warning("%p: tx request for another interface: %d", nss_ctx, ncm->interface);
		return NSS_TX_FAILURE;
	}

	if (ncm->type > NSS_TUNIPIP6_MAX) {
		nss_warning("%p: message type out of range: %d", nss_ctx, ncm->type);
		return NSS_TX_FAILURE;
	}

	return nss_core_send_cmd(nss_ctx, msg, sizeof(*msg), NSS_NBUF_PAYLOAD_SIZE);
}
EXPORT_SYMBOL(nss_tunipip6_tx);

/*
 * **********************************
 *  Register/Unregister/Miscellaneous APIs
 * **********************************
 */

/*
 * nss_register_tunipip6_if()
 */
struct nss_ctx_instance *nss_register_tunipip6_if(uint32_t if_num,
			nss_tunipip6_callback_t tunipip6_callback,
			nss_tunipip6_msg_callback_t event_callback,
			struct net_device *netdev,
			uint32_t features)
{
	struct nss_ctx_instance *nss_ctx = (struct nss_ctx_instance *)&nss_top_main.nss[nss_top_main.tunipip6_handler_id];

	nss_assert(nss_ctx);
	nss_assert(nss_tunipip6_verify_if_num(if_num));

	nss_top_main.tunipip6_msg_callback = event_callback;
	nss_core_register_subsys_dp(nss_ctx, if_num, tunipip6_callback, NULL, NULL, netdev, features);
	nss_core_register_handler(nss_ctx, if_num, nss_tunipip6_handler, NULL);

	return nss_ctx;
}
EXPORT_SYMBOL(nss_register_tunipip6_if);

/*
 * nss_unregister_tunipip6_if()
 */
void nss_unregister_tunipip6_if(uint32_t if_num)
{
	struct nss_ctx_instance *nss_ctx = (struct nss_ctx_instance *)&nss_top_main.nss[nss_top_main.tunipip6_handler_id];

	nss_assert(nss_ctx);
	nss_assert(nss_tunipip6_verify_if_num(if_num));

	nss_core_unregister_subsys_dp(nss_ctx, if_num);

	nss_top_main.tunipip6_msg_callback = NULL;
}
EXPORT_SYMBOL(nss_unregister_tunipip6_if);

/*
 * nss_tunipip6_get_context()
 */
struct nss_ctx_instance *nss_tunipip6_get_context(void)
{
	return (struct nss_ctx_instance *)&nss_top_main.nss[nss_top_main.tunipip6_handler_id];
}
EXPORT_SYMBOL(nss_tunipip6_get_context);

/*
 * nss_tunipip6_register_handler()
 */
void nss_tunipip6_register_handler()
{
	struct nss_ctx_instance *nss_ctx = nss_tunipip6_get_context();

	nss_core_register_handler(nss_ctx, NSS_TUNIPIP6_INTERFACE, nss_tunipip6_handler, NULL);
}

/*
 * nss_tunipip6_msg_init()
 *	Initialize nss_tunipip6 msg.
 */
void nss_tunipip6_msg_init(struct nss_tunipip6_msg *ntm, uint16_t if_num, uint32_t type,  uint32_t len, void *cb, void *app_data)
{
	nss_cmn_msg_init(&ntm->cm, if_num, type, len, cb, app_data);
}
EXPORT_SYMBOL(nss_tunipip6_msg_init);
