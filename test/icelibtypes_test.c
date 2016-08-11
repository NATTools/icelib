
#include <string.h>

#include "test_utils.h"
#include "icelibtypes.h"

CTEST(icelibtypes, candidate_toString)
{
  ASSERT_TRUE( 0 ==
               strcmp(ICELIBTYPES_ICE_CANDIDATE_TYPE_toString(ICE_CAND_TYPE_NONE),
                      "none") );
  ASSERT_TRUE( 0 ==
               strcmp(ICELIBTYPES_ICE_CANDIDATE_TYPE_toString(ICE_CAND_TYPE_HOST),
                      "host") );
  ASSERT_TRUE( 0 ==
               strcmp(ICELIBTYPES_ICE_CANDIDATE_TYPE_toString(
                        ICE_CAND_TYPE_SRFLX),
                      "srflx") );
  ASSERT_TRUE( 0 ==
               strcmp(ICELIBTYPES_ICE_CANDIDATE_TYPE_toString(
                        ICE_CAND_TYPE_RELAY),
                      "relay") );
  ASSERT_TRUE( 0 ==
               strcmp(ICELIBTYPES_ICE_CANDIDATE_TYPE_toString(
                        ICE_CAND_TYPE_PRFLX),
                      "prflx") );
  ASSERT_TRUE( 0 ==
               strcmp(ICELIBTYPES_ICE_CANDIDATE_TYPE_toString(24), "unknown") );

}

CTEST(icelibtypes, transport_toString)
{
  ASSERT_TRUE( 0 ==
               strcmp(ICELIBTYPES_ICE_TRANSPORT_PROTO_toString(ICE_TRANS_NONE),
                      "NONE") );
  ASSERT_TRUE( 0 ==
               strcmp(ICELIBTYPES_ICE_TRANSPORT_PROTO_toString(ICE_TRANS_UDP),
                      "UDP") );
  ASSERT_TRUE( 0 ==
               strcmp(ICELIBTYPES_ICE_TRANSPORT_PROTO_toString(ICE_TRANS_TCPACT),
                      "TCP") );
  ASSERT_TRUE( 0 ==
               strcmp(ICELIBTYPES_ICE_TRANSPORT_PROTO_toString(ICE_TRANS_TCPPASS),
                      "TCP") );

  ASSERT_TRUE( 0 ==
               strcmp(ICELIBTYPES_ICE_TRANSPORT_toString(ICE_TRANS_NONE),
                      "none") );
  ASSERT_TRUE( 0 ==
               strcmp(ICELIBTYPES_ICE_TRANSPORT_toString(ICE_TRANS_UDP),
                      "udp") );
  ASSERT_TRUE( 0 ==
               strcmp(ICELIBTYPES_ICE_TRANSPORT_toString(ICE_TRANS_TCPACT),
                      "tcpact") );
  ASSERT_TRUE( 0 ==
               strcmp(ICELIBTYPES_ICE_TRANSPORT_toString(ICE_TRANS_TCPPASS),
                      "tcppass") );
}


CTEST(icelibtypes, iceMedia_empty)
{
  ICE_MEDIA iceMedia;
  ICELIBTYPES_ICE_MEDIA_reset(&iceMedia);


  ASSERT_TRUE( ICELIBTYPES_ICE_MEDIA_isEmpty(&iceMedia) );

  iceMedia.numberOfICEMediaLines = 2;

  ASSERT_FALSE( ICELIBTYPES_ICE_MEDIA_isEmpty(&iceMedia) );

}


CTEST(icelibtypes, mediastream_empty){
  ICE_MEDIA_STREAM iceMediaStream;

  ICELIBTYPES_ICE_MEDIA_STREAM_reset(&iceMediaStream);
  ASSERT_TRUE( ICELIBTYPES_ICE_MEDIA_STREAM_isEmpty(&iceMediaStream) );

  iceMediaStream.numberOfCandidates = 3;

  ASSERT_FALSE( ICELIBTYPES_ICE_MEDIA_STREAM_isEmpty(&iceMediaStream) );
}
