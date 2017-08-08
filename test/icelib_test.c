#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#include "test_utils.h"
#include "icelib.h"
#include "../src/icelib_intern.h"

#define PRIORITY_HOST_1     ( (126 << 24) | (65535 << 8) | (256 - 1) )
#define PRIORITY_HOST_2     ( (126 << 24) | (65535 << 8) | (256 - 2) )
#define PRIORITY_SRFLX_1    ( (100 << 24) | (65535 << 8) | (256 - 1) )
#define PRIORITY_SRFLX_2    ( (100 << 24) | (65535 << 8) | (256 - 2) )
#define PRIORITY_RELAY_1    ( (0 << 24) | (65535 << 8) | (256 - 1) )
#define PRIORITY_RELAY_2    ( (0 << 24) | (65535 << 8) | (256 - 2) )
#define PRIORITY_PRFLX_1    ( (110 << 24) | (65535 << 8) | (256 - 1) )
#define PRIORITY_PRFLX_2    ( (110 << 24) | (65535 << 8) | (256 - 2) )


#define FOUNDATION_HOST     "1"
#define FOUNDATION_SRFLX    "3"
#define FOUNDATION_RELAY    "4"
#define FOUNDATION_PRFLX    "2"


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


ICELIB_Result
ICELIB_TEST_sendConnectivityCheck(void*                  pUserData,
                                  int                    proto,
                                  int socket,
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
                                  StunMsgId              transactionId);


static bool
isLegalCharacter(char ch)
{
  const char* set =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  const char* ptr = set;

  while (*ptr)
  {
    if (ch == *ptr++)
    {
      return true;
    }
  }
  return false;
}


static bool
isLegalString(const char* str)
{
  while (*str)
  {
    if ( !isLegalCharacter(*str++) )
    {
      return false;
    }
  }

  return true;
}


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
}ConncheckCB;

ConncheckCB connChkCB;


ICELIB_Result
ICELIB_TEST_sendConnectivityCheck(void*                  pUserData,
                                  int                    proto,
                                  int socket,
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
  (void) pUserData;
  (void) proto; /* Todo add TCP checks; */
  (void) socket;

  printf("\n --------- Sending connectivity check -----------\n");
  if (useCandidate)
  {
    printf("           Use Candidate   \n");
  }
  connChkCB.gotCB       = true;
  connChkCB.destination = destination;
  connChkCB.source      = source;
  connChkCB.userValue1  = userValue1;
  connChkCB.userValue2  = userValue2;
  connChkCB.componentId = componentId;
  connChkCB.useRelay    = useRelay;
  strncpy(connChkCB.ufrag, pUfrag, ICE_MAX_UFRAG_LENGTH);
  connChkCB.pPasswd        = pPasswd;
  connChkCB.peerPriority   = peerPriority;
  connChkCB.useCandidate   = useCandidate;
  connChkCB.iceControlling = iceControlling;
  connChkCB.iceControlled  = iceControlled;
  connChkCB.transactionId  = transactionId;
  connChkCB.tieBreaker     = tieBreaker;
  return 0;
}


ICELIB_INSTANCE* icelib;

static char remoteHostRtpAddr[]   = "10.47.2.246:47936";
static char remoteHostRtcpAddr[]  = "10.47.2.246:47937";
static char remoteRflxRtpAddr[]   = "67.70.2.252:3807";
static char remoteRflxRtcpAddr[]  = "67.70.2.252:32629";
static char remoteRelayRtpAddr[]  = "93.95.67.89:52948";
static char remoteRelayRtcpAddr[] = "93.95.67.89:49832";

static char remoteUfrag[]  = "remUf";
static char remotePasswd[] = "remPa";
/* static char localUfrag[] = "locUf"; */
/* static char localPasswd[] = "locPa"; */

CTEST_DATA(data)
{
  int a;
};


CTEST_SETUP(data)
{
  (void) data;
  struct sockaddr_storage defaultAddr;
  struct sockaddr_storage localHostRtp;
  struct sockaddr_storage localHostRtcp;
  struct sockaddr_storage localRflxRtp;
  struct sockaddr_storage localRflxRtcp;
  struct sockaddr_storage localRelayRtp;
  struct sockaddr_storage localRelayRtcp;



  ICELIB_CONFIGURATION iceConfig;

  int mediaIdx = 0;

  srand( time(NULL) );

  icelib = (ICELIB_INSTANCE*)malloc( sizeof(ICELIB_INSTANCE) );

  sockaddr_initFromString( (struct sockaddr*)&localHostRtp,
                           "192.168.2.10:3456" );
  sockaddr_initFromString( (struct sockaddr*)&localHostRtcp,
                           "192.168.2.10:3457" );
  sockaddr_initFromString( (struct sockaddr*)&localRflxRtp,  "67.45.4.6:1045" );
  sockaddr_initFromString( (struct sockaddr*)&localRflxRtcp, "67.45.4.6:3451" );
  sockaddr_initFromString( (struct sockaddr*)&localRelayRtp,
                           "158.38.46.10:2312" );
  sockaddr_initFromString( (struct sockaddr*)&localRelayRtcp,
                           "158.38.46.10:4567" );


  iceConfig.tickIntervalMS       = 20;
  iceConfig.keepAliveIntervalS   = 15;
  iceConfig.maxCheckListPairs    = ICELIB_MAX_PAIRS;
  iceConfig.aggressiveNomination = false;
  iceConfig.iceLite              = false;
  iceConfig.logLevel             = ICELIB_logDebug;
  /* iceConfig.logLevel = ICELIB_logDisable; */


  ICELIB_Constructor(icelib,
                     &iceConfig);

  ICELIB_setCallbackOutgoingBindingRequest(icelib,
                                           ICELIB_TEST_sendConnectivityCheck,
                                           NULL);
  ICELIB_setCallbackLog(icelib,
                        printLog,
                        NULL,
                        ICELIB_logDebug);


  /* Local side */
  mediaIdx = ICELIB_addLocalMediaStream(icelib,
                                        42,
                                        42,
                                        ICE_CAND_TYPE_HOST);


  ICELIB_addLocalCandidate(icelib,
                           mediaIdx,
                           1,
                           5,
                           (struct sockaddr*)&localHostRtp,
                           NULL,
                           ICE_TRANS_UDP,
                           ICE_CAND_TYPE_HOST,
                           0xffff);

  ICELIB_addLocalCandidate(icelib,
                           mediaIdx,
                           2,
                           5,
                           (struct sockaddr*)&localHostRtcp,
                           NULL,
                           ICE_TRANS_UDP,
                           ICE_CAND_TYPE_HOST,
                           0xffff);


  /* add rflx candidates */


  ICELIB_addLocalCandidate(icelib,
                           mediaIdx,
                           1,
                           5,
                           (struct sockaddr*)&localRflxRtp,
                           (struct sockaddr*)&localHostRtp,
                           ICE_TRANS_UDP,
                           ICE_CAND_TYPE_SRFLX,
                           0xffff);

  ICELIB_addLocalCandidate(icelib,
                           mediaIdx,
                           2,
                           5,
                           (struct sockaddr*)&localRflxRtcp,
                           (struct sockaddr*)&localHostRtp,
                           ICE_TRANS_UDP,
                           ICE_CAND_TYPE_SRFLX,
                           0xffff);




  /* add relay candidates */
  ICELIB_addLocalCandidate(icelib,
                           mediaIdx,
                           1,
                           5,
                           (struct sockaddr*)&localRelayRtp,
                           (struct sockaddr*)&localRflxRtp,
                           ICE_TRANS_UDP,
                           ICE_CAND_TYPE_RELAY,
                           0xffff);

  ICELIB_addLocalCandidate(icelib,
                           mediaIdx,
                           2,
                           5,
                           (struct sockaddr*)&localRelayRtcp,
                           (struct sockaddr*)&localRflxRtcp,
                           ICE_TRANS_UDP,
                           ICE_CAND_TYPE_RELAY,
                           0xffff);


  ICELIB_setTurnState(icelib, 0, ICE_TURN_ALLOCATED);


  /* Remote side */
  sockaddr_initFromString( (struct sockaddr*)&defaultAddr,
                           "10.47.2.246:47936" );




  ICELIB_addRemoteMediaStream(icelib,
                              remoteUfrag,
                              remotePasswd,
                              (struct sockaddr*)&defaultAddr);




  /* sockaddr_initFromString( (struct sockaddr *)&defaultAddr, */
  /*                         "0.0.0.0:0"); */




  ICELIB_addRemoteCandidate(icelib,
                            0,
                            "1",
                            1,
                            1,
                            2130706431,
                            remoteHostRtpAddr,
                            47936,
                            ICE_TRANS_UDP,
                            ICE_CAND_TYPE_HOST);


  ICELIB_addRemoteCandidate(icelib,
                            0,
                            "1",
                            1,
                            2,
                            2130706430,
                            remoteHostRtcpAddr,
                            47937,
                            ICE_TRANS_UDP,
                            ICE_CAND_TYPE_HOST);

  ICELIB_addRemoteCandidate(icelib,
                            0,
                            "3",
                            1,
                            1,
                            1694498815,
                            remoteRflxRtpAddr,
                            3807,
                            ICE_TRANS_UDP,
                            ICE_CAND_TYPE_SRFLX);

  ICELIB_addRemoteCandidate(icelib,
                            0,
                            "3",
                            1,
                            2,
                            1694498814,
                            remoteRflxRtcpAddr,
                            32629,
                            ICE_TRANS_UDP,
                            ICE_CAND_TYPE_SRFLX);



  ICELIB_addRemoteCandidate(icelib,
                            0,
                            "4",
                            1,
                            1,
                            16777215,
                            remoteRelayRtpAddr,
                            52948,
                            ICE_TRANS_UDP,
                            ICE_CAND_TYPE_RELAY);


  ICELIB_addRemoteCandidate(icelib,
                            0,
                            "4",
                            1,
                            2,
                            16777214,
                            remoteRelayRtcpAddr,
                            49832,
                            ICE_TRANS_UDP,
                            ICE_CAND_TYPE_RELAY);
}



CTEST_TEARDOWN(data)
{
  (void) data;
  ICELIB_Destructor(icelib);
  free(icelib);
}


