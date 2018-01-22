/*
 *  See License file
 */

#ifndef ICELIB_ICELIB_CONFIG_H
#define ICELIB_ICELIB_CONFIG_H

#define ICELIB_RANDOM_SEGMENT_LENGTH    (32 / 6)

#define ICE_MAX_DEBUG_STRING      200

#define ICELIB_UFRAG_LENGTH       (4 + 1)     /* includes zero ('\0')
                                               * termination */
#define ICELIB_PASSWD_LENGTH      (22 + 1)    /* includes zero ('\0')
                                               * termination */
#define ICELIB_FOUNDATION_LENGTH  (16 + 1)     /* includes zero ('\0')
                                               * termination */

#define ICE_MAX_UFRAG_PAIR_LENGTH       ( (ICE_MAX_UFRAG_LENGTH      * 2) + 1 )
#define ICE_MAX_FOUNDATION_PAIR_LENGTH  ( (ICE_MAX_FOUNDATION_LENGTH * 2) )

#define ICELIB_MAX_PAIRS          40
#define ICELIB_MAX_FIFO_ELEMENTS  40
#define ICELIB_MAX_COMPONENTS     5

#define ICELIB_LOCAL_TYPEPREF   126
#define ICELIB_PEERREF_TYPEREF  110
#define ICELIB_REFLEX_TYPEREF   100
#define ICELIB_RELAY_TYPEREF    0

#define ICELIB_RTP_COMPONENT_ID 1
#define ICELIB_RTCP_COMPONENT_ID 2

#define ICELIB_RTP_COMPONENT_INDEX 0
#define ICELIB_RTCP_COMPONENT_INDEX 1

#define ICELIB_FAIL_AFTER_MS                5000 /*5 sec*/

#define ICE_MAX_UFRAG_LENGTH                (256 + 1) /* zero terminated */
#define ICE_MAX_PASSWD_LENGTH               (256 + 1) /* zero terminated */
#define ICE_MAX_CANDIDATES                  32
#define ICE_MAX_FOUNDATION_LENGTH           (32 + 1)  /* zero terminated */
#define ICE_MAX_MEDIALINES                  20
#define ICE_MAX_COMPONENTS                  2

#endif //ICELIB_ICELIB_CONFIG_H
