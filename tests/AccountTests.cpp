#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "Account.h"
#include "MockAccount.h"

using ::testing::Return;
using ::testing::Exactly;
using ::testing::Throw;
using ::testing::_;

class AccountTest : public ::testing::Test {
protected:
    void SetUp() override {
        account = std::make_unique<Account>(1, 1000);
    }
    
    void TearDown() override {
        account.reset();  
    }
    
    std::unique_ptr<Account> account;
};

TEST_F(AccountTest, ConstructorInitializesCorrectly){
    EXPECT_EQ(account->GetBalance(), 1000);
}

TEST_F(AccountTest, GetBalaceReturnsCorrectValue){
    EXPECT_EQ(account->GetBalance(), 1000);
    account->Lock();
    account->ChangeBalance(500);
    EXPECT_EQ(account->GetBalance(), 1500);
}

TEST_F(AccountTest, LockSuccess){
    EXPECT_NO_THROW(account->Lock());
}

TEST_F(AccountTest, LockThoeWhenAlreadyLocked){
    account->Lock();
    EXPECT_THROW(account->Lock(), std::runtime_error);
}

TEST_F(AccountTest, UnlockWorksCorrectly){
    account->Lock();
    EXPECT_NO_THROW(account->Unlock());
    EXPECT_NO_THROW(account->Unlock());
}

TEST_F(AccountTest, ChangeBalanceWorksWhenLockedPositive) {
    account->Lock();
    EXPECT_NO_THROW(account->ChangeBalance(500));
    EXPECT_EQ(account->GetBalance(), 1500);
}

TEST_F(AccountTest, ChangeBalanceWorksWhenLockedNegative) {
    account->Lock();
    EXPECT_NO_THROW(account->ChangeBalance(-300));
    EXPECT_EQ(account->GetBalance(), 700);
}

TEST_F(AccountTest, ChangeBalanceThrowsWhenNotLocked) {
    EXPECT_THROW(account->ChangeBalance(500), std::runtime_error);
}

TEST_F(AccountTest, FullLockChangeBalanceUnlockSequence) {
    account->Lock();
    account->ChangeBalance(200);
    account->Unlock();
    EXPECT_EQ(account->GetBalance(), 1200);
}

TEST_F(AccountTest, MultipleOperations) {
    account->Lock();
    account->ChangeBalance(100);
    account->ChangeBalance(200);
    account->ChangeBalance(-50);
    account->Unlock();
    EXPECT_EQ(account->GetBalance(), 1250);
}

TEST_F(AccountTest, MockAccountTest) {
    MockAccount mockAccount(1, 500);
    
    EXPECT_CALL(mockAccount, GetBalance())
        .Times(Exactly(1))
        .WillOnce(Return(500));
    
    EXPECT_CALL(mockAccount, Lock()).Times(Exactly(1));
    EXPECT_CALL(mockAccount, ChangeBalance(300)).Times(Exactly(1));
    EXPECT_CALL(mockAccount, Unlock()).Times(Exactly(1));
    
    mockAccount.Lock();
    mockAccount.ChangeBalance(300);
    mockAccount.Unlock();
    
    EXPECT_EQ(mockAccount.GetBalance(), 500);
}

TEST_F(AccountTest, MockAccountChangeBalanceThrows) {
    MockAccount mockAccount(1, 500);
    
    EXPECT_CALL(mockAccount, Lock()).Times(Exactly(1));
    EXPECT_CALL(mockAccount, ChangeBalance(100))
        .WillOnce(Throw(std::runtime_error("at first lock the account")));
    
    mockAccount.Lock();
    EXPECT_THROW(mockAccount.ChangeBalance(100), std::runtime_error);
}

TEST_F(AccountTest, MockAccountDoubleLockThrows) {
    MockAccount mockAccount(1, 500);
    
    EXPECT_CALL(mockAccount, Lock())
        .WillOnce(Return())
        .WillOnce(Throw(std::runtime_error("already locked")));
    
    EXPECT_NO_THROW(mockAccount.Lock());
    EXPECT_THROW(mockAccount.Lock(), std::runtime_error);
}

TEST_F(AccountTest, UnlockWithoutLock) {
    EXPECT_NO_THROW(account->Unlock());
    account->Lock();
    EXPECT_NO_THROW(account->Unlock());
    EXPECT_NO_THROW(account->Unlock());
}
