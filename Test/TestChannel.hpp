#include "Channel.hpp"
#include "TestUtils.hpp"

#define TMP_CH_NAME "&chname"

class TestChannel {
    public:
        TestChannel(void);
    
    private:
        void TestKick(void);
        void TestJoin(void);
        void TestInvite(void);
        void TestIsValidPrefix(void);
        void TestMode(void);
        void TestSetMode(void);

        char get_valid_prefix(void);
        char get_invalid_prefix(void);
};