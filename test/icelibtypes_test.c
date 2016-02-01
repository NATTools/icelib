
#include <string.h>

#include "test_utils.h"
#include "icelibtypes.h"

CTEST(icelibtypes, candidate_toString)
{
  ASSERT_TRUE( 0 ==
               strcmp(ICELIBTYPES_ICE_CANDIDATE_TYPE_toString(ICE_CAND_TYPE_NONE),
                      "NONE") );
  ASSERT_TRUE( 0 ==
               strcmp(ICELIBTYPES_ICE_CANDIDATE_TYPE_toString(ICE_CAND_TYPE_HOST),
                      "HOST") );
  ASSERT_TRUE( 0 ==
               strcmp(ICELIBTYPES_ICE_CANDIDATE_TYPE_toString(
                        ICE_CAND_TYPE_SRFLX),
                      "SRFLX") );
  ASSERT_TRUE( 0 ==
               strcmp(ICELIBTYPES_ICE_CANDIDATE_TYPE_toString(
                        ICE_CAND_TYPE_RELAY),
                      "RELAY") );
  ASSERT_TRUE( 0 ==
               strcmp(ICELIBTYPES_ICE_CANDIDATE_TYPE_toString(
                        ICE_CAND_TYPE_PRFLX),
                      "PRFLX") );
  ASSERT_TRUE( 0 ==
               strcmp(ICELIBTYPES_ICE_CANDIDATE_TYPE_toString(24), "UNKNOWN") );

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
