```Account test file```

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

``` Tranaction test file```


#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "Transaction.h"
#include "Account.h"

using ::testing::Return;
using ::testing::Exactly;
using ::testing::Throw;
using ::testing::_;

class TransactionTest : public ::testing::Test {
protected:
    void SetUp() override {
        transaction = std::make_unique<Transaction>();
        transaction->set_fee(1);
    }
    
    std::unique_ptr<Transaction> transaction;
};

TEST_F(TransactionTest, ConstructorInitializesCorrectly) {
    Transaction tx;
    tx.set_fee(1);
    SUCCEED();
}

TEST_F(TransactionTest, SetFeeWorksCorrectly) {
    Transaction tx;
    tx.set_fee(5);
    Account from(1, 1000);
    Account to(2, 500);
    tx.set_fee(60); 
    EXPECT_FALSE(tx.Make(from, to, 100));
}

TEST_F(TransactionTest, CreditIncreasesBalance) {
    transaction->set_fee(1);
    Account from(1, 1000);
    Account to(2, 500);
    
    EXPECT_TRUE(transaction->Make(from, to, 200));
    EXPECT_EQ(to.GetBalance(), 700);  
    EXPECT_EQ(from.GetBalance(), 799);
}

TEST_F(TransactionTest, MakeTransactionSuccess) {
    transaction->set_fee(1);
    Account from(1, 1000);
    Account to(2, 500);
    
    EXPECT_TRUE(transaction->Make(from, to, 200));
    EXPECT_EQ(from.GetBalance(), 799);
    EXPECT_EQ(to.GetBalance(), 700);
}

TEST_F(TransactionTest, MakeTransactionFailsDueToInsufficientFunds) {
    transaction->set_fee(1);
    Account from(1, 100);
    Account to(2, 500);
    
    EXPECT_FALSE(transaction->Make(from, to, 200));
    EXPECT_EQ(from.GetBalance(), 100);
    EXPECT_EQ(to.GetBalance(), 500);
}

TEST_F(TransactionTest, MakeTransactionThrowsWhenSumTooSmall) {
    transaction->set_fee(1);
    Account from(1, 1000);
    Account to(2, 500);
    
    EXPECT_THROW(transaction->Make(from, to, 50), std::logic_error);
}

TEST_F(TransactionTest, MakeTransactionThrowsWhenSumNegative) {
    transaction->set_fee(1);
    Account from(1, 1000);
    Account to(2, 500);
    
    EXPECT_THROW(transaction->Make(from, to, -100), std::invalid_argument);
}

TEST_F(TransactionTest, MakeTransactionThrowsWhenSameAccount) {
    transaction->set_fee(1);
    Account from(1, 1000);
    
    EXPECT_THROW(transaction->Make(from, from, 200), std::logic_error);
}

TEST_F(TransactionTest, MakeTransactionReturnsFalseWhenFeeTooHigh) {
    Account from(1, 1000);
    Account to(2, 500);
    
    transaction->set_fee(60); 
    
    EXPECT_FALSE(transaction->Make(from, to, 100));
    EXPECT_EQ(from.GetBalance(), 1000);
    EXPECT_EQ(to.GetBalance(), 500);
}

TEST_F(TransactionTest, MakeTransactionWhenFeeEqualsHalfSum) {
    Account from(1, 1000);
    Account to(2, 500);
    
    transaction->set_fee(50);
    
    EXPECT_FALSE(transaction->Make(from, to, 100));
    EXPECT_EQ(from.GetBalance(), 1000);
    EXPECT_EQ(to.GetBalance(), 500);
}

TEST_F(TransactionTest, MakeTransactionWhenFeeLessThanHalfSum) {
    Account from(1, 1000);
    Account to(2, 500);
    
    transaction->set_fee(1);
    
    EXPECT_TRUE(transaction->Make(from, to, 200));
    EXPECT_EQ(from.GetBalance(), 799);
    EXPECT_EQ(to.GetBalance(), 700);
}

TEST_F(TransactionTest, MakeTransactionWithMinimalSum) {
    Account from(1, 1000);
    Account to(2, 500);
    
    transaction->set_fee(1);
    
    EXPECT_TRUE(transaction->Make(from, to, 100));
    EXPECT_EQ(from.GetBalance(), 899);  
    EXPECT_EQ(to.GetBalance(), 600);    
}

