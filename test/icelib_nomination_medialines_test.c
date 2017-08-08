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


typedef struct {
  uint64_t                priority;
  int32_t                 proto;
  struct sockaddr_storage local;
  struct sockaddr_storage remote;
}m_NominationCB;

m_ConncheckCB m_connChkCB[50];
uint32_t      num_checks = 0;

m_ConncheckCB m_nomChkCB[50];
uint32_t      num_nom = 0;  /* sorry could not resist the name..*/

m_NominationCB m_nominationCB[50];
uint32_t       num_pair_nom = 0;


ICELIB_Result
Complete(void*        pUserData,
         unsigned int userval1,
         bool         controlling,
         bool         failed)
{
  (void) pUserData;
  (void) userval1;
  (void) controlling;
  (void) failed;

  return 0;
}

ICELIB_Result
Nominated(void*                  pUserData,
          uint32_t               userValue1,
          uint32_t               userValue2,
          uint32_t               componentId,
          uint64_t               priority,
          int32_t                proto,
          const struct sockaddr* local,
          const struct sockaddr* remote)
{
  (void)pUserData;
  (void)userValue1;
  (void)userValue2;
  (void)componentId;
  (void)priority;
  (void) proto;
  (void)local;
  (void) remote;

  m_nominationCB[num_pair_nom].priority = priority;
  m_nominationCB[num_pair_nom].proto    = proto;
  sockaddr_copy( (struct sockaddr*)&m_nominationCB[num_pair_nom].local,
                 local );
  sockaddr_copy( (struct sockaddr*)&m_nominationCB[num_pair_nom].remote,
                 remote );
  num_pair_nom++;
  return 0;
}