CTEST(icelib, create_ufrag)
{
  char tmp1[ ICE_MAX_UFRAG_LENGTH];
  char tmp2[ ICE_MAX_UFRAG_LENGTH];

  memset( tmp1, '#', sizeof(tmp1) );
  memset( tmp2, '#', sizeof(tmp2) );


  srand( time(NULL) );

  ICELIB_createUfrag(tmp1, ICELIB_UFRAG_LENGTH);
  ICELIB_createUfrag(tmp2, ICELIB_UFRAG_LENGTH);

  ASSERT_TRUE( isLegalString(tmp1) );
  ASSERT_TRUE( isLegalString(tmp2) );


  ASSERT_TRUE( (strlen(tmp1) + 1) == ICELIB_UFRAG_LENGTH );

  ASSERT_TRUE( (strlen(tmp2) + 1) == ICELIB_UFRAG_LENGTH );

  ASSERT_TRUE( strcmp(tmp1, tmp2) );

  ICELIB_createUfrag(tmp1, ICE_MAX_UFRAG_LENGTH + 999);

  ASSERT_TRUE( (strlen(tmp1) + 1) == ICE_MAX_UFRAG_LENGTH );

  ASSERT_TRUE( isLegalString(tmp1) );
}



CTEST(iclib, create_passwd)
{
  char         tmp1[ ICE_MAX_PASSWD_LENGTH];
  char         tmp2[ ICE_MAX_PASSWD_LENGTH];
  unsigned int i;

  memset( tmp1, '#', sizeof(tmp1) );
  memset( tmp2, '#', sizeof(tmp2) );

  ICELIB_createPasswd(tmp1, ICELIB_PASSWD_LENGTH);
  ICELIB_createPasswd(tmp2, ICELIB_PASSWD_LENGTH);

  ASSERT_TRUE( isLegalString(tmp1) );
  ASSERT_TRUE( isLegalString(tmp2) );

  ASSERT_TRUE( (strlen(tmp1) + 1) == ICELIB_PASSWD_LENGTH );
  ASSERT_TRUE( (strlen(tmp2) + 1) == ICELIB_PASSWD_LENGTH );
  ASSERT_TRUE( strcmp(tmp1, tmp2) );

  memset( tmp2, '#', sizeof(tmp2) );
  tmp2[ ICE_MAX_PASSWD_LENGTH - 1] = '\0';

  for (i = 0; i < ICE_MAX_PASSWD_LENGTH + 10; ++i)
  {
    memset( tmp1, '#', sizeof(tmp1) );
    tmp1[ICE_MAX_PASSWD_LENGTH - 1] = '\0';

    ICELIB_createPasswd(tmp1, i);

    if (i == 0)
    {
      ASSERT_TRUE(strcmp(tmp1, tmp2) == 0);
    }
    else if (i > ICE_MAX_PASSWD_LENGTH)
    {
      ASSERT_TRUE( isLegalString(tmp1) );
      ASSERT_TRUE( (strlen(tmp1) + 1) == ICE_MAX_PASSWD_LENGTH );
    }
    else
    {
      ASSERT_TRUE( isLegalString(tmp1) );
      ASSERT_TRUE( (strlen(tmp1) + 1) == i );
    }
  }

}



CTEST(icelib, calculate_priority)
{
  uint32_t priority1;
  uint32_t priority2;
  uint32_t priority3;
  uint32_t priority4;
  uint32_t priority5;
  uint32_t priority6;
  uint32_t priority7;
  uint32_t priority8;

  priority1 = ICELIB_calculatePriority(ICE_CAND_TYPE_HOST,
                                       ICE_TRANS_UDP,
                                       1,
                                       0xffff);
  priority2 = ICELIB_calculatePriority(ICE_CAND_TYPE_HOST,
                                       ICE_TRANS_UDP,
                                       2,
                                       0xffff);
  priority3 = ICELIB_calculatePriority(ICE_CAND_TYPE_SRFLX,
                                       ICE_TRANS_UDP,
                                       1,
                                       0xffff);
  priority4 = ICELIB_calculatePriority(ICE_CAND_TYPE_SRFLX,
                                       ICE_TRANS_UDP,
                                       2,
                                       0xffff);
  priority5 = ICELIB_calculatePriority(ICE_CAND_TYPE_RELAY,
                                       ICE_TRANS_UDP,
                                       1,
                                       0xffff);
  priority6 = ICELIB_calculatePriority(ICE_CAND_TYPE_RELAY,
                                       ICE_TRANS_UDP,
                                       2,
                                       0xffff);
  priority7 = ICELIB_calculatePriority(ICE_CAND_TYPE_PRFLX,
                                       ICE_TRANS_UDP,
                                       1,
                                       0xffff);
  priority8 = ICELIB_calculatePriority(ICE_CAND_TYPE_PRFLX,
                                       ICE_TRANS_UDP,
                                       2,
                                       0xffff);

  ASSERT_TRUE(priority1 == PRIORITY_HOST_1);
  ASSERT_TRUE(priority2 == PRIORITY_HOST_2);
  ASSERT_TRUE(priority3 == PRIORITY_SRFLX_1);
  ASSERT_TRUE(priority4 == PRIORITY_SRFLX_2);
  ASSERT_TRUE(priority5 == PRIORITY_RELAY_1);
  ASSERT_TRUE(priority6 == PRIORITY_RELAY_2);
  ASSERT_TRUE(priority7 == PRIORITY_PRFLX_1);
  ASSERT_TRUE(priority8 == PRIORITY_PRFLX_2);


}



CTEST(icelib, create_foundation)
{

  char tmp[ ICE_MAX_FOUNDATION_LENGTH];

  memset( tmp, '#', sizeof(tmp) );

  ICELIB_createFoundation(tmp,
                          ICE_CAND_TYPE_HOST,
                          ICE_TRANS_UDP,
                          0,
                          ICE_MAX_FOUNDATION_LENGTH);

  ASSERT_TRUE( strcmp(tmp, FOUNDATION_HOST) == 0);

  memset( tmp, '#', sizeof(tmp) );

  ICELIB_createFoundation(tmp,
                          ICE_CAND_TYPE_SRFLX,
                          ICE_TRANS_UDP,
                          0,
                          ICE_MAX_FOUNDATION_LENGTH);

  ASSERT_TRUE( strcmp(tmp, FOUNDATION_SRFLX) == 0);

  memset( tmp, '#', sizeof(tmp) );

  ICELIB_createFoundation(tmp,
                          ICE_CAND_TYPE_RELAY,
                          ICE_TRANS_UDP,
                          0,
                          ICE_MAX_FOUNDATION_LENGTH);

  ASSERT_TRUE( strcmp(tmp, FOUNDATION_RELAY) == 0);

  memset( tmp, '#', sizeof(tmp) );

  ICELIB_createFoundation(tmp,
                          ICE_CAND_TYPE_PRFLX,
                          ICE_TRANS_UDP,
                          0,
                          ICE_MAX_FOUNDATION_LENGTH);

  ASSERT_TRUE( strcmp(tmp, FOUNDATION_PRFLX) == 0);

  memset( tmp, '#', sizeof(tmp) );

  ICELIB_createFoundation(tmp,
                          ICE_CAND_TYPE_HOST + 999,
                          ICE_TRANS_UDP,
                          0,
                          ICE_MAX_FOUNDATION_LENGTH);

  ASSERT_TRUE( strcmp(tmp, "unknowntype") == 0);

}



CTEST(icelib, create_localMediaStream)
{
  ICELIB_INSTANCE      localIcelib;
  ICELIB_CONFIGURATION localIceConfig;
  ICE_MEDIA const*     localIceMedia;

  int32_t mediaIdx,i;


  /* localIceConfig.logLevel = ICELIB_logDebug; */
  localIceConfig.logLevel = ICELIB_logDisable;


  ICELIB_Constructor(&localIcelib,
                     &localIceConfig);


  for (i = 0; i < ICE_MAX_MEDIALINES; i++)
  {
    mediaIdx = ICELIB_addLocalMediaStream(&localIcelib,
                                          45,
                                          34,
                                          ICE_CAND_TYPE_HOST);
    ASSERT_TRUE( mediaIdx == i);

    localIceMedia = ICELIB_getLocalIceMedia(&localIcelib);
    ASSERT_TRUE( isLegalString(localIceMedia->mediaStream[i].ufrag) );
    ASSERT_TRUE( isLegalString(localIceMedia->mediaStream[i].passwd) );
  }

  i++;
  mediaIdx = ICELIB_addLocalMediaStream(&localIcelib,
                                        45,
                                        34,
                                        ICE_CAND_TYPE_HOST);
  ASSERT_TRUE(mediaIdx == -1);

}



CTEST(icelib, create_remoteMediaStream)
{
  ICELIB_INSTANCE      remoteIcelib;
  ICELIB_CONFIGURATION remoteIceConfig;
  struct sockaddr      defaultAddr;

  int32_t result,i;

  sockaddr_initFromString( (struct sockaddr*)&defaultAddr,
                           "10.47.2.246:47936" );


  remoteIceConfig.logLevel = ICELIB_logDisable;
  /* remoteIceConfig.logLevel = ICELIB_logDebug; */

  ICELIB_Constructor(&remoteIcelib,
                     &remoteIceConfig);


  for (i = 0; i < ICE_MAX_MEDIALINES; i++)
  {
    result = ICELIB_addRemoteMediaStream(&remoteIcelib,
                                         "ufrag",
                                         "pass",
                                         &defaultAddr);
    ASSERT_TRUE(result != -1);

  }

  i++;
  result = ICELIB_addRemoteMediaStream(&remoteIcelib,
                                       "ufrag",
                                       "pass",
                                       &defaultAddr);
  ASSERT_TRUE(result == -1);

}



