#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#include "test_utils.h"
#include "icelib.h"


ICELIB_INSTANCE* m_icelib;
static char      remoteUfrag[]  = "rm0Uf";
static char      remotePasswd[] = "rm0Pa";


typedef struct {
  bool                   gotCB;
  const struct sockaddr* destination;
  const struct sockaddr* source;
  uint32_t               userValue1;
  uint32_t               userValue2;
  uint32_t               componentId;
  bool                   useRelay;
  char                   ufrag[ ICE_MAX_UFRAG_LENGTH];
  /* const char              *pUfrag; */
  const char* pPasswd;
  uint32_t    peerPriority;
  bool        useCandidate;
  bool        iceControlling;
  bool        iceControlled;
  uint64_t    tieBreaker;
  StunMsgId   transactionId;
}m_ConncheckCB;

m_ConncheckCB m_connChkCB[100];
uint32_t      num_checks = 0;


ICELIB_Result
sendConnectivityCheck(void*                  pUserData,
                      int                    proto,
                      const struct sockaddr* destination,
                      const struct sockaddr* source,
                      uint32_t               userValue1,
                      uint32_t               userValue2,
                      uint32_t               componentId,
                      bool                   useRelay,
                      const char*            pUfrag,
                      const char*            pPasswd,
                      uint32_t               peerPriority,
                      bool                   useCandidate,
                      bool                   iceControlling,
                      bool                   iceControlled,
                      uint64_t               tieBreaker,
                      StunMsgId              transactionId)
{
  (void)pUserData;
  (void) proto;
  m_connChkCB[num_checks].gotCB       = true;
  m_connChkCB[num_checks].destination = destination;
  m_connChkCB[num_checks].source      = source;
  m_connChkCB[num_checks].userValue1  = userValue1;
  m_connChkCB[num_checks].userValue2  = userValue2;
  m_connChkCB[num_checks].componentId = componentId;
  m_connChkCB[num_checks].useRelay    = useRelay;
  strncpy(m_connChkCB[num_checks].ufrag, pUfrag, ICE_MAX_UFRAG_LENGTH);
  m_connChkCB[num_checks].pPasswd        = pPasswd;
  m_connChkCB[num_checks].peerPriority   = peerPriority;
  m_connChkCB[num_checks].useCandidate   = useCandidate;
  m_connChkCB[num_checks].iceControlling = iceControlling;
  m_connChkCB[num_checks].iceControlled  = iceControlled;
  m_connChkCB[num_checks].transactionId  = transactionId;
  m_connChkCB[num_checks].tieBreaker     = tieBreaker;

  num_checks++;
  return 0;
}

void
printLog(void*           pUserData,
         ICELIB_logLevel logLevel,
         const char*     str)
{
  (void)pUserData;
  (void)logLevel;
  (void)str;
  printf("%s\n", str);
}

CTEST_DATA(data)
{
  int a;
};


CTEST_SETUP(data)
{
  (void)data;
  struct sockaddr_storage m0_defaultAddr;
  struct sockaddr_storage m0_localHostRtp;

  ICELIB_CONFIGURATION iceConfig;

  uint32_t mediaIdx;

  srand( time(NULL) );

  m_icelib = (ICELIB_INSTANCE*)malloc( sizeof(ICELIB_INSTANCE) );

  /* ------------ Media Line: 0 */
  sockaddr_initFromString( (struct sockaddr*)&m0_localHostRtp,
                           "192.168.2.10:56780" );


  iceConfig.tickIntervalMS       = 20;
  iceConfig.keepAliveIntervalS   = 15;
  iceConfig.maxCheckListPairs    = ICELIB_MAX_PAIRS;
  iceConfig.aggressiveNomination = false;
  iceConfig.iceLite              = false;
  iceConfig.logLevel             = ICELIB_logDebug;
  /* iceConfig.logLevel = ICELIB_logDisable; */


  ICELIB_Constructor(m_icelib,
                     &iceConfig);

  ICELIB_setCallbackOutgoingBindingRequest(m_icelib,
                                           sendConnectivityCheck,
                                           NULL);

  ICELIB_setCallbackLog(m_icelib,
                        printLog,
                        NULL,
                        ICELIB_logDebug);

  /* Local side */
  /* Medialine: 0 */
  mediaIdx = ICELIB_addLocalMediaStream(m_icelib, 42, 42, ICE_CAND_TYPE_HOST);
  ICELIB_addLocalCandidate(m_icelib,
                           mediaIdx,
                           1,
                           (struct sockaddr*)&m0_localHostRtp,
                           NULL,
                           ICE_TRANS_UDP,
                           ICE_CAND_TYPE_HOST,
                           0xffff);

  /* Remote side */
  /* Medialine:
   *  0 */
  sockaddr_initFromString( (struct sockaddr*)&m0_defaultAddr,
                           "158.38.48.10:5004" );

  ICELIB_addRemoteMediaStream(m_icelib, remoteUfrag, remotePasswd,
                              (struct sockaddr*)&m0_defaultAddr);
  ICELIB_addRemoteCandidate(m_icelib,
                            0,
                            "1",
                            1,
                            1,
                            2130706431,
                            "158.38.48.10",
                            5004,
                            ICE_TRANS_UDP,
                            ICE_CAND_TYPE_HOST);

  ICELIB_addRemoteCandidate(m_icelib,
                            0,
                            "1",
                            1,
                            1,
                            2130705430,
                            "158.38.48.10",
                            3478,
                            ICE_TRANS_UDP,
                            ICE_CAND_TYPE_HOST);

  ICELIB_addRemoteCandidate(m_icelib,
                            0,
                            "1",
                            1,
                            1,
                            2130206431,
                            "158.38.48.10",
                            33343,
                            ICE_TRANS_UDP,
                            ICE_CAND_TYPE_HOST);
}




CTEST_TEARDOWN(data)
{
  (void) data;
  ICELIB_Destructor(m_icelib);
  free(m_icelib);
}



CTEST2(data, multiple_host_addr)
{
  (void) data;
  memset( &m_connChkCB, 0, sizeof(m_ConncheckCB) );

  ASSERT_TRUE( ICELIB_Start(m_icelib, true) );

  ASSERT_TRUE( ICELIB_isRunning(m_icelib) );

  ASSERT_FALSE( ICELIB_Mangled(m_icelib) );

  for (int i = 0; i < 15; i++)
  {
    ICELIB_Tick(m_icelib);

  }
  /* Reverse the order vi got the checks */
  for (int i = num_checks; i >= 0; i--)
  {
    ICELIB_Tick(m_icelib);
    printf("-----> Binding respinse (%i)\n", i);
    ICELIB_incomingBindingResponse(m_icelib,
                                   200,
                                   m_connChkCB[i].transactionId,
                                   m_connChkCB[i].destination,
                                   m_connChkCB[i].source,
                                   m_connChkCB[i].source);

  }
  num_checks = 0;
  for (int i = 0; i < 15; i++)
  {
    ICELIB_Tick(m_icelib);
    if (num_checks == 1)
    {
      ICELIB_incomingBindingResponse(m_icelib,
                                     200,
                                     m_connChkCB[0].transactionId,
                                     m_connChkCB[0].destination,
                                     m_connChkCB[0].source,
                                     m_connChkCB[0].source);
      num_checks = 0;
    }
  }

  ASSERT_TRUE( ICELIB_isIceComplete(m_icelib) );
/* ICELIB_validListDump(&m_icelib->streamControllers[0].validList); */

  ASSERT_TRUE( m_icelib->iceState == ICELIB_COMPLETED);

}