ICELIB_Result
sendConnectivityCheck(void*                  pUserData,
                      int                    proto,
                      int                    socket,
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
  (void) socket;
  if (useCandidate)
  {
    m_nomChkCB[num_nom].gotCB       = true;
    m_nomChkCB[num_nom].destination = destination;
    m_nomChkCB[num_nom].source      = source;
    m_nomChkCB[num_nom].userValue1  = userValue1;
    m_nomChkCB[num_nom].userValue2  = userValue2;
    m_nomChkCB[num_nom].componentId = componentId;
    m_nomChkCB[num_nom].useRelay    = useRelay;
    strncpy(m_nomChkCB[num_nom].ufrag, pUfrag, ICE_MAX_UFRAG_LENGTH);
    m_nomChkCB[num_nom].pPasswd        = pPasswd;
    m_nomChkCB[num_nom].peerPriority   = peerPriority;
    m_nomChkCB[num_nom].useCandidate   = useCandidate;
    m_nomChkCB[num_nom].iceControlling = iceControlling;
    m_nomChkCB[num_nom].iceControlled  = iceControlled;
    m_nomChkCB[num_nom].transactionId  = transactionId;
    m_nomChkCB[num_nom].tieBreaker     = tieBreaker;
    num_nom++;
  }
  else
  {
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
  }
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
  /* printf("%s\n", str); */
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
  struct sockaddr_storage m1_localHostRtp;

  ICELIB_CONFIGURATION iceConfig;

  uint32_t mediaIdx;

  srand( time(NULL) );
  num_checks   = 0;
  num_nom      = 0;
  num_pair_nom = 0;

  m_icelib = (ICELIB_INSTANCE*)malloc( sizeof(ICELIB_INSTANCE) );

  sockaddr_initFromString( (struct sockaddr*)&m0_localHostRtp,
                           "192.168.2.10:56780" );
  sockaddr_initFromString( (struct sockaddr*)&m1_localHostRtp,
                           "192.168.2.10:56788" );


  iceConfig.tickIntervalMS       = 20;
  iceConfig.keepAliveIntervalS   = 15;
  iceConfig.maxCheckListPairs    = ICELIB_MAX_PAIRS;
  iceConfig.aggressiveNomination = true;
  iceConfig.iceLite              = false;
  iceConfig.logLevel             = ICELIB_logDebug;
  /* iceConfig.logLevel = ICELIB_logDisable; */


  ICELIB_Constructor(m_icelib,
                     &iceConfig);

  ICELIB_setCallbackOutgoingBindingRequest(m_icelib,
                                           sendConnectivityCheck,
                                           NULL);

  ICELIB_setCallbackConnecitivityChecksComplete(m_icelib,
                                                Complete,
                                                NULL);


  ICELIB_setCallbackLog(m_icelib,
                        printLog,
                        NULL,
                        ICELIB_logDebug);

  ICELIB_setCallbackNominated(m_icelib,
                              Nominated,
                              NULL);

  /* Local side */
  /* Medialine: 0 */
  mediaIdx = ICELIB_addLocalMediaStream(m_icelib, 42, 42, ICE_CAND_TYPE_HOST);
  ICELIB_addLocalCandidate(m_icelib,
                           mediaIdx,
                           1,
                           5,
                           (struct sockaddr*)&m0_localHostRtp,
                           NULL,
                           ICE_TRANS_UDP,
                           ICE_CAND_TYPE_HOST,
                           0xffff);
  mediaIdx = ICELIB_addLocalMediaStream(m_icelib, 42, 43, ICE_CAND_TYPE_HOST);
  ICELIB_addLocalCandidate(m_icelib,
                           mediaIdx,
                           1,
                           5,
                           (struct sockaddr*)&m1_localHostRtp,
                           NULL,
                           ICE_TRANS_UDP,
                           ICE_CAND_TYPE_HOST,
                           0xffff);

  /* Remote side */
  /* Medialine: 0 */
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
                            33434,
                            ICE_TRANS_UDP,
                            ICE_CAND_TYPE_HOST);
  /* Medialine: 1 */
  sockaddr_initFromString( (struct sockaddr*)&m0_defaultAddr,
                           "158.38.48.10:5004" );

  ICELIB_addRemoteMediaStream(m_icelib, remoteUfrag, remotePasswd,
                              (struct sockaddr*)&m0_defaultAddr);
  ICELIB_addRemoteCandidate(m_icelib,
                            1,
                            "1",
                            1,
                            1,
                            2130706431,
                            "158.38.48.10",
                            5004,
                            ICE_TRANS_UDP,
                            ICE_CAND_TYPE_HOST);

  ICELIB_addRemoteCandidate(m_icelib,
                            1,
                            "1",
                            1,
                            1,
                            2130705430,
                            "158.38.48.10",
                            3478,
                            ICE_TRANS_UDP,
                            ICE_CAND_TYPE_HOST);

  ICELIB_addRemoteCandidate(m_icelib,
                            1,
                            "1",
                            1,
                            1,
                            2130206431,
                            "158.38.48.10",
                            33434,
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
  memset(&m_connChkCB,    0, sizeof(m_ConncheckCB) * 50);
  memset(&m_nomChkCB,     0, sizeof(m_ConncheckCB) * 50);
  memset(&m_nominationCB, 0, sizeof(m_NominationCB) * 50);

  ASSERT_TRUE( ICELIB_Start(m_icelib, true) );

  ASSERT_TRUE( ICELIB_isRunning(m_icelib) );

  ASSERT_FALSE( ICELIB_Mangled(m_icelib) );

  for (int i = 0; i < 40; i++)
  {
    ICELIB_Tick(m_icelib);

  }
  /* All the chacks are sent.. Lets trigger some responses */
  for (uint32_t i = 0; i < num_checks; i++)
  {
    ICELIB_incomingBindingResponse(m_icelib,
                                   200,
                                   m_connChkCB[i].transactionId,
                                   m_connChkCB[i].destination,
                                   m_connChkCB[i].source,
                                   m_connChkCB[i].source);
    ICELIB_Tick(m_icelib);
  }

  /* Wait for nominations.. */
  for (uint32_t i = 0; i < 30; i++)
  {
    ICELIB_Tick(m_icelib);

  }

  for (uint32_t i = 0; i < num_nom; i++)
  {
    ICELIB_incomingBindingResponse(m_icelib,
                                   200,
                                   m_nomChkCB[i].transactionId,
                                   m_nomChkCB[i].destination,
                                   m_nomChkCB[i].source,
                                   m_nomChkCB[i].source);


    ICELIB_Tick(m_icelib);
  }
  ICELIB_Tick(m_icelib);

  ASSERT_TRUE(sockaddr_ipPort(
                (const struct sockaddr*)&m_nominationCB[0].local) == 56780);
  ASSERT_TRUE(sockaddr_ipPort(
                (const struct sockaddr*)&m_nominationCB[0].remote) == 5004);

  ASSERT_TRUE(sockaddr_ipPort(
                (const struct sockaddr*)&m_nominationCB[1].local) == 56788);
  ASSERT_TRUE(sockaddr_ipPort(
                (const struct sockaddr*)&m_nominationCB[1].remote) == 5004);


  ASSERT_TRUE( ICELIB_isIceComplete(m_icelib) );

  ASSERT_TRUE( m_icelib->iceState == ICELIB_COMPLETED);
}