CTEST(icelib, pairPriority)
{
  uint32_t G;
  uint32_t D;
  uint64_t priority;
  uint64_t expected;

  G        = 0x12345678;
  D        = 0x76543210;
  priority = ICELIB_pairPriority(G, D);

  expected = 0x12345678eca86420LL;
  ASSERT_TRUE(priority == expected);

  G        = 0x76543210;
  D        = 0x12345678;
  priority = ICELIB_pairPriority(G, D);
  expected = 0x12345678eca86421LL;
  ASSERT_TRUE(priority == expected);

  G        = 0x11111111;
  D        = 0x22222222;
  priority = ICELIB_pairPriority(G, D);
  expected = 0x1111111144444444LL;
  ASSERT_TRUE(priority == expected);

  G        = 0x22222222;
  D        = 0x11111111;
  priority = ICELIB_pairPriority(G, D);
  expected = 0x1111111144444445LL;
  ASSERT_TRUE(priority == expected);

}



CTEST(icelib, triggereedcheck_queue)
{
  bool                  result;
  unsigned int          i;
  unsigned int          j;
  unsigned int          k;
  ICELIB_FIFO_ELEMENT   element;
  ICELIB_TRIGGERED_FIFO f;


  ICELIB_fifoClear(&f);
  ASSERT_TRUE(ICELIB_fifoCount(&f) == 0);
  ASSERT_TRUE(ICELIB_fifoIsEmpty(&f) == true);
  ASSERT_TRUE(ICELIB_fifoIsFull(&f) == false);

  ASSERT_TRUE(ICELIB_fifoGet(&f) == ICELIB_FIFO_IS_EMPTY);
/*  */
/* ----- Run FIFO full */
/*  */
  for (i = 0; i < ICELIB_MAX_FIFO_ELEMENTS; ++i)
  {
    result = ICELIB_fifoPut(&f, ( ICELIB_FIFO_ELEMENT) i);
    ASSERT_TRUE(result == false);
    ASSERT_TRUE(ICELIB_fifoCount(&f) == i + 1);
  }

  ASSERT_TRUE(ICELIB_fifoCount(&f) == ICELIB_MAX_FIFO_ELEMENTS);
  ASSERT_TRUE(ICELIB_fifoIsEmpty(&f) == false);
  ASSERT_TRUE(ICELIB_fifoIsFull(&f) == true);

  ASSERT_TRUE(ICELIB_fifoPut(&f, ( ICELIB_FIFO_ELEMENT) 999) == true);
/*  */
/* ----- Remove some elements */
/*  */
  for (i = 0; i < 10; ++i)
  {
    element = ICELIB_fifoGet(&f);
    ASSERT_TRUE(element == ( ICELIB_FIFO_ELEMENT) i);
    ASSERT_TRUE(ICELIB_fifoCount(&f) == ICELIB_MAX_FIFO_ELEMENTS - i - 1);
  }

  ASSERT_TRUE(ICELIB_fifoCount(&f) == ICELIB_MAX_FIFO_ELEMENTS - 10);
  ASSERT_TRUE(ICELIB_fifoIsEmpty(&f) == false);
  ASSERT_TRUE(ICELIB_fifoIsFull(&f) == false);
/*  */
/* ----- Remove more elements */
/*  */
  for (i = 10; i < ICELIB_MAX_FIFO_ELEMENTS - 5; ++i)
  {
    element = ICELIB_fifoGet(&f);
    ASSERT_TRUE(element == ( ICELIB_FIFO_ELEMENT) i);
    ASSERT_TRUE(ICELIB_fifoCount(&f) == ICELIB_MAX_FIFO_ELEMENTS - i - 1);
  }

  ASSERT_TRUE(ICELIB_fifoCount(&f) == 5);
  ASSERT_TRUE(ICELIB_fifoIsEmpty(&f) == false);
  ASSERT_TRUE(ICELIB_fifoIsFull(&f) == false);
/*  */
/* ----- Remove the rest of the elements */
/*  */
  for (i = ICELIB_MAX_FIFO_ELEMENTS - 5; i < ICELIB_MAX_FIFO_ELEMENTS; ++i)
  {
    element = ICELIB_fifoGet(&f);
    ASSERT_TRUE(element == ( ICELIB_FIFO_ELEMENT) i);
    ASSERT_TRUE(ICELIB_fifoCount(&f) == ICELIB_MAX_FIFO_ELEMENTS - i - 1);
  }

  ASSERT_TRUE(ICELIB_fifoCount(&f) == 0);
  ASSERT_TRUE(ICELIB_fifoIsEmpty(&f) == true);
  ASSERT_TRUE(ICELIB_fifoIsFull(&f) == false);
/*  */
/* ----- Fill 3/4 of the FIFO */
/*  */
  for (i = 0; i < (ICELIB_MAX_FIFO_ELEMENTS * 3) / 4; ++i)
  {
    result = ICELIB_fifoPut(&f, ( ICELIB_FIFO_ELEMENT) i + 1000);
    ASSERT_TRUE(result == false);
    ASSERT_TRUE(ICELIB_fifoCount(&f) == i + 1);
  }

  ASSERT_TRUE(ICELIB_fifoCount(&f) == (ICELIB_MAX_FIFO_ELEMENTS * 3) / 4);
  ASSERT_TRUE(ICELIB_fifoIsEmpty(&f) == false);
  ASSERT_TRUE(ICELIB_fifoIsFull(&f) == false);
/*  */
/* ----- Remove 1/2 FIFO */
/*  */
  for (j = 0; j < ICELIB_MAX_FIFO_ELEMENTS / 2; ++j)
  {
    element = ICELIB_fifoGet(&f);
    ASSERT_TRUE(element == ( ICELIB_FIFO_ELEMENT) j + 1000);
    ASSERT_TRUE(ICELIB_fifoCount(
                  &f) == (ICELIB_MAX_FIFO_ELEMENTS * 3) / 4 - j - 1);
  }

  ASSERT_TRUE(ICELIB_fifoCount(&f) == ICELIB_MAX_FIFO_ELEMENTS / 4);
  ASSERT_TRUE(ICELIB_fifoIsEmpty(&f) == false);
  ASSERT_TRUE(ICELIB_fifoIsFull(&f) == false);
/*  */
/* ----- Fill another 1/2 FIFO */
/*  */
  for (k = 0; k < ICELIB_MAX_FIFO_ELEMENTS / 2; ++k, ++i)
  {
    result = ICELIB_fifoPut(&f, ( ICELIB_FIFO_ELEMENT) i + 1000);
    ASSERT_TRUE(result == false);
    ASSERT_TRUE(ICELIB_fifoCount(&f) == (ICELIB_MAX_FIFO_ELEMENTS) / 4 + k + 1);
  }

  ASSERT_TRUE(ICELIB_fifoCount(&f) == (ICELIB_MAX_FIFO_ELEMENTS * 3) / 4);
  ASSERT_TRUE(ICELIB_fifoIsEmpty(&f) == false);
  ASSERT_TRUE(ICELIB_fifoIsFull(&f) == false);
/*  */
/* ----- Remove 3/4 FIFO */
/*  */
  for (k = 0; k < (ICELIB_MAX_FIFO_ELEMENTS * 3) / 4; ++k, ++j)
  {
    element = ICELIB_fifoGet(&f);
    ASSERT_TRUE(element == ( ICELIB_FIFO_ELEMENT) j + 1000);
    ASSERT_TRUE(ICELIB_fifoCount(
                  &f) == (ICELIB_MAX_FIFO_ELEMENTS * 3) / 4 - k - 1);
  }

  ASSERT_TRUE(ICELIB_fifoCount(&f) == 0);
  ASSERT_TRUE(ICELIB_fifoIsEmpty(&f) == true);
  ASSERT_TRUE(ICELIB_fifoIsFull(&f) == false);

}


