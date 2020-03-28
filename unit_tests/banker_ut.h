#ifndef __BANKER_UT__
#define __BANKER_UT__

#include "cppunit/TestFixture.h" 							      // CppUnit::TestFixture
#include "cppunit/extensions/HelperMacros.h" 				// for assert macros

class BankerUT : public CppUnit::TestFixture
{
CPPUNIT_TEST_SUITE(BankerUT);
    CPPUNIT_TEST_SUITE_END();

public:
    /** */
    BankerUT();
    ~BankerUT();
    virtual void setUp(){}
    virtual void tearDown(){}

    /* 
    void campaignBid_HasBudget();
    void campaignBid_HasNoBudget();
    void campaignBid_CampaignNotExist();
    void campaignRefund();
    void campaignRefund_CampaignNotInThisInstance();
    */

};

#endif //__BANKER_UT__