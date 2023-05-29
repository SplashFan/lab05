#include "Account.h"
#include "Transaction.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

class AccountMock : public Account {
public:
    AccountMock(int id, int balance) : Account(id, balance) {}
    MOCK_CONST_METHOD0(GetBalance, int());
    MOCK_METHOD1(ChangeBalance, void(int diff)); 
    MOCK_METHOD0(Lock, void());
    MOCK_METHOD0(Unlock, void());
};

class TransactionMock : public Transaction {
public:
    MOCK_METHOD3(Make, bool(Account& from, Account& to, int sum));
};

TEST(Account, Mock) {
    AccountMock acc(1, 666);
    EXPECT_CALL(acc, GetBalance()).Times(1);
    EXPECT_CALL(acc, ChangeBalance(testing::_)).Times(2);
    EXPECT_CALL(acc, Lock()).Times(2);
    EXPECT_CALL(acc, Unlock()).Times(1);
    acc.GetBalance();
    acc.ChangeBalance(100); 
    acc.Lock();
    acc.ChangeBalance(100);
    acc.Lock(); 
    acc.Unlock();
}

TEST(Account, SimpleTest) {
    Account acc(1, 666);
    EXPECT_EQ(acc.id(), 1);
    EXPECT_EQ(acc.GetBalance(), 666);
    EXPECT_THROW(acc.ChangeBalance(200), std::runtime_error);
    EXPECT_NO_THROW(acc.Lock());
    acc.ChangeBalance(200);
    EXPECT_EQ(acc.GetBalance(), 866);
    EXPECT_THROW(acc.Lock(), std::runtime_error);
    EXPECT_NO_THROW(acc.Unlock());
}

TEST(Transaction, Mock) {
    TransactionMock tr;
    Account ac1(1, 100);
    Account ac2(2, 300);
    EXPECT_CALL(tr, Make(testing::_, testing::_, testing::_))
    .Times(5);
    tr.set_fee(200);
    tr.Make(ac1, ac2, 200);
    tr.Make(ac2, ac1, 300);
    tr.Make(ac1, ac1, 0); 
    tr.Make(ac1, ac2, -5); 
    tr.Make(ac2, ac1, 50); 
}

TEST(Transaction, SimpleTest) {
    Transaction tr;
    Account ac1(1, 100);
    Account ac2(2, 300);
    tr.set_fee(10);
    EXPECT_EQ(tr.fee(), 10);
    EXPECT_THROW(tr.Make(ac1, ac2, 40), std::logic_error);
    EXPECT_THROW(tr.Make(ac1, ac2, -5), std::invalid_argument);
    EXPECT_THROW(tr.Make(ac1, ac1, 100), std::logic_error);
    EXPECT_FALSE(tr.Make(ac1, ac2, 400));
    EXPECT_FALSE(tr.Make(ac2, ac1, 300));
    EXPECT_FALSE(tr.Make(ac2, ac1, 290));
    EXPECT_TRUE(tr.Make(ac2, ac1, 150));
}