CTEST(icelib, triggereedcheck_queue_ekstra)
{
  bool                           result;
  unsigned int                   i;
  unsigned int                   j;
  unsigned int                   k;
  unsigned int                   l;
  ICELIB_FIFO_ELEMENT            element;
  ICELIB_FIFO_ELEMENT*           pElement;
  ICELIB_TRIGGERED_FIFO          f;
  ICELIB_TRIGGERED_FIFO_ITERATOR tfIterator;


  ICELIB_fifoClear(&f);
  ASSERT_TRUE(ICELIB_fifoCount(&f) == 0);
  ASSERT_TRUE(ICELIB_fifoIsEmpty(&f) == true);
  ASSERT_TRUE(ICELIB_fifoIsFull(&f) == false);
/*  */
/* ----- Fill 3/4 of the FIFO */
/*  */
  for (i = 0; i < (ICELIB_MAX_FIFO_ELEMENTS * 3) / 4; ++i)
  {
    result = ICELIB_fifoPut(&f, ( ICELIB_FIFO_ELEMENT) i + 1000);
    ASSERT_TRUE(result == false);
    ASSERT_TRUE(ICELIB_fifoCount(&f) == i + 1);
  }

  ASSERT_TRUE(ICELIB_fifoCount(&f)   == (ICELIB_MAX_FIFO_ELEMENTS * 3) / 4);
  ASSERT_TRUE(ICELIB_fifoIsEmpty(&f) == false);
  ASSERT_TRUE(ICELIB_fifoIsFull(&f)  == false);
/*  */
/* ----- Iterate through elements in FIFO */
/*  */
  ICELIB_fifoIteratorConstructor(&tfIterator, &f);

  j = 0;
  while ( ( pElement =
              (ICELIB_FIFO_ELEMENT*)pICELIB_fifoIteratorNext(&tfIterator) ) !=
          NULL )
  {
    ASSERT_TRUE(*pElement == ( ICELIB_FIFO_ELEMENT) j + 1000);
    ++j;
  }

  ASSERT_TRUE(j == (ICELIB_MAX_FIFO_ELEMENTS * 3) / 4);
/*  */
/* ----- Remove 1/2 FIFO */
/*  */
  for (j = 0; j < ICELIB_MAX_FIFO_ELEMENTS / 2; ++j)
  {
    element = ICELIB_fifoGet(&f);
    ASSERT_TRUE(element == ( ICELIB_FIFO_ELEMENT) j + 1000);
    ASSERT_TRUE(ICELIB_fifoCount(
                  &f) == (ICELIB_MAX_FIFO_ELEMENTS * 3) / 4 - j - 1);
  }

  ASSERT_TRUE(ICELIB_fifoCount(&f)   == ICELIB_MAX_FIFO_ELEMENTS / 4);
  ASSERT_TRUE(ICELIB_fifoIsEmpty(&f) == false);
  ASSERT_TRUE(ICELIB_fifoIsFull(&f)  == false);
/*  */
/* ----- Fill another 1/2 FIFO */
/*  */
  for (k = 0; k < ICELIB_MAX_FIFO_ELEMENTS / 2; ++k, ++i)
  {
    result = ICELIB_fifoPut(&f, ( ICELIB_FIFO_ELEMENT) i + 1000);
    ASSERT_TRUE(result == false);
    ASSERT_TRUE(ICELIB_fifoCount(&f) == (ICELIB_MAX_FIFO_ELEMENTS) / 4 + k + 1);
  }

  ASSERT_TRUE(ICELIB_fifoCount(&f)   == (ICELIB_MAX_FIFO_ELEMENTS * 3) / 4);
  ASSERT_TRUE(ICELIB_fifoIsEmpty(&f) == false);
  ASSERT_TRUE(ICELIB_fifoIsFull(&f)  == false);
/*  */
/* ----- Iterate through elements in FIFO */
/*  */
  ICELIB_fifoIteratorConstructor(&tfIterator, &f);

  k = 0;
  l = j;
  while ( ( pElement =
              (ICELIB_FIFO_ELEMENT*)pICELIB_fifoIteratorNext(&tfIterator) ) !=
          NULL )
  {
    ASSERT_TRUE(*pElement == ( ICELIB_FIFO_ELEMENT) l + 1000);
    ++k;
    ++l;
  }

  ASSERT_TRUE(k == (ICELIB_MAX_FIFO_ELEMENTS * 3) / 4);
/*  */
/* ----- Fill another 1/4 FIFO (should then be full) */
/*  */
  for (k = 0; k < ICELIB_MAX_FIFO_ELEMENTS / 4; ++k, ++i)
  {
    result = ICELIB_fifoPut(&f, ( ICELIB_FIFO_ELEMENT) i + 1000);
    ASSERT_TRUE(result == false);
    ASSERT_TRUE(ICELIB_fifoCount(
                  &f) == (ICELIB_MAX_FIFO_ELEMENTS * 3) / 4 + k + 1);
  }

  ASSERT_TRUE(ICELIB_fifoCount(&f)   == ICELIB_MAX_FIFO_ELEMENTS);
  ASSERT_TRUE(ICELIB_fifoIsEmpty(&f) == false);
  ASSERT_TRUE(ICELIB_fifoIsFull(&f)  == true);
/*  */
/* ----- Iterate through elements in FIFO */
/*  */
  ICELIB_fifoIteratorConstructor(&tfIterator, &f);

  k = 0;
  l = j;
  while ( ( pElement =
              (ICELIB_FIFO_ELEMENT*)pICELIB_fifoIteratorNext(&tfIterator) ) !=
          NULL )
  {
    ASSERT_TRUE(*pElement == ( ICELIB_FIFO_ELEMENT) l + 1000);
    ++k;
    ++l;
  }

  ASSERT_TRUE(k == ICELIB_MAX_FIFO_ELEMENTS);

}



CTEST(icelib, ice_timer)
{
  unsigned int         i;
  ICELIB_INSTANCE      Instance;
  ICELIB_CONFIGURATION config;
  ICELIB_TIMER         timer0;
  ICELIB_TIMER*        pTimer0 = &timer0;


  memset( &config, 0, sizeof(config) );

  config.tickIntervalMS       = 20;         /* Number of ms between timer ticks
                                             * */
  config.keepAliveIntervalS   = 15;
  config.maxCheckListPairs    = ICELIB_MAX_PAIRS;
  config.aggressiveNomination = false;
  config.iceLite              = false;
  /* config.logLevel             = 3; */
  config.logLevel = ICELIB_logDebug;

  ICELIB_Constructor(&Instance, &config);
  ICELIB_timerConstructor(pTimer0, config.tickIntervalMS);

  ASSERT_TRUE(ICELIB_timerIsTimedOut(pTimer0) == false);
  ASSERT_TRUE(ICELIB_timerIsRunning(pTimer0)  == false);
/*  */
/* - First run */
/*  */
  ICELIB_timerStart(pTimer0, 5 * 1000);     /* Timeout in 5 seconds */
  ASSERT_TRUE(ICELIB_timerIsRunning(pTimer0) == true);

  for (i = 0; i < ( (5 * 1000 / 20) - 1 ); ++i)
  {
    ICELIB_timerTick(pTimer0);
  }

  ASSERT_TRUE(ICELIB_timerIsTimedOut(pTimer0) == false);
  ICELIB_timerTick(pTimer0);
  ASSERT_TRUE(ICELIB_timerIsTimedOut(pTimer0) == true);
/*  */
/* - Second run */
/*  */
  ASSERT_TRUE(ICELIB_timerIsRunning(pTimer0) == false);
  ICELIB_timerStart(pTimer0, 3 * 1000);     /* Timeout in 5 seconds */
  ASSERT_TRUE(ICELIB_timerIsRunning(pTimer0) == true);

  for (i = 0; i < ( (3 * 1000 / 20) - 1 ); ++i)
  {
    ICELIB_timerTick(pTimer0);
  }

  ASSERT_TRUE(ICELIB_timerIsTimedOut(pTimer0) == false);
  ICELIB_timerTick(pTimer0);
  ASSERT_TRUE(ICELIB_timerIsTimedOut(pTimer0) == true);
}



CTEST2(data, controlling)
{
  (void) data;
  ASSERT_TRUE( ICELIB_Start(icelib, true) );
  ASSERT_TRUE( icelib->numberOfMediaStreams == 1);
  ASSERT_TRUE( icelib->iceControlling == true);
  ASSERT_TRUE( icelib->iceControlled == false);
}


CTEST2(data, initialState)
{
  (void)data;
  ASSERT_TRUE( icelib->iceState == ICELIB_IDLE);
  ASSERT_TRUE( ICELIB_Start(icelib, true) );
  ASSERT_TRUE( icelib->iceState == ICELIB_RUNNING);
}


CTEST2(data, iceSupportVerified)
{
  (void) data;
  ASSERT_FALSE(icelib->iceSupportVerified);
  ASSERT_TRUE( ICELIB_Start(icelib, true) );
  ASSERT_TRUE( icelib->iceSupportVerified);
}



CTEST2(data, simpleTick)
{
  (void)data;
  ASSERT_TRUE( ICELIB_Start(icelib, true) );
  ASSERT_TRUE( icelib->tickCount == 0);
  ICELIB_Tick(icelib);
  ASSERT_TRUE(icelib->tickCount == 1);
  ICELIB_Tick(icelib);
  ASSERT_TRUE(icelib->tickCount == 2);
}



CTEST2(data, checklistInitialState)
{
  (void)data;
  ASSERT_TRUE( ICELIB_Start(icelib, true) );

  ASSERT_TRUE(
    icelib->streamControllers[0].checkList.checkListState ==
    ICELIB_CHECKLIST_RUNNING);

  ASSERT_TRUE(icelib->streamControllers[0].checkList.numberOfPairs == 12);

  ASSERT_TRUE(
    icelib->streamControllers[0].checkList.checkListPairs[0].pairState ==
    ICELIB_PAIR_WAITING);
  ASSERT_TRUE(
    icelib->streamControllers[0].checkList.checkListPairs[1].pairState ==
    ICELIB_PAIR_FROZEN);
  ASSERT_TRUE(
    icelib->streamControllers[0].checkList.checkListPairs[2].pairState ==
    ICELIB_PAIR_WAITING);
  ASSERT_TRUE(
    icelib->streamControllers[0].checkList.checkListPairs[3].pairState ==
    ICELIB_PAIR_FROZEN);
  ASSERT_TRUE(
    icelib->streamControllers[0].checkList.checkListPairs[4].pairState ==
    ICELIB_PAIR_WAITING);
  ASSERT_TRUE(
    icelib->streamControllers[0].checkList.checkListPairs[5].pairState ==
    ICELIB_PAIR_WAITING);
  ASSERT_TRUE(
    icelib->streamControllers[0].checkList.checkListPairs[6].pairState ==
    ICELIB_PAIR_WAITING);
  ASSERT_TRUE(
    icelib->streamControllers[0].checkList.checkListPairs[7].pairState ==
    ICELIB_PAIR_WAITING);
  ASSERT_TRUE(
    icelib->streamControllers[0].checkList.checkListPairs[8].pairState ==
    ICELIB_PAIR_FROZEN);
  ASSERT_TRUE(
    icelib->streamControllers[0].checkList.checkListPairs[9].pairState ==
    ICELIB_PAIR_FROZEN);
  ASSERT_TRUE(
    icelib->streamControllers[0].checkList.checkListPairs[10].pairState ==
    ICELIB_PAIR_FROZEN);
  ASSERT_TRUE(
    icelib->streamControllers[0].checkList.checkListPairs[11].pairState ==
    ICELIB_PAIR_FROZEN);
}



CTEST2(data, checklistTick)
{
  (void)data;
  ASSERT_TRUE( ICELIB_Start(icelib, true) );

  ICELIB_Tick(icelib);
  ASSERT_TRUE(
    icelib->streamControllers[0].checkList.checkListPairs[0].pairState ==
    ICELIB_PAIR_INPROGRESS);


  ICELIB_Tick(icelib);
  ASSERT_TRUE(
    icelib->streamControllers[0].checkList.checkListPairs[2].pairState ==
    ICELIB_PAIR_INPROGRESS);


  ICELIB_Tick(icelib);
  ASSERT_TRUE(
    icelib->streamControllers[0].checkList.checkListPairs[4].pairState ==
    ICELIB_PAIR_INPROGRESS);


  ICELIB_Tick(icelib);
  ASSERT_TRUE(
    icelib->streamControllers[0].checkList.checkListPairs[5].pairState ==
    ICELIB_PAIR_INPROGRESS);


  ICELIB_Tick(icelib);
  ASSERT_TRUE(
    icelib->streamControllers[0].checkList.checkListPairs[6].pairState ==
    ICELIB_PAIR_INPROGRESS);

  ICELIB_Tick(icelib);
  ASSERT_TRUE(
    icelib->streamControllers[0].checkList.checkListPairs[7].pairState ==
    ICELIB_PAIR_INPROGRESS);

  ICELIB_Tick(icelib);
  ASSERT_TRUE(
    icelib->streamControllers[0].checkList.checkListPairs[1].pairState ==
    ICELIB_PAIR_INPROGRESS);

  ICELIB_Tick(icelib);
  ASSERT_TRUE(
    icelib->streamControllers[0].checkList.checkListPairs[2].pairState ==
    ICELIB_PAIR_INPROGRESS);

  ICELIB_Tick(icelib);
  ASSERT_TRUE(
    icelib->streamControllers[0].checkList.checkListPairs[6].pairState ==
    ICELIB_PAIR_INPROGRESS);

  ICELIB_Tick(icelib);
  ASSERT_TRUE(
    icelib->streamControllers[0].checkList.checkListPairs[8].pairState ==
    ICELIB_PAIR_INPROGRESS);

  ICELIB_Tick(icelib);
  ASSERT_TRUE(
    icelib->streamControllers[0].checkList.checkListPairs[9].pairState ==
    ICELIB_PAIR_INPROGRESS);

  ICELIB_Tick(icelib);
  ASSERT_TRUE(
    icelib->streamControllers[0].checkList.checkListPairs[11].pairState ==
    ICELIB_PAIR_INPROGRESS);

}



