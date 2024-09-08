#include "TestChannel.hpp"

TestChannel::TestChannel(void) {
    std::cout << "====== TestChannel ======\n";
    TestKick();
    TestJoin();
    TestInvite();
    TestMode();
    TestSetMode();
}

char TestChannel::get_valid_prefix(void) {
    return ' ';
}

char TestChannel::get_invalid_prefix(void) {
    return 'j';
}

void TestChannel::TestKick(void) {
    std::cout << "====== Kick METHOD ======\n";
    Channel *ch = new Channel(TMP_CH_NAME);
    
    // case1 : memeber 0, kick someone
    IsIntEqual(ch->Kick(4), 0);

    // case2 : member 1, kick someone does not match -> return should be 1
    const int& clinet_sock = 1;
    ch->Join(clinet_sock, get_valid_prefix());
    IsIntEqual(ch->Kick(clinet_sock + 2), 1);
    
    // case3 : member 1, kick that member -> return should be 0
    IsIntEqual(ch->Kick(clinet_sock), 0);

    delete ch;
}

void TestChannel::TestInvite(void) {
    std::cout << "====== Invite METHOD ======\n";
    Channel *ch = new Channel(TMP_CH_NAME);

    //case 1: Invite and check IsInvite
    const int& client_sock = 1;
    IsBoolEqual(ch->Invite(client_sock), true);
    IsBoolEqual(ch->IsInvited(client_sock), true);

    //case 2: Invite again
    IsBoolEqual(ch->Invite(client_sock), false);
    delete ch;
}

void TestChannel::TestJoin(void) {
    std::cout << "====== Join METHOD ======\n";
    // case 1 : wrong prefix
    Channel *ch = new Channel(TMP_CH_NAME);
    const int& client_sock = 1;
    IsBoolEqual(ch->Join(client_sock, get_invalid_prefix()), false);
    IsIntEqual(ch->get_size(), 0);

    // case 2 : join failed with channel limit
    const int& limit = 1;
    ch->set_limit(limit);
    ch->Join(client_sock - 1, get_valid_prefix());
    IsBoolEqual(ch->Join(client_sock, get_valid_prefix()), false);
    IsIntEqual(ch->get_size(), limit);

    // case 3 : join failed with already joined socket
    ch->set_limit(limit + 3);
    ch->Join(client_sock, get_valid_prefix());
    IsBoolEqual(ch->Join(client_sock, get_valid_prefix()), false);
    IsIntEqual(ch->get_size(), 2);

    // case 4 : join success with invite list;
    const int& invited_client = 5;
    if (ch->Invite(invited_client) == true) {
        IsBoolEqual(ch->IsInvited(invited_client), true);
        IsBoolEqual(ch->Join(invited_client, get_valid_prefix()), true);
        IsBoolEqual(ch->IsInvited(invited_client), false);
    }
    delete ch;
}

void TestChannel::TestMode(void) {
    std::cout << "====== Mode METHOD ======\n";
    Channel *ch = new Channel(TMP_CH_NAME);

    const int& client_sock = 1;
    if (ch->Join(client_sock, get_valid_prefix()) && \
        ch->IsMember(client_sock)) {
        IsBoolEqual(ch->IsOperator(client_sock), false);
        const char& operator_mode = '@';
        ch->Mode(client_sock, operator_mode);
        IsBoolEqual(ch->IsOperator(client_sock), true);
    }
    delete ch;
}

void TestChannel::TestSetMode(void) {
    std::cout << "====== set_mode METHOD ======\n";
    Channel *ch =  new Channel(TMP_CH_NAME);

    // initial mode should be MODE_LIMIT
    int flag = ch->get_mode();
    IsIntEqual(flag, MODE_LIMIT);

    int flags[4] = {MODE_LIMIT, MODE_INVITE, MODE_KEY, MODE_TOPIC};
    // MODE change test 1
    for (int i = 0; i < 4; ++i) {
        flag = flags[i];
        ch->set_mode(flag, true);
        IsIntEqual(flag, ch->get_mode());
        ch->set_mode(flag, false);
        IsIntEqual(0, ch->get_mode());
    }

    // MODE change test 2
    flag = flags[0];
    ch->set_mode(flag, true);
    flag = flags[1];
    ch->set_mode(flag, true);
    IsIntEqual(flags[0] | flags[1], ch->get_mode());
    delete ch;
}