TEST_F(TransactionTest, MakeTransactionWithLargeSum) {
    Account from(1, 1000000);
    Account to(2, 500000);
    
    transaction->set_fee(1);
    
    EXPECT_TRUE(transaction->Make(from, to, 50000));
    EXPECT_EQ(from.GetBalance(), 949999);  
    EXPECT_EQ(to.GetBalance(), 550000);    
}

TEST_F(TransactionTest, MakeTransactionRollbackOnFailedDebit) {
    Account from(1, 100);
    Account to(2, 500);
    
    transaction->set_fee(1);
    
    EXPECT_FALSE(transaction->Make(from, to, 200));
    EXPECT_EQ(from.GetBalance(), 100);
    EXPECT_EQ(to.GetBalance(), 500);
}

TEST_F(TransactionTest, MultipleTransactions) {
    Account from(1, 1000);
    Account to(2, 500);
    
    transaction->set_fee(1);
    
    EXPECT_TRUE(transaction->Make(from, to, 100));
    EXPECT_EQ(from.GetBalance(), 899);
    EXPECT_EQ(to.GetBalance(), 600);
    
    EXPECT_TRUE(transaction->Make(from, to, 50));
    EXPECT_EQ(from.GetBalance(), 848); 
    EXPECT_EQ(to.GetBalance(), 650);    
    
    EXPECT_FALSE(transaction->Make(from, to, 1000));
    EXPECT_EQ(from.GetBalance(), 848);
    EXPECT_EQ(to.GetBalance(), 650);
}

TEST_F(TransactionTest, DifferentSumsAndFees) {
    Account from(1, 1000);
    Account to(2, 500);
    
    transaction->set_fee(10);
    EXPECT_TRUE(transaction->Make(from, to, 200));
    EXPECT_EQ(from.GetBalance(), 790); 
    EXPECT_EQ(to.GetBalance(), 700);   
    transaction->set_fee(30);
    EXPECT_TRUE(transaction->Make(from, to, 100));
    EXPECT_EQ(from.GetBalance(), 660);  
    EXPECT_EQ(to.GetBalance(), 800);    
    
    transaction->set_fee(60);
    EXPECT_FALSE(transaction->Make(from, to, 100));
    EXPECT_EQ(from.GetBalance(), 660);
    EXPECT_EQ(to.GetBalance(), 800);
}

TEST_F(TransactionTest, TransactionWithLockedAccounts) {
    Account from(1, 1000);
    Account to(2, 500);
    
    from.Lock();
    
    transaction->set_fee(1);
    EXPECT_THROW(transaction->Make(from, to, 100), std::runtime_error);

    from.Unlock();
    EXPECT_TRUE(transaction->Make(from, to, 100));
}

TEST_F(TransactionTest, VerySmallFee) {
    Account from(1, 1000);
    Account to(2, 500);
    
    transaction->set_fee(0);
    
    EXPECT_TRUE(transaction->Make(from, to, 100));
    EXPECT_EQ(from.GetBalance(), 900);  
    EXPECT_EQ(to.GetBalance(), 600);    
}

TEST_F(TransactionTest, SaveToDataBaseOutput) {
    testing::internal::CaptureStdout();
    
    Account from(1, 1000);
    Account to(2, 500);
    
    transaction->set_fee(1);
    transaction->Make(from, to, 200);
    
    std::string output = testing::internal::GetCapturedStdout();
    
    EXPECT_NE(output.find("send to"), std::string::npos);
    EXPECT_NE(output.find("Balance 1 is"), std::string::npos);
    EXPECT_NE(output.find("Balance 2 is"), std::string::npos);
}

TEST_F(TransactionTest, SumJustAboveMinimum) {
    Account from(1, 1000);
    Account to(2, 500);
    
    transaction->set_fee(1);
    EXPECT_TRUE(transaction->Make(from, to, 101));
    EXPECT_EQ(from.GetBalance(), 898); 
    EXPECT_EQ(to.GetBalance(), 601);  
}

TEST_F(TransactionTest, HighFeeWithSufficientFunds) {
    Account from(1, 1000);
    Account to(2, 500);
    
    transaction->set_fee(40);
    EXPECT_TRUE(transaction->Make(from, to, 200));
    EXPECT_EQ(from.GetBalance(), 760);
    EXPECT_EQ(to.GetBalance(), 700);    
}