CTEST2(data, conncheck)
{
  (void)data;
  /*
   *  connChkCB.destination = destination;
   *  connChkCB.source = source;;
   *  connChkCB.userValue1 = userValue1;
   *  connChkCB.userValue2 = userValue2;
   *  connChkCB.componentId = componentId;
   *  connChkCB.useRelay = useRelay;
   *  connChkCB.pUfrag = pUfrag;
   *  connChkCB.pPasswd = pPasswd;
   *  connChkCB.peerPriority = peerPriority;
   *  connChkCB.useCandidate = useCandidate;
   *  connChkCB.iceControlling = iceControlling;
   *  connChkCB.iceControlled = iceControlled;
   */
  char ipaddr[SOCKADDR_MAX_STRLEN];

  memset( &connChkCB, 0, sizeof(ConncheckCB) );

  ASSERT_TRUE( ICELIB_Start(icelib, true) );

  /* 1. Tick */

  ICELIB_Tick(icelib);
  ASSERT_TRUE(strncmp(remoteHostRtpAddr,
                      sockaddr_toString( (const struct sockaddr*)connChkCB.
                                         destination,
                                         ipaddr,
                                         SOCKADDR_MAX_STRLEN,
                                         true ),
                      INET_ADDRSTRLEN) == 0);

  ASSERT_FALSE(connChkCB.useRelay);

  /* 2. Tick */
  memset( &connChkCB, 0, sizeof(ConncheckCB) );
  ICELIB_Tick(icelib);

  ASSERT_TRUE(strncmp(remoteRflxRtpAddr,
                      sockaddr_toString( (const struct sockaddr*)connChkCB.
                                         destination,
                                         ipaddr,
                                         SOCKADDR_MAX_STRLEN,
                                         true ),
                      INET_ADDRSTRLEN) == 0);
  ASSERT_FALSE(connChkCB.useRelay);

  /* 3. Tick */
  memset( &connChkCB, 0, sizeof(ConncheckCB) );
  ICELIB_Tick(icelib);

  ASSERT_TRUE(strncmp(remoteRelayRtpAddr,
                      sockaddr_toString( (const struct sockaddr*)connChkCB.
                                         destination,
                                         ipaddr,
                                         SOCKADDR_MAX_STRLEN,
                                         true ),
                      INET_ADDRSTRLEN) == 0);

  ASSERT_FALSE(connChkCB.useRelay);
  ASSERT_FALSE(connChkCB.useCandidate);

  /* 4. Tick */
  memset( &connChkCB, 0, sizeof(ConncheckCB) );
  ICELIB_Tick(icelib);

  ASSERT_TRUE(strncmp(remoteHostRtpAddr,
                      sockaddr_toString( (const struct sockaddr*)connChkCB.
                                         destination,
                                         ipaddr,
                                         SOCKADDR_MAX_STRLEN,
                                         true ),
                      INET_ADDRSTRLEN) == 0);
  ASSERT_TRUE(connChkCB.useRelay);


  /* 5. Tick */
  memset( &connChkCB, 0, sizeof(ConncheckCB) );
  ICELIB_Tick(icelib);

  ASSERT_TRUE(strncmp(remoteRflxRtpAddr,
                      sockaddr_toString( (const struct sockaddr*)connChkCB.
                                         destination,
                                         ipaddr,
                                         SOCKADDR_MAX_STRLEN,
                                         true ),
                      INET_ADDRSTRLEN) == 0);
  ASSERT_TRUE(connChkCB.useRelay);


  /* 6. Tick */
  memset( &connChkCB, 0, sizeof(ConncheckCB) );
  ICELIB_Tick(icelib);

  ASSERT_TRUE(strncmp(remoteRelayRtpAddr,
                      sockaddr_toString( (const struct sockaddr*)connChkCB.
                                         destination,
                                         ipaddr,
                                         SOCKADDR_MAX_STRLEN,
                                         true ),
                      INET_ADDRSTRLEN) == 0);
  ASSERT_TRUE(connChkCB.useRelay);

  /* 7. Tick */
  memset( &connChkCB, 0, sizeof(ConncheckCB) );
  ICELIB_Tick(icelib);

  ASSERT_TRUE(strncmp(remoteHostRtcpAddr,
                      sockaddr_toString( (const struct sockaddr*)connChkCB.
                                         destination,
                                         ipaddr,
                                         SOCKADDR_MAX_STRLEN,
                                         true ),
                      INET_ADDRSTRLEN) == 0);
  ASSERT_FALSE(connChkCB.useRelay);


  /* 8. Tick */
  memset( &connChkCB, 0, sizeof(ConncheckCB) );
  ICELIB_Tick(icelib);

  ASSERT_TRUE(strncmp(remoteRflxRtcpAddr,
                      sockaddr_toString( (const struct sockaddr*)connChkCB.
                                         destination,
                                         ipaddr,
                                         SOCKADDR_MAX_STRLEN,
                                         true ),
                      INET_ADDRSTRLEN) == 0);
  ASSERT_FALSE(connChkCB.useRelay);


  /* 9. Tick */
  memset( &connChkCB, 0, sizeof(ConncheckCB) );
  ICELIB_Tick(icelib);

  ASSERT_TRUE(strncmp(remoteRelayRtcpAddr,
                      sockaddr_toString( (const struct sockaddr*)connChkCB.
                                         destination,
                                         ipaddr,
                                         SOCKADDR_MAX_STRLEN,
                                         true ),
                      INET_ADDRSTRLEN) == 0);
  /* 10. Tick */
  memset( &connChkCB, 0, sizeof(ConncheckCB) );
  ICELIB_Tick(icelib);

  ASSERT_TRUE(strncmp(remoteHostRtcpAddr,
                      sockaddr_toString( (const struct sockaddr*)connChkCB.
                                         destination,
                                         ipaddr,
                                         SOCKADDR_MAX_STRLEN,
                                         true ),
                      INET_ADDRSTRLEN) == 0);

  /* 11. Tick */
  memset( &connChkCB, 0, sizeof(ConncheckCB) );
  ICELIB_Tick(icelib);

  ASSERT_TRUE(strncmp(remoteRflxRtcpAddr,
                      sockaddr_toString( (const struct sockaddr*)connChkCB.
                                         destination,
                                         ipaddr,
                                         SOCKADDR_MAX_STRLEN,
                                         true ),
                      INET_ADDRSTRLEN) == 0);
  ASSERT_TRUE(connChkCB.useRelay);

  /* 12. Tick */
  memset( &connChkCB, 0, sizeof(ConncheckCB) );
  ICELIB_Tick(icelib);

  ASSERT_TRUE(strncmp(remoteRelayRtcpAddr,
                      sockaddr_toString( (const struct sockaddr*)connChkCB.
                                         destination,
                                         ipaddr,
                                         SOCKADDR_MAX_STRLEN,
                                         true ),
                      INET_ADDRSTRLEN) == 0);
  ASSERT_TRUE(connChkCB.useRelay);

  /* 13. Tick */
  memset( &connChkCB, 0, sizeof(ConncheckCB) );
  ICELIB_Tick(icelib);
  ASSERT_FALSE(connChkCB.gotCB);

  /* 14. Tick */
  memset( &connChkCB, 0, sizeof(ConncheckCB) );
  ICELIB_Tick(icelib);
  ASSERT_FALSE(connChkCB.gotCB);

}





