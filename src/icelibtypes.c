/*
 *  See License file
 */

#include "sockaddr_util.h"
#include "icelibtypes.h"
#include <stdlib.h>
#include <string.h>

char const*
ICELIBTYPES_ICE_CANDIDATE_TYPE_toString(const ICE_CANDIDATE_TYPE candidateType)
{
  switch (candidateType)
  {
  case ICE_CAND_TYPE_NONE:
    return "NONE";
  case ICE_CAND_TYPE_HOST:
    return "HOST";
  case ICE_CAND_TYPE_SRFLX:
    return "SRFLX";
  case ICE_CAND_TYPE_RELAY:
    return "RELAY";
  case ICE_CAND_TYPE_PRFLX:
    return "PRFLX";
  }
  return "UNKNOWN";
}

char const*
ICELIBTYPES_ICE_CANDIDATE_Component_toString (uint32_t componentid)
{
  if (componentid == ICELIB_RTP_COMPONENT_ID)
  {
    return "RTP";
  }
  else if (componentid == ICELIB_RTCP_COMPONENT_ID)
  {
    return "RTCP";
  }

  return "UNKNOWN Component";

}

void
ICELIBTYPES_ICE_CANDIDATE_reset(ICE_CANDIDATE* candidate)
{
  memset( candidate, 0, sizeof(*candidate) );
}

bool
ICELIBTYPES_ICE_MEDIA_STREAM_isEmpty(const ICE_MEDIA_STREAM* iceMediaStream)
{
  if (iceMediaStream->numberOfCandidates > 0)
  {
    return false;
  }
  return true;
}

bool
ICELIBTYPES_ICE_MEDIA_isEmpty(const ICE_MEDIA* iceMedia)
{
  if (iceMedia->numberOfICEMediaLines > 0)
  {
    return false;
  }
  return true;
}

void
ICELIBTYPES_ICE_MEDIA_STREAM_reset(ICE_MEDIA_STREAM* iceMediaStream)
{
  memset( iceMediaStream, 0, sizeof(*iceMediaStream) );
}

void
ICELIBTYPES_ICE_MEDIA_reset(ICE_MEDIA* iceMedia)
{
  memset( iceMedia, 0, sizeof(*iceMedia) );
}

int
ICE_TRANSPORT_proto(ICE_TRANSPORT transport)
{
  switch (transport)
  {
  case ICE_TRANS_UDP:
    return IPPROTO_UDP;

  case ICE_TRANS_TCPACT:
  case ICE_TRANS_TCPPASS:
    return IPPROTO_TCP;
  }

  abort();
}
