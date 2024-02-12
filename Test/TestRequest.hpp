#ifndef TESTREQUEST_HPP
# define TESTREQUEST_HPP

#include "Request.hpp"
#include "TestUtils.hpp"

class   TestRequest {
    public:
        TestRequest(Server *s, Client *c);

    private:
        void    TestBaseAlphaToNumber(void);
        void    TestSeperateWhiteSpace(void);
        void    TestRemoveDuplicateSpace(void);
        void    TestSplitRequest(void);
};

#endif