CTEST2(data, conncheck_withIncomming)
{
  (void)data;
  /*
   *  connChkCB.destination = destination;
   *  connChkCB.source = source;;
   *  connChkCB.userValue1 = userValue1;
   *  connChkCB.userValue2 = userValue2;
   *  connChkCB.componentId = componentId;
   *  connChkCB.useRelay = useRelay;
   *  connChkCB.pUfrag = pUfrag;
   *  connChkCB.pPasswd = pPasswd;
   *  connChkCB.peerPriority = peerPriority;
   *  connChkCB.useCandidate = useCandidate;
   *  connChkCB.iceControlling = iceControlling;
   *  connChkCB.iceControlled = iceControlled;
   */
  char      ipaddr[SOCKADDR_MAX_STRLEN];
  char      ufragPair[ICE_MAX_UFRAG_PAIR_LENGTH];
  StunMsgId stunId;
  char      srcAddrStr[]      = "10.47.1.23:52456";
  char      respRflxAddrStr[] = "158.38.48.10:52423";

  struct sockaddr_storage srcAddr;
  struct sockaddr_storage dstAddr;

  struct sockaddr_storage respRflxAddr;

  memset( &connChkCB, 0, sizeof(ConncheckCB) );

  ASSERT_TRUE( ICELIB_Start(icelib, true) );

  /* 1. Tick */

  ICELIB_Tick(icelib);
  ASSERT_TRUE(strncmp(remoteHostRtpAddr,
                      sockaddr_toString( (const struct sockaddr*)connChkCB.
                                         destination,
                                         ipaddr,
                                         SOCKADDR_MAX_STRLEN,
                                         true ),
                      INET_ADDRSTRLEN) == 0);
  ASSERT_FALSE(connChkCB.useRelay);

  /* 2. Tick */
  memset( &connChkCB, 0, sizeof(ConncheckCB) );
  ICELIB_Tick(icelib);

  ASSERT_TRUE(strncmp(remoteRflxRtpAddr,
                      sockaddr_toString( (const struct sockaddr*)connChkCB.
                                         destination,
                                         ipaddr,
                                         SOCKADDR_MAX_STRLEN,
                                         true ),
                      INET_ADDRSTRLEN) == 0);
  ASSERT_FALSE(connChkCB.useRelay);

  /* 3. Tick */
  memset( &connChkCB, 0, sizeof(ConncheckCB) );
  ICELIB_Tick(icelib);

  ASSERT_TRUE(strncmp(remoteRelayRtpAddr,
                      sockaddr_toString( (const struct sockaddr*)connChkCB.
                                         destination,
                                         ipaddr,
                                         SOCKADDR_MAX_STRLEN,
                                         true ),
                      INET_ADDRSTRLEN) == 0);
  ASSERT_FALSE(connChkCB.useRelay);

  stunlib_createId(&stunId);
  sockaddr_initFromString( (struct sockaddr*)&srcAddr, srcAddrStr );
  sockaddr_initFromString( (struct sockaddr*)&dstAddr, "192.168.2.10:3456" );
  ICELIB_getCheckListRemoteUsernamePair(ufragPair,
                                        ICE_MAX_UFRAG_PAIR_LENGTH,
                                        &icelib->streamControllers[0].checkList,
                                        false);
  ICELIB_incomingBindingRequest(icelib,
                                1,
                                2,
                                ufragPair,
                                connChkCB.peerPriority,
                                false,
                                false,
                                true,
                                45678,
                                stunId,
                                7,
                                IPPROTO_UDP,
                                (struct sockaddr*)&srcAddr,
                                (const struct sockaddr*)&dstAddr,
                                false,
                                NULL,
                                0);



  /* 4. Tick */
  memset( &connChkCB, 0, sizeof(ConncheckCB) );
  ICELIB_Tick(icelib);


  ASSERT_TRUE(strncmp(remoteHostRtpAddr,
                      sockaddr_toString( (const struct sockaddr*)connChkCB.
                                         destination,
                                         ipaddr,
                                         SOCKADDR_MAX_STRLEN,
                                         true ),
                      INET_ADDRSTRLEN) == 0);
  ASSERT_TRUE(connChkCB.useRelay);




  /* 5. Tick */
  memset( &connChkCB, 0, sizeof(ConncheckCB) );
  ICELIB_Tick(icelib);

  ASSERT_TRUE(strncmp(remoteRflxRtpAddr,
                      sockaddr_toString( (const struct sockaddr*)connChkCB.
                                         destination,
                                         ipaddr,
                                         SOCKADDR_MAX_STRLEN,
                                         true ),
                      INET_ADDRSTRLEN) == 0);
  ASSERT_TRUE(connChkCB.useRelay);
  ASSERT_FALSE(connChkCB.useCandidate);


  /* 6. Tick */
  memset( &connChkCB, 0, sizeof(ConncheckCB) );
  ICELIB_Tick(icelib);


  ASSERT_TRUE(strncmp(remoteRelayRtpAddr,
                      sockaddr_toString( (const struct sockaddr*)connChkCB.
                                         destination,
                                         ipaddr,
                                         SOCKADDR_MAX_STRLEN,
                                         true ),
                      INET_ADDRSTRLEN) == 0);
  ASSERT_TRUE(connChkCB.useRelay);

  /* 7. Tick */
  memset( &connChkCB, 0, sizeof(ConncheckCB) );
  ICELIB_Tick(icelib);

  ASSERT_TRUE(strncmp(remoteHostRtcpAddr,
                      sockaddr_toString( (const struct sockaddr*)connChkCB.
                                         destination,
                                         ipaddr,
                                         SOCKADDR_MAX_STRLEN,
                                         true ),
                      INET_ADDRSTRLEN) == 0);
  ASSERT_FALSE(connChkCB.useRelay);


  /* 8. Tick */
  memset( &connChkCB, 0, sizeof(ConncheckCB) );
  ICELIB_Tick(icelib);


  ASSERT_TRUE(strncmp(remoteRflxRtcpAddr,
                      sockaddr_toString( (const struct sockaddr*)connChkCB.
                                         destination,
                                         ipaddr,
                                         SOCKADDR_MAX_STRLEN,
                                         true ),
                      INET_ADDRSTRLEN) == 0);
  ASSERT_FALSE(connChkCB.useRelay);


  /* 9. Tick */
  memset( &connChkCB, 0, sizeof(ConncheckCB) );
  ICELIB_Tick(icelib);


  ASSERT_TRUE(strncmp(remoteRelayRtcpAddr,
                      sockaddr_toString( (const struct sockaddr*)connChkCB.
                                         destination,
                                         ipaddr,
                                         SOCKADDR_MAX_STRLEN,
                                         true ),
                      INET_ADDRSTRLEN) == 0);
  ASSERT_FALSE(connChkCB.useRelay);
  sockaddr_initFromString( (struct sockaddr*)&respRflxAddr, respRflxAddrStr );

  ICELIB_incomingBindingResponse(icelib,
                                 200,
                                 connChkCB.transactionId,
                                 connChkCB.destination,
                                 connChkCB.source,
                                 (struct sockaddr*)&respRflxAddr);






  /* 10. Tick */
  memset( &connChkCB, 0, sizeof(ConncheckCB) );
  ICELIB_Tick(icelib);

  ASSERT_TRUE(strncmp(remoteHostRtcpAddr,
                      sockaddr_toString( (const struct sockaddr*)connChkCB.
                                         destination,
                                         ipaddr,
                                         SOCKADDR_MAX_STRLEN,
                                         true ),
                      INET_ADDRSTRLEN) == 0);

  /* 11. Tick */
  memset( &connChkCB, 0, sizeof(ConncheckCB) );
  ICELIB_Tick(icelib);

  ASSERT_TRUE(strncmp(remoteRflxRtcpAddr,
                      sockaddr_toString( (const struct sockaddr*)connChkCB.
                                         destination,
                                         ipaddr,
                                         SOCKADDR_MAX_STRLEN,
                                         true ),
                      INET_ADDRSTRLEN) == 0);

  /* 12. Tick */
  memset( &connChkCB, 0, sizeof(ConncheckCB) );
  ICELIB_Tick(icelib);

  ASSERT_TRUE(strncmp(remoteRelayRtcpAddr,
                      sockaddr_toString( (const struct sockaddr*)connChkCB.
                                         destination,
                                         ipaddr,
                                         SOCKADDR_MAX_STRLEN,
                                         true ),
                      INET_ADDRSTRLEN) == 0);
  ASSERT_TRUE(connChkCB.useRelay);

  /* 13. Tick */
  memset( &connChkCB, 0, sizeof(ConncheckCB) );
  ICELIB_Tick(icelib);
  /*  ASSERT_FALSE( connChkCB.gotCB ); */

  /* 14. Tick */
  memset( &connChkCB, 0, sizeof(ConncheckCB) );
  ICELIB_Tick(icelib);
  ASSERT_FALSE(connChkCB.gotCB);

}



CTEST2(data, ICE_Incomming_Response)
{
  (void)data;
  char ipaddr[SOCKADDR_MAX_STRLEN];
  /* char      ufragPair[ICE_MAX_UFRAG_PAIR_LENGTH]; */
/*  StunMsgId stunId; */
/* char      srcAddrStr[]      = "10.47.1.23:52456"; */
  char respRflxAddrStr[] = "158.38.48.10:52423";
  int  i;

/*  struct sockaddr_storage srcAddr; */
/*  struct sockaddr_storage dstAddr; */

  struct sockaddr_storage respRflxAddr;

  memset( &connChkCB, 0, sizeof(ConncheckCB) );

  ASSERT_TRUE( ICELIB_Start(icelib, true) );

  sockaddr_initFromString( (struct sockaddr*)&respRflxAddr, respRflxAddrStr );
  /* 1. Tick */


  for (i = 0; i < 5000; i++)
  {
    ICELIB_Tick(icelib);
    if (connChkCB.gotCB)
    {
      sockaddr_toString( (const struct sockaddr*)connChkCB.destination,
                         ipaddr,
                         SOCKADDR_MAX_STRLEN,
                         true );

      /* We pretend to be the perfect network. Responses arrive imediately! */
      printf(" -------> Testclient sending response\n");
      ICELIB_incomingBindingResponse(icelib,
                                     200,
                                     connChkCB.transactionId,
                                     connChkCB.destination,
                                     connChkCB.source,
                                     connChkCB.source);

      memset( &connChkCB, 0, sizeof(ConncheckCB) );
    }
  }

  ICELIB_validListDump(&icelib->streamControllers[0].validList);

  ASSERT_TRUE(icelib->iceState == ICELIB_COMPLETED);
}






CTEST(icelib, compareTransactionId)
{
  StunMsgId id1;
  StunMsgId id2;
  StunMsgId id3;

  stunlib_createId(&id1);


  memcpy(&id3, &id1, STUN_MSG_ID_SIZE);

  ASSERT_TRUE( 0 == ICELIB_compareTransactionId(&id1,
                                                &id3) );

  stunlib_createId(&id2);
  ASSERT_FALSE( 0 == ICELIB_compareTransactionId(&id1,
                                                 &id2) );

}


CTEST(icelib,makeTieBreaker)
{
  uint64_t tie1 = ICELIB_makeTieBreaker();
  uint64_t tie2 = ICELIB_makeTieBreaker();

  ASSERT_FALSE(tie1 == tie2);


}


