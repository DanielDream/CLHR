#ifndef CO_MM_O1
#define CO_MM_O1

#include "Q2DTorus.h"
#include "Event.h"
#include "MFR.h"
#include "MIN.h"
#include "SAMEVC.h"
#include "WC_SAME.h"
#include "WC_SAME2.h"
#include "PB.h"
#include "PB1.h"
#include "VAL.h"
#include "FCMFR.h"
#include  <fstream>
#include  <vector>
//º¯ÊýÎ´ÓÃµ½
void outtotest(vector<Message*>* vecmess, Q2DTorus* tor);
void bufferleft(Q2DTorus* tor, int knode);
void drain(vector<Message*>* vecmess, Q2DTorus* tor, Event* s);

#endif