CTEST2(data, multiple_host_addr_medialine_fail)
{
  (void) data;
  memset(&m_connChkCB,    0, sizeof(m_ConncheckCB) * 50);
  memset(&m_nomChkCB,     0, sizeof(m_ConncheckCB) * 50);
  memset(&m_nominationCB, 0, sizeof(m_NominationCB) * 50);


  ASSERT_TRUE( ICELIB_Start(m_icelib, true) );

  ASSERT_TRUE( ICELIB_isRunning(m_icelib) );

  ASSERT_FALSE( ICELIB_Mangled(m_icelib) );

  for (int i = 0; i < 40; i++)
  {
    ICELIB_Tick(m_icelib);

  }
  /* All the chacks are sent.. Lets trigger some responses */
  for (uint32_t i = 0; i < num_checks; i++)
  {

    if (m_connChkCB[i].userValue2 == 43)
    {
      /* This is medialine 2.. Ignore.. */
    }
    else
    {
      ICELIB_incomingBindingResponse(m_icelib,
                                     200,
                                     m_connChkCB[i].transactionId,
                                     m_connChkCB[i].destination,
                                     m_connChkCB[i].source,
                                     m_connChkCB[i].source);
      ICELIB_Tick(m_icelib);
    }
  }

  /* Wait for nominations.. */
  for (uint32_t i = 0; i < 30; i++)
  {
    ICELIB_Tick(m_icelib);

  }

  for (uint32_t i = 0; i < num_nom; i++)
  {
    ICELIB_incomingBindingResponse(m_icelib,
                                   200,
                                   m_nomChkCB[i].transactionId,
                                   m_nomChkCB[i].destination,
                                   m_nomChkCB[i].source,
                                   m_nomChkCB[i].source);


    ICELIB_Tick(m_icelib);
  }
  ICELIB_Tick(m_icelib);

  for (uint32_t i = 0; i < 2000; i++)
  {
    ICELIB_Tick(m_icelib);

  }

  ASSERT_TRUE(sockaddr_ipPort(
                (const struct sockaddr*)&m_nominationCB[0].local) == 56780);
  ASSERT_TRUE(sockaddr_ipPort(
                (const struct sockaddr*)&m_nominationCB[0].remote) == 5004);

  ASSERT_FALSE(sockaddr_ipPort(
                 (const struct sockaddr*)&m_nominationCB[1].local) == 56788);
  ASSERT_FALSE(sockaddr_ipPort(
                 (const struct sockaddr*)&m_nominationCB[1].remote) == 5004);


  ASSERT_FALSE( ICELIB_isIceComplete(m_icelib) );

  ASSERT_TRUE(m_icelib->iceState == ICELIB_FAILED);
}