CTEST(icelib,makeUserNamePair)
{
  char ufrag1[] = "ufr1";
  char ufrag2[] = "ufr2";

  char pair[ICE_MAX_UFRAG_PAIR_LENGTH];

  ICELIB_makeUsernamePair(pair,
                          ICE_MAX_UFRAG_PAIR_LENGTH,
                          ufrag1,
                          ufrag2);

  ASSERT_TRUE( 0 == strncmp(pair, "ufr1:ufr2", ICE_MAX_UFRAG_PAIR_LENGTH) );
  ASSERT_FALSE( 0 == strncmp(pair, "ufr2:ufr1", ICE_MAX_UFRAG_PAIR_LENGTH) );

  ICELIB_makeUsernamePair(pair,
                          ICE_MAX_UFRAG_PAIR_LENGTH,
                          NULL,
                          ufrag2);
  ASSERT_FALSE( 0 == strncmp(pair, "ufr1:ufr2", ICE_MAX_UFRAG_PAIR_LENGTH) );
  ASSERT_TRUE( 0 == strncmp(pair, "--no_ufrags--", ICE_MAX_UFRAG_PAIR_LENGTH) );


  ICELIB_makeUsernamePair(pair,
                          ICE_MAX_UFRAG_PAIR_LENGTH,
                          ufrag1,
                          NULL);
  ASSERT_FALSE( 0 == strncmp(pair, "ufr1:ufr2", ICE_MAX_UFRAG_PAIR_LENGTH) );
  ASSERT_TRUE( 0 == strncmp(pair, "--no_ufrags--", ICE_MAX_UFRAG_PAIR_LENGTH) );

  ICELIB_makeUsernamePair(pair,
                          ICE_MAX_UFRAG_PAIR_LENGTH,
                          NULL,
                          NULL);
  ASSERT_FALSE( 0 == strncmp(pair, "ufr1:ufr2", ICE_MAX_UFRAG_PAIR_LENGTH) );
  ASSERT_TRUE( 0 == strncmp(pair, "--no_ufrags--", ICE_MAX_UFRAG_PAIR_LENGTH) );




}



CTEST(icelib,findCandidates)
{

  ICE_MEDIA_STREAM mediaStream;

  struct sockaddr_storage addr1;

  sockaddr_initFromString( (struct sockaddr*)&addr1,
                           "10.47.1.34" );

  ICELIBTYPES_ICE_MEDIA_STREAM_reset(&mediaStream);

  ASSERT_TRUE( NULL == pICELIB_findCandidate(&mediaStream,
                                             ICE_TRANS_UDP,
                                             (struct sockaddr*)&addr1,
                                             1) );

  mediaStream.numberOfCandidates = 1;

  sockaddr_copy( (struct sockaddr*)&mediaStream.candidate[0].connectionAddr,
                 (struct sockaddr*)&addr1 );

  mediaStream.candidate[0].transport = ICE_TRANS_UDP;
  /* mediaStream.candidate[0].transport   = ICE_TRANSPORT_proto(ICE_TRANS_UDP);
   * */
  /* mediaStream.candidate[0].transport   = IPPROTO_UDP; */
  mediaStream.candidate[0].componentid = 1;


  ASSERT_FALSE( NULL == pICELIB_findCandidate(&mediaStream,
                                              IPPROTO_UDP,
                                              (struct sockaddr*)&addr1,
                                              1) );

}


CTEST(icelib,splitUfragPair)
{
  char   ufragPair[] = "ufr1:ufr2";
  size_t idx;

  ASSERT_TRUE( 0 == strcmp(pICELIB_splitUfragPair(ufragPair, &idx), "ufr2") );

}



CTEST(icelib,compareUfragPair)
{

  char pair[]   = "ufrag1:ufrag2";
  char ufrag1[] = "ufrag1";
  char ufrag2[] = "ufrag2";

  ASSERT_TRUE( ICELIB_compareUfragPair(pair,
                                       ufrag1,
                                       ufrag2) );

  ASSERT_FALSE( ICELIB_compareUfragPair("hei",
                                        ufrag1,
                                        ufrag2) );

  ASSERT_FALSE( ICELIB_compareUfragPair(pair,
                                        "hei",
                                        ufrag2) );

  ASSERT_FALSE( ICELIB_compareUfragPair(pair,
                                        ufrag1,
                                        "hei") );

  ASSERT_FALSE( ICELIB_compareUfragPair(NULL,
                                        ufrag1,
                                        ufrag2) );

  ASSERT_FALSE( ICELIB_compareUfragPair(pair,
                                        NULL,
                                        NULL) );


}



CTEST(icelib,timerStop)
{
  ICELIB_TIMER timer;

  ICELIB_timerStop(&timer);

  ASSERT_TRUE(timer.timerState == ICELIB_timerStopped);

}


CTEST(icelib,createRandomString)
{
  char randomStr[128];

  ICELIB_createRandomString(randomStr, 127);

  ASSERT_TRUE(randomStr[127] == '\0');


}


CTEST(icelib,longToIceChar)
{
  char b64[6];
  long data = 45678944;
  ICELIB_longToIcechar(data, b64, 5);

  ASSERT_TRUE(b64[0] == 'g');
  ASSERT_TRUE(b64[1] == 'F');
  ASSERT_TRUE(b64[2] == 'Q');
  ASSERT_TRUE(b64[3] == 'u');
  ASSERT_TRUE(b64[4] == 'C');

}


CTEST(icelib,isEmptyCandidate)
{

  ICE_CANDIDATE candidate;


  candidate.foundation[0] = '\0';

  ASSERT_TRUE( ICELIB_isEmptyCandidate(&candidate) );

  strcpy(candidate.foundation, "ert");

  ASSERT_FALSE( ICELIB_isEmptyCandidate(&candidate) );

  ICELIB_resetCandidate(&candidate);
  ASSERT_TRUE( ICELIB_isEmptyCandidate(&candidate) );


}



CTEST(icelib,toStringCheckListState)
{

  ASSERT_TRUE( 0 ==
               strcmp(ICELIB_toString_CheckListState(ICELIB_CHECKLIST_IDLE),
                      "ICELIB_CHECKLIST_IDLE") );

  ASSERT_TRUE( 0 ==
               strcmp(ICELIB_toString_CheckListState(ICELIB_CHECKLIST_RUNNING),
                      "ICELIB_CHECKLIST_RUNNING") );

  ASSERT_TRUE( 0 ==
               strcmp(ICELIB_toString_CheckListState(ICELIB_CHECKLIST_COMPLETED),
                      "ICELIB_CHECKLIST_COMPLETED") );

  ASSERT_TRUE( 0 ==
               strcmp(ICELIB_toString_CheckListState(ICELIB_CHECKLIST_FAILED),
                      "ICELIB_CHECKLIST_FAILED") );

  ASSERT_TRUE( 0 == strcmp(ICELIB_toString_CheckListState(7),
                           "--unknown--") );

}



CTEST(icelib,toStringCheckListPairState)
{
  ASSERT_TRUE( 0 == strcmp(ICELIB_toString_CheckListPairState(ICELIB_PAIR_IDLE),
                           "IDLE") );

  ASSERT_TRUE( 0 ==
               strcmp(ICELIB_toString_CheckListPairState(ICELIB_PAIR_PAIRED),
                      "PAIRED") );

  ASSERT_TRUE( 0 ==
               strcmp(ICELIB_toString_CheckListPairState(ICELIB_PAIR_FROZEN),
                      "FROZEN") );

  ASSERT_TRUE( 0 ==
               strcmp(ICELIB_toString_CheckListPairState(ICELIB_PAIR_WAITING),
                      "WAITING") );

  ASSERT_TRUE( 0 ==
               strcmp(ICELIB_toString_CheckListPairState(ICELIB_PAIR_INPROGRESS),
                      "INPROGRESS") );

  ASSERT_TRUE( 0 ==
               strcmp(ICELIB_toString_CheckListPairState(ICELIB_PAIR_SUCCEEDED),
                      "SUCCEEDED") );

  ASSERT_TRUE( 0 ==
               strcmp(ICELIB_toString_CheckListPairState(ICELIB_PAIR_FAILED),
                      "FAILED") );

  ASSERT_TRUE( 0 == strcmp(ICELIB_toString_CheckListPairState(10),
                           "--unknown--") );

}



CTEST(icelib,removeChecksFromCheckList)
{
  ICELIB_CHECKLIST checkList;

  ICELIB_removChecksFromCheckList(&checkList);

  ASSERT_TRUE(checkList.numberOfPairs == 0);

  ASSERT_TRUE(checkList.nextPairId == 0);
}


CTEST(icelib,resetCandidate)
{
  ICE_CANDIDATE candidate;
  ICELIB_resetCandidate(&candidate);
  ASSERT_TRUE( ICELIB_isEmptyCandidate(&candidate) );
}