#if 0
CTEST2(data, multiple_host_addr_missing)
{
  (void) data;
  memset( &m_connChkCB,    0, sizeof(m_ConncheckCB) );
  memset( &m_nomChkCB,     0, sizeof(m_ConncheckCB) );
  memset( &m_nominationCB, 0, sizeof(m_NominationCB) );

  ASSERT_TRUE( ICELIB_Start(m_icelib, true) );

  ASSERT_TRUE( ICELIB_isRunning(m_icelib) );

  ASSERT_FALSE( ICELIB_Mangled(m_icelib) );

  for (int i = 0; i < 15; i++)
  {
    ICELIB_Tick(m_icelib);

  }
  /* All the chacks are sent.. Lets trigger some responses */
  /* Let the lowest pri finish first.. */
  ICELIB_incomingBindingResponse(m_icelib,
                                 200,
                                 m_connChkCB[2].transactionId,
                                 m_connChkCB[2].destination,
                                 m_connChkCB[2].source,
                                 m_connChkCB[2].source);
  ICELIB_Tick(m_icelib);
  ICELIB_Tick(m_icelib);

  ICELIB_incomingBindingResponse(m_icelib,
                                 200,
                                 m_nomChkCB[0].transactionId,
                                 m_nomChkCB[0].destination,
                                 m_nomChkCB[0].source,
                                 m_nomChkCB[0].source);
  ICELIB_Tick(m_icelib);
  ICELIB_Tick(m_icelib);

  ASSERT_TRUE(sockaddr_ipPort(
                (const struct sockaddr*)&m_nominationCB[0].local) == 56780);
  ASSERT_TRUE(sockaddr_ipPort(
                (const struct sockaddr*)&m_nominationCB[0].remote) == 33434);


  /* So lets see what happens if a beetr pri pair shows up.. */
  ICELIB_incomingBindingResponse(m_icelib,
                                 200,
                                 m_connChkCB[1].transactionId,
                                 m_connChkCB[1].destination,
                                 m_connChkCB[1].source,
                                 m_connChkCB[1].source);

  ICELIB_Tick(m_icelib);
  ICELIB_Tick(m_icelib);

  ICELIB_incomingBindingResponse(m_icelib,
                                 200,
                                 m_nomChkCB[1].transactionId,
                                 m_nomChkCB[1].destination,
                                 m_nomChkCB[1].source,
                                 m_nomChkCB[1].source);
  ICELIB_Tick(m_icelib);
  ICELIB_Tick(m_icelib);
  ASSERT_TRUE(sockaddr_ipPort(
                (const struct sockaddr*)&m_nominationCB[1].local) == 56780);
  ASSERT_TRUE(sockaddr_ipPort(
                (const struct sockaddr*)&m_nominationCB[1].remote) == 3478);


  for (int i = 0; i < 2000; i++)
  {
    ICELIB_Tick(m_icelib);

  }


  ASSERT_TRUE( ICELIB_isIceComplete(m_icelib) );

  ASSERT_TRUE( m_icelib->iceState == ICELIB_COMPLETED);

}

CTEST2(data, ice_failure)
{
  (void) data;
  memset( &m_connChkCB,    0, sizeof(m_ConncheckCB) );
  memset( &m_nomChkCB,     0, sizeof(m_ConncheckCB) );
  memset( &m_nominationCB, 0, sizeof(m_NominationCB) );

  ASSERT_TRUE( ICELIB_Start(m_icelib, true) );

  ASSERT_TRUE( ICELIB_isRunning(m_icelib) );

  ASSERT_FALSE( ICELIB_Mangled(m_icelib) );

  for (int i = 0; i < 2000; i++)
  {
    ICELIB_Tick(m_icelib);

  }

  ASSERT_FALSE( ICELIB_isIceComplete(m_icelib) );
  ASSERT_TRUE(m_icelib->iceState == ICELIB_FAILED);

}
#endif