CTEST(icelib,formPairs_IPv4)
{

  ICELIB_CHECKLIST CheckList;
  /* ICELIB_CALLBACK_LOG CallbackLog; */
  ICE_MEDIA_STREAM LocalMediaStream;
  ICE_MEDIA_STREAM RemoteMediaStream;
  /* unsigned int        maxPairs; */

  ICE_CANDIDATE* cand;

  /* set up addresses */
  struct sockaddr_storage localHost;
  struct sockaddr_storage localRelay;
  struct sockaddr_storage remoteHost;
  struct sockaddr_storage remoteRelay;

  sockaddr_initFromString( (struct sockaddr*)&localHost,
                           "192.168.2.10:3456" );
  sockaddr_initFromString( (struct sockaddr*)&localRelay,
                           "158.38.48.10:4534" );
  sockaddr_initFromString( (struct sockaddr*)&remoteHost,
                           "192.168.2.10:3459" );
  sockaddr_initFromString( (struct sockaddr*)&remoteRelay, "8.8.8.8:4444" );

  ICELIBTYPES_ICE_MEDIA_STREAM_reset(&LocalMediaStream);
  ICELIBTYPES_ICE_MEDIA_STREAM_reset(&RemoteMediaStream);

  cand = &LocalMediaStream.candidate[0];


  /* Local */
  ICELIB_fillLocalCandidate(cand,
                            1,
                            5,
                            (struct sockaddr*)&localHost,
                            NULL,
                            ICE_TRANS_UDP,
                            ICE_CAND_TYPE_HOST,
                            0);
  LocalMediaStream.numberOfCandidates++;

  cand = &LocalMediaStream.candidate[1];


  ICELIB_fillLocalCandidate(cand,
                            1,
                            5,
                            (struct sockaddr*)&localRelay,
                            NULL,
                            ICE_TRANS_UDP,
                            ICE_CAND_TYPE_RELAY,
                            0);
  LocalMediaStream.numberOfCandidates++;



  qsort(LocalMediaStream.candidate,
        LocalMediaStream.numberOfCandidates,
        sizeof(ICE_CANDIDATE),
        ICELIB_candidateSort);
  /* Remote */


  cand = &RemoteMediaStream.candidate[0];

  ICELIB_fillRemoteCandidate(cand,
                             1,
                             "TJA",
                             3,
                             2130706431,
                             (struct sockaddr*)&remoteHost,
                             ICE_TRANS_UDP,
                             ICE_CAND_TYPE_HOST);
  RemoteMediaStream.numberOfCandidates++;

  cand = &RemoteMediaStream.candidate[1];




  ICELIB_fillRemoteCandidate(cand,
                             1,
                             "TJA",
                             3,
                             30706431,
                             (struct sockaddr*)&remoteRelay,
                             ICE_TRANS_UDP,
                             ICE_CAND_TYPE_RELAY);
  RemoteMediaStream.numberOfCandidates++;


  ICELIB_resetCheckList(&CheckList, 0);

  ICELIB_formPairs(&CheckList,
                   NULL,
                   &LocalMediaStream,
                   &RemoteMediaStream,
                   10);

  ICELIB_computeListPairPriority(&CheckList, true);
  ICELIB_sortPairsCL(&CheckList);

  ASSERT_TRUE( CheckList.numberOfPairs  == 4);

  /* Check pair 0 */
  ASSERT_TRUE( CheckList.checkListPairs[0].pairState == ICELIB_PAIR_PAIRED);
  ASSERT_TRUE( sockaddr_sameAddr( (struct sockaddr*)&CheckList.checkListPairs[0]
                                  .
                                  pLocalCandidate->connectionAddr,
                                  (struct sockaddr*)&localHost ) );

  ASSERT_TRUE( sockaddr_sameAddr( (struct sockaddr*)&CheckList.checkListPairs[0]
                                  .
                                  pRemoteCandidate->connectionAddr,
                                  (struct sockaddr*)&remoteHost ) );

  /* Check pair 1 */
  ASSERT_TRUE( CheckList.checkListPairs[1].pairState == ICELIB_PAIR_PAIRED);
  ASSERT_TRUE( sockaddr_sameAddr( (struct sockaddr*)&CheckList.checkListPairs[1]
                                  .
                                  pLocalCandidate->connectionAddr,
                                  (struct sockaddr*)&localHost ) );

  ASSERT_TRUE( sockaddr_sameAddr( (struct sockaddr*)&CheckList.checkListPairs[1]
                                  .
                                  pRemoteCandidate->connectionAddr,
                                  (struct sockaddr*)&remoteRelay ) );

  /* Check pair 2 */
  ASSERT_TRUE( CheckList.checkListPairs[2].pairState == ICELIB_PAIR_PAIRED);
  ASSERT_TRUE( sockaddr_sameAddr( (struct sockaddr*)&CheckList.checkListPairs[2]
                                  .
                                  pLocalCandidate->connectionAddr,
                                  (struct sockaddr*)&localRelay ) );

  ASSERT_TRUE( sockaddr_sameAddr( (struct sockaddr*)&CheckList.checkListPairs[2]
                                  .
                                  pRemoteCandidate->connectionAddr,
                                  (struct sockaddr*)&remoteHost ) );

  /* Check pair 3 */
  ASSERT_TRUE( CheckList.checkListPairs[3].pairState == ICELIB_PAIR_PAIRED);
  ASSERT_TRUE( sockaddr_sameAddr( (struct sockaddr*)&CheckList.checkListPairs[3]
                                  .
                                  pLocalCandidate->connectionAddr,
                                  (struct sockaddr*)&localRelay ) );

  ASSERT_TRUE( sockaddr_sameAddr( (struct sockaddr*)&CheckList.checkListPairs[3]
                                  .
                                  pRemoteCandidate->connectionAddr,
                                  (struct sockaddr*)&remoteRelay ) );

}



CTEST(icelib,formPairs_IPv6)
{

  ICELIB_CHECKLIST CheckList;
  /* ICELIB_CALLBACK_LOG CallbackLog; */
  ICE_MEDIA_STREAM LocalMediaStream;
  ICE_MEDIA_STREAM RemoteMediaStream;
  /* unsigned int        maxPairs; */

  ICE_CANDIDATE* cand;

  /* set up addresses */
  struct sockaddr_storage localHost_6;
  struct sockaddr_storage localRelay_6;
  struct sockaddr_storage remoteHost_6;
  struct sockaddr_storage remoteRelay_6;


  sockaddr_initFromString( (struct sockaddr*)&localHost_6,
                           "[2001:420:4:eb66:119a:ddff:fe3a:27d1]:2345" );
  sockaddr_initFromString( (struct sockaddr*)&localRelay_6,
                           "[2001:420:2:ea63:119a:ddff:fe3a:27d1]:6789" );
  sockaddr_initFromString( (struct sockaddr*)&remoteHost_6,
                           "[2001:421:4:eb46:119a:ddff:fe1a:27d4]:4381" );
  sockaddr_initFromString( (struct sockaddr*)&remoteRelay_6,
                           "[2001:420:2:eb66:119a:ddff:fe3a:27d0]:2176" );


  ICELIBTYPES_ICE_MEDIA_STREAM_reset(&LocalMediaStream);
  ICELIBTYPES_ICE_MEDIA_STREAM_reset(&RemoteMediaStream);

  cand = &LocalMediaStream.candidate[0];


  /* Local */
  ICELIB_fillLocalCandidate(cand,
                            1,
                            5,
                            (struct sockaddr*)&localHost_6,
                            NULL,
                            ICE_TRANS_UDP,
                            ICE_CAND_TYPE_HOST,
                            0);
  LocalMediaStream.numberOfCandidates++;

  cand = &LocalMediaStream.candidate[1];

  ICELIB_fillLocalCandidate(cand,
                            1,
                            5,
                            (struct sockaddr*)&localRelay_6,
                            NULL,
                            ICE_TRANS_UDP,
                            ICE_CAND_TYPE_RELAY,
                            0);
  LocalMediaStream.numberOfCandidates++;



  qsort(LocalMediaStream.candidate,
        LocalMediaStream.numberOfCandidates,
        sizeof(ICE_CANDIDATE),
        ICELIB_candidateSort);
  /* Remote */
  cand = &RemoteMediaStream.candidate[0];

  ICELIB_fillRemoteCandidate(cand,
                             1,
                             "TJA",
                             3,
                             2130706431,
                             (struct sockaddr*)&remoteHost_6,
                             ICE_TRANS_UDP,
                             ICE_CAND_TYPE_HOST);
  RemoteMediaStream.numberOfCandidates++;

  cand = &RemoteMediaStream.candidate[1];

  ICELIB_fillRemoteCandidate(cand,
                             1,
                             "TJA",
                             3,
                             30706431,
                             (struct sockaddr*)&remoteRelay_6,
                             ICE_TRANS_UDP,
                             ICE_CAND_TYPE_RELAY);
  RemoteMediaStream.numberOfCandidates++;


  ICELIB_resetCheckList(&CheckList, 0);

  ICELIB_formPairs(&CheckList,
                   NULL,
                   &LocalMediaStream,
                   &RemoteMediaStream,
                   10);

  ICELIB_computeListPairPriority(&CheckList, true);
  ICELIB_sortPairsCL(&CheckList);

  ASSERT_TRUE( CheckList.numberOfPairs  == 4);

  /* Check pair 0 */
  ASSERT_TRUE( CheckList.checkListPairs[0].pairState == ICELIB_PAIR_PAIRED);
  ASSERT_TRUE( sockaddr_sameAddr( (struct sockaddr*)&CheckList.checkListPairs[0]
                                  .
                                  pLocalCandidate->connectionAddr,
                                  (struct sockaddr*)&localHost_6 ) );

  ASSERT_TRUE( sockaddr_sameAddr( (struct sockaddr*)&CheckList.checkListPairs[0]
                                  .
                                  pRemoteCandidate->connectionAddr,
                                  (struct sockaddr*)&remoteHost_6 ) );
  /* Check pair 1 */
  ASSERT_TRUE( CheckList.checkListPairs[1].pairState == ICELIB_PAIR_PAIRED);
  ASSERT_TRUE( sockaddr_sameAddr( (struct sockaddr*)&CheckList.checkListPairs[1]
                                  .
                                  pLocalCandidate->connectionAddr,
                                  (struct sockaddr*)&localHost_6 ) );

  ASSERT_TRUE( sockaddr_sameAddr( (struct sockaddr*)&CheckList.checkListPairs[1]
                                  .
                                  pRemoteCandidate->connectionAddr,
                                  (struct sockaddr*)&remoteRelay_6 ) );

  /* Check pair 2 */
  ASSERT_TRUE( CheckList.checkListPairs[2].pairState == ICELIB_PAIR_PAIRED);
  ASSERT_TRUE( sockaddr_sameAddr( (struct sockaddr*)&CheckList.checkListPairs[2]
                                  .
                                  pLocalCandidate->connectionAddr,
                                  (struct sockaddr*)&localRelay_6 ) );

  ASSERT_TRUE( sockaddr_sameAddr( (struct sockaddr*)&CheckList.checkListPairs[2]
                                  .
                                  pRemoteCandidate->connectionAddr,
                                  (struct sockaddr*)&remoteHost_6 ) );
  /* Check pair 3 */
  ASSERT_TRUE( CheckList.checkListPairs[3].pairState == ICELIB_PAIR_PAIRED);
  ASSERT_TRUE( sockaddr_sameAddr( (struct sockaddr*)&CheckList.checkListPairs[3]
                                  .
                                  pLocalCandidate->connectionAddr,
                                  (struct sockaddr*)&localRelay_6 ) );

  ASSERT_TRUE( sockaddr_sameAddr( (struct sockaddr*)&CheckList.checkListPairs[3]
                                  .
                                  pRemoteCandidate->connectionAddr,
                                  (struct sockaddr*)&remoteRelay_6 ) );
}
